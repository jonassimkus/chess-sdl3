#include "peice.hpp"
#include <SDL_image.h>
#include <string>
    
Peice::Peice(){
    peice = -1;
    team = -1;
}

Peice::Peice(int x, int y, int team, int type){
    this->position.x = x;
    this->position.y = y;
    this->team = team;
    this->peice = type;
}

Peice::~Peice(){
}

bool Peice::IsOpponent(Peice other){
    if(other.team == -1){
        return false;
    }

    if(other.team != team){
        return true;
    }

    return false;
}

int Peice::GetOtherTeam(){
    if(team == 1){
        return 0;
    }

    return 1;
}

std::vector<Move> Peice::GenerateMoves(Peice board[64]){
    std::vector<Move> moves = std::vector<Move>();
    std::vector<Peice> peices;
    
    if (peice == 0){
        int direction = (team == 0 ? -1 : +1);

        if(position.y + direction < 0 && position.y + direction > 7){
            return moves;
        }
    
        if(board[(int)(position.x+(position.y+direction)*8)].peice == -1){
            peices = std::vector<Peice>();

            Move move;
            peices.push_back(Peice( position.x, position.y + direction, team, 0));
            peices.push_back(Peice( position.x, position.y, -1, -1));

            move.updatedPeices = peices;
            moves.push_back(move);  
        }

        if(IsOpponent(board[(int)(position.x-1+(position.y+direction)*8)])){
            peices = std::vector<Peice>();
            
            Move move;
            peices.push_back(Peice( position.x - 1, position.y + direction, team, 0));
            peices.push_back(Peice( position.x, position.y, -1, -1));

            move.updatedPeices = peices;
            moves.push_back(move);  
        }


        if(IsOpponent(board[(int)(position.x+1+(position.y+direction)*8)])){
            peices = std::vector<Peice>();
 
            Move move;
            peices.push_back(Peice( position.x + 1, position.y + direction, team, 0));
            peices.push_back(Peice( position.x, position.y, -1, -1));

            move.updatedPeices = peices;
            moves.push_back(move);  
        }
    }

    
    if (peice == 3 || peice == 4){
        for(int y = 1; y < 9; ++ y){
            if(position.y - y < 0){
                break;
            }
            
            if(board[(int)(position.x+((position.y-y)*8))].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x, position.y - y, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));

                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }

            if(IsOpponent(board[(int)(position.x+((position.y-y)*8))])){
                break;
            }
        }

        for(int y = 1; y < 9; ++ y){
            if(position.y + y > 7){
                break;
            }

            if(board[(int)(position.x+((position.y+y)*8))].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x, position.y + y, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));

                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }

            if(IsOpponent(board[(int)(position.x+((position.y+y)*8))])){
                break;
            }
        }

        for(int x = 1; x < 9; ++ x){
            if(position.x + x > 7){
                break;
            }
            
            if(board[(int)(position.x+x+(position.y)*8)].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x + x, position.y, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));

                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }

            if(IsOpponent(board[(int)(position.x+x+(position.y)*8)])){
                break;
            }
        }

        for(int x = 1; x < 9; ++ x){
            if(position.x - x < 0){
                break;
            }

            if(board[(int)(position.x-x+(position.y)*8)].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x - x, position.y, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));

                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }
            
      

            if(IsOpponent(board[(int)(position.x-x+(position.y)*8)])){
                break;
            }
        }
       
    }
    
    if (peice == 4 || peice == 2){
        for (int i = 1; i < 9; ++i){
            if(position.x + i > 7 || position.y + i > 7){
                break;
            }

            if(board[(int)(position.x+i+(position.y+i)*8)].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x + i, position.y + i, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));

                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }

            if(IsOpponent(board[(int)(position.x+i+(position.y+i)*8)])){
                break;
            }
        }

        for (int i = 1; i < 9; ++i){
            if(position.x - i < 0 || position.y + i > 7){
                break;
            }

            if(board[(int)(position.x-i+(position.y+i)*8)].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x - i, position.y + i, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));
   
                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }

            if(IsOpponent(board[(int)(position.x-i+(position.y+i)*8)])){
                break;
            }
        }

        for (int i = 1; i < 9; ++i){
            if(position.x + i > 7 || position.y - i < 0){
                break;
            }



            if(board[(int)(position.x+i+(position.y-i)*8)].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x + i, position.y - i, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));
                
                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }

            if(IsOpponent(board[(int)(position.x+i+(position.y-i)*8)])){
                break;
            }
        }

        for (int i = 1; i < 9; ++i){
            if(position.x - i < 0 || position.y - i < 0){
                break;
            }
            
            if(board[(int)(position.x-i+(position.y-i)*8)].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x - i, position.y - i, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));
                
                move.updatedPeices = peices;
                moves.push_back(move);
            }else{
                break;
            }

             if(IsOpponent(board[(int)(position.x-i+(position.y-i)*8)])){
                break;
            }
        }
    }

    if(peice == 1){
        for (int i = 0; i < 4; ++i){
            int offX = (((i+1) % 2) == 1 ? 1 : -1);
            int offY = (i < 2 ? -2 : 2);

            if(position.x + offX < 8 && position.x + offX > -1 && position.y + offY < 8 && position.y + offY > -1){
                if(board[(int)(position.x + offX + (position.y + offY)*8)].team != team){
                    peices = std::vector<Peice>();
                    Move move;

                    peices.push_back(Peice( position.x + offX, position.y + offY, team, peice));
                    peices.push_back(Peice( position.x, position.y, -1, -1));

                    move.updatedPeices = peices;
                    moves.push_back(move);
                }
            }
        }

        for (int i = 0; i < 4; ++i){
            int offY = (((i+1) % 2) == 1 ? 1 : -1);
            int offX = (i < 2 ? -2 : 2);

            if(position.x + offX < 8 && position.x + offX > -1 && position.y + offY < 8 && position.y + offY > -1){
                if(board[(int)(position.x + offX + (position.y + offY)*8)].team != team){
                    peices = std::vector<Peice>();
                    Move move;

                    peices.push_back(Peice( position.x + offX, position.y + offY, team, peice));
                    peices.push_back(Peice( position.x, position.y, -1, -1));

                    move.updatedPeices = peices;
                    moves.push_back(move);
                }
            }
        }
    }

    if(peice == 5){
        for(int i = 0; i < 9; ++ i){
            int x = i % 3;
            int y = i / 3;

            if(position.x-1 + x < 0 || position.x-1 + x > 7 || position.y-1 + y < 0 || position.y-1+y > 7){
                continue;
            }

            if(board[(int)(position.x - 1 + x + (position.y-1 + y)*8)].team != team){
                peices = std::vector<Peice>();
                Move move;

                peices.push_back(Peice( position.x - 1 + x, position.y - 1 + y, team, peice));
                peices.push_back(Peice( position.x, position.y, -1, -1));

                move.updatedPeices = peices;
                moves.push_back(move);
            }
        }
    }
    
    return moves;
}