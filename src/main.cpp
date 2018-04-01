#include <iostream>
#include "SDL.h"
#include "Helpers/Logger.h"
#include "Rom/Cart.h"
#include "Core/Memory.h"
#include "Core/Cpu.h"
#include "Rom/Nrom.h"
#include "Core/Ppu.h"

static SDL_Window *window = NULL;
static SDL_Window *chrWindow = NULL;
//static SDL_GLContext gl_context;
static SDL_Renderer *renderer = NULL;
static SDL_Renderer *chrRenderer = NULL;
static SDL_Texture  *buffer = NULL;
static SDL_Texture  *chrBuffer = NULL;
static bool quitting = false;

SDL_Texture* generateScanlineTexture(SDL_Renderer* renderer)
{
    // Create a scanline texture for 3x rendering
    SDL_Texture* scanlineTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 256 * 3, 262 * 3);
    uint32_t* scanlineTextureBuffer = new uint32_t[256 * 262 * 3 * 3];
    for (int y = 0; y < 262; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    uint32_t color = 0xff000000;
                    switch (j)
                    {
                        case 0:
                            color |= 0xfdd6c7;
                            break;
                        case 1:
                            color |= 0xbef5e1;
                            break;
                        case 2:
                            color |= 0xcfe2ff;
                            break;
                    }
                    scanlineTextureBuffer[((y * 3) + i) * (256 * 3) + (x * 3) + j] = color;
                }
            }
        }
    }
    SDL_SetTextureBlendMode(scanlineTexture, SDL_BLENDMODE_MOD);
    SDL_UpdateTexture(scanlineTexture, NULL, scanlineTextureBuffer, sizeof(uint16_t) * 256 * 3);
    delete [] scanlineTextureBuffer;

    return scanlineTexture;
}

void render(uint32_t* screenBuffer) {

    SDL_UpdateTexture(buffer, NULL, screenBuffer, 256 * sizeof(uint32_t));

    SDL_RenderClear(renderer);

    // Render the screen
    SDL_RenderSetLogicalSize(renderer, 512, 480);
    SDL_RenderCopy(renderer, buffer, NULL, NULL);

    // Render scanlines
//    SDL_RenderSetLogicalSize(renderer, 256 * 3, 262 * 3);
//    SDL_RenderCopy(renderer, scanlineTexture, NULL, NULL);

    SDL_RenderPresent(renderer);
} //render

void chrRender(uint32_t* chr) {
    SDL_UpdateTexture(chrBuffer, NULL, chr, 256 * sizeof(uint32_t));

    SDL_RenderClear(chrRenderer);

    SDL_RenderSetLogicalSize(chrRenderer, 512, 256);
    SDL_RenderCopy(chrRenderer, chrBuffer, NULL, NULL);

    SDL_RenderPresent(chrRenderer);
}


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
    system->Reset();

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("pNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, SDL_WINDOW_OPENGL);
    chrWindow = SDL_CreateWindow("pNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 256, SDL_WINDOW_OPENGL);

    //gl_context = SDL_GL_CreateContext(window);

    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    chrRenderer = SDL_CreateRenderer(chrWindow, 0, SDL_RENDERER_ACCELERATED);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    chrBuffer = SDL_CreateTexture(chrRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, 256, 32);

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

        if (ppu->clocks <= 0) ppu->Cycle();
        if (ppu->Dot() == 340 && ppu->Scanline() == 0xF0) system->totalClocks = 0;

        if (cpu->clocks <= 0) {
            if (cpu->Interrupt()) continue;
            cpu->Cycle();
        }

        if (ppu->render) {
            render(ppu->ScreenBuffer());
            chrRender(ppu->ChrData());
        }
    }

    SDL_DelEventWatch(watch, NULL);
    SDL_DestroyRenderer(chrRenderer);
    SDL_DestroyTexture(chrBuffer);
    SDL_DestroyWindow(chrWindow);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(buffer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete cart;
    delete logger;

    exit(0);
}