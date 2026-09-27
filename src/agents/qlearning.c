#include "qlearning.h"
#include "rng.h"

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Codifica o estado do tabuleiro em um único inteiro na base 3 sob a ótica do jogador.
 *
 * Mapeamento de cada casa (0 a 8):
 *  - 0: Célula vazia (EMPTY);
 *  - 1: Peça própria do jogador avaliado (player);
 *  - 2: Peça do oponente (opponent).
 *
 * Fórmula: State = Sum_{i=0..8} (digit[i] * 3^i).
 * Produz um valor no intervalo [0, 3^9 - 1] = [0, 19682].
 * Como a codificação é relativa ao jogador, a mesma tabela Q é usada tanto para 'X' quanto para 'O'.
 *
 * @param board Tabuleiro a codificar.
 * @param player Jogador de referência para a perspectiva.
 * @return Inteiro identificador único do estado (0 a 19682).
 */
static int encode_state(const Board *board, char player)
{
    int value=0, factor=1;
    char opponent=other_player(player);

    for (int i=0;i<BOARD_SIZE;++i) {
        int digit=0;
        if (board->cells[i] == player) digit=1;
        else if (board->cells[i] == opponent) digit=2;
        value += digit*factor;
        factor *= 3;
    }
    return value;
}

/**
 * @brief Retorna um ponteiro direto para a célula da tabela Q correspondente ao par (state, action).
 *
 * @param agent Agente Q-Learning.
 * @param state Índice do estado codificado (0 a 19682).
 * @param action Ação executada (índice da casa de 0 a 8).
 * @return Ponteiro para o valor double de Q(state, action).
 */
static double *qcell(QLearningAgent *agent, int state, int action)
{
    return &agent->q[state*Q_ACTION_COUNT+action];
}

/**
 * @brief Calcula o valor máximo Q(s', a') entre todas as ações válidas disponíveis no próximo estado.
 *
 * @param agent Agente Q-Learning.
 * @param board Tabuleiro no estado seguinte s'.
 * @param player Jogador de referência.
 * @return O maior valor Q disponível no estado s', ou 0.0 se não houver jogadas válidas.
 */
static double max_q(const QLearningAgent *agent, const Board *board, char player)
{
    int state=encode_state(board,player);
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    if (count == 0) return 0.0;

    double best=agent->q[state*Q_ACTION_COUNT+moves[0]];
    for (int i=1;i<count;++i) {
        double value=agent->q[state*Q_ACTION_COUNT+moves[i]];
        if (value > best) best=value;
    }
    return best;
}

/**
 * @brief Seleciona uma ação usando a política epsilon-greedy.
 *
 * Com probabilidade epsilon, escolhe uma ação aleatória uniforme dentre as válidas;
 * caso contrário, seleciona a ação legal que maximiza Q(s, a).
 *
 * @param agent Agente Q-Learning.
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param epsilon Taxa de exploração (0.0 = totalmente ganancioso).
 * @return Ação escolhida (0 a 8) ou -1 se não houver jogadas livres.
 */
static int choose_action(QLearningAgent *agent, Board *board, char player, double epsilon)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    if (count == 0) return -1;

    if (epsilon > 0.0 && rng_unit() < epsilon)
        return moves[rng_index((size_t)count)];

    int state=encode_state(board,player);
    int best_move=moves[0];
    double best=*qcell(agent,state,best_move);

    for (int i=1;i<count;++i) {
        double value=*qcell(agent,state,moves[i]);
        if (value > best) {
            best=value;
            best_move=moves[i];
        }
    }
    return best_move;
}

/**
 * @brief Realiza um movimento estocástico aleatório entre as casas livres disponíveis.
 *
 * Usado como oponente durante o treinamento do Q-Learning.
 *
 * @param board Tabuleiro atual.
 * @return Movimento aleatório (0 a 8) ou -1 se cheio.
 */
static int random_move(Board *board)
{
    int moves[BOARD_SIZE];
    int count=board_available_moves(board,moves);
    return count > 0 ? moves[rng_index((size_t)count)] : -1;
}

/**
 * @brief Aplica a atualização temporal de Bellman para a função valor-ação Q(s, a):
 *        Q(s, a) <- Q(s, a) + alpha * [ reward + gamma * next_max - Q(s, a) ]
 *
 * @param agent Agente Q-Learning.
 * @param state Estado atual codificado.
 * @param action Ação selecionada.
 * @param reward Recompensa imediata obtida (+1.0 vitória, -1.0 derrota, 0.0 empate/transição).
 * @param next_max Maior valor Q das ações possíveis no próximo estado (max_a' Q(s', a')).
 */
static void update_q(QLearningAgent *agent, int state, int action,
                     double reward, double next_max)
{
    double *value=qcell(agent,state,action);
    *value += agent->alpha*(reward+agent->gamma*next_max-*value);
}

