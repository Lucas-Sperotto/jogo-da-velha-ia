#ifndef GAME_H
#define GAME_H

#include <stddef.h>

/**
 * @def BOARD_SIZE
 * @brief Quantidade total de casas no tabuleiro 3x3 do Jogo da Velha.
 */
#define BOARD_SIZE 9

/**
 * @def EMPTY
 * @brief Representação textual de uma casa vazia no tabuleiro.
 */
#define EMPTY ' '

/**
 * @def PLAYER_X
 * @brief Representação do jogador X (geralmente o primeiro a jogar).
 */
#define PLAYER_X 'X'

/**
 * @def PLAYER_O
 * @brief Representação do jogador O (segundo a jogar).
 */
#define PLAYER_O 'O'

/**
 * @brief Estrutura que representa o estado do tabuleiro 3x3.
 *
 * As células são indexadas linearmente de 0 a 8:
 *  0 | 1 | 2
 *  3 | 4 | 5
 *  6 | 7 | 8
 */
typedef struct {
    char cells[BOARD_SIZE];
} Board;

/**
 * @brief Tipo de ponteiro de função para seleção de jogadas por agentes.
 *
 * @param board Ponteiro para o tabuleiro atual.
 * @param player Símbolo do jogador da vez (PLAYER_X ou PLAYER_O).
 * @param context Ponteiro genérico para estado/parâmetros do agente (pode ser NULL).
 * @return Índice da jogada escolhida (0 a 8) ou -1 se não houver jogadas válidas.
 */
typedef int (*MoveSelector)(Board *board, char player, void *context);

/**
 * @brief Inicializa o tabuleiro limpando todas as casas com EMPTY (' ').
 *
 * @param board Ponteiro para o tabuleiro a ser inicializado.
 */
void board_init(Board *board);

/**
 * @brief Exibe o tabuleiro no console formatado com bordas e cores ANSI.
 *
 * @param board Ponteiro para o tabuleiro a ser impresso.
 */
void board_print(const Board *board);

/**
 * @brief Verifica se um movimento é válido em uma dada posição.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param move Índice da casa (0 a 8).
 * @return 1 se a posição for válida e estiver vazia; 0 caso contrário.
 */
int board_is_valid_move(const Board *board, int move);

/**
 * @brief Aplica uma jogada no tabuleiro para o jogador informado.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param move Índice da casa (0 a 8).
 * @param player Símbolo do jogador (PLAYER_X ou PLAYER_O).
 * @return 1 se a jogada foi aplicada com sucesso; 0 caso inválida.
 */
int board_make_move(Board *board, int move, char player);

/**
 * @brief Desfaz uma jogada, marcando a casa novamente como EMPTY.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param move Índice da casa a ser esvaziada (0 a 8).
 */
void board_undo_move(Board *board, int move);

/**
 * @brief Identifica se há um vencedor no tabuleiro.
 *
 * @param board Ponteiro para o tabuleiro.
 * @return PLAYER_X ou PLAYER_O se houver vencedor; EMPTY (' ') se não houver.
 */
char board_winner(const Board *board);

/**
 * @brief Verifica se todas as casas do tabuleiro estão preenchidas.
 *
 * @param board Ponteiro para o tabuleiro.
 * @return 1 se o tabuleiro estiver cheio; 0 caso haja pelo menos uma casa vazia.
 */
int board_is_full(const Board *board);

/**
 * @brief Verifica se o estado atual do jogo é terminal (vitória ou empate/velha).
 *
 * @param board Ponteiro para o tabuleiro.
 * @return 1 se o jogo terminou; 0 se ainda houver jogadas e nenhum vencedor.
 */
int board_is_terminal(const Board *board);

/**
 * @brief Obtém a lista e a contagem de índices de casas livres no tabuleiro.
 *
 * @param board Ponteiro para o tabuleiro.
 * @param moves Vetor onde serão gravados os índices disponíveis (pode ser NULL para apenas contar).
 * @return Quantidade de movimentos disponíveis (0 a 9).
 */
int board_available_moves(const Board *board, int moves[BOARD_SIZE]);

/**
 * @brief Retorna o símbolo do jogador adversário.
 *
 * @param player Símbolo do jogador atual (PLAYER_X ou PLAYER_O).
 * @return PLAYER_O se o atual for PLAYER_X, e vice-versa.
 */
char other_player(char player);

/**
 * @brief Lê uma jogada informada pelo usuário no terminal com validação de entrada.
 *
 * Converte a entrada de 1-9 (base 1) para o índice interno 0-8 (base 0).
 *
 * @param board Ponteiro para o tabuleiro atual.
 * @param player Símbolo do jogador humano da vez.
 * @return Índice da jogada (0 a 8) ou -1 em caso de EOF/erro fatal de leitura.
 */
int read_human_move(const Board *board, char player);

/**
 * @brief Limpa a tela do terminal (suporta Windows via cls e Unix via sequências ANSI).
 */
void clear_screen(void);

/**
 * @brief Pausa a execução e aguarda que o usuário pressione ENTER para prosseguir.
 */
void wait_enter(void);

/**
 * @brief Executa uma partida interativa completa de Humano contra Humano.
 */
void play_human_vs_human(void);

/**
 * @brief Executa uma partida interativa de Humano (como X) contra um Agente IA (como O).
 *
 * @param agent_name Nome amigável do agente para exibição.
 * @param selector Função de escolha de movimento do agente.
 * @param context Contexto de configuração ou estado do agente (SearchStats, pesos, etc.).
 */
void play_human_vs_agent(const char *agent_name, MoveSelector selector, void *context);

#endif
