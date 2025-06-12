#pragma once
#include <SDL.h>
#include "vector2.hpp"
#include <vector>
#include "move.hpp"

class Peice{
public:
    // -1 no team, 0 white, 1 black
    int team;
    Vector2 position;
    // Id rather have peice int than do loads of dynamic_casts if I would like to know the type of peice
    // -1 no peice, 0 pawn, 1 knight, 2 bishop, 3 rook, 4 queen, 5 king
    int peice; 
    bool enpassant;
    bool first;
    // Attacked [0] is white attacked[1] is black
    bool attacked[2];

    Peice();
    Peice(int x, int y, int team, int type);
    Peice(int x, int y, int team, int type, bool first);
    ~Peice();
    
    bool IsOpponent(Peice other);
    int GetOtherTeam();

    std::vector<Move> GenerateMoves(Peice board[64]);

};