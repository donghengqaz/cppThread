project(main_test)

cmake_minimum_required (VERSION 3.5)

set(CMAKE_C_COMPILER g++)

add_compile_options(--std=c++11 -DCONFIG_USING_POSIX)
link_libraries(pthread rt)

include_directories(components/cpp_thread/include)

set(PLATFORM_DIR components/cpp_thread/platform)
set(PLATFORM ${PLATFORM_DIR}/posix_osapi.cc)

set(LIBRARY_DIR components/cpp_thread/library)
set(LIBRARY ${LIBRARY_DIR}/io.cc ${LIBRARY_DIR}/mutex.cc ${LIBRARY_DIR}/socket.cc ${LIBRARY_DIR}/types.cc)

set(MAIN_SRCS main/thread_test.cpp)

add_executable(main_test ${PLATFORM} ${MAIN_SRCS} ${LIBRARY})
