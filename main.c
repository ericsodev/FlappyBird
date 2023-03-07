#include <stdio.h>
#include <time.h>
#include "game.h"
#include <math.h>

#ifdef _WIN32
#include <SDL2/SDL.h> /* Windows-specific SDL2 library */
#else
#include <SDL2/SDL.h> /* macOS- and GNU/Linux-specific */
#endif

/* Sets constants */
#define WIDTH 800
#define HEIGHT 600
#define DELAY 3000

void render(Game *game, SDL_Renderer *renderer)
{
    if (game->draw = 0)
        return;
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Draw bird
    SDL_Rect bird = {
        BIRD_X,
        game->birdYVal,
        BIRD_SIZE,
        BIRD_SIZE,
    };
    SDL_RenderFillRect(renderer, &bird);

    // Draw barriers
    int x = game->queue[0]->xVal;
    for (int i = 0; i < PRERENDER_BUFFER_SIZE; i++)
    {
        Barrier b = *game->queue[i];
        x += b.offsetFromLast;
        if (x >= WIDTH)
        {
            // off screen
            break;
        }

        printf("bot height %d\n", b.botHeight);
        SDL_Rect topRect = {
            x,
            0,
            BARRIER_WIDTH,
            HEIGHT - (b.botHeight + b.gap)};

        SDL_Rect botRect = {
            x,
            HEIGHT - b.botHeight,
            BARRIER_WIDTH,
            b.botHeight};

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &botRect);
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderFillRect(renderer, &topRect);

        x += BARRIER_WIDTH;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv)
{
    /* Initialises data */
    SDL_Window *window = NULL;
    srand(time(NULL));

    /*
     * Initialises the SDL video subsystem (as well as the events subsystem).
     * Returns 0 on success or a negative error code on failure using SDL_GetError().
     */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    /* Creates a SDL window */
    window = SDL_CreateWindow("Flappy Bird",           /* Title of the SDL window */
                              SDL_WINDOWPOS_UNDEFINED, /* Position x of the window */
                              SDL_WINDOWPOS_UNDEFINED, /* Position y of the window */
                              WIDTH,                   /* Width of the window in pixels */
                              HEIGHT,                  /* Height of the window in pixels */
                              0);                      /* Additional flag(s) */

    /* Checks if window has been created; if not, exits program */
    if (window == NULL)
    {
        fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL)
    {
        fprintf(stderr, "SDL renderer failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    char running = 1;
    SDL_Event e;
    Game *game = createGame(WIDTH, HEIGHT);
    Uint64 startTime;
    while (running)
    {
        startTime = SDL_GetPerformanceCounter();
        while (SDL_PollEvent(&e) != 0)
        {
            // User requests quit
            switch (e.type)
            {
            case SDL_QUIT:
                running = 0;
                goto end;
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_w)
                {
                    processInput(game);
                }
                break;
            }
        }
        cycle(game);
        render(game, renderer);
        if (game->gameRunning == 0)
        {
            printf("you're shit.\n");
            break;
        }
        float elapsed = (SDL_GetPerformanceCounter() - startTime) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        SDL_Delay(floor(16.0f - elapsed));
    }
end:
    /* Frees memory */
    destroyGame(game);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    /* Shuts down all SDL subsystems */
    SDL_Quit();

    return 0;
}
