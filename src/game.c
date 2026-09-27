#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define YELLOW  "\033[1;33m"
#define GRAY    "\033[0;37m"

/**
 * @brief Imprime uma célula individual formatada com cores ANSI.
 *
 * Exibe 'X' em vermelho, 'O' em azul, ou o número da posição (1-9) em cinza se vazia.
 *
 * @param value Conteúdo da célula ('X', 'O' ou ' ').
 * @param position Índice da célula no tabuleiro (0 a 8).
 */
static void print_cell(char value, int position)
{
    if (value == PLAYER_X) printf(RED " X " RESET);
    else if (value == PLAYER_O) printf(BLUE " O " RESET);
    else printf(GRAY " %d " RESET, position + 1);
}

/**
 * @brief Inicializa o tabuleiro preenchendo todas as posições com EMPTY (' ').
 *
 * @param board Ponteiro para a estrutura Board a ser inicializada.
 */
void board_init(Board *board)
{
    if (board == NULL) return;
    for (int i = 0; i < BOARD_SIZE; ++i) board->cells[i] = EMPTY;
}

/**
 * @brief Limpa o terminal console (suporta Windows cls e sequências ANSI em Unix).
 */
void clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

/**
 * @brief Renderiza graficamente o tabuleiro 3x3 no console com bordas e posições.
 *
 * @param board Ponteiro para o tabuleiro a exibir.
 */
void board_print(const Board *board)
{
    if (board == NULL) return;
    printf("\n");
    printf(CYAN "╔════════════════════════════╗\n");
    printf("║       JOGO DA VELHA        ║\n");
    printf("╚════════════════════════════╝" RESET "\n\n");
    printf("       "); print_cell(board->cells[0], 0); printf(" │ ");
    print_cell(board->cells[1], 1); printf(" │ "); print_cell(board->cells[2], 2);
    printf("\n      ────┼─────┼────\n");
    printf("       "); print_cell(board->cells[3], 3); printf(" │ ");
    print_cell(board->cells[4], 4); printf(" │ "); print_cell(board->cells[5], 5);
    printf("\n      ────┼─────┼────\n");
    printf("       "); print_cell(board->cells[6], 6); printf(" │ ");
    print_cell(board->cells[7], 7); printf(" │ "); print_cell(board->cells[8], 8);
    printf("\n\n");
}

/**
 * @brief Valida se um movimento em determinada posição é permitido.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param move Posição a testar (0 a 8).
 * @return 1 se a posição for válida e estiver livre; 0 caso contrário.
 */
int board_is_valid_move(const Board *board, int move)
{
    return board != NULL && move >= 0 && move < BOARD_SIZE &&
           board->cells[move] == EMPTY;
}

/**
 * @brief Realiza uma jogada no tabuleiro para o jogador especificado.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param move Posição da jogada (0 a 8).
 * @param player Símbolo do jogador (PLAYER_X ou PLAYER_O).
 * @return 1 se a jogada foi executada com sucesso; 0 se inválida.
 */
int board_make_move(Board *board, int move, char player)
{
    if (!board_is_valid_move(board, move)) return 0;
    if (player != PLAYER_X && player != PLAYER_O) return 0;
    board->cells[move] = player;
    return 1;
}

/**
 * @brief Desfaz uma jogada anteriormente executada, restaurando a célula para EMPTY.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param move Posição a reverter (0 a 8).
 */
void board_undo_move(Board *board, int move)
{
    if (board != NULL && move >= 0 && move < BOARD_SIZE) board->cells[move] = EMPTY;
}

/**
 * @brief Verifica se há um vencedor no tabuleiro analisando as 8 linhas possíveis.
 *
 * @param board Ponteiro para o tabuleiro.
 * @return PLAYER_X ou PLAYER_O se houver vencedor; EMPTY (' ') se não houver.
 */
char board_winner(const Board *board)
{
    static const int lines[8][3] = {
        {0,1,2},{3,4,5},{6,7,8},{0,3,6},
        {1,4,7},{2,5,8},{0,4,8},{2,4,6}
    };
    if (board == NULL) return EMPTY;
    for (int i = 0; i < 8; ++i) {
        int a=lines[i][0], b=lines[i][1], c=lines[i][2];
        if (board->cells[a] != EMPTY &&
            board->cells[a] == board->cells[b] &&
            board->cells[b] == board->cells[c]) return board->cells[a];
    }
    return EMPTY;
}

/**
 * @brief Verifica se o tabuleiro está completamente preenchido (sem casas vazias).
 *
 * @param board Ponteiro para o tabuleiro.
 * @return 1 se o tabuleiro estiver cheio; 0 se houver ao menos uma casa vazia.
 */
int board_is_full(const Board *board)
{
    if (board == NULL) return 0;
    for (int i=0;i<BOARD_SIZE;++i) if (board->cells[i] == EMPTY) return 0;
    return 1;
}

