#include <iostream>
#include "peice.hpp"
#include "vector2.hpp"
#include "board.hpp"
#include <vector>

std::vector<Move> GetAllMoves(Board board){
    std::vector<Move> moves = std::vector<Move>();
    for(int i = 0; i <64 ; ++i){
        std::vector<Move> peiceMoves = board.peices[i].GenerateMoves(board.peices);
        for(Move move : peiceMoves){
            if(!board.IsSafeMove(move)){
                continue;
            }

            moves.push_back(move);
        }
    }

    return moves;
}

static Move bestMove;


float minimax(Board board, int depth){
    if(board.IsMate() != -1 || depth == 0){
        return board.Evaluate();
    }

    float best;

    if (board.turn == 1){
        best = -100000;
        
        std::vector<Move> moves = GetAllMoves(board);

        for(Move  move : moves){
            board.MakeMove(move);

            float value = minimax(board, depth-1);

            if (value > best){
                best = value;
                bestMove = move;
            }
        }
    }else{
        best = 100000;

        std::vector<Move> moves = GetAllMoves(board);

        for(Move move : moves){
            board.MakeMove(move);

            float value = minimax(board, depth-1);

            if (value < best){
                best = value;
                bestMove = move;
            }
        }
    }

    return best;
}


Move GetBestMove(Board board){
    minimax(board, 2);
    return bestMove;
}