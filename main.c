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
    app->window = SDL_CreateWindow("Space invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1440, 1080, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!app->window)
    {
        printf("Error: %s\n", SDL_GetError());
        exit(1);
    }
    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetLogicalSize(app->renderer, 1440, 1080);
}

void allocate_memory(AppState **app, Player **player, KeyPress **keys_pressed, Bullet **bullet)
{
    *app = calloc(1, sizeof(AppState));
    if (!*app)
    {
        fprintf(stderr, "Fatal error out of memory");
        exit(1);
    }
    *player = calloc(1, sizeof(Player));
    if (!*player)
    {
        fprintf(stderr, "Fatal error out of memory");
        exit(1);
    }
    *keys_pressed = calloc(1, sizeof(KeyPress));
    if (!*keys_pressed)
    {
        fprintf(stderr, "Fatal error out of memory");
        exit(1);
    }
    *bullet = calloc(1, sizeof(Bullet));
    if (!*bullet)
    {
        fprintf(stderr, "Fatal error out of memory");
        exit(1);
    }
}

void load_textures(AppState *app, Player *player, Bullet *bullet)
{

    SDL_Surface *temporary_spaceship = NULL;
    SDL_Surface *temporary_bullet = NULL;

    temporary_spaceship = IMG_Load("resources/spaceship.png");
    temporary_bullet = IMG_Load("resources/space_shot.png");
    if (!temporary_spaceship || !temporary_bullet)
    {
        printf("Error: %s\n", IMG_GetError());
        exit(1);
    }
    player->sprite = SDL_CreateTextureFromSurface(app->renderer, temporary_spaceship);
    bullet->sprite = SDL_CreateTextureFromSurface(app->renderer, temporary_bullet);

    SDL_FreeSurface(temporary_spaceship);
    SDL_FreeSurface(temporary_bullet);
}

void destroy(AppState *app, Player *player, Bullet *bullet)
{
    SDL_DestroyTexture(player->sprite);
    SDL_DestroyTexture(bullet->sprite);
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

void init_bullet(Bullet *bullet)
{
    bullet->bullet_speed = 15;
    bullet->dest.w = 4;
    bullet->dest.h = 20;
}

void keyboard_input(int *running, KeyPress *keys_pressed)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            *running = 0;
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
                keys_pressed->left = 0;
                break;
            case SDLK_d:
                keys_pressed->right = 0;
                break;
            case SDLK_SPACE:
                keys_pressed->space = 0;
                break;
            }
            break;
        }
    }
}

int main(int argc, char **argv)
{
    int running = 1;
    AppState *app;
    Player *player;
    Bullet *bullet;
    KeyPress *keys_pressed;

    allocate_memory(&app, &player, &keys_pressed, &bullet);
    init(app);
    init_player(player);
    init_bullet(bullet);
    load_textures(app, player, bullet);

    while (running)
    {

        keyboard_input(&running, keys_pressed);

        if (keys_pressed->left == 1 && keys_pressed->right != 1)
        {
            player->state = move_left;
        }
        else if (keys_pressed->right == 1 && keys_pressed->left != 1)
        {
            player->state = move_right;
        }
        else
        {
            player->state = idle;
        }

        if (keys_pressed->space == 1 && bullet->state != moving)
        {
            bullet->state = moving;
            bullet->dest.x = player->dest.x + (player->dest.w / 2) - (bullet->dest.w / 2);
            bullet->dest.y = player->dest.y;
        }

        if (player->state == move_left && player->dest.x > LEFTBORDER)
        {
            player->dest.x -= player->player_speed;
        }
        if (player->state == move_right && player->dest.x < RIGHTBORDER)
        {
            player->dest.x += player->player_speed;
        }
        if (bullet->dest.y <= 30)
        {
            bullet->state = hit;
        }
        if (bullet->state == moving && bullet->state != hit)
        {
            bullet->dest.y -= bullet->bullet_speed;
        }

        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);
        SDL_RenderCopy(app->renderer, player->sprite, NULL, &player->dest);
        if (bullet->state == moving)
        {
            SDL_RenderCopy(app->renderer, bullet->sprite, NULL, &bullet->dest);
        }
        SDL_RenderPresent(app->renderer);
        SDL_Delay(16);
    }

    destroy(app, player, bullet);
    free(keys_pressed);
    free(bullet);

    return 0;
}
