#include "include/aios.h"
#include <array>
#include <chrono>
#include <csignal>
#include <netdb.h>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace {
volatile std::sig_atomic_t gSignalStatus;
}
void signal_handler(int signal) { gSignalStatus = signal; }

struct chat {
  std::mutex m;
  std::queue<std::pair<size_t, std::string>> tasks;
  std::queue<std::pair<size_t, std::string>> answers;
};

static chat multithread_data;

class server {
public:
  server(aios::io_processor &proc) noexcept
      : proc{std::ref(proc)}, acceptors{aios::tcp_acceptor(
                                  proc,
                                  aios::ip::endpoint(
                                      aios::ip::address_v4::loopback(), 7666),
                                  ec)},
        sockets{aios::tcp_socket(proc)}, max_used{0} {
    for (size_t i = 0; i < acceptors.size(); ++i) {
      accept_new(i);
    }
  }

  void accept_new(size_t pos) {
    acceptors[pos].async_accept(
        sockets[pos],
        [this, pos](aios::system::error_code ec) {
          if (ec != aios::system::error_code::no_errors) {
            std::printf("Error in accept_new(%zu) -- %.*s\n", pos,
                        static_cast<int>(ec.message().size()),
                        ec.message().data());
            std::fflush(stdout);
          } else {
            std::printf("Catched new connection\n");
            std::fflush(stdout);
            size_t id = 0;
            if (!freed.empty()) {
              id = freed.back();
              freed.pop_back();
            } else {
              id = ++max_used;
            }
            clients.emplace(id, std::move(sockets[pos]));
            read_buffer[id] = {{}, 0, 0};
            write_buffer[id] = {{}, 0, 0};
            clients.find(id)->second.async_read_some(
                read_buffer[id].data.data(), buffer_for_client,
                [this, id](size_t recieved, aios::system::error_code ec) {
                  if (ec != aios::system::error_code::no_errors ||
                      recieved == 0) {
                    std::printf("Client [%zu] disconnected\n", id);
                    std::fflush(stdout);
                    clients.erase(id);
                    read_buffer.erase(id);
                    write_buffer.erase(id);
                    freed.push_back(id);
                    return;
                  }
                  read_buffer[id].fpos = recieved;
                  handle_new_data(id);
                });
          }
          accept_new(pos);
        },
        std::chrono::minutes(1));
  }

  void handle_new_data(size_t id) {
    for (size_t i = read_buffer[id].spos; i < read_buffer[id].fpos; ++i) {
      if (read_buffer[id].data[i] == '\r' && i < buffer_for_client - 1) {
        {
          std::lock_guard<std::mutex> lock(multithread_data.m);
          multithread_data.tasks.emplace(
              id,
              std::string(read_buffer[id].data.data() + read_buffer[id].spos,
                          i - read_buffer[id].spos));
        }
        while (i < read_buffer[id].fpos && (read_buffer[id].data[i] == '\n' ||
                                            read_buffer[id].data[i] == '\r')) {
          ++i;
        }
        for (size_t j = 0; j + i < read_buffer[id].fpos; ++j) {
          read_buffer[id].data[j] = read_buffer[id].data[j + i];
        }
        read_buffer[id].fpos -= i;
        read_buffer[id].spos = 0;
        return;
      }
    }
    clients.find(id)->second.async_read_some(
        read_buffer[id].data.data() + read_buffer[id].spos,
        buffer_for_client - read_buffer[id].fpos,
        [this, id](size_t recieved, aios::system::error_code ec) {
          if (ec != aios::system::error_code::no_errors || recieved == 0) {
            std::printf("Client [%zu] disconnected\n", id);
            std::fflush(stdout);
            clients.erase(id);
            read_buffer.erase(id);
            write_buffer.erase(id);
            freed.push_back(id);
            return;
          }
          read_buffer[id].fpos += recieved;
          handle_new_data(id);
        },
        std::chrono::minutes(1));
  }

  bool send_addrinfo() {
    std::lock_guard<std::mutex> lock(multithread_data.m);
    if (multithread_data.answers.empty()) {
      return false;
    }
    write_buffer[multithread_data.answers.front().first].spos = 0;
    write_buffer[multithread_data.answers.front().first].fpos =
        multithread_data.answers.front().second.size();
    std::copy_n(
        multithread_data.answers.front().second.data(),
        write_buffer[multithread_data.answers.front().first].fpos,
        write_buffer[multithread_data.answers.front().first].data.data());
    send_addrinfo_impl(multithread_data.answers.front().first);
    multithread_data.answers.pop();
    return true;
  }

  void send_addrinfo_impl(size_t id) {
    clients.find(id)->second.async_write_some(
        write_buffer[id].data.data() + write_buffer[id].spos,
        write_buffer[id].fpos - write_buffer[id].spos,
        [this, id](size_t sent, aios::system::error_code ec) {
          if (ec != aios::system::error_code::no_errors || sent == 0) {
            std::printf("Client [%zu] disconnected\n", id);
            std::fflush(stdout);
            clients.erase(id);
            read_buffer.erase(id);
            write_buffer.erase(id);
            freed.push_back(id);
            return;
          }
          write_buffer[id].spos += sent;
          if (write_buffer[id].spos == write_buffer[id].fpos) {
            handle_new_data(id);
          } else {
            send_addrinfo_impl(id);
          }
        },
        std::chrono::seconds(10));
  }

private:
  static constexpr size_t amount_of_working_listeners = 1;
  static constexpr size_t buffer_for_client = 128;
  struct buffer {
    std::array<char, buffer_for_client> data;
    size_t spos;
    size_t fpos;
  };

  aios::system::error_code ec;
  std::reference_wrapper<aios::io_processor> proc;
  std::array<aios::tcp_acceptor, amount_of_working_listeners> acceptors;
  std::array<aios::tcp_socket, amount_of_working_listeners> sockets;
  std::unordered_map<size_t, aios::tcp_socket> clients;
  std::unordered_map<size_t, buffer> read_buffer;
  std::unordered_map<size_t, buffer> write_buffer;
  std::vector<size_t> freed;
  size_t max_used;
};

void addrinfo_thread() {
  while (gSignalStatus != SIGINT) {
    std::unique_lock<std::mutex> lock(multithread_data.m);
    while (gSignalStatus != SIGINT && !multithread_data.tasks.empty()) {
      auto task = std::move(multithread_data.tasks.front());
      multithread_data.tasks.pop();
      lock.unlock();
      addrinfo hints{}, *result = nullptr;
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      if (getaddrinfo(task.second.c_str(), nullptr, &hints, &result)) {
        lock.lock();
        multithread_data.answers.emplace(task.first, "No such host\n");
      } else {
        std::string ans;
        for (addrinfo *nd = result; nd != nullptr; nd = nd->ai_next)
          ans += aios::ip::address_v4(
                     aios::system::network_to_host(
                         reinterpret_cast<sockaddr_in *>(nd->ai_addr)
                             ->sin_addr.s_addr))
                     .to_string() +
                 " ";
        ans.back() = '\n';
        lock.lock();
        multithread_data.answers.emplace(task.first, ans);
      }
      freeaddrinfo(result);
    }
    lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

int main() {
  std::signal(SIGINT, signal_handler);

  aios::io_processor proc;
  server s(proc);
  std::thread addr_thread(&addrinfo_thread);
  while (gSignalStatus != SIGINT) {
    proc.poll();
    while (s.send_addrinfo()) {
    }
  }
  addr_thread.join();
  return 0;
}
