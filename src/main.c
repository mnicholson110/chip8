#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <stdbool.h>

typedef enum {
    QUIT,
    RUNNING,
    PAUSED
} vm_state_t;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    vm_state_t vm_state;
} state_t;

int main(int argc, char** argv) {
    state_t state = {0};
    // init SDL systems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Unable to init SDL: %s\n", SDL_GetError());
        return 1;
    }

    // init window and renderer
    state.window = SDL_CreateWindow(
        "Chip8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280,  // * 20
        640,   // * 20
        0);

    if (!state.window) {
        SDL_Log("Unable to create SDL window: %s", SDL_GetError());
        return 1;
    }

    state.renderer = SDL_CreateRenderer(
        state.window,
        -1,
        SDL_RENDERER_ACCELERATED);

    if (!state.renderer) {
        SDL_Log("Unable to create SDL renderer: %s", SDL_GetError());
        return 1;
    }

    state.vm_state = RUNNING;

    SDL_SetRenderDrawColor(state.renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state.renderer);

    // main loop
    while (state.vm_state != QUIT) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            state.vm_state = QUIT;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_QUIT:
                    state.vm_state = QUIT;
                    break;
                default:
                    break;
            }
        }

        SDL_Delay(16);

        SDL_RenderPresent(state.renderer);
    }

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
    return 0;
}
