cmake_minimum_required(VERSION 3.5)

project(aios LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(endian STATIC
  endian.h
  endian.cpp
)


include(TestBigEndian)

TEST_BIG_ENDIAN(BIGENDIAN)

if(${BIGENDIAN})
    target_compile_definitions(endian PRIVATE -DBIG_ENDIAN)
else(${BIGENDIAN})
    target_compile_definitions(endian PRIVATE -DLITTLE_ENDIAN)
endif(${BIGENDIAN})
