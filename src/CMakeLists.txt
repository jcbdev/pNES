
set(REPO ../repo)

#find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

#find_package(Boost 1.65.0 COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
#add_executable(BoostTest main.cpp)
#target_link_libraries(BoostTest ${Boost_LIBRARIES})

add_subdirectory(Modules/keystone)

set(SOURCE_FILES
        Core/Cpu.cpp Core/Cpu.h Core/Memory.cpp Core/Memory.h Core/Core.h
        Core/Ppu.h Core/Ppu.cpp Core/System.h Core/System.cpp Core/PpuNew.h Core/PpuNew.cpp
        Helpers/Logger.h Helpers/Logger.cpp
        Rom/Cart.h Rom/Cart.cpp Rom/RomHeader.h Rom/RomTypes.h
        Rom/Nrom.cpp Rom/Nrom.h Core/System.cpp Core/System.h Core/Debug.cpp Core/Debug.h
        Core/Controller.h Core/Controller.cpp Core/NoDebug.h Core/NoDebug.cpp Core/Apu.cpp Core/Apu.h
        Core/Filter.h Core/Filter.cpp Core/CpuJit.cpp Core/CpuJit.h ../llvmsnippets/microcodes.c)

add_library(core ${SOURCE_FILES})
add_executable(pNes main.cpp)
#${GPERFTOOLS_TCMALLOC_AND_PROFILER}

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()

target_link_libraries(pNes core ${SDL2_LIBRARY} ${SDL2_TTF_LIBRARIES} ${LLVM_BINARY_DIR}/lib/libkeystoned.a )