/**
 * @brief Aloca dinamicamente a tabela Q na memória heap (19.683 x 9 doubles) e define hiperparâmetros padrão.
 *
 * Hiperparâmetros:
 *  - alpha: 0.20
 *  - gamma: 0.95
 *  - epsilon: 0.20
 *
 * @param agent Ponteiro para a estrutura QLearningAgent.
 * @return 1 se a memória foi alocada com sucesso; 0 em caso de falta de memória (NULL).
 */
int qlearning_init(QLearningAgent *agent)
{
    agent->q=calloc((size_t)Q_STATE_COUNT*Q_ACTION_COUNT,sizeof(double));
    if (agent->q == NULL) return 0;
    agent->alpha=0.20;
    agent->gamma=0.95;
    agent->epsilon=0.20;
    agent->episodes=0;
    return 1;
}

/**
 * @brief Libera a memória alocada para a tabela Q e zera o ponteiro.
 *
 * @param agent Ponteiro para a estrutura QLearningAgent.
 */
void qlearning_free(QLearningAgent *agent)
{
    if (agent == NULL) return;
    free(agent->q);
    agent->q=NULL;
}

/**
 * @brief Treina o agente Q-Learning por um número determinado de episódios contra oponente aleatório.
 *
 * Alterna entre jogar como primeiro jogador (X) e segundo jogador (O) para aprender
 * ambas as perspectivas de abertura e contra-jogo.
 *
 * @param agent Agente Q-Learning inicializado.
 * @param episodes Quantidade de partidas a simular durante o treinamento.
 */
void qlearning_train(QLearningAgent *agent, int episodes)
{
    if (agent == NULL || agent->q == NULL || episodes <= 0) return;

    for (int episode=0;episode<episodes;++episode) {
        Board board;
        board_init(&board);
        char learner=(episode%2 == 0) ? PLAYER_X : PLAYER_O;
        char opponent=other_player(learner);

        if (learner == PLAYER_O) {
            int opening=random_move(&board);
            if (opening >= 0) board_make_move(&board,opening,opponent);
        }

        while (!board_is_terminal(&board)) {
            int state=encode_state(&board,learner);
            int action=choose_action(agent,&board,learner,agent->epsilon);
            if (action < 0) break;
            board_make_move(&board,action,learner);

            if (board_winner(&board) == learner) {
                update_q(agent,state,action,1.0,0.0);
                break;
            }
            if (board_is_full(&board)) {
                update_q(agent,state,action,0.0,0.0);
                break;
            }

            int response=random_move(&board);
            if (response >= 0) board_make_move(&board,response,opponent);

            if (board_winner(&board) == opponent) {
                update_q(agent,state,action,-1.0,0.0);
                break;
            }
            if (board_is_full(&board)) {
                update_q(agent,state,action,0.0,0.0);
                break;
            }

            update_q(agent,state,action,0.0,max_q(agent,&board,learner));
        }
        ++agent->episodes;
    }
}

/**
 * @brief Seleciona a melhor ação segundo a tabela Q em modo puramente exploratório/ganancioso (epsilon=0.0).
 *
 * @param board Tabuleiro atual.
 * @param player Jogador da vez.
 * @param context Ponteiro para a estrutura QLearningAgent.
 * @return Posição escolhida (0 a 8) ou -1 se inválido.
 */
int agent_qlearning_move(Board *board, char player, void *context)
{
    QLearningAgent *agent=context;
    if (agent == NULL || agent->q == NULL) return -1;
    return choose_action(agent,board,player,0.0);
}

/**
 * @brief Salva a tabela Q completa e o contador de episódios em formato binário compacto.
 *
 * @param agent Agente cujos dados serão gravados.
 * @param path Caminho do arquivo binário (ex: "data/qtable.bin").
 * @return 1 se a escrita foi concluída com sucesso; 0 caso ocorra erro.
 */
int qlearning_save(const QLearningAgent *agent, const char *path)
{
    FILE *file=fopen(path,"wb");
    if (file == NULL) return 0;

    if (fwrite(&agent->episodes,sizeof(agent->episodes),1,file) != 1 ||
        fwrite(agent->q,sizeof(double),(size_t)Q_STATE_COUNT*Q_ACTION_COUNT,file) !=
        (size_t)Q_STATE_COUNT*Q_ACTION_COUNT) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}

/**
 * @brief Carrega os dados da tabela Q e contador de episódios a partir de arquivo binário.
 *
 * @param agent Agente já inicializado com tabela Q alocada.
 * @param path Caminho do arquivo a ser lido.
 * @return 1 se lido com sucesso e integridade preservada; 0 em caso de erro.
 */
int qlearning_load(QLearningAgent *agent, const char *path)
{
    FILE *file=fopen(path,"rb");
    if (file == NULL) return 0;

    int episodes=0;
    if (fread(&episodes,sizeof(episodes),1,file) != 1 ||
        fread(agent->q,sizeof(double),(size_t)Q_STATE_COUNT*Q_ACTION_COUNT,file) !=
        (size_t)Q_STATE_COUNT*Q_ACTION_COUNT) {
        fclose(file);
        return 0;
    }

    agent->episodes=episodes;
    fclose(file);
    return 1;
}
