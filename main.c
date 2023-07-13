#include "game.h"
#include "logica.c"
#include "minmax.c"
#include "render.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void click_on_cell(game_t *, int, int);

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "Could not initialize sdl2: %s\n", "Jogo da Velha");
    return EXIT_FAILURE;
  }
  SDL_Window *w = SDL_CreateWindow("Jogo da Velha", 100, 100, SCREEN_WIDTH,
                                   SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (w == NULL) {
    fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_Renderer *renderer = SDL_CreateRenderer(
      w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    SDL_DestroyWindow(w);
    fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }
  SDL_SetWindowResizable(w, false);
  game_t game = {.board = {EMPTY},
                 .player = PLAYER_X,
                 .state = RUNNING_STATE,
                 .whoStarts = true,
                 .turn = true,
                 .gamesPlayed = 0};
  limpaTabuleiro(game.board);
  generateMovements();
  SDL_Event e;
  while (game.state != QUIT_STATE) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        game.state = QUIT_STATE;
        break;
      case SDL_MOUSEBUTTONDOWN:
        click_on_cell(&game, e.button.y / CELL_HEIGHT, e.button.x / CELL_WIDTH);
        break;
      }
      SDL_SetWindowSize(w, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    render_game(renderer, &game);
    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(w);
  SDL_Quit();
  free(arrayPossibleMovs);
  return EXIT_SUCCESS;
}
void click_on_cell(game_t *game, int linha, int coluna) {
  if (game->state == RUNNING_STATE) {
    if (game->turn) {
      playerTurn(game, linha, coluna);
    } else {
      nextmove(game->board, 2);
      game->turn = !game->turn;
    }
    game_over_condition(game);
  } else
    resetGame(game);
}
