#include "../include/io_processor.h"

aios::io_processor::io_processor() noexcept : efd{epoll_create1(0)} {}

aios::io_processor::io_processor(aios::system::error_code &ec) noexcept
    : efd{epoll_create1(0)} {
  ec = aios::system::error_code::no_errors;
  if (efd == -1) {
    ec = aios::system::error_code::cant_create_epoll;
  }
}

void aios::io_processor::add_fd(int fd, aios::system::error_code &ec) const
    noexcept {
  ec = aios::system::error_code::no_errors;
  epoll_event tmp_ev{};
  tmp_ev.events = 0;
  tmp_ev.data.fd = fd;
  if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &tmp_ev) < 0) {
    ec = aios::system::error_code::cant_add_to_epoll;
  }
}

void aios::io_processor::add_fd(int fd) const noexcept {
  aios::system::error_code ec;
  add_fd(fd, ec);
}

void aios::io_processor::remove_fd(int fd,
                                   aios::system::error_code &ec) noexcept {
  ec = aios::system::error_code::no_errors;
  epoll_event tmp_ev{};
  tmp_ev.events = 0;
  tmp_ev.data.fd = fd;
  acceptors.erase(fd);
  readers.erase(fd);
  writers.erase(fd);
  if (epoll_ctl(efd, EPOLL_CTL_DEL, fd, &tmp_ev) < 0) {
    ec = aios::system::error_code::cant_del_from_epoll;
  }
}

void aios::io_processor::remove_fd(int fd) noexcept {
  aios::system::error_code ec;
  remove_fd(fd, ec);
}

void aios::io_processor::register_acceptor(
    int fd, std::function<void(int, aios::system::error_code)> &&handler) {
  epoll_event tmp_ev{};
  tmp_ev.events = EPOLLIN | EPOLLET;
  tmp_ev.data.fd = fd;
  if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev) < 0) {
    handler(-1, aios::system::error_code::cant_mod_epoll);
    return;
  }
  acceptors.emplace(fd, l_struct{std::move(handler), {}});
}

void aios::io_processor::register_acceptor(
    int fd, std::function<void(int, aios::system::error_code)> &&handler,
    std::chrono::seconds dur) {
  epoll_event tmp_ev{};
  tmp_ev.events = EPOLLIN | EPOLLET;
  tmp_ev.data.fd = fd;
  if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev) < 0) {
    handler(-1, aios::system::error_code::cant_mod_epoll);
    return;
  }
  acceptors.emplace(
      fd, l_struct{std::move(handler),
                   time_proc.add_timer(
                       [this, fd]() {
                         auto handler = std::move(acceptors[fd].handler);
                         unregister_acceptor(fd);
                         handler(-1, aios::system::error_code::time_exceeded);
                       },
                       dur)});
}

void aios::io_processor::register_reader(
    int fd, char *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler) {
  epoll_event tmp_ev{};
  tmp_ev.events = (writers.count(fd) > 0 ? EPOLLOUT : 0) | EPOLLIN | EPOLLET;
  tmp_ev.data.fd = fd;
  if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev) < 0) {
    handler(0, aios::system::error_code::cant_mod_epoll);
    return;
  }
  readers.emplace(fd, r_struct{data, length, std::move(handler), {}});
}

void aios::io_processor::register_reader(
    int fd, char *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler,
    std::chrono::seconds dur) {
  epoll_event tmp_ev{};
  tmp_ev.events = (writers.count(fd) ? EPOLLOUT : 0) | EPOLLIN | EPOLLET;
  tmp_ev.data.fd = fd;
  if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev) < 0) {
    handler(0, aios::system::error_code::cant_mod_epoll);
    return;
  }
  readers.emplace(
      fd, r_struct{data, length, std::move(handler),
                   time_proc.add_timer(
                       [this, fd]() {
                         auto handler = std::move(readers[fd].handler);
                         unregister_reader(fd);
                         handler(0, aios::system::error_code::time_exceeded);
                       },
                       dur)});
}

void aios::io_processor::register_writer(
    int fd, const char *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler) {
  epoll_event tmp_ev{};
  tmp_ev.events = (readers.count(fd) ? EPOLLIN : 0) | EPOLLOUT | EPOLLET;
  tmp_ev.data.fd = fd;
  if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev) < 0) {
    handler(0, aios::system::error_code::cant_mod_epoll);
    return;
  }
  writers.emplace(fd, w_struct{data, length, std::move(handler), {}});
}

