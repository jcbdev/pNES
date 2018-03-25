#include <iostream>
#include "SDL.h"
#include "Helpers/Logger.h"
#include "Rom/Cart.h"
#include "Core/Memory.h"
#include "Core/Cpu.h"
#include "Rom/Nrom.h"
#include "Core/Ppu.h"

static SDL_Window *window = NULL;
//static SDL_GLContext gl_context;
static SDL_Renderer *renderer = NULL;
static bool quitting = false;

void render(uint16_t* screenBuffer) {

    //SDL_GL_MakeCurrent(window, gl_context);

    //SDL_GL_SwapWindow(window);

    SDL_Texture* buffer = SDL_CreateTexture(renderer,
                                            SDL_PIXELFORMAT_RGB555,
                                            SDL_TEXTUREACCESS_STREAMING,
                                            256,
                                            262);

    int pitch = 256;
    void *buf = malloc(256*262*2);
    SDL_LockTexture(buffer,
                    NULL,      // NULL means the *whole texture* here.
                    &buf,
                    &pitch);

    memcpy(buf, screenBuffer, 256*262*2);

    SDL_UnlockTexture(buffer);

    SDL_RenderCopy(renderer, buffer, NULL, NULL);
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

    //gl_context = SDL_GL_CreateContext(window);

    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

    SDL_AddEventWatch(watch, NULL);

    cpu->Reset();
    while(!quitting && !cpu->error) {

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if(event.type == SDL_QUIT) {
                quitting = true;
            }
        }

        cpu->clocks--;
        ppu->clocks--;

        if (cpu->clocks <= 0) {
            if (cpu->Interrupt()) continue;
            cpu->Cycle();
        }
        if (ppu->clocks <= 0) ppu->Cycle();

        if (ppu->render) render(ppu->ScreenBuffer());
        //SDL_Delay(2);

    }

    SDL_DelEventWatch(watch, NULL);
    SDL_DestroyRenderer(renderer);
    //SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete cart;
    delete logger;

    exit(0);
}