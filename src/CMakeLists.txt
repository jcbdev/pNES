
set(REPO ../repo)

#find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

#find_package(Boost 1.65.0 COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
#add_executable(BoostTest main.cpp)
#target_link_libraries(BoostTest ${Boost_LIBRARIES})

set(SOURCE_FILES
        Core/Cpu.cpp Core/Cpu.h Core/Memory.cpp Core/Memory.h Core/Core.h
        Core/Ppu.h Core/Ppu.cpp Core/System.h Core/System.cpp Core/PpuNew.h Core/PpuNew.cpp
        Helpers/Logger.h Helpers/Logger.cpp
        Rom/Cart.h Rom/Cart.cpp Rom/RomHeader.h Rom/RomTypes.h
        Rom/Nrom.cpp Rom/Nrom.h Core/System.cpp Core/System.h Core/Debug.cpp Core/Debug.h
        Core/Controller.h Core/Controller.cpp Core/NoDebug.h Core/NoDebug.cpp Core/Apu.cpp Core/Apu.h)

add_library(core ${SOURCE_FILES})
add_executable(pNes main.cpp)

target_link_libraries(pNes core ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${GPERFTOOLS_TCMALLOC_AND_PROFILER})