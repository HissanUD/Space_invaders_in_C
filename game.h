#ifndef GAME_H // Header Guard (prevents including file twice)
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <unistd.h>

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;

} AppState;

typedef enum
{
    idle,
    move_left,
    move_right,
    dead

} player_state;

typedef struct
{
    SDL_Texture *sprite;
    player_state state;
    int player_speed;
    SDL_Rect dest;
} Player;

typedef struct game
{
    int left;
    int right;
    int space;
} KeyPress;

void init(AppState *app);
void load(AppState *app, Player *player);
void destroy(AppState *app, Player *player);
void init_player(Player *player);

#endif