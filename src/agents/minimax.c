#include "agents.h"

static int minimax(Board *board, char root_player, char turn, int depth, SearchStats *stats)
{
    if (stats != NULL) ++stats->nodes;

    char winner=board_winner(board);
    if (winner == root_player) return 10-depth;
    if (winner == other_player(root_player)) return depth-10;
    if (board_is_full(board)) return 0;

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);

    if (turn == root_player) {
        int best=-1000;
        for (int i=0;i<count;++i) {
            board_make_move(board,moves[i],turn);
            int score=minimax(board,root_player,other_player(turn),depth+1,stats);
            board_undo_move(board,moves[i]);
            if (score > best) best=score;
        }
        return best;
    }

    int best=1000;
    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],turn);
        int score=minimax(board,root_player,other_player(turn),depth+1,stats);
        board_undo_move(board,moves[i]);
        if (score < best) best=score;
    }
    return best;
}

int agent_minimax_move(Board *board, char player, void *context)
{
    SearchStats *stats=context;
    if (stats != NULL) {
        stats->nodes=0;
        stats->prunes=0;
    }

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    int best_move=-1;
    int best_score=-1000;

    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],player);
        int score=minimax(board,player,other_player(player),1,stats);
        board_undo_move(board,moves[i]);

        if (score > best_score) {
            best_score=score;
            best_move=moves[i];
        }
    }
    return best_move;
}
