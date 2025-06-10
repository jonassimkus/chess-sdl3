#include <SDL.h>
#include <SDL_main.h>
#include <SDL_image.h>
#include <iostream>
#include "peice.cpp"
#include "vector2.hpp"
#include "move.hpp"

static int WIDTH = 720;
static int HEIGHT = 720;
static int CELL_SIZE = WIDTH / 8;
static int TURN = 0;


std::vector<Peice*> ProcessMove(Peice* peices[64], Move* move){
    std::vector<Peice*> newBoard = std::vector<Peice*>();
    for(int i = 0; i < 64; ++i){
        newBoard.push_back(peices[i]);
    }

    for(Peice* peice : move->updatedPeices){
        newBoard[peice->position.x + peice->position.y*8] = peice;
    }

    return newBoard;
} 

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

    Vector2 mouse;
    Vector2 select = {-1,-1};

    SDL_Color boardPrimary = {235,236,208,255};
    SDL_Color boardSecondary = {119,149,86,255};
    SDL_FRect boardSquare = {0.f, 0.f, (float)CELL_SIZE, (float)CELL_SIZE};

    SDL_Texture* dot = SDL_CreateTextureFromSurface(renderer, IMG_Load("./textures/dot.png"));
    SDL_Texture* circle = SDL_CreateTextureFromSurface(renderer, IMG_Load("./textures/circle.png"));

    Peice* selectedPeice;
    std::vector<Move*> moves;
    Peice* peices[64] = {};

    peices[0] = new Peice(renderer, 0, 0, 1, 3);
    peices[7] = new Peice(renderer, 7, 0, 1, 3);
    peices[1] = new Peice(renderer, 1, 0, 1, 1);
    peices[6] = new Peice(renderer, 6, 0, 1, 1);
    peices[2] = new Peice(renderer, 2, 0, 1, 2);
    peices[5] = new Peice(renderer, 5, 0, 1, 2);
    peices[3] = new Peice(renderer, 3, 0, 1, 4);
    peices[4] = new Peice(renderer, 4, 0, 1, 5);

    peices[0+(7*8)] = new Peice(renderer, 0, 7, 0, 3);
    peices[7+(7*8)] = new Peice(renderer, 7, 7, 0, 3);
    peices[1+(7*8)] = new Peice(renderer, 1, 7, 0, 1);
    peices[6+(7*8)] = new Peice(renderer, 6, 7, 0, 1);
    peices[2+(7*8)] = new Peice(renderer, 2, 7, 0, 2);
    peices[5+(7*8)] = new Peice(renderer, 5, 7, 0, 2);
    peices[3+(7*8)] = new Peice(renderer, 3, 7, 0, 4);
    peices[4+(7*8)] = new Peice(renderer, 4, 7, 0, 5);

    for(int x = 0; x < 8; ++x){
        peices[x+8] = new Peice(renderer, x, 1, 1, 0);
    }

    for(int x = 0; x < 8; ++x){
        peices[x+(6*8)] = new Peice(renderer, x, 6, 0, 0);
    }

    for(int y = 0; y < 4; ++y){
        for(int x = 0; x < 8; ++x){
            peices[x+((y+2)*8)] = new Peice(renderer, x, y, -1, -1);
        }
    }

    while (running){
        selectedPeice = nullptr;

		SDL_GetMouseState(&mouse.x, &mouse.y);

        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_EVENT_QUIT){
                running = false;
            }

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == 1) {
                    int x = (int)(mouse.x / CELL_SIZE);
                    int y = (int)(mouse.y / CELL_SIZE);
                    
                    if (select.x != -1 || select.y != -1){
                        for(Move* move : moves){
                            if(move->updatedPeices[0]->position.x == x){
                                if(move->updatedPeices[0]->position.y == y){
                                    std::vector<Peice*> processedMove = ProcessMove(peices, move);
                                    for (int i = 0; i < 64; ++i){
                                        peices[i] = processedMove[i];
                                    }
                                }
                            }
                        }

                        select.x = -1;
                        select.y = -1;
                    }
                    else{
                        if(peices[x+y*8]->peice != -1){
                            select.x = x;
                            select.y = y;
                        }
                    }
                }
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
                Peice* peice = peices[x+(y*8)];
                
                if(select.x != x || select.y != y){
                    peice->Draw(CELL_SIZE);
                }else{
                    selectedPeice = peice;
                }
            }
        }


        if(selectedPeice){
            moves = selectedPeice->GenerateMoves(peices);
        }else{
            moves = std::vector<Move*>();
        }

        for(Move* move : moves){
            Vector2 movePos = move->updatedPeices[0]->position;

            if(peices[(int)(movePos.x + movePos.y*8)]->peice != -1){
                SDL_FRect src = {0, 0, (float)circle->w, (float)circle->h};
                SDL_FRect dst = {move->updatedPeices[0]->position.x*CELL_SIZE, move->updatedPeices[0]->position.y*CELL_SIZE, (float)CELL_SIZE, (float)CELL_SIZE};
                SDL_RenderTexture(renderer, circle, &src, &dst);
            }else{
                SDL_FRect src = {0, 0, (float)dot->w, (float)dot->h};
                SDL_FRect dst = {move->updatedPeices[0]->position.x*CELL_SIZE, move->updatedPeices[0]->position.y*CELL_SIZE, (float)CELL_SIZE, (float)CELL_SIZE};
                SDL_RenderTexture(renderer, dot, &src, &dst);
            }
       
        }

        if(selectedPeice){
            SDL_Texture* texture = selectedPeice->texture;
            SDL_FRect src = {0, 0, (float)texture->w, (float)texture->h};
            SDL_FRect dst = {mouse.x - CELL_SIZE/2, mouse.y - CELL_SIZE/2, (float)CELL_SIZE, (float)CELL_SIZE};
            SDL_RenderTexture(renderer, texture, &src, &dst);
        }

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}
