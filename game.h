#ifndef GAME_H_
#define GAME_H_
#include <stdbool.h>
/*
 * N define as dimensões do tabuleiro do jogo
 * Fique a vontade para alterar esse valor, mas o algoritimo não lida muito bem
 * com valores muito acima de 3.
 * O limite máximo para N é 11, pois o algoritimo converte o tabuleiro em uma
 * representação utilizando um inteiro de 128 bits.
 * logo 11 é o maior valor para N^2 <= 128
 * */
#define N 3
#define DEFGAME 3
#define SCREEN_WIDTH 640.0
#define SCREEN_HEIGHT 480.0
#define CELL_WIDTH (SCREEN_WIDTH / N)
#define CELL_HEIGHT (SCREEN_HEIGHT / N)

#define EMPTY 0
#define PLAYER_X 1
#define PLAYER_O 2

#define RUNNING_STATE 0
#define PLAYER_X_WON_STATE 1
#define PLAYER_O_WON_STATE 2
#define TIE_STATE 3
#define QUIT_STATE 4

#define HWI_POINTS 100000
#define WIN_POINTS 1000000

// total de partidas que podem ser jogadas
// jogador e cpu jogam de forma alternada
#define TOTALPARTIDAS 10
typedef struct {
  int board[N * N];
  int player;
  int state;
  bool whoStarts;
  bool turn;
  int gamesPlayed;
} game_t;
typedef struct tabNode {
  int tab[N * N];
  struct tabNode *son;
  struct tabNode *next;
  struct tabNode *father;
  int value;
  bool turn;
  bool visitado;
} tabNode;

typedef __uint128_t Uint;
static int PointsToWin = N > DEFGAME ? DEFGAME + (N - DEFGAME) / 2 : N;
#endif
