#include "agents.h"
#include "experiment.h"
#include "game.h"
#include "genetic.h"
#include "qlearning.h"
#include "samuel.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Exibe na tela o menu principal do Laboratório de Inteligência Artificial.
 */
static void print_menu(void)
{
    printf("╔══════════════════════════════════╗\n");
    printf("║       LABORATÓRIO DE IA          ║\n");
    printf("║          JOGO DA VELHA           ║\n");
    printf("╠══════════════════════════════════╣\n");
    printf("║                                  ║\n");
    printf("║  1. Humano × Humano              ║\n");
    printf("║  2. Humano × Aleatório           ║\n");
    printf("║  3. Humano × Heurístico          ║\n");
    printf("║  4. Humano × Minimax             ║\n");
    printf("║  5. Humano × Alpha-Beta          ║\n");
    printf("║  6. IA estilo Arthur Samuel      ║\n");
    printf("║  7. Algoritmo Genético           ║\n");
    printf("║  8. Q-Learning                   ║\n");
    printf("║                                  ║\n");
    printf("║  9. IA × IA                      ║\n");
    printf("║ 10. Executar experimento         ║\n");
    printf("║                                  ║\n");
    printf("║  0. Sair                         ║\n");
    printf("╚══════════════════════════════════╝\n");
}

/**
 * @brief Lê um número inteiro da entrada padrão com validação de faixa [min, max].
 *
 * @param prompt Mensagem exibida solicitando a entrada.
 * @param min Valor mínimo aceitável.
 * @param max Valor máximo aceitável.
 * @return Inteiro validado dentro do intervalo.
 */
static int read_int(const char *prompt, int min, int max)
{
    char line[64];
    while (1) {
        char *end=NULL;
        printf("%s",prompt);
        if (fgets(line,sizeof(line),stdin) == NULL) return min;
        long value=strtol(line,&end,10);
        if (end != line && value >= min && value <= max) return (int)value;
        printf("Valor inválido. Escolha entre %d e %d.\n",min,max);
    }
}

/**
 * @brief Lê a opção selecionada no menu principal (entre 0 e 10).
 *
 * @return Opção inteira escolhida.
 */
static int read_option(void)
{
    return read_int("\nEscolha uma opção: ",0,10);
}

/**
 * @brief Apresenta submenu interativo para seleção de um tipo de agente.
 *
 * @param label Título descritivo exibido acima das opções (ex: "Agente X:").
 * @return Tipo de agente escolhido (AgentKind).
 */
static AgentKind choose_agent(const char *label)
{
    printf("\n%s\n",label);
    for (int i=AGENT_RANDOM;i<=AGENT_QLEARNING;++i)
        printf("  %d. %s\n",i,agent_kind_name((AgentKind)i));
    return (AgentKind)read_int("Escolha: ",AGENT_RANDOM,AGENT_QLEARNING);
}

/**
 * @brief Inicializa com segurança um par de agentes em tempo de execução.
 *
 * Se a inicialização do segundo agente falhar, garante a desalocação do primeiro.
 *
 * @param a Ponteiro para o primeiro RuntimeAgent.
 * @param ka Tipo do primeiro agente.
 * @param b Ponteiro para o segundo RuntimeAgent.
 * @param kb Tipo do segundo agente.
 * @return 1 se ambos foram inicializados com sucesso; 0 caso contrário.
 */
static int init_pair(RuntimeAgent *a, AgentKind ka, RuntimeAgent *b, AgentKind kb)
{
    if (!runtime_agent_init(a,ka)) return 0;
    if (!runtime_agent_init(b,kb)) {
        runtime_agent_destroy(a);
        return 0;
    }
    return 1;
}

/**
 * @brief Submenu e execução de partida única interativa IA contra IA com exibição visual.
 */
