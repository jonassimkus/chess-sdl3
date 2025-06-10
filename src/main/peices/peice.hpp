#pragma once
#include <SDL.h>

class Peice{
public:
    // -1 no team, 0 white, 1 black
    int team;
    int x;
    int y;
    // Id rather have peice int than do loads of dynamic_casts if I would like to know the type of peice
    // -1 no peice, 0 pawn, 1 knight, 2 bishop, 3 rook, 4 queen, 5 queen
    int peice; 
    SDL_Texture* texture;

    Peice(SDL_Renderer* renderer, int x, int y, int team, int type);
    
    virtual void Draw(SDL_Renderer*, int cellSize);
};