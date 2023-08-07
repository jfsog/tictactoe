#include "game.h"
#include "minmax.c"
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_render.h>
#include <sys/types.h>

const SDL_Color GRID_COLOR = {.r = 255, .g = 255, .b = 255};
const SDL_Color PLAYER_X_COLOR = {.r = 255, .g = 50, .b = 50};
const SDL_Color PLAYER_O_COLOR = {.r = 50, .g = 100, .b = 255};
const SDL_Color TIE_COLOR = {.r = 100, .g = 100, .b = 100};

void render_game(SDL_Renderer *, game_t *);
void render_x(SDL_Renderer *renderer, int row, int column,
              const SDL_Color *color) {
  const float half_box_side = fmin(CELL_WIDTH, CELL_HEIGHT) * 0.25;
  const float center_x = CELL_WIDTH * 0.5 + column * CELL_WIDTH;
  const float center_y = CELL_HEIGHT * 0.5 + row * CELL_HEIGHT;

  thickLineRGBA(renderer, center_x - half_box_side, center_y - half_box_side,
                center_x + half_box_side, center_y + half_box_side, 10,
                color->r, color->g, color->b, 255);
  thickLineRGBA(renderer, center_x + half_box_side, center_y - half_box_side,
                center_x - half_box_side, center_y + half_box_side, 10,
                color->r, color->g, color->b, 255);
}

void render_o(SDL_Renderer *renderer, int row, int column,
              const SDL_Color *color) {
  const float half_box_side = fmin(CELL_WIDTH, CELL_HEIGHT) * 0.25;
  const float center_x = CELL_WIDTH * 0.5 + column * CELL_WIDTH;
  const float center_y = CELL_HEIGHT * 0.5 + row * CELL_HEIGHT;

  filledCircleRGBA(renderer, center_x, center_y, half_box_side + 5, color->r,
                   color->g, color->b, 255);
  filledCircleRGBA(renderer, center_x, center_y, half_box_side - 5, 50, 50, 50,
                   255);
}
void render_board(SDL_Renderer *renderer, const int *board,
                  const SDL_Color *player_x_color,
                  const SDL_Color *player_o_color) {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      switch (board[i * N + j]) {
      case PLAYER_X:
        render_x(renderer, i, j, player_x_color);
        break;
      case PLAYER_O:
        render_o(renderer, i, j, player_o_color);
        break;
      }
    }
  }
}
void render_grid(SDL_Renderer *renderer, const SDL_Color *color) {
  SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, 255);

  for (int i = 1; i < N; ++i) {
    SDL_RenderDrawLine(renderer, i * CELL_WIDTH, 0, i * CELL_WIDTH,
                       SCREEN_HEIGHT);
    SDL_RenderDrawLine(renderer, 0, i * CELL_HEIGHT, SCREEN_WIDTH,
                       i * CELL_HEIGHT);
  }
}
void render_running_state(SDL_Renderer *renderer, const game_t *game) {
  render_grid(renderer, &GRID_COLOR);
  render_board(renderer, game->board, &PLAYER_X_COLOR, &PLAYER_O_COLOR);
}
Uint getWinningPos(Uint board) {
  for (int i = 0; i < possibleMovsAmount; i++)
    if ((arrayPossibleMovs[i] & board) == arrayPossibleMovs[i])
      return arrayPossibleMovs[i];
  return 0;
}
Uint highAndLowerBits(Uint wpos) {
  static int hf = sizeof(wpos) * 4;
  Uint low = 0;
  int i = 0;
  while ((wpos & 1) == 0)
    i++, wpos >>= 1;
  low = i;
  while (wpos > 1)
    i++, wpos >>= 1;
  return ((Uint)i << hf) | low;
}
void evalVictoryLine(game_t *game, int player) {
  if (game->winningPos == 0) {
    Uint wpos = getWinningPos(tabToBit(game->board, player));
    game->winningPos = highAndLowerBits(wpos);
  }
}
void render_game_over_state(SDL_Renderer *renderer, const game_t *game,
                            const SDL_Color *color) {
  static int hf = sizeof(game->winningPos) * 4;
  render_grid(renderer, color);
  render_board(renderer, game->board, color, color);
  int low = (game->winningPos << hf) >> hf;
  int hig = game->winningPos >> hf;
  if (low % N > hig % N) {
    int t = low;
    low = hig;
    hig = t;
  }
  int x1 = (low % N) * CELL_WIDTH + CELL_WIDTH * 0.5;
  int x2 = (hig % N) * CELL_WIDTH + CELL_WIDTH * 0.5;
  int y1 = (int)(low / N) * CELL_HEIGHT + CELL_HEIGHT * 0.5;
  int y2 = (int)(hig / N) * CELL_HEIGHT + CELL_HEIGHT * 0.5;
  thickLineRGBA(renderer, x1, y1, x2, y2, 20, color->r, color->g, color->b,
                255);
}

void render_game(SDL_Renderer *renderer, game_t *game) {
  switch (game->state) {
  case RUNNING_STATE:
    render_running_state(renderer, game);
    break;
  case PLAYER_X_WON_STATE:
    evalVictoryLine(game, PLAYER_X);
    render_game_over_state(renderer, game, &PLAYER_X_COLOR);
    break;
  case PLAYER_O_WON_STATE:
    evalVictoryLine(game, PLAYER_O);
    render_game_over_state(renderer, game, &PLAYER_O_COLOR);
    break;
  case TIE_STATE:
    render_game_over_state(renderer, game, &TIE_COLOR);
    break;
  }
}
