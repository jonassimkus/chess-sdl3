#include <SDL.h>
#include <SDL_main.h>
#include <SDL_image.h>
#include <iostream>
#include "peices/peice.cpp"

static int WIDTH = 720;
static int HEIGHT = 720;
static int CELL_SIZE = WIDTH / 8;
static int TURN = 0;

int main(){

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Chess - Jonas", WIDTH, HEIGHT, SDL_WINDOW_VULKAN);

    if (window == nullptr){
        std::cout << "SDL3 - Window failed to initialize" << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (renderer == nullptr){
        std::cout << "SDL3 - renderer failed to initialize" << std::endl;
        return 1;
    }

    bool running = true;

    SDL_Color boardPrimary = {235,236,208,255};
    SDL_Color boardSecondary = {119,149,86,255};
    SDL_FRect boardSquare = {0.f, 0.f, (float)CELL_SIZE, (float)CELL_SIZE};

    Peice* peices[64] = {};

    for (int y = 0; y < 8; ++ y){
        for(int x = 0; x < 8; ++ x){
            Peice* peice = new Peice(renderer, x, y, -1, 0);
            peices[(y*8)+x] = peice;
        }
    }



    while (running){
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255);
        SDL_RenderClear(renderer);

        for (int y = 0; y < 8; ++y){
            for (int x = 0; x < 8; ++x){
                if((x+(y^2)) % 2 == 0){
                    SDL_SetRenderDrawColor(renderer, boardSecondary.r, boardSecondary.g, boardSecondary.b, 255);
                }else{
                    SDL_SetRenderDrawColor(renderer, boardPrimary.r, boardPrimary.g, boardPrimary.b, 255);
                }
                boardSquare.x = x * CELL_SIZE;
                boardSquare.y = y * CELL_SIZE;
                SDL_RenderFillRect(renderer, &boardSquare);
                
                peices[x+(y*8)]->Draw(renderer, CELL_SIZE);
            }
        }

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}