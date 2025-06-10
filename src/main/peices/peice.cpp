#include "peice.hpp"
#include <SDL_image.h>
    
Peice::Peice(SDL_Renderer* renderer, int x, int y, int team, int type){
    this->x = x;
    this->y = y;
    this->team = team;
    this->peice = type;

    if(peice == 0){
        SDL_Surface* surface = IMG_Load("././textures/wp.png");
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
    }
}

void Peice::Draw(SDL_Renderer* renderer, int cellSize){
    if (peice == -1){
        return;
    }

    SDL_FRect src = {0, 0, (float)texture->w, (float)texture->h};
    SDL_FRect dst = {(float)x*cellSize, (float)y*cellSize, (float)cellSize, (float)cellSize};
    SDL_RenderTexture(renderer, texture, &src, &dst);
}