static void play_ai_vs_ai_menu(void)
{
    AgentKind xkind=choose_agent("Agente X:");
    AgentKind okind=choose_agent("Agente O:");
    RuntimeAgent x,o;

    if (!init_pair(&x,xkind,&o,okind)) {
        printf("Falha ao inicializar agentes.\n");
        wait_enter();
        return;
    }

    char winner=play_ai_match(&x,&o,1);
    if (winner == EMPTY) printf("Resultado: empate.\n");
    else printf("Resultado: %c venceu.\n",winner);

    runtime_agent_destroy(&x);
    runtime_agent_destroy(&o);
    wait_enter();
}

/**
 * @brief Submenu e execução de experimento com múltiplas partidas e exportação para CSV.
 */
static void run_experiment_menu(void)
{
    AgentKind akind=choose_agent("Agente A:");
    AgentKind bkind=choose_agent("Agente B:");
    int games=read_int("Número de partidas [1-100000]: ",1,100000);
    RuntimeAgent a,b;

    if (!init_pair(&a,akind,&b,bkind)) {
        printf("Falha ao inicializar agentes.\n");
        wait_enter();
        return;
    }

    ExperimentResult result=run_experiment(&a,&b,games);
    print_experiment_result(&a,&b,&result);
    if (append_experiment_csv("results/experiments.csv",&a,&b,&result))
        printf("Resultado registrado em results/experiments.csv\n");

    runtime_agent_destroy(&a);
    runtime_agent_destroy(&b);
    wait_enter();
}

/**
 * @brief Ponto de entrada principal do programa (CLI interativo).
 *
 * Inicializa a semente de números pseudoaleatórios com base no relógio do sistema (time(NULL))
 * e executa o laço de menu até que a opção 0 (Sair) seja acionada.
 *
 * @return Código de término de execução (0).
 */
int main(void)
{
    int option;
    srand((unsigned int)time(NULL));

    do {
        clear_screen();
        print_menu();
        option=read_option();

        switch (option) {
            case 0:
                printf("\nEncerrando o laboratório.\n");
                break;
            case 1:
                play_human_vs_human();
                break;
            case 2:
                play_human_vs_agent("Aleatório",agent_random_move,NULL);
                break;
            case 3:
                play_human_vs_agent("Heurístico",agent_heuristic_move,NULL);
                break;
            case 4: {
                SearchStats stats={0};
                play_human_vs_agent("Minimax",agent_minimax_move,&stats);
                break;
            }
            case 5: {
                SearchStats stats={0};
                play_human_vs_agent("Alpha-Beta",agent_alphabeta_move,&stats);
                break;
            }
            case 6: {
                SamuelAgent samuel;
                samuel_init(&samuel);
                if (!samuel_load(&samuel,"data/samuel_weights.dat")) {
                    printf("\nTreinando agente inspirado em Arthur Samuel...\n");
                    samuel_train(&samuel,5000);
                    (void)samuel_save(&samuel,"data/samuel_weights.dat");
                }
                play_human_vs_agent("Samuel-style",agent_samuel_move,&samuel);
                break;
            }
            case 7: {
                GeneticAgent genetic;
                genetic_init(&genetic);
                if (!genetic_load(&genetic,"data/genetic_weights.dat")) {
                    printf("\nEvoluindo população de estratégias...\n");
                    genetic_train(&genetic,60);
                    (void)genetic_save(&genetic,"data/genetic_weights.dat");
                }
                play_human_vs_agent("Genético",agent_genetic_move,&genetic);
                break;
            }
            case 8: {
                QLearningAgent qagent;
                if (!qlearning_init(&qagent)) {
                    printf("\nFalha ao alocar tabela Q.\n");
                    wait_enter();
                    break;
                }
                if (!qlearning_load(&qagent,"data/qtable.bin")) {
                    printf("\nTreinando Q-Learning...\n");
                    qlearning_train(&qagent,50000);
                    (void)qlearning_save(&qagent,"data/qtable.bin");
                }
                play_human_vs_agent("Q-Learning",agent_qlearning_move,&qagent);
                qlearning_free(&qagent);
                break;
            }
            case 9:
                play_ai_vs_ai_menu();
                break;
            case 10:
                run_experiment_menu();
                break;
        }
    } while (option != 0);

    return 0;
}
