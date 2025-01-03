#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum {
    QUIT,
    RUNNING,
    PAUSED
} vm_state_t;

typedef struct {
    vm_state_t state;
    uint8_t ram[4096];
    // todo: make these pointers into ram
    bool display[64 * 32];
    uint16_t stack[12];
    uint8_t V[16];
    uint16_t I;
    uint16_t PC;
    uint8_t delay_time;
    uint8_t sound_timer;
    bool keyboard[16];
    char* rom_name;
} vm_t;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
} sdl_t;

bool init_vm(vm_t* vm, char* rom_name) {
    const uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80,  // F
    };

    memcpy(&vm->ram[0], font, sizeof(font));

    FILE* rom = fopen(rom_name, "rb");
    if (!rom) {
        SDL_Log("Unable to open rom: %s\n", rom_name);
        return false;
    }

    fseek(rom, 0, SEEK_END);
    const long rom_size = ftell(rom);
    const long max_size = sizeof(vm->ram) - 0x200;
    rewind(rom);

    if (rom_size > max_size) {
        SDL_Log("Rom file is too big: %s\n", rom_name);
        return false;
    }

    if (fread(&vm->ram[0x200], rom_size, 1, rom) != 1) {
        SDL_Log("Unable to read rom file: %s\n", rom_name);
        return false;
    };

    fclose(rom);

    vm->state = RUNNING;
    vm->PC = 0x200;
    vm->rom_name = rom_name;

    return true;
}

bool init_sdl(sdl_t* sdl) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to init SDL: %s\n", SDL_GetError());
        return false;
    }

    sdl->window = SDL_CreateWindow(
        "Chip8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,  // 64 * 20
        640,   // 32 * 20
        0);

    if (!sdl->window) {
        SDL_Log("Unable to create SDL window: %s", SDL_GetError());
        return 1;
    }

    sdl->renderer = SDL_CreateRenderer(
        sdl->window,
        -1,
        SDL_RENDERER_ACCELERATED);

    if (!sdl->renderer) {
        SDL_Log("Unable to create SDL renderer: %s", SDL_GetError());
        return false;
    }
    return true;
}

void cleanup(sdl_t* sdl) {
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}

void process_input(vm_t* vm) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        vm->state = QUIT;
                        break;
                    case SDLK_SPACE:
                        if (vm->state == RUNNING) {
                            vm->state = PAUSED;
                        } else {
                            vm->state = RUNNING;
                        }
                        return;
                    default:
                        break;
                }
                break;

            case SDL_QUIT:
                vm->state = QUIT;
                break;

            default:
                break;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rom_name>\n", *argv);
        return 1;
    }

    sdl_t sdl = {};
    init_sdl(&sdl);

    vm_t vm;
    init_vm(&vm, *(argv + 1));

    // main loop
    while (vm.state != QUIT) {
        process_input(&vm);

        if (vm.state == PAUSED) {
            continue;
        }

        SDL_Delay(16);

        SDL_SetRenderDrawColor(sdl.renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(sdl.renderer);
        SDL_RenderPresent(sdl.renderer);
    }

    cleanup(&sdl);
    return 0;
}
