#include <SDL.h>
#include <SDL_main.h>
#include <SDL_image.h>
#include <iostream>
#include "peice.cpp"
#include "vector2.hpp"
#include "move.hpp"
#include <algorithm>

static int WIDTH = 1024;
static int HEIGHT = 1024;
static int CELL_W = WIDTH / 8;
static int CELL_H = HEIGHT / 8;
static int TURN = 0;


std::vector<Peice*> ProcessMove(Peice* peices[64], Move* move){
    std::vector<Peice*> newBoard = std::vector<Peice*>();
    for(int i = 0; i < 64; ++i){
        newBoard.push_back(peices[i]);
    }

    for(Peice* peice : move->updatedPeices){
        newBoard[peice->position.x + peice->position.y*8] = peice;
    }
    
    move->free = false;

    return newBoard;
} 

Peice* FindKing(Peice* peices[64], int team){
    for(int i = 0; i < 64; ++i){
        Peice* peice = peices[i];
        if(peice->team == team && peice->peice==5){
            return peice;
        }
    }

    return nullptr;
}

void DrawPeice(SDL_Renderer* renderer, SDL_Texture* textures[12],Peice* peice){
    if (peice->team == -1 || peice->peice == -1){
        return;
    }
    
    SDL_Texture* texture = textures[peice->peice + (peice->team == 1 ? 6 : 0)];

    SDL_FRect src = {0, 0, (float)texture->w, (float)texture->h};
    SDL_FRect dst = {(float)peice->position.x*CELL_W, (float)peice->position.y*CELL_H, (float)CELL_W, (float)CELL_H};
    SDL_RenderTexture(renderer, texture, &src, &dst);
}

bool SafeMove(Peice* peices[64], Move* move){
    Peice* peice = move->updatedPeices[0];

    Peice* tempBoard[64];
    for(int i = 0; i< 64; ++i){
        tempBoard[i] = peices[i];
    } 

    std::vector<Peice*> processedMove = ProcessMove(tempBoard, move);
    move->free = true;

    for (int i = 0; i < 64; ++i){
        tempBoard[i] = processedMove[i];
    }

    processedMove.clear();

    Peice* king = FindKing(tempBoard, peice->team);

    if(king == nullptr){
        return true;
    }

    bool safe = true;
    
    for(int i = 0; i < 64; ++i){
        Peice* peice1 = tempBoard[i];
        if(peice1->team == peice->team){
            continue;
        }

        std::vector<Move*> moves = peice1->GenerateMoves(tempBoard);
        for(Move* move1 : moves){
            for(Peice* pMove : move1->updatedPeices){
                if(pMove->position.x == king->position.x && pMove->position.y == king->position.y){
                    safe = false;
                    break;
                }
            }
        }

        for(Move* move1: moves){
            for(int i = 0; i < move1->updatedPeices.size(); ++i){
               delete move1->updatedPeices[0];
               move1->updatedPeices.erase(move1->updatedPeices.begin());
            }
        }

        for(int i = 0; i < moves.size(); ++i){
            delete moves[0];
            moves.erase(moves.begin());
        }
    }

    return safe;
}

