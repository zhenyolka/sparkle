
cmake_minimum_required(VERSION 3.4.1)

set(SPARKLE_ROOT "../../../../../")

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS}")

add_executable(libsparkle.so
            main.cpp
            ${SPARKLE_ROOT}/were/src/were_event_source.cpp
            ${SPARKLE_ROOT}/were/src/were_call_queue.cpp
            ${SPARKLE_ROOT}/were/src/were_signal_handler.cpp
            ${SPARKLE_ROOT}/were/src/were_event_loop.cpp
            ${SPARKLE_ROOT}/were/src/were_timer.cpp
            ${SPARKLE_ROOT}/were/src/were_socket_unix.cpp
            ${SPARKLE_ROOT}/were/src/were_server_unix.cpp
            ${SPARKLE_ROOT}/were/src/were_stream.cpp
            ${SPARKLE_ROOT}/were/src/were_socket_unix_message_stream.cpp
            ${SPARKLE_ROOT}/platform/na/platform_na.cpp
            ${SPARKLE_ROOT}/compositor/gl/compositor_gl.cpp
            ${SPARKLE_ROOT}/compositor/gl/texture.cpp
            ${SPARKLE_ROOT}/common/sparkle_connection.cpp
            ${SPARKLE_ROOT}/common/sparkle_server.cpp
            ${SPARKLE_ROOT}/common/sparkle_surface_shm.cpp
            ${SPARKLE_ROOT}/common/were_benchmark.cpp
            ${SPARKLE_ROOT}/sound/sles/sound_sles.cpp
            ${SPARKLE_ROOT}/were/src/were_exception.cpp
            ${SPARKLE_ROOT}/common/sparkle_protocol.cpp
            ${SPARKLE_ROOT}/shm/shm.c
            ${SPARKLE_ROOT}/common/sparkle_sound_buffer.cpp
)

target_include_directories(libsparkle.so PRIVATE
    ${SPARKLE_ROOT}/
    ${SPARKLE_ROOT}/were/include
    ${SPARKLE_ROOT}/were/include/were
)

# add lib dependencies
target_link_libraries(libsparkle.so
    android
    log
    EGL
    GLESv2
    OpenSLES
)

