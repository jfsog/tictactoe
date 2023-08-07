#include "game.h"
#include <string.h>

void limpaTabuleiro(int *b) { memset(b, 0, sizeof(int) * (N * N)); }
int checkWins(const int *board, int player);
bool checkTie(const int *board) {
  for (int i = 0; i < N * N; i++)
    if (board[i] == EMPTY)
      return false;
  return true;
}
void game_over_condition(game_t *game) {
  if (checkWins(game->board, PLAYER_X)) {
    game->state = PLAYER_X_WON_STATE;
  } else if (checkWins(game->board, PLAYER_O)) {
    game->state = PLAYER_O_WON_STATE;
  } else if (checkTie(game->board)) {
    game->state = TIE_STATE;
  }
}
void playerTurn(game_t *game, int linha, int coluna) {
  if (game->board[N * linha + coluna] == EMPTY) {
    game->board[N * linha + coluna] = game->player;
    game->turn = !game->turn;
  }
}
void resetGame(game_t *game) {
  game->gamesPlayed++;
  if (game->gamesPlayed >= TOTALPARTIDAS) {
    game->state = QUIT_STATE;
    return;
  }
  game->state = RUNNING_STATE;
  game->whoStarts = !game->whoStarts;
  game->turn = game->whoStarts;
  limpaTabuleiro(game->board);
  game->winningPos = 0;
}
