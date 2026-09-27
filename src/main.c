#include "agents.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    printf("║  8. Q-Learning                    ║\n");
    printf("║                                  ║\n");
    printf("║  9. IA × IA                      ║\n");
    printf("║ 10. Executar experimento         ║\n");
    printf("║                                  ║\n");
    printf("║  0. Sair                         ║\n");
    printf("╚══════════════════════════════════╝\n");
}

static int read_option(void)
{
    char line[32], *end=NULL;
    printf("\nEscolha uma opção: ");
    if (fgets(line,sizeof(line),stdin) == NULL) return 0;
    long value=strtol(line,&end,10);
    if (end == line || value < 0 || value > 10) return -1;
    return (int)value;
}

static void not_implemented(int option)
{
    printf("\nOpção %d será implementada em uma etapa posterior do laboratório.\n",option);
    wait_enter();
}

int main(void)
{
    int option;
    srand((unsigned int)time(NULL));

    do {
        clear_screen();
        print_menu();
        option=read_option();

        switch (option) {
            case 0: printf("\nEncerrando o laboratório.\n"); break;
            case 1: play_human_vs_human(); break;
            case 2: play_human_vs_agent("Aleatório",agent_random_move,NULL); break;
            case 3: play_human_vs_agent("Heurístico",agent_heuristic_move,NULL); break;
            case 4: case 5: case 6: case 7: case 8: case 9: case 10:
                not_implemented(option); break;
            default: printf("\nOpção inválida.\n"); wait_enter(); break;
        }
    } while (option != 0);

    return 0;
}