void aios::io_processor::register_writer(
    int fd, const char *data, size_t length,
    std::function<void(size_t, aios::system::error_code)> &&handler,
    std::chrono::seconds dur) {
  epoll_event tmp_ev{};
  tmp_ev.events = (readers.count(fd) ? EPOLLIN : 0) | EPOLLOUT | EPOLLET;
  tmp_ev.data.fd = fd;
  if (epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev) < 0) {
    handler(0, aios::system::error_code::cant_mod_epoll);
    return;
  }
  writers.emplace(
      fd, w_struct{data, length, std::move(handler),
                   time_proc.add_timer(
                       [this, fd]() {
                         auto handler = std::move(writers[fd].handler);
                         unregister_writer(fd);
                         handler(0, aios::system::error_code::time_exceeded);
                       },
                       dur)});
}

void aios::io_processor::unregister_acceptor(int fd) {
  epoll_event tmp_ev{};
  tmp_ev.events = 0;
  tmp_ev.data.fd = fd;
  epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev);
  acceptors.erase(fd);
}

void aios::io_processor::unregister_reader(int fd) {
  epoll_event tmp_ev{};
  tmp_ev.events = writers.count(fd) ? EPOLLOUT | EPOLLET : 0;
  tmp_ev.data.fd = fd;
  epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev);
  readers.erase(fd);
}

void aios::io_processor::unregister_writer(int fd) {
  epoll_event tmp_ev{};
  tmp_ev.events = readers.count(fd) ? EPOLLIN | EPOLLET : 0;
  tmp_ev.data.fd = fd;
  epoll_ctl(efd, EPOLL_CTL_MOD, fd, &tmp_ev);
  writers.erase(fd);
}

void aios::io_processor::poll() {
  epoll_event events[max_events];
  int nfds = epoll_wait(efd, events, max_events, 100);
  if (nfds == -1) {
    return;
  }

  for (size_t i = 0; i < static_cast<size_t>(nfds); ++i) {
    if (acceptors.count(events[i].data.fd)) {
      int new_sock =
          accept4(events[i].data.fd, nullptr, nullptr, SOCK_NONBLOCK);
      auto handler = std::move(acceptors[events[i].data.fd].handler);
      if (auto it = acceptors.find(events[i].data.fd); it != acceptors.end()) {
        if (it->second.waiter.has_value()) {
          time_proc.remove_timer(it->second.waiter.value());
        }
      }
      unregister_acceptor(events[i].data.fd);
      handler(new_sock, new_sock > 0
                            ? aios::system::error_code::no_errors
                            : aios::system::error_code::cant_accept_socket);
    } else if ((events[i].events & EPOLLIN) &&
               readers.count(events[i].data.fd)) {
      long recieved = read(events[i].data.fd, readers[events[i].data.fd].data,
                           readers[events[i].data.fd].length);
      auto handler = std::move(readers[events[i].data.fd].handler);
      if (auto it = readers.find(events[i].data.fd); it != readers.end()) {
        if (it->second.waiter.has_value()) {
          time_proc.remove_timer(it->second.waiter.value());
        }
      }
      unregister_reader(events[i].data.fd);
      handler(static_cast<size_t>(recieved),
              recieved >= 0 ? aios::system::error_code::no_errors
                            : aios::system::error_code::error_on_read);
    } else if ((events[i].events & EPOLLOUT) &&
               writers.count(events[i].data.fd)) {
      long sent = write(events[i].data.fd, writers[events[i].data.fd].data,
                        writers[events[i].data.fd].length);
      auto handler = std::move(writers[events[i].data.fd].handler);
      if (auto it = writers.find(events[i].data.fd); it != writers.end()) {
        if (it->second.waiter.has_value()) {
          time_proc.remove_timer(it->second.waiter.value());
        }
      }
      unregister_writer(events[i].data.fd);
      handler(static_cast<size_t>(sent),
              sent >= 0 ? aios::system::error_code::no_errors
                        : aios::system::error_code::error_on_write);
    }
  }
  time_proc.poll_this_moment();
}
