#pragma once
#include "peice.hpp"

class Board{
public:
    int turn;
    int move;
    int winner;
    Peice peices[64];    
    std::vector<Move> moveHistory = std::vector<Move>();

    Board();

    void ResetBoard();
    void MakeMove(Move move);
    void UnmakeMove();
    
    Peice GetPeice(int x, int y);
    Peice FindKing(int team);

    int IsMate();
    int IsCheck();
    bool IsSafeMove(Move move);
    bool IsAttacked(Peice peice, int team);

    void GetLegalMoveCount(int* count);
    void GetBlockedPawnCount(int* count);
    void GetDoubledPawnCount(int* count);
    void GetIsolatedPawnCount(int* count);


    float EvaluateMove(Move move);
    float Evaluate();
    int GetPeiceCount();
};
