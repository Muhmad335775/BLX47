cmake_minimum_required(VERSION 3.22.1)
project(pulse_x_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

add_library(
    pulse_x_core
    SHARED
    pulse_x_core.cpp
    pulse_x_audio_engine.cpp
    pulse_x_procedural_world.cpp
    pulse_x_voice_scene_engine.cpp
    pulse_x_render_engine.cpp
    pulse_x_brand_output.cpp
)

find_library(log-lib log)

target_link_libraries(
    pulse_x_core
    ${log-lib}
)
