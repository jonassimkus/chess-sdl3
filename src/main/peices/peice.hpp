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
    SDL_Texture* texture;

    Peice(SDL_Renderer* renderer, int x, int y, int team, int type);
    
    void Draw(int cellSize);

    bool IsOpponent(Peice* other);
    std::vector<Move*> GenerateMoves(Peice* board[64]);

private:
    SDL_Renderer* renderer;
};