

set(SOURCE_FILES Core/Cpu.cpp Core/Cpu.h Core/Memory.cpp Core/Memory.h Core/Core.h)

add_library(core ${SOURCE_FILES})
add_executable(LittlePNes main.cpp)
target_link_libraries(LittlePNes core)