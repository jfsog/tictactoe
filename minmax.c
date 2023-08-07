#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int possibleMovsAmount = 0;
Uint *arrayPossibleMovs = NULL;
int possibleFinalMoves(void);
tabNode *creat_tab(void) {
  tabNode *novo = calloc(sizeof(tabNode), 1);
  novo->turn = false;
  return novo;
}
int evalGameState(const int *b, bool o_TURN);
tabNode *createNode(tabNode *tab1, short int index) {
  tabNode *novo = calloc(sizeof(tabNode), 1);
  memcpy(novo->tab, tab1->tab, sizeof(int) * (N * N));
  novo->turn = !tab1->turn;
  novo->tab[index] = novo->turn ? PLAYER_O : PLAYER_X;
  novo->value = evalGameState(novo->tab, novo->turn);
  novo->father = tab1;
  novo->next = tab1->son;
  tab1->son = novo;
  return novo;
}
static inline int setedBitCount(Uint l) {
  int c = 0;
  while (l > 0)
    c += (1 & l), l >>= 1;
  return c;
}
Uint tabToBit(const int *board, int player) {
  Uint b = 0;
  for (int i = 0; i < N * N; i++)
    b |= ((board[i] == player) << i);
  return b;
}
bool checkPlayerWin(const Uint board) {
  for (int i = 0; i < possibleMovsAmount; i++)
    if ((arrayPossibleMovs[i] & board) == arrayPossibleMovs[i])
      return true;
  return false;
}
int checkWins(const int *board, int player) {
  return checkPlayerWin(tabToBit(board, player));
}

bool isHardWin(Uint b1, Uint b2) {
  return (setedBitCount(b1) == PointsToWin - 1 &&
          setedBitCount(b2) == PointsToWin - 1 && setedBitCount(b1 & b2) == 1);
}
int evalAux(const int *board, bool o_TURN) {
  int p1 = o_TURN ? PLAYER_O : PLAYER_X;
  int p2 = o_TURN ? PLAYER_X : PLAYER_O;
  if (checkWins(board, p1))
    return WIN_POINTS;
  Uint p1arr[possibleMovsAmount];
  int ip1 = 0;
  int bp1 = tabToBit(board, p1);
  int bp2 = tabToBit(board, p2);
  for (int i = 0; i < possibleMovsAmount; i++)
    if ((arrayPossibleMovs[i] & bp2) == 0)
      p1arr[ip1++] = arrayPossibleMovs[i] & bp1;

  for (int i = 0; i < ip1 - 1; i++)
    for (int j = i + 1; j < ip1; j++)
      if (isHardWin(p1arr[i], p1arr[j]))
        return HWI_POINTS;
  return ip1;
}
int evalGameState(const int *b, bool o_TURN) {
  int p1 = evalAux(b, o_TURN);
  int p2 = evalAux(b, !o_TURN);
  if (p2 == WIN_POINTS)
    return WIN_POINTS;
  if (p1 == -WIN_POINTS)
    return WIN_POINTS;
  return p2 - p1;
}

void horizontal_Gen(int *indx) {
  Uint m = ((Uint) ~(0L & 0L)) >> (sizeof(Uint) * 8 - PointsToWin);
  for (int i = 0; i < N; i++) {
    for (int j = 0; j <= N - PointsToWin; j++) {
      Uint w = m << (i * N);
      Uint t = w << j;
      arrayPossibleMovs[(*indx)++] = t;
    }
  }
}
void vertical_Gen(int *indx) {
  Uint m = 0;
  for (int i = 0; i < PointsToWin; i++)
    m |= (1 << N * i);
  for (int i = 0; i <= N - PointsToWin; i++) {
    for (int j = 0; j < N; j++) {
      Uint w = m << j;
      Uint t = w << N * i;
      arrayPossibleMovs[(*indx)++] = t;
    }
  }
}
void diagonal_Gen(int *indx) {
  Uint m = 0, d = 0;
  for (int i = 0; i < PointsToWin; i++) {
    m |= (1 << (N * i + i));
    d |= (1 << (N * i + N - i - (N - PointsToWin + 1)));
  }
  for (int i = 0; i <= N - PointsToWin; i++) {
    for (int j = 0; j <= N - PointsToWin; j++) {
      Uint w = m << j;
      Uint t = w << N * i;
      Uint w1 = d << j;
      Uint t1 = w1 << N * i;
      arrayPossibleMovs[(*indx)++] = t;
      arrayPossibleMovs[(*indx)++] = t1;
    }
  }
}

int possibleFinalMoves(void) {
  int base = (N - (PointsToWin - 1));
  int hv = base * N * 2;
  int diag = base * base * 2;
  return diag + hv;
}
void generateMovements() {
  if (possibleMovsAmount == 0) {
    possibleMovsAmount = possibleFinalMoves();
    arrayPossibleMovs = calloc(sizeof(Uint), possibleMovsAmount);
    int idx = 0;
    horizontal_Gen(&idx), vertical_Gen(&idx), diagonal_Gen(&idx);
  }
}
void MinMaxAlgorithm(tabNode *pai, tabNode *atual, tabNode **best) {
  if (!pai->visitado) {
    pai->value = atual->value;
    pai->visitado = 1;
    if (!pai->father)
      *best = atual;
  } else if (pai->turn) {
    if (atual->value < pai->value) {
      pai->value = atual->value;
      if (!pai->father)
        *best = atual;
    }
  } else {
    if (atual->value > pai->value) {
      pai->value = atual->value;
      if (!pai->father)
        *best = atual;
    }
  }
}
int alfa_beta(tabNode *nodo) {
  tabNode *p = nodo->father;
  int status = 0;
  while (p != NULL) {
    if (p->visitado) {
      if (nodo->turn) {
        if (!p->turn && p->value > nodo->value)
          status++;
      } else {
        if (p->turn && p->value < nodo->value)
          status++;
      }
    }
    p = p->father;
  }
  return status;
}

void expandirArvore(tabNode *ptree, int p, int depth, tabNode **best) {
  if (p < depth) {
    for (int i = 0; i < N * N; i++) {
      if (ptree->tab[i] == EMPTY) {
        createNode(ptree, i);
        expandirArvore(ptree->son, p + 1, depth, best);
        MinMaxAlgorithm(ptree, ptree->son, best);
        if (alfa_beta(ptree))
          return;
      }
    }
  } else {
    ptree->visitado = true;
  }
}
tabNode *buildtree(int *brd, int profun, tabNode **best) {
  tabNode *ptree = creat_tab();
  memcpy(ptree->tab, brd, sizeof(int) * (N * N));
  expandirArvore(ptree, 0, profun, best);
  return ptree;
}
void freeTreeOfBoards(tabNode **node) {
  if (*node) {
    freeTreeOfBoards(&(*node)->son);
    freeTreeOfBoards(&(*node)->next);
    free(*node);
    *node = NULL;
  }
}
void nextmove(int *brd, int profun) {
  tabNode *ptree, *best = NULL;
  ptree = buildtree(brd, profun, &best);
  memcpy(brd, best->tab, sizeof(int) * (N * N));
  freeTreeOfBoards(&ptree);
}
