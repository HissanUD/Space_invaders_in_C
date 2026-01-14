#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "game.h"

#define LEFTBORDER 0
#define RIGHTBORDER 1340

void init(AppState *app)
{
    int spaceship_png = IMG_INIT_PNG;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Failed to initialise SDL\n");
        exit(1);
    }
    if (!IMG_Init(spaceship_png))
    {
        printf("Error: %s\n", IMG_GetError());
        exit(1);
    }
    app->window = SDL_CreateWindow("Space invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1440, 1080, SDL_WINDOW_SHOWN);
    if (!app->window)
    {
        printf("Error: %s\n", SDL_GetError());
        exit(1);
    }
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void load(AppState *app, Player *player)
{

    SDL_Surface *temporary_spaceship = NULL;

    temporary_spaceship = IMG_Load("resources/spaceship.png");
    if (!temporary_spaceship)
    {
        printf("Error: %s\n", IMG_GetError());
        exit(1);
    }
    player->sprite = SDL_CreateTextureFromSurface(app->renderer, temporary_spaceship);

    SDL_FreeSurface(temporary_spaceship);
}

void destroy(AppState *app, Player *player)
{
    SDL_DestroyTexture(player->sprite);
    SDL_DestroyWindow(app->window);
    SDL_DestroyRenderer(app->renderer);
    IMG_Quit();
    SDL_Quit();
    free(app);
    free(player);
}

void init_player(Player *player)
{
    player->player_speed = 15;
    player->dest.x = 670;
    player->dest.y = 1020;
    player->dest.w = 100;
    player->dest.h = 50;
}

int main(int argc, char **argv)
{
    int running = 1;
    SDL_Event event;
    AppState *app;
    Player *player;
    KeyPress *keys_pressed;

    app = calloc(1, sizeof(AppState));
    if (!app)
    {
        fprintf(stderr, "Fatal error out of memory");
        return 1;
    }
    player = calloc(1, sizeof(Player));
    if (!player)
    {
        fprintf(stderr, "Fatal error out of memory");
        return 1;
    }
    keys_pressed = calloc(1, sizeof(KeyPress));
    if (!keys_pressed)
    {
        fprintf(stderr, "Fatal error out of memory");
        return 1;
    }

    init(app);
    init_player(player);

    load(app, player);

    SDL_UpdateWindowSurface(app->window);

    while (running)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = 0;
                break;
            case SDL_WINDOW_MINIMIZED:
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_a:

                    keys_pressed->left = 1;

                    break;

                case SDLK_d:

                    keys_pressed->right = 1;

                    break;

                case SDLK_SPACE:

                    keys_pressed->space = 1;

                    break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                case SDLK_a:
                    if (keys_pressed->left == 1)
                    {
                        keys_pressed->left = 0;
                    }
                    break;

                case SDLK_d:
                    if (keys_pressed->right == 1)
                    {
                        keys_pressed->right = 0;
                    }

                    break;
                }
                break;
            }
        }
        if (keys_pressed->left == 1 && keys_pressed->right != 1)
        {
            player->state = move_left;
        }
        if (keys_pressed->right == 1 && keys_pressed->left != 1)
        {
            player->state = move_right;
        }
        if (keys_pressed->right != 1 && keys_pressed->left != 1)
        {
            player->state = idle;
        }
        if (keys_pressed->right == 1 && keys_pressed->left == 1)
        {
            player->state = idle;
        }

        if (player->state == move_left && player->dest.x > LEFTBORDER)
        {
            player->dest.x -= player->player_speed;
        }
        if (player->state == move_right && player->dest.x < RIGHTBORDER)
        {
            player->dest.x += player->player_speed;
        }

        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);
        SDL_RenderCopy(app->renderer, player->sprite, NULL, &player->dest);
        SDL_RenderPresent(app->renderer);
        SDL_Delay(16);
    }

    destroy(app, player);
    free(keys_pressed);

    return 0;
}
