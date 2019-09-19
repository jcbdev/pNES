#include <iostream>
#include "SDL.h"
#include <SDL2/SDL_ttf.h>
#include <algorithm>
#include "Helpers/Logger.h"
#include "Rom/Cart.h"
#include "Core/Memory.h"
#include "Core/Cpu.h"
#include "Core/CpuJit.h"
#include "Rom/Nrom.h"
#include "Core/Ppu.h"
#include "Core/Debug.h"
#include "Core/NoDebug.h"
#include "Core/PpuNew.h"
#include "Core/Controller.h"

static SDL_Window *window = NULL;
static SDL_Window *debugWindow = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Renderer *debugRenderer = NULL;
static SDL_Texture  *buffer = NULL;

static TTF_Font* Sans = NULL;
static bool quitting = false;


void render(uint32_t* screenBuffer) {

    SDL_UpdateTexture(buffer, NULL, screenBuffer, 256 * sizeof(uint32_t));

    SDL_RenderClear(renderer);

    // Render the screen
    SDL_RenderSetLogicalSize(renderer, 512, 480);
    SDL_RenderCopy(renderer, buffer, NULL, NULL);

    SDL_RenderPresent(renderer);
} //render

void renderText(std::string text, int x, int y, SDL_Color color){
    if (text == "") return;
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

void renderAssembly(Disassembly d, int line, int pc, int cursorPosition, bool isBrk, int y, int xoffset = 0) {
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
    renderText(linetext.str(), 0 + xoffset, y, color);
    renderText(d.bytes, 70 + xoffset, y, color);
    renderText(d.assembly, 170 + xoffset, y, color);

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
    renderText(debug->status.pc, 270, 1, White);
    renderText(debug->status.a, 270, 15, White);
    renderText(debug->status.x, 270, 29, White);
    renderText(debug->status.y, 270, 43, White);
    renderText(debug->status.sp, 270, 57, White);
    renderText(debug->status.p, 270, 71, White);
    renderText(debug->status.clock, 270, 85, White);

    //Output various PPU status data
    renderText(debug->status.PPU0, 420, 1, White);
    renderText(debug->status.PPU1, 420, 15, White);
    renderText(debug->status.PPU2, 420, 29, White);
    renderText(debug->status.PPU3, 420, 43, White);
    renderText(debug->status.xaddr, 420, 57, White);
    renderText(debug->status.vaddr, 420, 71, White);
    renderText(debug->status.taddr, 420, 85, White);
    renderText(debug->status.buffer, 420, 104, White);
    renderText(debug->status.frame, 420, 118, White);
    renderText(debug->status.scanline, 420, 132, White);
    renderText(debug->status.dot, 420, 146, White);

    //output trace
    auto traceIterator = debug->trace.rbegin();
    int instructions = 0;
    ypos = 750;
    while (traceIterator != debug->trace.rend() && instructions < 55){
        instructions++;
        ypos -= 14;

        iterator = debug->disassembly.find(*traceIterator);
        if (iterator == debug->disassembly.end()){
            if (*traceIterator > 0x8000) {
                int opcodeSize = 0;
                debug->Decode(*traceIterator, &opcodeSize, true);
                iterator = debug->disassembly.find(*traceIterator);
                renderAssembly(iterator->second, iterator->first, cpu->pc, debug->cursorPosition, false, ypos, 600);
                std::cout << "Unassembled missing instruction at: " << *traceIterator << std::endl;
            }

        }
        else
            renderAssembly(iterator->second, iterator->first, cpu->pc, debug->cursorPosition, false, ypos, 600);
        traceIterator++;
    }

    if (isEditing) {
        std::stringstream addr;
        addr << "Goto address --> " << gotoAddress;
        renderText(addr.str(), 800, 750, White);
    }

    SDL_RenderPresent(debugRenderer);
}

int main() {
    std::cout << "pNES!" << std::endl;

    ISystem *system = new System();

    //Load Rom
    ILogger *logger = new ConsoleLogger(system);
    Cart *cart = new Nrom(system);
    CpuMemory *memory = new CpuMemory(system);
    CpuJit *cpu = new CpuJit(system);
    PpuNew *ppu = new PpuNew(system);
    //Ppu *ppu = new Ppu(system);
    IDebug *debug = new Debug(system);

    Controller *controller1 = new Controller(system);

    system->Configure(cpu, memory, cart, ppu, controller1, debug, logger);
    cart->LoadRom("/home/jimbo/CLionProjects/pNES/test.nes");
    //cart->LoadRom("/Users/james/ClionProjects/LittlePNes/test.nes");
    //cart->LoadRom("/home/jimbo/CLionProjects/pNES/color_test.nes");
    //cart->LoadRom("/home/jimbo/CLionProjects/pNES/palette.nes");
    //cart->LoadRom("/home/jimbo/CLionProjects/pNES/nestest.nes");
    //cart->LoadRom("/Users/james/ClionProjects/LittlePNes/nestest.nes");
    system->Reset();

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    TTF_Init();
    Sans = TTF_OpenFont("/home/jimbo/CLionProjects/pNES/Monospace.ttf", 12); //this opens a font style and sets a size
    //Sans = TTF_OpenFont("/Users/james/ClionProjects/LittlePNes/Monospace.ttf", 12); //this opens a font style and sets a size

    window = SDL_CreateWindow("pNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 960, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);
    buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 256, 240);

    cpu->Reset();
    std::string gotoAddress = "8000";
    bool isEditing = false;

    uint32_t startTime, currentTime;
    const int FPS   = 30;
    const int DELAY = 1000 / FPS;
    while(!quitting) {// && !cpu->error
        startTime = SDL_GetTicks();

        SDL_Event event;
        while( SDL_PollEvent(&event) ) {
            if(event.type == SDL_QUIT) {
                quitting = true;
            }

            if (event.type == SDL_KEYDOWN) {

                if (event.key.keysym.sym == SDLK_d)
                    debug->pause = !debug->pause;

                if (event.key.keysym.sym == SDLK_d && event.key.keysym.mod == KMOD_LCTRL)
                {
                    if (debug->enabled) {
                        SDL_DestroyRenderer(debugRenderer);
                        SDL_DestroyWindow(debugWindow);
                    }
                    else {
                        debugWindow = SDL_CreateWindow("pNES", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768,
                                                       SDL_WINDOW_OPENGL);
                        debugRenderer = SDL_CreateRenderer(debugWindow, 0, SDL_RENDERER_ACCELERATED);
                    }
                    debug->enabled = !debug->enabled;
                }
            }

            if (!debug->pause) {
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_z) {
                        controller1->SetButton(0);
                    }
                    if (event.key.keysym.sym == SDLK_x) {
                        controller1->SetButton(1);
                    }
                    if (event.key.keysym.sym == SDLK_a) {
                        controller1->SetButton(2);
                    }
                    if (event.key.keysym.sym == SDLK_s) {
                        controller1->SetButton(3);
                    }
                    if (event.key.keysym.sym == SDLK_UP) {
                        controller1->SetButton(4);
                    }
                    if (event.key.keysym.sym == SDLK_DOWN) {
                        controller1->SetButton(5);
                    }
                    if (event.key.keysym.sym == SDLK_LEFT) {
                        controller1->SetButton(6);
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT) {
                        controller1->SetButton(7);
                    }
                }
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.sym == SDLK_z) {
                        controller1->UnsetButton(0);
                    }
                    if (event.key.keysym.sym == SDLK_x) {
                        controller1->UnsetButton(1);
                    }
                    if (event.key.keysym.sym == SDLK_a) {
                        controller1->UnsetButton(2);
                    }
                    if (event.key.keysym.sym == SDLK_s) {
                        controller1->UnsetButton(3);
                    }
                    if (event.key.keysym.sym == SDLK_UP) {
                        controller1->UnsetButton(4);
                    }
                    if (event.key.keysym.sym == SDLK_DOWN) {
                        controller1->UnsetButton(5);
                    }
                    if (event.key.keysym.sym == SDLK_LEFT) {
                        controller1->UnsetButton(6);
                    }
                    if (event.key.keysym.sym == SDLK_RIGHT) {
                        controller1->UnsetButton(7);
                    }
                }
            }

            if (debug->enabled && debug->pause) {
                if (event.type == SDL_KEYDOWN && isEditing) {
                    if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        gotoAddress.pop_back();
                    } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                        SDL_StopTextInput();
                        isEditing = false;
                    } else if (event.key.keysym.sym == SDLK_RETURN) {
                        SDL_StopTextInput();
                        isEditing = false;
                        debug->cursorPosition = std::stoul(gotoAddress, nullptr, 16);
                    }


                } else if (event.type == SDL_TEXTINPUT) {
                    gotoAddress += event.text.text;
                } else if (event.type == SDL_KEYDOWN && !isEditing) {
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
                        ss << std::setfill('0') << std::setw(4) << (int) cpu->pc;
                        gotoAddress = ss.str();
                        SDL_StartTextInput();
                    }
                    if (event.key.keysym.sym == SDLK_l) {
                        ppu->breakOnNextScanline = true;
                    }
                    if (event.key.keysym.sym == SDLK_f) {
                        ppu->framesToRunFor = 1;
                    }
                    if (event.key.keysym.sym == SDLK_k) {
                        ppu->scanlinesToRunFor = 128;
                    }
                }
            }
        }

        while (!ppu->render && !debug->pause) {
            system->Step();
        }

        render(ppu->ScreenBuffer());
        if (debug->enabled) debugRender(debug, cpu, isEditing, gotoAddress);

        if (debug->step) {
            debug->step = false;
            debug->pause = true;
        }

        currentTime = SDL_GetTicks() - startTime;
        if (currentTime < DELAY){
            SDL_Delay((int)(DELAY - currentTime));
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(buffer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    delete cart;
    delete logger;

    exit(0);
}