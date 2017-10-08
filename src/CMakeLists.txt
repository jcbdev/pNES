
set(REPO ../repo)

#find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES Core/Cpu.cpp Core/Cpu.h Core/Memory.cpp Core/Memory.h Core/Core.h
        Helpers/Logger.h Helpers/Logger.cpp
        Rom/Cart.h Rom/Cart.cpp Rom/RomHeader.h)

add_library(core ${SOURCE_FILES})
add_executable(LittlePNes main.cpp)

target_link_libraries(LittlePNes core ${SDL2_LIBRARY})