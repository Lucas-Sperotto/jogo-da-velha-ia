#include "experiment.h"
#include "rng.h"

#include <inttypes.h>
#include <stdio.h>

/**
 * @brief Armazena o número de jogadas realizadas na última partida concluída.
 */
static unsigned long long last_match_moves=0;

/**
 * @brief Simula uma única partida entre dois agentes de IA (x como 'X' e o como 'O').
 *
 * Se o parâmetro visual for ativado (1), imprime o tabuleiro colorido a cada lance
 * e identifica textualmente a jogada realizada.
 *
 * @param x Agente que joga com 'X'.
 * @param o Agente que joga com 'O'.
 * @param visual Flag booleana (1 para exibir animação/tabuleiro no terminal, 0 para modo silencioso).
 * @return Símbolo do vencedor ('X', 'O') ou EMPTY (' ') em caso de empate.
 */
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

/**
 * @brief Executa um experimento com games partidas entre o Agente A e o Agente B.
 *
 * Alternância estrita:
 *  - Partidas pares (0, 2, 4...): A joga como 'X' e B joga como 'O';
 *  - Partidas ímpares (1, 3, 5...): B joga como 'X' e A joga como 'O'.
 * Isso elimina viés decorrente da vantagem de jogar primeiro.
 *
 * @param a Agente A.
 * @param b Agente B.
 * @param games Quantidade de partidas a executar.
 * @return Estrutura ExperimentResult com contagem agregada de vitórias, empates e jogadas.
 */
ExperimentResult run_experiment_seeded(RuntimeAgent *a, RuntimeAgent *b,
                                       int games, uint64_t seed)
{
    ExperimentResult result={0};
    result.seed=seed;
    if (games <= 0) return result;

    rng_seed(seed);
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

ExperimentResult run_experiment(RuntimeAgent *a, RuntimeAgent *b, int games)
{
    return run_experiment_seeded(a,b,games,rng_seed_auto());
}

/**
 * @brief Exibe na saída padrão um resumo legível e estatístico do experimento.
 *
 * Mostra vitórias de A e B, empates, total de jogadas, média por partida,
 * bem como total de nós visitados e podas registradas na árvore de busca.
 *
 * @param a Agente A.
 * @param b Agente B.
 * @param result Ponteiro para os resultados coletados.
 */
void print_experiment_result(const RuntimeAgent *a, const RuntimeAgent *b,
                             const ExperimentResult *result)
{
    printf("\n=== RESULTADO DO EXPERIMENTO ===\n");
    printf("Agente A: %s\n",a->name);
    printf("Agente B: %s\n",b->name);
    printf("Partidas: %d\n",result->games);
    printf("Seed:     %" PRIu64 "\n",result->seed);
    printf("Vitórias A: %d\n",result->wins_a);
    printf("Vitórias B: %d\n",result->wins_b);
    printf("Empates:    %d\n",result->draws);
    printf("Jogadas:    %llu\n",result->moves);
    if (result->games > 0)
        printf("Média de jogadas: %.2f\n",(double)result->moves/result->games);
    printf("Nós A:      %llu | Podas A: %llu\n",a->total_nodes,a->total_prunes);
    printf("Nós B:      %llu | Podas B: %llu\n",b->total_nodes,b->total_prunes);
}

/**
 * @brief Salva os dados do experimento em formato CSV no arquivo especificado.
 *
 * Insere a linha de cabeçalho na primeira escrita se o arquivo estiver vazio.
 *
 * @param path Caminho do arquivo CSV (ex: "results/experiments.csv").
 * @param a Agente A.
 * @param b Agente B.
 * @param result Ponteiro para o resultado agregado.
 * @return 1 se gravado com sucesso; 0 em caso de falha de arquivo.
 */
int append_experiment_csv(const char *path, const RuntimeAgent *a,
                          const RuntimeAgent *b, const ExperimentResult *result)
{
    FILE *file=fopen(path,"a+");
    if (file == NULL) return 0;

    if (fseek(file,0,SEEK_END) == 0 && ftell(file) == 0)
        fprintf(file,"seed,agent_a,agent_b,games,wins_a,wins_b,draws,moves,nodes_a,prunes_a,nodes_b,prunes_b\n");

    fprintf(file,"%" PRIu64 ",%s,%s,%d,%d,%d,%d,%llu,%llu,%llu,%llu,%llu\n",
            result->seed,a->name,b->name,result->games,result->wins_a,result->wins_b,
            result->draws,result->moves,a->total_nodes,a->total_prunes,
            b->total_nodes,b->total_prunes);
    fclose(file);
    return 1;
}
