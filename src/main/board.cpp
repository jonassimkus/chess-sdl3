#include "board.hpp"
#include <iostream>

Board::Board(){
    ResetBoard();
}

void Board::ResetBoard(){
    winner = -1; 
    turn = 0;
    moveHistory = std::vector<Move>();

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

void Board::MakeMove(Move move){
    for(Peice peice : move.updatedPeices){
        Peice oldPeice =GetPeice(peice.position.x, peice.position.y);
        oldPeice.position.x = peice.position.x;
        oldPeice.position.y = peice.position.y;
        move.preupdatedPeices.push_back(oldPeice);
    }

    for(int i = 0; i < 64; ++i){
        peices[i].enpassant = false;
    }

    for(Peice peice : move.updatedPeices){
        peices[(int)(peice.position.x + peice.position.y*8)] = peice;
    }

    moveHistory.push_back(move);
    this->move += 1;

    if(turn == 0){
        turn = 1;
    }else{
        turn = 0;
    }
}

void Board::UnmakeMove(){
    if(moveHistory.size() == 0){
        return;
    }

    Move move = moveHistory[moveHistory.size()-1];
    moveHistory.resize(moveHistory.size()-1);

    this->move -=1;
    
    if(turn == 0){
        turn = 1;
    }else{
        turn = 0;
    }

    for(Peice peice : move.preupdatedPeices){
        peices[(int)(peice.position.x+peice.position.y*8)] = peice;
    }
}


bool Board::IsAttacked(Peice peice, int team){
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

Peice Board::GetPeice(int x, int y){
    return peices[x+y*8];
}

int Board::GetPeiceCount(){
    int whites, blacks;

    for(int i = 0; i < 64; ++i){
        if(peices[i].team == 0){
            whites += 1;
        }

        if(peices[i].team == 1){
            blacks += 1;
        }
    }

    return whites + blacks;
}



int Board::IsMate(){
    bool white = true;
    bool black = true;

    Peice king = FindKing(0);

    king.attacked[0] = IsAttacked(king, 1);
    king.attacked[1] = IsAttacked(king, 0);

    if(king.attacked[0]){
        for(int i = 0; i < 64; ++i){
            if (peices[i].team != 0){
                continue;
            }

            std::vector<Move> moves = peices[i].GenerateMoves(peices);
            for (Move move : moves){
                if(move.updatedPeices[0].position.x == -1 || move.updatedPeices[1].position.y == -1){
                    continue;
                }
                if (IsSafeMove(move)){
                    white = false;
                    break;
                }
            }
        }
    }else{
        white = false;
    }

    king = FindKing(1);

    king.attacked[0] = IsAttacked(king, 1);
    king.attacked[1] = IsAttacked(king, 0);

    if(king.attacked[1]){
        for(int i = 0; i < 64; ++i){
            if (peices[i].team != 1){
                continue;
            }

            std::vector<Move> moves = peices[i].GenerateMoves(peices);
            for (Move move : moves){
                if(move.updatedPeices[0].position.x == -1 || move.updatedPeices[1].position.y == -1){
                    continue;
                }
                if (IsSafeMove(move)){
                    black = false;
                    break;
                }
            }
        }
    }else{
        black = false;
    }

    if(white){
        return 0;
    }

    if(black){
        return 1;
    }

    return -1;
}

int Board::IsCheck(){
    Peice king = FindKing(0);

    king.attacked[0] = IsAttacked(king, 1);
    king.attacked[1] = IsAttacked(king, 0);

    if(king.attacked[0]){
        return 0;
    }
    
    king = FindKing(1);

    king.attacked[0] = IsAttacked(king, 1);
    king.attacked[1] = IsAttacked(king, 0);

    if(king.attacked[1]){
        return 1;
    }

    return -1;
}


Peice Board::FindKing(int team){
    for(int i = 0; i < 64; ++i){
        Peice peice = peices[i];
        if(peice.team == team && peice.peice==5){
            return peice;
        }
    }

    return Peice();
}

bool Board::IsSafeMove(Move move){
    Peice peice = move.updatedPeices[0];

    Board tempBoard = *this;

    tempBoard.MakeMove(move);

    Peice king = FindKing(peice.team);
    if(king.peice == -1){
        return true;
    }


    if(tempBoard.IsCheck() == king.team){
        return false;
    }

    return true;
}

void Board::GetLegalMoveCount(int *moveCount){
    for (int i = 0; i < 64; ++i){
        if(peices[i].team == -1){
            continue;
        }

        std::vector<Move> moves = peices[i].GenerateMoves(peices);
        for(Move move : moves){
            if(IsSafeMove(move)){
                moveCount[peices[i].team] += 1;
            }
        }
    }
}

void Board::GetBlockedPawnCount(int *blockCount){
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

void Board::GetDoubledPawnCount(int *doubleCount){
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

void Board::GetIsolatedPawnCount(int *isolatedCount){
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


float Board::EvaluateMove(Move move){
    Board tempBoard = *this;
    
    tempBoard.MakeMove(move);
    float processed = tempBoard.Evaluate();

    return (processed);
}

float Board::Evaluate(){
    int peiceTypes[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int moveCount[2]={0,0};
    int blocked[2]={0,0};
    int isolated[2]={0,0};
    int doubled[2]={0,0};

    GetLegalMoveCount(moveCount);
    GetBlockedPawnCount(blocked);
    GetDoubledPawnCount(doubled);
    GetIsolatedPawnCount(isolated);

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
