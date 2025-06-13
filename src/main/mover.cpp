#include "peice.hpp"

#ifndef MOVER
#define MOVER
std::vector<Peice> ProcessMove(Peice peices[64], Move move){
    std::vector<Peice> newBoard = std::vector<Peice>();
    for(int i = 0; i < 64; ++i){
        newBoard.push_back(peices[i]);
        newBoard[i].enpassant = false;
    }

    for(Peice peice : move.updatedPeices){
        newBoard[peice.position.x + peice.position.y*8] = peice;
    }
    
    return newBoard;
} 

Peice FindKing(Peice peices[64], int team){
    for(int i = 0; i < 64; ++i){
        Peice peice = peices[i];
        if(peice.team == team && peice.peice==5){
            return peice;
        }
    }

    return Peice();
}


bool SafeMove(Peice peices[64], Move move){
    Peice peice = move.updatedPeices[0];

    Peice tempBoard[64];
    for(int i = 0; i< 64; ++i){
        tempBoard[i] = peices[i];
    } 

    std::vector<Peice> processedMove = ProcessMove(tempBoard, move);

    for (int i = 0; i < 64; ++i){
        tempBoard[i] = processedMove[i];
    }

    Peice king = FindKing(tempBoard, peice.team);
    if(king.peice == -1){
        return true;
    }

    bool safe = true;
    
    for(int i = 0; i < 64; ++i){
        Peice peice1 = tempBoard[i];
        if(peice1.team == peice.team){
            continue;
        }

        std::vector<Move> moves = peice1.GenerateMoves(tempBoard);

        for(Move move1 : moves){
            for(Peice pMove : move1.updatedPeices){
                if(pMove.position.x == king.position.x && pMove.position.y == king.position.y){
                    safe = false;
                    break;
                }
            }
        }

        moves.clear();
    }

    return safe;
}


bool IsAttacked(Peice peices[64], Peice peice, int team){
    bool attacked = false;

    for(int i = 0; i < 64; ++i){
        Peice peice1 = peices[i];
        if(peice1.team != team || peice1.team == -1){
            continue;
        }

        std::vector<Move> moves = peice1.GenerateMoves(peices);

        for(Move move1 : moves){
            for(Peice pMove : move1.updatedPeices){
                if(pMove.position.x == peice.position.x && pMove.position.y == peice.position.y){
                    attacked = true;
                    break;
                }
            }
        }

        moves.clear();
    }

    return attacked;
}

bool IsMate(Peice peices[64], int team){
    Peice king = FindKing(peices, team);

    if(king.attacked[team]){
        for(int i = 0; i < 64; ++i){
            if (peices[i].team != team){
                continue;
            }

            std::vector<Move> moves = peices[i].GenerateMoves(peices);
            for (Move move : moves){
                if(move.updatedPeices[0].position.x == -1 || move.updatedPeices[1].position.y == -1){
                    continue;
                }
                if (SafeMove(peices, move)){
                    return false;
                }
            }
        }
    }else{
        return false;
    }

    return true;
}

#endif