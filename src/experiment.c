#include "experiment.h"

#include <stdio.h>

static unsigned long long last_match_moves=0;

char play_ai_match(RuntimeAgent *x, RuntimeAgent *o, int visual)
{
    Board board;
    char turn=PLAYER_X;
    unsigned long long moves=0;
    board_init(&board);

    if (visual) {
        clear_screen();
        printf("X = %s | O = %s\n",x->name,o->name);
        board_print(&board);
    }

    while (!board_is_terminal(&board)) {
        RuntimeAgent *current=turn == PLAYER_X ? x : o;
        int move=runtime_agent_move(&board,turn,current);
        if (move < 0 || !board_make_move(&board,move,turn)) {
            last_match_moves=moves;
            return other_player(turn);
        }
        ++moves;

        if (visual) {
            printf("%s (%c) joga na posição %d.\n",current->name,turn,move+1);
            board_print(&board);
        }
        turn=other_player(turn);
    }

    last_match_moves=moves;
    return board_winner(&board);
}

ExperimentResult run_experiment(RuntimeAgent *a, RuntimeAgent *b, int games)
{
    ExperimentResult result={0};
    if (games <= 0) return result;

    result.games=games;
    for (int i=0;i<games;++i) {
        char winner;
        if (i%2 == 0) {
            winner=play_ai_match(a,b,0);
            if (winner == PLAYER_X) ++result.wins_a;
            else if (winner == PLAYER_O) ++result.wins_b;
            else ++result.draws;
        } else {
            winner=play_ai_match(b,a,0);
            if (winner == PLAYER_X) ++result.wins_b;
            else if (winner == PLAYER_O) ++result.wins_a;
            else ++result.draws;
        }
        result.moves += last_match_moves;
    }
    return result;
}

void print_experiment_result(const RuntimeAgent *a, const RuntimeAgent *b,
                             const ExperimentResult *result)
{
    printf("\n=== RESULTADO DO EXPERIMENTO ===\n");
    printf("Agente A: %s\n",a->name);
    printf("Agente B: %s\n",b->name);
    printf("Partidas: %d\n",result->games);
    printf("Vitórias A: %d\n",result->wins_a);
    printf("Vitórias B: %d\n",result->wins_b);
    printf("Empates:    %d\n",result->draws);
    printf("Jogadas:    %llu\n",result->moves);
    if (result->games > 0)
        printf("Média de jogadas: %.2f\n",(double)result->moves/result->games);
    printf("Nós A:      %llu | Podas A: %llu\n",a->total_nodes,a->total_prunes);
    printf("Nós B:      %llu | Podas B: %llu\n",b->total_nodes,b->total_prunes);
}

int append_experiment_csv(const char *path, const RuntimeAgent *a,
                          const RuntimeAgent *b, const ExperimentResult *result)
{
    FILE *file=fopen(path,"a+");
    if (file == NULL) return 0;

    if (fseek(file,0,SEEK_END) == 0 && ftell(file) == 0)
        fprintf(file,"agent_a,agent_b,games,wins_a,wins_b,draws,moves,nodes_a,prunes_a,nodes_b,prunes_b\n");

    fprintf(file,"%s,%s,%d,%d,%d,%d,%llu,%llu,%llu,%llu,%llu\n",
            a->name,b->name,result->games,result->wins_a,result->wins_b,
            result->draws,result->moves,a->total_nodes,a->total_prunes,
            b->total_nodes,b->total_prunes);
    fclose(file);
    return 1;
}
