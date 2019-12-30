cmake_minimum_required(VERSION 3.5)

project(aios LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

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

include(TestBigEndian)

TEST_BIG_ENDIAN(BIGENDIAN)

if(${BIGENDIAN})
    target_compile_definitions(endian PRIVATE -DBIG_ENDIAN)
else(${BIGENDIAN})
    target_compile_definitions(endian PRIVATE -DLITTLE_ENDIAN)
endif(${BIGENDIAN})
