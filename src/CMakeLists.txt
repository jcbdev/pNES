
set(REPO ../repo)

#find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

set(SOURCE_FILES
        Core/Cpu.cpp Core/Cpu.h Core/Memory.cpp Core/Memory.h Core/Core.h
        Core/Ppu.h Core/Ppu.cpp Core/System.h Core/System.cpp
        Helpers/Logger.h Helpers/Logger.cpp
        Rom/Cart.h Rom/Cart.cpp Rom/RomHeader.h Rom/RomTypes.h
        Rom/Nrom.cpp Rom/Nrom.h Core/System.cpp Core/System.h Core/Debug.cpp Core/Debug.h)

add_library(core ${SOURCE_FILES})
add_executable(pNes main.cpp)

target_link_libraries(pNes core ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES})