#include "agents.h"

static int alphabeta(Board *board, char root_player, char turn, int depth,
                     int alpha, int beta, SearchStats *stats)
{
    if (stats != NULL) ++stats->nodes;

    char winner=board_winner(board);
    if (winner == root_player) return 10-depth;
    if (winner == other_player(root_player)) return depth-10;
    if (board_is_full(board)) return 0;

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);

    if (turn == root_player) {
        int value=-1000;
        for (int i=0;i<count;++i) {
            board_make_move(board,moves[i],turn);
            int score=alphabeta(board,root_player,other_player(turn),depth+1,alpha,beta,stats);
            board_undo_move(board,moves[i]);
            if (score > value) value=score;
            if (value > alpha) alpha=value;
            if (alpha >= beta) {
                if (stats != NULL) ++stats->prunes;
                break;
            }
        }
        return value;
    }

    int value=1000;
    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],turn);
        int score=alphabeta(board,root_player,other_player(turn),depth+1,alpha,beta,stats);
        board_undo_move(board,moves[i]);
        if (score < value) value=score;
        if (value < beta) beta=value;
        if (alpha >= beta) {
            if (stats != NULL) ++stats->prunes;
            break;
        }
    }
    return value;
}

int agent_alphabeta_move(Board *board, char player, void *context)
{
    SearchStats *stats=context;
    if (stats != NULL) {
        stats->nodes=0;
        stats->prunes=0;
    }

    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    int best_move=-1, best_score=-1000, alpha=-1000;
    const int beta=1000;

    for (int i=0;i<count;++i) {
        board_make_move(board,moves[i],player);
        int score=alphabeta(board,player,other_player(player),1,alpha,beta,stats);
        board_undo_move(board,moves[i]);

        if (score > best_score) {
            best_score=score;
            best_move=moves[i];
        }
        if (best_score > alpha) alpha=best_score;
    }

    return best_move;
}