/**
 * @brief Verifica se o jogo atingiu um estado terminal (vitória de alguém ou empate/cheio).
 *
 * @param board Ponteiro para o tabuleiro.
 * @return 1 se o jogo terminou; 0 caso o jogo ainda esteja em andamento.
 */
int board_is_terminal(const Board *board)
{
    return board_winner(board) != EMPTY || board_is_full(board);
}

/**
 * @brief Enumera os movimentos disponíveis no tabuleiro e preenche o vetor fornecido.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param moves Vetor onde as posições livres serão salvas (pode ser NULL para apenas contar).
 * @return Quantidade de movimentos disponíveis encontrados (0 a 9).
 */
int board_available_moves(const Board *board, int moves[BOARD_SIZE])
{
    int count=0;
    if (board == NULL) return 0;
    for (int i=0;i<BOARD_SIZE;++i) {
        if (board->cells[i] == EMPTY) {
            if (moves != NULL) moves[count]=i;
            ++count;
        }
    }
    return count;
}

/**
 * @brief Retorna o símbolo do jogador adversário.
 *
 * @param player Jogador atual (PLAYER_X ou PLAYER_O).
 * @return PLAYER_O se o atual for PLAYER_X; PLAYER_X caso contrário.
 */
char other_player(char player)
{
    return player == PLAYER_X ? PLAYER_O : PLAYER_X;
}

/**
 * @brief Lê e valida a jogada de um jogador humano via entrada padrão (stdin).
 *
 * Converte a entrada de base 1 (1-9) para índice interno de base 0 (0-8).
 *
 * @param board Ponteiro para o tabuleiro.
 * @param player Símbolo do jogador humano.
 * @return Índice da jogada (0 a 8) ou -1 em caso de EOF ou erro irrecuperável.
 */
int read_human_move(const Board *board, char player)
{
    char line[64];
    while (1) {
        printf(YELLOW "Jogador %c, escolha uma posição [1-9]: " RESET,player);
        if (fgets(line,sizeof(line),stdin) == NULL) return -1;
        char *end=NULL;
        long value=strtol(line,&end,10);
        if (end == line || value < 1 || value > 9) {
            printf(RED "Entrada inválida. Digite um número de 1 a 9.\n" RESET);
            continue;
        }
        int move=(int)value-1;
        if (!board_is_valid_move(board,move)) {
            printf(RED "Essa posição já está ocupada.\n" RESET);
            continue;
        }
        return move;
    }
}

/**
 * @brief Pausa a execução e aguarda o pressionamento da tecla ENTER pelo usuário.
 */
void wait_enter(void)
{
    char line[8];
    printf("\nPressione ENTER para voltar ao menu...");
    if (fgets(line,sizeof(line),stdin) == NULL) {
        clearerr(stdin);
    }
}

/**
 * @brief Controla o ciclo completo de uma partida Humano contra Humano.
 */
void play_human_vs_human(void)
{
    Board board;
    char current=PLAYER_X;
    board_init(&board);
    while (!board_is_terminal(&board)) {
        clear_screen();
        board_print(&board);
        int move=read_human_move(&board,current);
        if (move < 0) return;
        board_make_move(&board,move,current);
        current=other_player(current);
    }
    clear_screen();
    board_print(&board);
    char winner=board_winner(&board);
    if (winner == EMPTY) printf(YELLOW "Empate!\n" RESET);
    else printf(YELLOW "Jogador %c venceu!\n" RESET,winner);
    wait_enter();
}

/**
 * @brief Controla o ciclo de uma partida interativa de Humano (X) contra Agente IA (O).
 *
 * @param agent_name Nome textual do agente de IA para exibição.
 * @param selector Função de seleção de jogada do agente de IA.
 * @param context Ponteiro opcional de contexto com parâmetros ou estado do agente.
 */
void play_human_vs_agent(const char *agent_name, MoveSelector selector, void *context)
{
    Board board;
    char current=PLAYER_X;
    board_init(&board);

    while (!board_is_terminal(&board)) {
        clear_screen();
        board_print(&board);

        if (current == PLAYER_X) {
            int move=read_human_move(&board,current);
            if (move < 0) return;
            board_make_move(&board,move,current);
        } else {
            printf("Computador (%s) analisando...\n",agent_name);
            int move=selector(&board,current,context);
            if (move < 0 || !board_make_move(&board,move,current)) {
                printf("Erro: o agente retornou uma jogada inválida.\n");
                wait_enter();
                return;
            }
        }
        current=other_player(current);
    }

    clear_screen();
    board_print(&board);
    char winner=board_winner(&board);
    if (winner == PLAYER_X) printf("Você venceu!\n");
    else if (winner == PLAYER_O) printf("O computador (%s) venceu.\n",agent_name);
    else printf("Empate!\n");
    wait_enter();
}
