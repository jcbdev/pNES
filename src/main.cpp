#include <iostream>
#include "SDL.h"
#include "Helpers/Logger.h"
#include "Rom/Cart.h"
#include "Core/Memory.h"
#include "Core/Cpu.h"
#include "Rom/Nrom.h"
#include "Core/Ppu.h"

static SDL_Window *window = NULL;
static SDL_GLContext gl_context;
static bool quitting = false;

void render() {

    SDL_GL_MakeCurrent(window, gl_context);

    SDL_GL_SwapWindow(window);

} //render


int SDLCALL watch(void *userdata, SDL_Event* event) {

    if (event->type == SDL_APP_WILLENTERBACKGROUND) {
        quitting = true;
    }

    return 1;
}

int main() {
    std::cout << "pNES!" << std::endl;

    ISystem *system = new System();

    //Load Rom
    ILogger *logger = new ConsoleLogger();
    Cart *cart = new Nrom(system);
    CpuMemory *memory = new CpuMemory(system);
    Cpu *cpu = new Cpu(system);
    Ppu *ppu = new Ppu(system);

    system->Configure(cpu, memory, cart, ppu, logger);
    cart->LoadRom("/home/jimbo/CLionProjects/pNES/test.nes");

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("pNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 256, 240, SDL_WINDOW_OPENGL);

    gl_context = SDL_GL_CreateContext(window);

    SDL_AddEventWatch(watch, NULL);

    cpu->Reset();
    while(!quitting && !cpu->error) {

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if(event.type == SDL_QUIT) {
                quitting = true;
            }
        }

        if (cpu->Interrupt()) continue;
        cpu->Cycle();
        render();
        SDL_Delay(2);

    }

    SDL_DelEventWatch(watch, NULL);
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete cart;
    delete logger;

    exit(0);
}