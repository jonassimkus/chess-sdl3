#include <SDL.h>
#include <SDL_main.h>
#include <SDL_image.h>
#include <iostream>
#include "peice.cpp"
#include "vector2.hpp"
#include "move.hpp"
#include "ai.cpp"
#include "mover.cpp"

static int WIDTH = 720;
static int HEIGHT = 720;
static int CELL_W = WIDTH / 8;
static int CELL_H = HEIGHT / 8;
static int TURN = 0;


void DrawPeice(SDL_Renderer* renderer, SDL_Texture* textures[12],Peice peice){
    if (peice.team == -1 || peice.peice == -1){
        return;
    }
    
    SDL_Texture* texture = textures[peice.peice + (peice.team == 1 ? 6 : 0)];

    SDL_FRect src = {0, 0, (float)texture->w, (float)texture->h};
    SDL_FRect dst = {(float)peice.position.x*CELL_W, (float)peice.position.y*CELL_H, (float)CELL_W, (float)CELL_H};
    SDL_RenderTexture(renderer, texture, &src, &dst);
}

void ResetBoard(Peice *peices){
    peices[0] = Peice( 0, 0, 1, 3,true);
    peices[7] = Peice( 7, 0, 1, 3,true);
    peices[1] = Peice( 1, 0, 1, 1,true);
    peices[6] = Peice( 6, 0, 1, 1,true);
    peices[2] = Peice( 2, 0, 1, 2,true);
    peices[5] = Peice( 5, 0, 1, 2,true);
    peices[3] = Peice( 3, 0, 1, 4,true);
    peices[4] = Peice( 4, 0, 1, 5,true);

    peices[0+(7*8)] = Peice( 0, 7, 0, 3,true);
    peices[7+(7*8)] = Peice( 7, 7, 0, 3,true);
    peices[1+(7*8)] = Peice( 1, 7, 0, 1,true);
    peices[6+(7*8)] = Peice( 6, 7, 0, 1,true);
    peices[2+(7*8)] = Peice( 2, 7, 0, 2,true);
    peices[5+(7*8)] = Peice( 5, 7, 0, 2,true);
    peices[3+(7*8)] = Peice( 3, 7, 0, 4,true);
    peices[4+(7*8)] = Peice( 4, 7, 0, 5,true);

    for(int x = 0; x < 8; ++x){
        peices[x+8] = Peice( x, 1, 1, 0 ,true);
        peices[x+(6*8)] = Peice( x, 6, 0, 0, true);
    }

    for(int y = 0; y < 4; ++y){
        for(int x = 0; x < 8; ++x){
            peices[x+((y+2)*8)] = Peice( x, y, -1, -1);
        }
    } 
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

    Peice selectedPeice;
    std::vector<Move> moves;
    Peice peices[64] = {};
    ResetBoard(peices);

    while (running){
        selectedPeice = Peice();


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
                        for(Move move : moves){
                            if(move.updatedPeices[0].position.x == x && move.safe){
                                if(move.updatedPeices[0].position.y == y){
                                    std::vector<Peice> processedMove = ProcessMove(peices, move);
                                    TURN += 1;
                                    for (int i = 0; i < 64; ++i){
                                        peices[i] = processedMove[i];
                                    }

                                    std::cout<< Evaluate(peices) << std::endl;
                                }
                            }
                        }

                        select.x = -1;
                        select.y = -1;
                    }
                    else{
                        if(peices[x+y*8].peice != -1 && peices[x+y*8].team == TURN%2){
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
                Peice peice = peices[x+(y*8)];
                
                if(select.x != x || select.y != y){
                    DrawPeice(renderer, peiceTextures, peice);

                }else{
                    selectedPeice = peice;
                }
            }
        }

        if(selectedPeice.peice != -1){
            moves = selectedPeice.GenerateMoves(peices);
        }else{
            moves.clear();
        }

        for(int i = 0; i < 64; ++i){
            peices[i].attacked[0] = IsAttacked(peices, peices[i], 1);
            peices[i].attacked[1] = IsAttacked(peices, peices[i], 0);
        }

        /*
        if(TURN%2 == 1){
            Move best = GetBestMove(peices, TURN%2);
            TURN += 1;

            if(best.updatedPeices.size() != 0){
                std::cout << TURN%2 << std::endl;
            }

            std::vector<Peice> peices1 = ProcessMove(peices, best);

            for (int i = 0; i < 64; ++i){
                peices[i] = peices1[i];
            }
        }
  */    

        if(IsMate(peices, 0)){
            ResetBoard(peices);
            std::cout<<"Black has won!" <<std::endl;
        }

        if(IsMate(peices, 1)){
            ResetBoard(peices);
            std::cout<<"White has won!" <<std::endl;
        }


        for(int i = 0; i < moves.size(); ++i){
            Vector2 movePos = moves[i].updatedPeices[0].position;

            if(!SafeMove(peices, moves[i])){
                moves[i].safe = false;
                continue;
            }

            if(peices[(int)(movePos.x + movePos.y*8)].peice != -1){
                SDL_FRect src = {0, 0, (float)circle->w, (float)circle->h};
                SDL_FRect dst = {moves[i].updatedPeices[0].position.x*CELL_W, moves[i].updatedPeices[0].position.y*CELL_H, (float)CELL_W, (float)CELL_H};
                SDL_RenderTexture(renderer, circle, &src, &dst);
            }else{
                SDL_FRect src = {0, 0, (float)dot->w, (float)dot->h};
                SDL_FRect dst = {moves[i].updatedPeices[0].position.x*CELL_W, moves[i].updatedPeices[0].position.y*CELL_H, (float)CELL_W, (float)CELL_H};
                SDL_RenderTexture(renderer, dot, &src, &dst);
            }
        }

        if(selectedPeice.peice != -1){
            SDL_Texture* texture = peiceTextures[selectedPeice.peice + (selectedPeice.team == 1 ? 6 : 0)];

            SDL_FRect src = {0, 0, (float)texture->w, (float)texture->h};
            SDL_FRect dst = {mouse.x - CELL_W/2, mouse.y - CELL_H/2, (float)CELL_W, (float)CELL_H};
            SDL_RenderTexture(renderer, texture, &src, &dst);
        }

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}
