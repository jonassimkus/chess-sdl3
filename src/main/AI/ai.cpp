#include <iostream>
#include "peice.hpp"
#include "vector2.hpp"
#include "mover.cpp"
#include <vector>

void GetLegalMoveCount(Peice peices[64], int *moveCount){
    for (int i = 0; i < 64; ++i){
        if(peices[i].team == -1){
            continue;
        }

        std::vector<Move> moves = peices[i].GenerateMoves(peices);
        for(Move move : moves){
            if(SafeMove(peices, move)){
                moveCount[peices[i].team] += 1;
            }
        }
    }
}

void GetBlockedPawnCount(Peice peices[64], int *blockCount){
    for(int i = 0; i < 64; ++i){
        if(peices[i].peice != 0){
            continue;
        }

        int direction = (peices[i].team == 0 ? -1 : +1);
        if(peices[i].position.y + direction < 0 || peices[i].position.y + direction > 7){
            continue;
        }

        if(peices[(int)(peices[i].position.x + ((peices[i].position.y+direction)*8))].peice != -1){
            blockCount[peices[i].team] += 1;
        }
    }
}

void GetDoubledPawnCount(Peice peices[64], int *doubleCount){
    for(int x = 0; x < 8; ++x){
        int pawnsB = 0;
        int pawnsW = 0;
        for(int y = 0; y < 8; ++y){
            bool finish = false;

            if(peices[x+y*8].peice == 0 && peices[x+y*8].team == 0){
                pawnsW += 1;
            }
            if(peices[x+y*8].peice == 0 && peices[x+y*8].team == 1){
                pawnsB += 1;
            }

            if(pawnsB >= 2){
                doubleCount[1] += 1;
                finish = true;
            }
            
            if(pawnsW >= 2){
                doubleCount[0] += 1;
                finish = true;
            }

            if(finish){
                break;
            }
        }
    }
}

void GetIsolatedPawnCount(Peice peices[64], int *isolatedCount){
    for(int x = 0; x < 8; ++x){
        for(int y = 0; y < 8; ++y){
            if(peices[x+y*8].peice != 0){
                continue;
            }

            bool isolated = true;
            
            for(int y1 = 0; y1 < 8; ++y1){
                if(x-1 >= 0){
                    if(peices[x-1+y1*8].peice == 0 && peices[x-1+y1*8].team == peices[x+y*8].team){
                        isolated = false;
                    }
                }

                if(x+1 <= 7){
                    if(peices[x+1+y1*8].peice == 0 && peices[x+1+y1*8].team == peices[x+y*8].team){
                        isolated = false;
                    }
                }
            }

            if(isolated == false){
                continue;
            }else{
                isolatedCount[peices[x+y*8].team] += 1;
                break;
            }
        }
    }
}


float Evaluate(Peice peices[64]){
    int peiceTypes[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int moveCount[2]={0,0};
    int blocked[2]={0,0};
    int isolated[2]={0,0};
    int doubled[2]={0,0};

    GetLegalMoveCount(peices, moveCount);
    GetBlockedPawnCount(peices, blocked);
    GetDoubledPawnCount(peices, doubled);
    GetIsolatedPawnCount(peices, isolated);

    for (int i = 0; i < 64;++i){
        if(peices[i].peice == -1){
            continue;
        }
        peiceTypes[peices[i].peice + (peices[i].team * 6)] += 1;
    }

    float materialScore = 200*(peiceTypes[5] - peiceTypes[11]) 
    + 9*(peiceTypes[4] - peiceTypes[10])
    + 5*(peiceTypes[3] - peiceTypes[9])
    + 3*(peiceTypes[2] - peiceTypes[8] + peiceTypes[1] - peiceTypes[7])
    + 1*(peiceTypes[0] - peiceTypes[6])
    - 0.5*(blocked[0] - blocked[1] + doubled[0] - doubled[1] + isolated[0] - isolated[1])
    + 0.1*(moveCount[0] - moveCount[1]);

    return materialScore;
}

float EvaluateMove(Peice peices[64], Move move){
    float original = Evaluate(peices);

    Peice tempBoard[64];
    for(int i = 0; i< 64; ++i){
        tempBoard[i] = peices[i];
    } 

    std::vector<Peice> peicesVec = ProcessMove(tempBoard, move);

    for(int i = 0; i < 64; ++i){
        tempBoard[i] = peicesVec[i];
    }

    float processed = Evaluate(tempBoard);

    return (processed);
}

/*
int maxi( int depth ) {
    if ( depth == 0 ) return Evaluate();
    int max = -oo;
    for ( all moves) {
        score = mini( depth - 1 );
        if( score > max )
            max = score;
    }
    return max;
}

int mini( int depth ) {
    if ( depth == 0 ) return -Evaluate();
    int min = +oo;
    for ( all moves) {
        score = maxi( depth - 1 );
        if( score < min )
            min = score;
    }
    return min;
}*/

Move GetBestMove(Peice peices[64], int team){
    Move best = Move();

    return best;
}