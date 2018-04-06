#include <iostream>
#include "SDL.h"
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include "Helpers/Logger.h"
#include "Rom/Cart.h"
#include "Core/Memory.h"
#include "Core/Cpu.h"
#include "Rom/Nrom.h"
#include "Core/Ppu.h"
#include "Core/Debug.h"

static SDL_Window *window = NULL;
static SDL_Window *debugWindow = NULL;
//static SDL_GLContext gl_context;
static SDL_Renderer *renderer = NULL;
static SDL_Renderer *debugRenderer = NULL;
static SDL_Texture  *buffer = NULL;

//static SDL_Texture  *message = NULL;
//static SDL_Texture  *debugBuffer = NULL;
static TTF_Font* Sans = NULL;
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

//void debugRender(uint32_t* chr) {
//    SDL_UpdateTexture(debugBuffer, NULL, chr, 256 * sizeof(uint32_t));
//
//    SDL_RenderClear(debugRenderer);
//
//    SDL_RenderSetLogicalSize(debugRenderer, 512, 256);
//    SDL_RenderCopy(debugRenderer, debugBuffer, NULL, NULL);
//
//    SDL_RenderPresent(debugRenderer);
//}

void renderText(std::string text, int x, int y, SDL_Color color){
    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(Sans, text.c_str(), color);

    SDL_Texture *message = SDL_CreateTextureFromSurface(debugRenderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = x;  //controls the rect's x coordinate
    Message_rect.y = y; // controls the rect's y coordinte
    Message_rect.w = surfaceMessage->w; // controls the width of the rect
    Message_rect.h = surfaceMessage->h; // controls the height of the rect

//Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes

    SDL_RenderCopy(debugRenderer, message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture

//Don't forget too free your surface and texture

    SDL_DestroyTexture(message);
    SDL_FreeSurface(surfaceMessage);
}

void renderAssembly(Disassembly d, int line, int pc, int cursorPosition, bool isBrk, int y) {
    SDL_Color color = {255, 255, 255};
    std::stringstream linetext;
    if (line == pc) {
        color = {0, 255, 0};
        linetext << " ";
    }
    else if (line == cursorPosition) {
        color = {255, 0, 0};
        linetext << ">";
    }
    else if (isBrk){
        color = {200, 200, 240};
        linetext << "#";
    }
    else {
        linetext << " ";
    }

    linetext << d.address << ":  ";
    renderText(linetext.str(), 0, y, color);
    renderText(d.bytes, 50, y, color);
    renderText(d.assembly, 150, y, color);

}

void debugRender(IDebug *debug, ICpu *cpu, bool isEditing, std::string gotoAddress){
    SDL_Color White = {255, 255, 255};
    SDL_Color Green = {0, 255, 0};
    int fsize = 12;
    SDL_RenderClear(debugRenderer);

    auto iterator = debug->disassembly.find(debug->cursorPosition);
    int forward = 0;
    int back = 0;
//    //Get start item
    while (iterator != debug->disassembly.begin() && back < 24){
        back++;
        iterator--;
    }
    int ypos = 1;
    while (back>0) {
        renderAssembly(iterator->second, iterator->first, cpu->pc, debug->cursorPosition, debug->isBrk(iterator->first), ypos);
        ypos+=14;
        back--;
        iterator++;
        forward++;
    }

    if (iterator != debug->disassembly.end()) {
        renderAssembly(iterator->second, iterator->first, cpu->pc, debug->cursorPosition, debug->isBrk(iterator->first), ypos);
        ypos += 14;
        iterator++;
        forward++;
    }

    while (iterator != debug->disassembly.end() && forward < 60){
        renderAssembly(iterator->second, iterator->first, cpu->pc, debug->cursorPosition, debug->isBrk(iterator->first), ypos);
        ypos+=14;
        forward++;
        iterator++;
    }

    //Output various cpu status data
    renderText(debug->status.pc, 250, 1, White);
    renderText(debug->status.a, 250, 15, White);
    renderText(debug->status.x, 250, 29, White);
    renderText(debug->status.y, 250, 43, White);
    renderText(debug->status.sp, 250, 57, White);
    renderText(debug->status.p, 250, 71, White);
    renderText(debug->status.clock, 250, 85, White);

    //Output various PPU status data
    renderText(debug->status.PPU0, 400, 1, White);
    renderText(debug->status.PPU1, 400, 15, White);
    renderText(debug->status.PPU2, 400, 29, White);
    renderText(debug->status.PPU3, 400, 43, White);
    renderText(debug->status.xaddr, 400, 57, White);
    renderText(debug->status.vaddr, 400, 71, White);
    renderText(debug->status.taddr, 400, 85, White);
    renderText(debug->status.buffer, 400, 104, White);
    renderText(debug->status.frame, 400, 118, White);
    renderText(debug->status.scanline, 400, 132, White);
    renderText(debug->status.dot, 400, 146, White);

    if (isEditing) {
        std::stringstream addr;
        addr << "Goto address --> " << gotoAddress;
        renderText(addr.str(), 800, 750, White);
    }

    SDL_RenderPresent(debugRenderer);
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
    Debug *debug = new Debug(system);

    system->Configure(cpu, memory, cart, ppu, debug, logger);
    cart->LoadRom("/home/jimbo/CLionProjects/pNES/test.nes");
    system->Reset();

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    TTF_Init();
    Sans = TTF_OpenFont("/home/jimbo/CLionProjects/pNES/Monospace.ttf", 12); //this opens a font style and sets a size

    window = SDL_CreateWindow("pNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 512, 480, SDL_WINDOW_OPENGL);
    debugWindow = SDL_CreateWindow("pNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_OPENGL);

    //gl_context = SDL_GL_CreateContext(window);

    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    debugRenderer = SDL_CreateRenderer(debugWindow, 0, SDL_RENDERER_ACCELERATED);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, 256, 240);
    //debugBuffer = SDL_CreateTexture(debugRenderer, SDL_PIXELFORMAT_ARGB32, SDL_TEXTUREACCESS_STREAMING, 512, 512);

    SDL_AddEventWatch(watch, NULL);

    cpu->Reset();
    std::string gotoAddress = "8000";
    bool isEditing = false;
    while(!quitting && !cpu->error) {

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if(event.type == SDL_QUIT) {
                quitting = true;
            }

            if (event.type == SDL_KEYDOWN && isEditing) {
                if (event.key.keysym.sym == SDLK_BACKSPACE){
                    gotoAddress.pop_back();
                }

                else if (event.key.keysym.sym == SDLK_ESCAPE){
                    SDL_StopTextInput();
                    isEditing = false;
                }

                else if (event.key.keysym.sym == SDLK_RETURN){
                    SDL_StopTextInput();
                    isEditing = false;
                    debug->cursorPosition = std::stoul(gotoAddress, nullptr, 16);
                }


            }
            else if (event.type == SDL_TEXTINPUT) {
                gotoAddress += event.text.text;
            }
            else if (event.type == SDL_KEYDOWN && !isEditing) {
                if (event.key.keysym.sym == SDLK_d)
                    debug->pause = !debug->pause;
                if (event.key.keysym.sym == SDLK_s) {
                    debug->pause = false;
                    debug->step = true;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quitting = true;
                if (event.key.keysym.sym == SDLK_F10) {
                    system->Reset();
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    if (debug->cursorPosition > 0x8000) debug->GoPrev();
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    if (debug->cursorPosition < 0xFFFF) debug->GoNext();
                }
                if (event.key.keysym.sym == SDLK_SPACE) {
                    debug->AddBrk();
                }
                if (event.key.keysym.sym == SDLK_g) {
                    isEditing = true;
                    std::stringstream ss;
                    ss << std::setfill('0') << std::setw(4) << (int)cpu->pc;
                    gotoAddress = ss.str();
                    SDL_StartTextInput();
                }
            }

        }

        if (!debug->pause) {

            cpu->clocks--;
            ppu->clocks--;

            if (cpu->clocks <= 0) {
                debug->Refresh();
                if (debug->pause) {
                    cpu->clocks++;
                    ppu->clocks++;
                    continue;
                }
                if (debug->step) {
                    debug->step = false;
                    debug->pause = true;
                }

                if (!cpu->Interrupt())
                    cpu->Cycle();

                if (debug->pause) debug->cursorPosition = cpu->pc;
            }

            if (ppu->clocks <= 0) ppu->Cycle();
            if (ppu->dot == 340 && ppu->scanline == 0xF0) system->totalClocks = 0;

            if (ppu->render) {
                render(ppu->ScreenBuffer());
                debugRender(debug, cpu, isEditing, gotoAddress);
            }
        }
        else{
            debug->Refresh();
            debugRender(debug, cpu, isEditing, gotoAddress);
        }
    }

    SDL_DelEventWatch(watch, NULL);
    SDL_DestroyRenderer(debugRenderer);
    //SDL_DestroyTexture(debugBuffer);
    SDL_DestroyWindow(debugWindow);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(buffer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete cart;
    delete logger;

    exit(0);
}