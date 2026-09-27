# Arquitetura do laboratório

O projeto separa três responsabilidades:

1. **Motor do jogo** (`src/game.c`): conhece tabuleiro, regras, vitória, empate e entrada/saída.
2. **Agentes** (`src/agents/`): recebem um estado e escolhem uma jogada. Eles não implementam regras próprias.
3. **Laboratório** (`src/registry.c` e `src/experiment.c`): instancia agentes, executa IA × IA e coleta métricas.
4. **Aleatoriedade** (`src/rng.c`): fornece um PRNG único para todos os agentes estocásticos, com seed explícita e reprodução determinística da sequência.
5. **Entrada numérica** (`src/input.c`): faz parsing estrito de inteiros e seeds, separando validação textual da interface interativa.

Essa separação permite comparar algoritmos sobre exatamente o mesmo jogo e controlar a aleatoriedade sem espalhar dependências de `rand()` pelo código.

## Interface comum

Os agentes clássicos usam a assinatura:

```c
int agente(Board *board, char player, void *context);
```

O `context` transporta dados específicos, como métricas de busca, pesos aprendidos ou a tabela Q.

## Gerador pseudoaleatório

`include/rng.h` expõe a interface comum de aleatoriedade. A implementação usa SplitMix64 e oferece:

- seed explícita de 64 bits;
- seed automática para uso interativo;
- valores inteiros de 32/64 bits;
- índice uniforme com rejeição para evitar viés de módulo;
- valor `double` uniforme em `[0,1)`.

Testes usam seeds fixas; a aplicação interativa usa seed automática por padrão.

## Persistência

Arquivos gerados localmente não são versionados:

- `data/samuel_weights.dat`
- `data/genetic_weights.dat`
- `data/qtable.bin`
- `results/experiments.csv`

Os diretórios existem no repositório por meio de `.gitkeep`.

## Regra de projeto

Nenhum agente pode alterar permanentemente o tabuleiro apenas para analisar uma jogada. Algoritmos de busca devem sempre desfazer movimentos simulados antes de retornar.

## Documentação Detalhada dos Métodos

Para a especificação formal, assinaturas, parâmetros, tipos de dados e análise de cada método e agente, consulte a [Referência Completa dos Métodos](referencia_metodos.md).