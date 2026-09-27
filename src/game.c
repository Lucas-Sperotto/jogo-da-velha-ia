#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define YELLOW  "\033[1;33m"
#define GRAY    "\033[0;37m"

static void print_cell(char value, int position)
{
    if (value == PLAYER_X) printf(RED " X " RESET);
    else if (value == PLAYER_O) printf(BLUE " O " RESET);
    else printf(GRAY " %d " RESET, position + 1);
}

void board_init(Board *board)
{
    if (board == NULL) return;
    for (int i = 0; i < BOARD_SIZE; ++i) board->cells[i] = EMPTY;
}

void clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
    fflush(stdout);
#endif
}

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

int board_is_valid_move(const Board *board, int move)
{
    return board != NULL && move >= 0 && move < BOARD_SIZE &&
           board->cells[move] == EMPTY;
}

int board_make_move(Board *board, int move, char player)
{
    if (!board_is_valid_move(board, move)) return 0;
    if (player != PLAYER_X && player != PLAYER_O) return 0;
    board->cells[move] = player;
    return 1;
}

void board_undo_move(Board *board, int move)
{
    if (board != NULL && move >= 0 && move < BOARD_SIZE) board->cells[move] = EMPTY;
}

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

int board_is_full(const Board *board)
{
    if (board == NULL) return 0;
    for (int i=0;i<BOARD_SIZE;++i) if (board->cells[i] == EMPTY) return 0;
    return 1;
}

int board_is_terminal(const Board *board)
{
    return board_winner(board) != EMPTY || board_is_full(board);
}

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

char other_player(char player)
{
    return player == PLAYER_X ? PLAYER_O : PLAYER_X;
}

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

void wait_enter(void)
{
    char line[8];
    printf("\nPressione ENTER para voltar ao menu...");
    if (fgets(line,sizeof(line),stdin) == NULL) {
        clearerr(stdin);
    }
}

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