SDL_Texture* LoadPeiceTexture(SDL_Renderer* renderer, int team, int peice){
    SDL_Surface* surface;

    std::string peiceFile;

    if(team == 0){
        peiceFile = "w";
    }else if (team==1){
        peiceFile = "b";
    }

    if(peice == 0){
        peiceFile += "p";
    }
    if(peice == 1){
        peiceFile += "n";
    }
    if(peice == 2){
        peiceFile += "b";
    }
    if(peice == 3){
        peiceFile += "r";
    }
    if(peice == 4){
        peiceFile += "q";
    }
    if(peice == 5){
        peiceFile += "k";
    }

    std::string finalText = "./textures/" + peiceFile + ".png";

    surface = IMG_Load(finalText.c_str());

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    return texture;
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

    SDL_SetWindowResizable(window, true);

    bool running = true;

    Vector2 mouse;
    Vector2 select = {-1,-1};

    SDL_Color boardPrimary = {235,236,208,255};
    SDL_Color boardSecondary = {119,149,86,255};
    SDL_FRect boardSquare = {0.f, 0.f, (float)CELL_W, (float)CELL_H};

    SDL_Texture* dot = SDL_CreateTextureFromSurface(renderer, IMG_Load("./textures/dot.png"));
    SDL_Texture* circle = SDL_CreateTextureFromSurface(renderer, IMG_Load("./textures/circle.png"));
    SDL_Texture* peiceTextures[12];

    peiceTextures[0] = LoadPeiceTexture(renderer, 0, 0);
    peiceTextures[1] = LoadPeiceTexture(renderer, 0, 1);
    peiceTextures[2] = LoadPeiceTexture(renderer, 0, 2);
    peiceTextures[3] = LoadPeiceTexture(renderer, 0, 3);
    peiceTextures[4] = LoadPeiceTexture(renderer, 0, 4);
    peiceTextures[5] = LoadPeiceTexture(renderer, 0, 5);
    peiceTextures[6] = LoadPeiceTexture(renderer, 1, 0);
    peiceTextures[7] = LoadPeiceTexture(renderer, 1, 1);
    peiceTextures[8] = LoadPeiceTexture(renderer, 1, 2);
    peiceTextures[9] = LoadPeiceTexture(renderer, 1, 3);
    peiceTextures[10] = LoadPeiceTexture(renderer, 1, 4);
    peiceTextures[11] = LoadPeiceTexture(renderer, 1, 5);

    Peice* selectedPeice;
    std::vector<Move*> moves;
    Peice* peices[64] = {};

    peices[0] = new Peice( 0, 0, 1, 3);
    peices[7] = new Peice( 7, 0, 1, 3);
    peices[1] = new Peice( 1, 0, 1, 1);
    peices[6] = new Peice( 6, 0, 1, 1);
    peices[2] = new Peice( 2, 0, 1, 2);
    peices[5] = new Peice( 5, 0, 1, 2);
    peices[3] = new Peice( 3, 0, 1, 4);
    peices[4] = new Peice( 4, 0, 1, 5);

    peices[0+(7*8)] = new Peice( 0, 7, 0, 3);
    peices[7+(7*8)] = new Peice( 7, 7, 0, 3);
    peices[1+(7*8)] = new Peice( 1, 7, 0, 1);
    peices[6+(7*8)] = new Peice( 6, 7, 0, 1);
    peices[2+(7*8)] = new Peice( 2, 7, 0, 2);
    peices[5+(7*8)] = new Peice( 5, 7, 0, 2);
    peices[3+(7*8)] = new Peice( 3, 7, 0, 4);
    peices[4+(7*8)] = new Peice( 4, 7, 0, 5);

    for(int x = 0; x < 8; ++x){
        peices[x+8] = new Peice( x, 1, 1, 0);
    }

    for(int x = 0; x < 8; ++x){
        peices[x+(6*8)] = new Peice( x, 6, 0, 0);
    }

    for(int y = 0; y < 4; ++y){
        for(int x = 0; x < 8; ++x){
            peices[x+((y+2)*8)] = new Peice( x, y, -1, -1);
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
                if (event.button.button == 3){
                    select = {-1,-1};
                }                

                if (event.button.button == 1) {
                    int x = (int)(mouse.x / CELL_W);
                    int y = (int)(mouse.y / CELL_H);
                    
                    if (select.x != -1 || select.y != -1){
                        for(Move* move : moves){
                            if(move->updatedPeices[0]->position.x == x && move->safe){
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

            if (event.type == SDL_EVENT_WINDOW_RESIZED){
                SDL_GetWindowSize(window, &WIDTH, &HEIGHT);
                CELL_W = WIDTH / 8;
                CELL_H = HEIGHT / 8;
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
                boardSquare.x = x * CELL_W;
                boardSquare.y = y * CELL_H;
                boardSquare.w = CELL_W;
                boardSquare.h = CELL_H;
                SDL_RenderFillRect(renderer, &boardSquare);
                Peice* peice = peices[x+(y*8)];
                
                if(select.x != x || select.y != y){
                   // peice->Draw(CELL_SIZE);
                    DrawPeice(renderer, peiceTextures, peice);

                }else{
                    selectedPeice = peice;
                }
            }
        }


        for(Move* move : moves){
            if(move->free == false){
                continue;
            }

            int beginSize = move->updatedPeices.size()-1;
            for(int i = 0; i < beginSize; ++i){
                delete move->updatedPeices[0];
            }
        }

        for(int i = 0; i < moves.size(); ++ i){
            delete moves[i];
        }


        if(selectedPeice){
            moves = selectedPeice->GenerateMoves(peices);
        }else{
            moves = std::vector<Move*>();
        }

        for(Move* move : moves){
            Vector2 movePos = move->updatedPeices[0]->position;

            if(!SafeMove(peices, move)){
                move->safe = false;
                continue;
            }

            if(peices[(int)(movePos.x + movePos.y*8)]->peice != -1){
                SDL_FRect src = {0, 0, (float)circle->w, (float)circle->h};
                SDL_FRect dst = {move->updatedPeices[0]->position.x*CELL_W, move->updatedPeices[0]->position.y*CELL_H, (float)CELL_W, (float)CELL_H};
                SDL_RenderTexture(renderer, circle, &src, &dst);
            }else{
                SDL_FRect src = {0, 0, (float)dot->w, (float)dot->h};
                SDL_FRect dst = {move->updatedPeices[0]->position.x*CELL_W, move->updatedPeices[0]->position.y*CELL_H, (float)CELL_W, (float)CELL_H};
                SDL_RenderTexture(renderer, dot, &src, &dst);
            }
       
        }

        if(selectedPeice){
            SDL_Texture* texture = peiceTextures[selectedPeice->peice + (selectedPeice->team == 1 ? 6 : 0)];

            SDL_FRect src = {0, 0, (float)texture->w, (float)texture->h};
            SDL_FRect dst = {mouse.x - CELL_W/2, mouse.y - CELL_H/2, (float)CELL_W, (float)CELL_H};
            SDL_RenderTexture(renderer, texture, &src, &dst);
        }

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}
