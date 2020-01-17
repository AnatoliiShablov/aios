cmake_minimum_required(VERSION 3.5)

project(aios LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)

add_library(endian STATIC
        include/endian.h
        src/endian.cpp
        )

add_library(error_code STATIC
        include/error_code.h
        src/error_code.cpp
        )

add_library(address_v4 STATIC
        include/address_v4.h
        src/address_v4.cpp
        )
target_link_libraries(address_v4 endian error_code)

add_library(endpoint STATIC
        include/endpoint.h
        src/endpoint.cpp
        )
target_link_libraries(endpoint address_v4 endian)

add_library(timer STATIC
        include/timer.h
        src/timer.cpp
        )

add_library(timer_processor STATIC
        include/timer_processor.h
        src/timer_processor.cpp
        )
target_link_libraries(timer_processor timer)

add_library(tcp_socket STATIC
        include/tcp_socket.h
        src/tcp_socket.cpp
        )
target_link_libraries(tcp_socket io_processor)

add_library(tcp_acceptor STATIC
        include/tcp_acceptor.h
        src/tcp_acceptor.cpp
        )
target_link_libraries(tcp_acceptor io_processor)

add_library(io_processor STATIC
        include/io_processor.h
        src/io_processor.cpp
        )
target_link_libraries(io_processor endpoint error_code timer_processor)

add_library(aios STATIC include/aios.h)
target_link_libraries(aios io_processor endpoint error_code timer_processor timer tcp_acceptor tcp_socket address_v4 endian)

add_executable(addrinfo main.cpp)
target_link_libraries(addrinfo aios  ${CMAKE_THREAD_LIBS_INIT} )

include(TestBigEndian)

TEST_BIG_ENDIAN(BIGENDIAN)

if (${BIGENDIAN})
    target_compile_definitions(endian PRIVATE -DAIOS_BIG_ENDIAN)
else (${BIGENDIAN})
    target_compile_definitions(endian PRIVATE -DAIOS_LITTLE_ENDIAN)
endif (${BIGENDIAN})
