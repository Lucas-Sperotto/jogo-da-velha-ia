# Referência Completa dos Métodos e Módulos — Jogo da Velha IA

Este documento apresenta a análise técnica detalhada, especificação formal de métodos, assinaturas, fluxos de controle, complexidades e garantias de integridade do projeto.

---

## Sumário

1. [Visão Geral da Arquitetura](#1-visão-geral-da-arquitetura)
2. [Motor do Jogo (`include/game.h`, `src/game.c`)](#2-motor-do-jogo)
3. [Agentes de Busca Clássica (`include/agents.h`)](#3-agentes-de-busca-clássica)
   - [3.1 Aleatório (`src/agents/random.c`)](#31-agente-aleatório)
   - [3.2 Heurístico (`src/agents/heuristic.c`)](#32-agente-heurístico)
   - [3.3 Minimax (`src/agents/minimax.c`)](#33-agente-minimax)
   - [3.4 Alpha-Beta (`src/agents/alphabeta.c`)](#34-agente-alpha-beta)
4. [Módulo de Aprendizado e Características (`include/learning.h`, `src/agents/features.c`)](#4-módulo-de-aprendizado-e-características)
5. [Agente Samuel-Style (`include/samuel.h`, `src/agents/samuel.c`)](#5-agente-samuel-style)
6. [Algoritmo Genético (`include/genetic.h`, `src/agents/genetic.c`)](#6-algoritmo-genético)
7. [Q-Learning Tabular (`include/qlearning.h`, `src/agents/qlearning.c`)](#7-q-learning-tabular)
8. [Registro e Ciclo de Vida (`include/registry.h`, `src/registry.c`)](#8-registro-e-ciclo-de-vida)
9. [Laboratório de Experimentos (`include/experiment.h`, `src/experiment.c`)](#9-laboratório-de-experimentos)
10. [Interface e Entrada/Saída (`src/main.c`)](#10-interface-e-entradasaída)
11. [Verificação de Segurança de Memória e Testes](#11-verificação-de-segurança-de-memória-e-testes)

---

## 1. Visão Geral da Arquitetura

O sistema é construído sobre o princípio de responsabilidade única e desacoplamento através de ponteiros de função (`MoveSelector`):

```text
                  ┌──────────────────────┐
                  │      src/main.c      │ (Menu CLI e Coordenação)
                  └──────────┬───────────┘
                             │
            ┌────────────────┴────────────────┐
            ▼                                 ▼
┌──────────────────────┐           ┌──────────────────────┐
│    src/registry.c    │           │   src/experiment.c   │
│ (Gerenciador e Carga)│           │ (Torneios e Métricas)│
└───────────┬──────────┘           └──────────┬───────────┘
            │                                 │
            ├─────────────────────────────────┘
            ▼
┌──────────────────────┐
│     src/game.c       │ ◄── Compartilhado por todos os agentes
│  (Regras e Estados)  │
└──────────▲───────────┘
           │
 ┌─────────┴──────────────────────────────────────────┐
 │ Agentes:                                           │
 │ • random.c      • heuristic.c   • minimax.c        │
 │ • alphabeta.c   • features.c    • samuel.c         │
 │ • genetic.c     • qlearning.c                      │
 └────────────────────────────────────────────────────┘
```

A assinatura unificada que conecta os agentes ao motor é:
```c
typedef int (*MoveSelector)(Board *board, char player, void *context);
```

---

## 2. Motor do Jogo

**Arquivos:** `include/game.h`, `src/game.c`

### Constantes e Estruturas
- `BOARD_SIZE` (9): Número de células do tabuleiro 3×3.
- `EMPTY` (`' '`), `PLAYER_X` (`'X'`), `PLAYER_O` (`'O'`).
- `Board`:
  ```c
  typedef struct {
      char cells[BOARD_SIZE];
  } Board;
  ```

### Funções do Motor

#### `void board_init(Board *board)`
- **Objetivo:** Inicializa as 9 casas com o caractere `EMPTY` (' ').
- **Pré-condições:** Ponteiro `board` válido (caso `NULL`, retorna sem operação).
- **Complexidade:** $\mathcal{O}(1)$ no tempo e espaço.

#### `int board_is_valid_move(const Board *board, int move)`
- **Objetivo:** Valida se uma jogada é legal.
- **Critérios:** `board != NULL`, $0 \le move < 9$ e `board->cells[move] == EMPTY`.
- **Retorno:** `1` se válida, `0` caso contrário.

#### `int board_make_move(Board *board, int move, char player)`
- **Objetivo:** Aplica a jogada no tabuleiro.
- **Validações:** Verifica se o movimento é válido e se `player` é estritamente `PLAYER_X` ou `PLAYER_O`.
- **Retorno:** `1` em sucesso, `0` em falha.

#### `void board_undo_move(Board *board, int move)`
- **Objetivo:** Reverte uma jogada, atribuindo `EMPTY` à posição informada.
- **Garantia:** Permite buscas em árvore recursiva sem duplicar memória ou alterar o tabuleiro permanentemente.

#### `char board_winner(const Board *board)`
- **Objetivo:** Detecta se há uma linha formada por 3 peças idênticas.
- **Implementação:** Consulta a tabela estática com as 8 combinações vencedoras:
  $$\{(0,1,2), (3,4,5), (6,7,8), (0,3,6), (1,4,7), (2,5,8), (0,4,8), (2,4,6)\}$$
- **Retorno:** `PLAYER_X`, `PLAYER_O` ou `EMPTY`.

#### `int board_is_full(const Board *board)`
- **Objetivo:** Verifica se todas as 9 células estão preenchidas.
- **Retorno:** `1` se cheio, `0` caso contrário.

#### `int board_is_terminal(const Board *board)`
- **Objetivo:** Determina se a partida encerrou (vitória de alguém ou empate).
- **Retorno:** `board_winner(board) != EMPTY || board_is_full(board)`.

#### `int board_available_moves(const Board *board, int moves[BOARD_SIZE])`
- **Objetivo:** Lista os índices das casas livres.
- **Parâmetros:** Se `moves != NULL`, preenche o array; se `moves == NULL`, apenas contabiliza.
- **Retorno:** Número de movimentos válidos disponíveis ($0$ a $9$).

#### `char other_player(char player)`
- **Objetivo:** Alternância determinística de turno (`X` $\to$ `O`, `O` $\to$ `X`).

#### `int read_human_move(const Board *board, char player)`
- **Objetivo:** Leitura segura de entrada do jogador no terminal via `fgets` e `strtol`.
- **Tratamento:** Rejeita caracteres inválidos, valores fora do intervalo $[1, 9]$, posições ocupadas e trata `EOF` retornando `-1`.

#### `void play_human_vs_human(void)`
- **Objetivo:** Laço de jogo para dois jogadores humanos no terminal.

#### `void play_human_vs_agent(const char *agent_name, MoveSelector selector, void *context)`
- **Objetivo:** Laço de jogo Humano (`X`) versus IA (`O`), invocando `selector` a cada turno do computador.

---

## 3. Agentes de Busca Clássica

**Arquivos:** `include/agents.h`, `src/agents/*.c`

### 3.1 Agente Aleatório
- **Função:** `agent_random_move(Board *board, char player, void *context)`
- **Algoritmo:** Invoca `board_available_moves`. Se `count > 0`, sorteia um índice uniforme com `rng_index(count)` e retorna a casa correspondente.
- **Propósito didático:** Serve como *baseline* nulo (desempenho mínimo).

### 3.2 Agente Heurístico
- **Função:** `agent_heuristic_move(Board *board, char player, void *context)`
- **Árvore de Decisão:**
  1. *Vitória Imediata:* Se existe jogada em que o agente vence na próxima ação, escolhe-a.
  2. *Bloqueio Imediato:* Se o oponente tem vitória no próximo turno, bloqueia a casa.
  3. *Centro:* Ocupa a casa central ($4$) se livre.
  4. *Cantos:* Ocupa o primeiro canto livre $\{0, 2, 6, 8\}$.
  5. *Primeira Livre:* Seleciona a primeira casa disponível.
- **Complexidade:** $\mathcal{O}(1)$ (máximo de 9 checagens de vitória por turno).

### 3.3 Agente Minimax
- **Funções:**
  - `minimax(Board *board, char root_player, char turn, int depth, SearchStats *stats)` (estática)
  - `agent_minimax_move(Board *board, char player, void *context)`
- **Avaliação de Estados Terminais:**
  - Vitória da raiz: $+10 - \text{depth}$ (prioriza vitórias mais rápidas).
  - Vitória do adversário: $\text{depth} - 10$ (adia derrotas ao máximo).
  - Empate: $0$.
- **Propriedades:** Garante jogo perfeito. Os testes exploram todas as respostas possíveis do adversário e verificam que o agente não pode ser forçado a perder nem como `X` nem como `O`.
- **Estatísticas:** Contabiliza o número total de nós visitados via `stats->nodes`.

### 3.4 Agente Alpha-Beta
- **Funções:**
  - `alphabeta(Board *board, char root_player, char turn, int depth, int alpha, int beta, SearchStats *stats)` (estática)
  - `agent_alphabeta_move(Board *board, char player, void *context)`
- **Poda:**
  - Poda quando $\alpha \ge \beta$.
  - Incrementa `stats->prunes` quando um corte ocorre.
- **Equivalência Matemática:** Preserva o valor ótimo do Minimax, porém visitando menos nós graças às podas. Em posições com várias jogadas igualmente ótimas, implementações com critérios de desempate diferentes podem selecionar movimentos distintos sem perder optimalidade.

---

## 4. Módulo de Aprendizado e Características

**Arquivos:** `include/learning.h`, `src/agents/features.c`

Para permitir aprendizado supervisionado, auto-jogo e evolução genética, o estado do tabuleiro é mapeado em um espaço contínuo de características.

### Vetor de Características ($F \in \mathbb{R}^6$)
1. $F_0 = 1.0$ (Termo de viés / intercepto constante).
2. $F_1 = \text{wins}(\text{player}) - \text{wins}(\text{opponent})$ (Diferença de vitórias imediatas em 1 lance).
3. $F_2 \in \{-1.0, 0.0, 1.0\}$ (Controle da casa central: $+1$ se do jogador, $-1$ se do oponente, $0$ se vazia).
4. $F_3 = \text{corners}(\text{player}) - \text{corners}(\text{opponent})$ (Diferença de peças nos 4 cantos).
5. $F_4 = \text{line\_pot}(\text{player}) - \text{line\_pot}(\text{opponent})$ (Potencial de linhas abertas não bloqueadas).
6. $F_5 = \text{pieces}(\text{player}) - \text{pieces}(\text{opponent})$ (Diferença de material no tabuleiro).

### Funções
- `extract_features(const Board *board, char player, double out[FEATURE_COUNT])`:
  Preenche o vetor `out` calculando as 6 métricas acima.
- `evaluate_position(const Board *board, char player, const StrategyWeights *weights)`:
  Calcula o produto escalar:
  $$V(s) = \sum_{i=0}^{5} W_i \cdot F_i(s)$$
- `weighted_best_move(Board *board, char player, const StrategyWeights *weights, double epsilon)`:
  - Aplica exploração $\varepsilon$-greedy: com probabilidade $\varepsilon$, seleciona jogada aleatória.
  - Caso contrário, executa busca rasa de 2 níveis (1-ply do jogador + 1-ply de contra-resposta adversária):
    - Se a jogada vence imediatamente, executa-a sem hesitar.
    - Simula o movimento e busca a pior resposta do adversário (*minimax local* com a função linear).
    - Escolhe o movimento que maximiza a pior resposta admissível.

---

## 5. Agente Samuel-Style

**Arquivos:** `include/samuel.h`, `src/agents/samuel.c`

Inspirado nos estudos seminais de Arthur Samuel (1959) para Damas.

### Estrutura e Parâmetros
```c
typedef struct {
    StrategyWeights weights; // Pesos lineares
    double learning_rate;    // Default: 0.01
    double exploration;      // Default: 0.15 (durante treino)
} SamuelAgent;
```

### Algoritmo de Treinamento (`samuel_train`)
1. **Auto-jogo (Self-Play):** O agente joga contra si mesmo com exploração $\varepsilon = 0.15$.
2. **Histórico:** Grava a trajetória dos estados visitados e o jogador da vez.
3. **Alvo de Recompensa:**
   - Vitória: $Target = +10.0$
   - Derrota: $Target = -10.0$
   - Empate: $Target = 0.0$
4. **Regra de Atualização (Gradiente Descendente / Regra Delta):**
   $$Error = Target - \hat{V}(s)$$
   $$W_i \leftarrow W_i + \eta \cdot Error \cdot F_i(s)$$
   onde $\eta = 0.01$ é o `learning_rate`.

### Persistência
- `samuel_save`: Exporta os pesos em ASCII com precisão de 17 dígitos (`%.17g`).
- `samuel_load`: Recupera os pesos salvos em `data/samuel_weights.dat`.

---

## 6. Algoritmo Genético

**Arquivos:** `include/genetic.h`, `src/agents/genetic.c`

Evolui populações de vetores de pesos usando princípios neodarwinianos.

### Parâmetros Evolutivos
- `POPULATION_SIZE`: 32 indivíduos.
- `ELITE_COUNT`: 4 indivíduos preservados integralmente por geração.
- `EVAL_GAMES`: 12 partidas de avaliação por indivíduo (enfrentando agentes Heurísticos e Aleatórios, alternando como `X` e `O`).

### Funções e Mecanismos
- **Aptidão (Fitness):**
  - Vitória: $+3.0$
  - Empate: $+1.0$
  - Derrota: $-2.0$
- **Seleção:** Seleção por truncamento nos 50% melhores da população, usando `rng_index(POPULATION_SIZE / 2)` para escolher os progenitores.
- **Cruzamento (Crossover Aritmético):**
  $$Child_i = \lambda \cdot ParentA_i + (1 - \lambda) \cdot ParentB_i, \quad \lambda \sim \mathcal{U}(0, 1)$$
- **Mutação:** Taxa de mutação de 25% por gene, adicionando um valor perturbador $\Delta \sim \mathcal{U}(-1.0, 1.0)$.
- **`genetic_train(GeneticAgent *agent, int generations)`:**
  Executa o laço geracional, atualiza o melhor indivíduo histórico e registra o progresso da aptidão.

---

## 7. Q-Learning Tabular

**Arquivos:** `include/qlearning.h`, `src/agents/qlearning.c`

Implementação exata do algoritmo de Aprendizado por Reforço livre de modelo (Model-Free RL).

### Codificação do Espaço de Estados
O tabuleiro é mapeado em base 3 em relação ao agente que aprende:
- $0$: Célula vazia.
- $1$: Peça do próprio agente.
- $2$: Peça do adversário.

Número de estados: $3^9 = 19.683$.
Número de ações: $9$.
Tamanho da tabela: $19.683 \times 9 = 177.147$ valores `double` ($\approx 1.4$ MB na heap).

### Equação de Bellman para Q-Learning
A cada transição $(s, a, r, s')$:
$$Q(s, a) \leftarrow Q(s, a) + \alpha \left[ r + \gamma \max_{a'} Q(s', a') - Q(s, a) \right]$$
- $\alpha = 0.20$ (taxa de aprendizado)
- $\gamma = 0.95$ (fator de desconto temporal)
- Recompensas imediatas: $+1.0$ (vitória), $-1.0$ (derrota), $0.0$ (empate ou transição intermediária).

### Gerenciamento de Memória e I/O
- `qlearning_init`: Aloca via `calloc` zerado na memória heap.
- `qlearning_free`: Libera o bloco alocado com proteção de ponteiro nulo.
- `qlearning_save` / `qlearning_load`: Serialização binária direta de alto desempenho (`fwrite`/`fread`).

---

## 8. Registro e Ciclo de Vida

**Arquivos:** `include/registry.h`, `src/registry.c`

Centraliza a instanciação, configuração polimórfica e descarte de recursos de qualquer agente através da enumeração `AgentKind` e estrutura `RuntimeAgent`.

### Ciclo de Vida
```c
RuntimeAgent agent;
runtime_agent_init(&agent, AGENT_QLEARNING); // Carrega do disco ou treina se ausente
// ... partidas ...
runtime_agent_destroy(&agent); // Libera heap e recursos
```

### Inicialização Automática e Cache de Modelos
Quando um agente treinado (Samuel, Genético ou Q-Learning) é solicitado, o subsistema:
1. Verifica se existe o arquivo de modelo em `data/`.
2. Se existir, carrega os pesos/tabela imediatamente.
3. Se não existir, treina o agente automaticamente e persiste o modelo no disco para que as execuções subsequentes sejam instantâneas.

---

## 9. Laboratório de Experimentos

**Arquivos:** `include/experiment.h`, `src/experiment.c`

Fornece infraestrutura estatística para comparação empírica rigorosa entre quaisquer pares de agentes.

### Alternância de Turnos
Para eliminar a vantagem inerente do jogador que inicia (`PLAYER_X`):
- Em partidas de índice par ($i = 0, 2, 4, \dots$): Agente A é `X` e Agente B é `O`.
- Em partidas de índice ímpar ($i = 1, 3, 5, \dots$): Agente B é `X` e Agente A é `O`.

### Métricas Coletadas
- Vitórias de A, Vitórias de B, Empates.
- Contagem total de lances e média de lances por jogo.
- Nós visitados e podas realizadas em buscas Minimax e Alpha-Beta.
- Registro em formato tabular CSV (`results/experiments.csv`) com criação automática de cabeçalho.

---

## 10. Interface e Entrada/Saída

**Arquivo:** `src/main.c`

Oferece um menu interativo completo no console:
1. Humano × Humano
2. Humano × Aleatório
3. Humano × Heurístico
4. Humano × Minimax
5. Humano × Alpha-Beta
6. IA estilo Arthur Samuel
7. Algoritmo Genético
8. Q-Learning
9. IA × IA (com visualização lance a lance no terminal)
10. Executar experimento (torneio estatístico com geração de CSV)

---

## 11. Verificação de Segurança de Memória e Testes

### Verificação do Compilador
Compilado com todas as diretivas de alerta rigorosas do padrão C11:
```bash
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic
```
**Resultado:** Zero warnings e compilação limpa em todos os alvos e suítes de teste.

### Verificação de Vazamento de Memória (Valgrind Memcheck)
Executado com `--leak-check=full --error-exitcode=1` sobre todas as 10 suítes de testes:
- `tests/test_input`: valida parsing estrito, limites, sufixos inválidos e overflow.
- `tests/test_rng`: valida sequência conhecida, reseed determinístico e faixas das funções de amostragem.
- `tests/test_game`: 0 erros, 0 vazamentos.
- `tests/test_agents`: 0 erros, 0 vazamentos.
- `tests/test_minimax`: 0 erros, 0 vazamentos; a árvore de respostas do adversário é explorada para verificar invencibilidade como `X` e como `O`.
- `tests/test_alphabeta`: 0 erros, 0 vazamentos; os testes verificam podas, redução de nós, uma decisão representativa em igualdade com Minimax e invencibilidade como `X` e como `O`.
- `tests/test_samuel`: 0 erros, 0 vazamentos (atualização de pesos por gradiente validada).
- `tests/test_genetic`: 0 erros, 0 vazamentos (ciclo geracional e fitness validados).
- `tests/test_qlearning`: 0 erros, 0 vazamentos (alocação de 1,4 MB e desalocação limpa comprovada).
- `tests/test_experiment`: 0 erros, 0 vazamentos (torneio Minimax vs Aleatório comprovando 0 vitórias do agente aleatório).

**Resumo da verificação:** Todos os blocos da heap foram devidamente liberados. Nenhuma violação de acesso a ponteiros ou leitura não inicializada foi detectada.


### Integração Contínua

O workflow `.github/workflows/ci.yml` executa automaticamente:

- compilação e testes com GCC usando warnings como erro (`-Werror`);
- compilação e testes com Clang usando warnings como erro;
- ASan e UBSan;
- Valgrind Memcheck nas dez suítes.

Assim, as verificações de compilação, comportamento e segurança de memória deixam de depender apenas da execução manual local.


---

## 12. Gerador Pseudoaleatório e Reprodutibilidade

**Arquivos:** `include/rng.h`, `src/rng.c`

Todos os componentes estocásticos usam uma única interface de PRNG. A implementação é baseada em **SplitMix64**, com operações unsigned de 64 bits, o que define uma sequência determinística para uma determinada seed.

### Operações principais

- `rng_seed(seed)`: reinicializa a sequência com uma seed explícita;
- `rng_seed_auto()`: cria uma seed para uso interativo a partir do relógio do sistema;
- `rng_get_seed()`: informa a seed instalada;
- `rng_next_u64()` e `rng_next_u32()`: geram inteiros pseudoaleatórios;
- `rng_index(bound)`: escolhe um índice em `[0,bound)` com rejeição, evitando viés de módulo;
- `rng_unit()`: produz um `double` uniforme em `[0,1)`.

O teste `tests/test_rng.c` contém um vetor de referência para a seed `42`, garantindo que mudanças acidentais no algoritmo sejam detectadas. Os testes de Samuel-style, Algoritmo Genético e Q-Learning também definem seeds explícitas para que o CI não dependa do relógio.

### Experimentos

`run_experiment_seeded` reinicializa o PRNG antes das partidas e grava a seed no `ExperimentResult`. O CSV usa a coluna `seed` como primeira coluna, permitindo registrar e repetir condições estocásticas.


---

## 13. Parsing Estrito de Entrada

**Arquivos:** `include/input.h`, `src/input.c`

A conversão numérica foi extraída de `main.c` para permitir testes independentes da interface interativa.

- `parse_int_range`: aceita um inteiro completo, permite espaços em branco nas bordas, rejeita sufixos como `1abc`, decimais como `2.5`, overflow e valores fora da faixa.
- `parse_u64`: interpreta seeds de 64 bits, incluindo `UINT64_MAX`, e rejeita números negativos, overflow e caracteres extras.

O menu continua usando `fgets`, mas delega a interpretação ao módulo de entrada. Isso evita que uma entrada parcialmente numérica seja aceita silenciosamente.


---

## 14. Carregamento atômico de modelos

Os carregadores de Samuel-style, Algoritmo Genético e Q-Learning validam os dados em estruturas temporárias antes de alterar o agente em memória.

- `samuel_load`: lê todos os pesos em `StrategyWeights` temporário e só então publica o novo vetor;
- `genetic_load`: lê gerações, fitness e cromossomo em um `GeneticAgent` temporário;
- `qlearning_load`: lê a tabela Q em buffer temporário e só realiza `memcpy` após leitura completa.

Arquivos truncados ou malformados retornam falha sem deixar estado parcialmente carregado. As suítes correspondentes criam arquivos truncados de propósito para validar essa propriedade.
