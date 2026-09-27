#include "learning.h"

#include <stdlib.h>

static int count_immediate_wins(Board *board, char player)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    int wins=0;
    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],player);
        if (board_winner(board) == player) ++wins;
        board_undo_move(board,moves[i]);
    }
    return wins;
}

static int count_corners(const Board *board, char player)
{
    static const int corners[]={0,2,6,8};
    int count=0;
    for (int i=0;i<4;++i) if (board->cells[corners[i]] == player) ++count;
    return count;
}

static int line_potential(const Board *board, char player)
{
    static const int lines[8][3]={
        {0,1,2},{3,4,5},{6,7,8},{0,3,6},
        {1,4,7},{2,5,8},{0,4,8},{2,4,6}
    };
    char opponent=other_player(player);
    int score=0;
    for (int i=0;i<8;++i) {
        int own=0, blocked=0;
        for (int j=0;j<3;++j) {
            char c=board->cells[lines[i][j]];
            if (c == player) ++own;
            else if (c == opponent) blocked=1;
        }
        if (!blocked) score += own+1;
    }
    return score;
}

static int count_pieces(const Board *board, char player)
{
    int count=0;
    for (int i=0;i<BOARD_SIZE;++i) if (board->cells[i] == player) ++count;
    return count;
}

void extract_features(const Board *board, char player, double out[FEATURE_COUNT])
{
    char opponent=other_player(player);
    Board copy=*board;
    out[0]=1.0;
    out[1]=(double)(count_immediate_wins(&copy,player)-count_immediate_wins(&copy,opponent));
    out[2]=board->cells[4] == player ? 1.0 : board->cells[4] == opponent ? -1.0 : 0.0;
    out[3]=(double)(count_corners(board,player)-count_corners(board,opponent));
    out[4]=(double)(line_potential(board,player)-line_potential(board,opponent));
    out[5]=(double)(count_pieces(board,player)-count_pieces(board,opponent));
}

double evaluate_position(const Board *board, char player, const StrategyWeights *weights)
{
    double features[FEATURE_COUNT], score=0.0;
    extract_features(board,player,features);
    for (int i=0;i<FEATURE_COUNT;++i) score += weights->values[i]*features[i];
    return score;
}

int weighted_best_move(Board *board, char player, const StrategyWeights *weights, double epsilon)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    if (count == 0) return -1;

    if (epsilon > 0.0) {
        double r=(double)rand()/(double)RAND_MAX;
        if (r < epsilon) return moves[rand()%count];
    }

    int best_move=moves[0];
    double best_score=-1.0e30;

    for (int i=0;i<count;++i) {
        int move=moves[i];
        board_make_move(board,move,player);

        if (board_winner(board) == player) {
            board_undo_move(board,move);
            return move;
        }

        double score=evaluate_position(board,player,weights);
        int replies[BOARD_SIZE];
        int reply_count=board_available_moves(board,replies);
        double worst_reply=score;

        for (int j=0;j<reply_count;++j) {
            board_make_move(board,replies[j],other_player(player));
            double reply_score=board_winner(board) == other_player(player)
                ? -1000.0 : evaluate_position(board,player,weights);
            board_undo_move(board,replies[j]);
            if (j == 0 || reply_score < worst_reply) worst_reply=reply_score;
        }

        board_undo_move(board,move);
        if (worst_reply > best_score) {
            best_score=worst_reply;
            best_move=move;
        }
    }

    return best_move;
}
