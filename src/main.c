#include <SDL2/SDL.h>

typedef struct {
    SDL_Window* window;
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
        800,
        600,
        0);

    SDL_Quit();
    return 0;
}
