#include <SDL.h>
#include <SDL_main.h>
#include <SDL_image.h>
#include <iostream>
#include "peice.cpp"
#include "vector2.hpp"
#include "move.hpp"
#include "audio.hpp"
#include "board.cpp"

static int WIDTH = 720;
static int HEIGHT = 720;
static int CELL_W = WIDTH / 8;
static int CELL_H = HEIGHT / 8;
static bool AI = true;

void DrawPeice(SDL_Renderer* renderer, SDL_Texture* textures[12],Peice peice){
    if (peice.team == -1 || peice.peice == -1){
        return;
    }
    
    SDL_Texture* texture = textures[peice.peice + (peice.team == 1 ? 6 : 0)];

    SDL_FRect src = {0, 0, (float)texture->w, (float)texture->h};
    SDL_FRect dst = {(float)peice.position.x*CELL_W, (float)peice.position.y*CELL_H, (float)CELL_W, (float)CELL_H};
    SDL_RenderTexture(renderer, texture, &src, &dst);
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

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_SetAppMetadata("Chess - SDL3", "1.0", "me.jonas.chesssdl3");

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

    SDL_AudioStream *stream = NULL;

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL, NULL, NULL);
    if (!stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return 1;
    }

    SDL_ResumeAudioStreamDevice(stream);

    Sound* gameStartSound = new Sound(stream, "./sounds/game-start.wav", false, false);
    Sound* gameEndSound = new Sound(stream, "./sounds/game-end.wav", true, false);
    Sound* checkSound = new Sound(stream, "./sounds/move-check.wav", true, false);
    Sound* moveSound = new Sound(stream, "./sounds/move.wav", true, false);
    Sound* takeSound = new Sound(stream, "./sounds/take.wav", true, false);

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

    Board board;

    SDL_Texture* resetText = SDL_CreateTextureFromSurface(renderer, IMG_Load("./textures/reset.png"));
    SDL_FRect resetRect = {WIDTH/2 - CELL_W*1.5f, HEIGHT/2 - CELL_H/2.f, CELL_W*3.f, (float)CELL_H};
    bool showResetText = false;

    SDL_Texture* boardText = SDL_CreateTextureFromSurface(renderer, IMG_Load("./textures/boardnumbers.png"));
    SDL_FRect boardRect = {0,0, (float)WIDTH, (float)HEIGHT};

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
                    if(board.winner == -1){
                        showResetText = false;
                    }
                    
                    int x = (int)(mouse.x / CELL_W);
                    int y = (int)(mouse.y / CELL_H);
                    
                    if (select.x != -1 || select.y != -1){
                        for(Move move : moves){
                            if(move.updatedPeices[0].position.x == x && move.safe){
                                if(move.updatedPeices[0].position.y == y){
                                    int oldCount = board.GetPeiceCount();

                                    board.MakeMove(move);

                                    int newCount = board.GetPeiceCount();

                                    if(board.IsMate() != -1){
                                        continue;
                                    }

                                    if(board.IsCheck() != -1){
                                        checkSound->paused = false;
                                        continue;
                                    }

                                    if(oldCount > newCount){
                                        takeSound->paused = false;
                                    }else{
                                        moveSound->paused = false;
                                    }
                                }
                            }
                        }

                        select.x = -1;
                        select.y = -1;
                    }
                    else{
                        if(board.turn == board.GetPeice(x,y).team && board.winner == -1){
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

                resetRect = {WIDTH/2 - CELL_W*1.5f, HEIGHT/2 - CELL_H/2.f, CELL_W*3.f, (float)CELL_H};
                boardRect = {0.f,0.f, (float)WIDTH, (float)HEIGHT};
            }

            if(event.type == SDL_EVENT_KEY_DOWN){
                if(event.key.scancode == SDL_SCANCODE_R){
                    board.ResetBoard();
                    showResetText = false;
                    gameStartSound->paused = false;
                }

                if(event.key.scancode == SDL_SCANCODE_Q){
                    select = {-1,-1};
                    board.UnmakeMove();
                }
            }
        }

        gameStartSound->Play();
        gameEndSound->Play();
        moveSound->Play();
        takeSound->Play();
        checkSound->Play();

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
                Peice peice = board.GetPeice(x, y);
                
                if(select.x != x || select.y != y){
                    DrawPeice(renderer, peiceTextures, peice);

                }else{
                    selectedPeice = peice;
                }
            }
        }

        if(selectedPeice.peice != -1){
           moves = selectedPeice.GenerateMoves(board.peices);
        }else{
            moves.clear();
        }

        for(int i = 0; i < 64; ++i){
            board.peices[i].attacked[0] = board.IsAttacked(board.peices[i], 1);
            board.peices[i].attacked[1] = board.IsAttacked(board.peices[i], 0);
        }

        if(board.IsMate() != -1 && board.winner == -1){
            board.winner = 1;
            gameEndSound->paused = false;
            std::cout<<"Black has won!" <<std::endl;
            showResetText = true;
        }

        if(board.IsMate() != -1 && board.winner == -1){
            board.winner = 0;
            gameEndSound->paused = false;
            std::cout<<"White has won!" <<std::endl;
            showResetText = true;
        }

        for(int i = 0; i < moves.size(); ++i){
            Vector2 movePos = moves[i].updatedPeices[0].position;

            if(!board.IsSafeMove(moves[i])){
                moves[i].safe = false;
                continue;
            }

            if(board.GetPeice(movePos.x, movePos.y).peice != -1){
                SDL_FRect dst = {moves[i].updatedPeices[0].position.x*CELL_W, moves[i].updatedPeices[0].position.y*CELL_H, (float)CELL_W, (float)CELL_H};
                SDL_RenderTexture(renderer, circle, NULL, &dst);
            }else{
                SDL_FRect dst = {moves[i].updatedPeices[0].position.x*CELL_W, moves[i].updatedPeices[0].position.y*CELL_H, (float)CELL_W, (float)CELL_H};
                SDL_RenderTexture(renderer, dot, NULL, &dst);
            }
        }

        if(selectedPeice.peice != -1){
            SDL_Texture* texture = peiceTextures[selectedPeice.peice + (selectedPeice.team == 1 ? 6 : 0)];

            SDL_FRect dst = {mouse.x - CELL_W/2, mouse.y - CELL_H/2, (float)CELL_W, (float)CELL_H};
            SDL_RenderTexture(renderer, texture, NULL, &dst);
        }

        if(showResetText){
            SDL_RenderTexture(renderer, resetText, NULL, &resetRect);
        }

        SDL_RenderTexture(renderer, boardText, NULL, &boardRect);

        SDL_RenderPresent(renderer);
    }


    SDL_DestroyTexture(resetText);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}
