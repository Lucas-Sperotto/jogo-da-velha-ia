# Arquitetura do laboratório

O projeto separa três responsabilidades:

1. **Motor do jogo** (`src/game.c`): conhece tabuleiro, regras, vitória, empate e entrada/saída.
2. **Agentes** (`src/agents/`): recebem um estado e escolhem uma jogada. Eles não implementam regras próprias.
3. **Laboratório** (`src/registry.c` e `src/experiment.c`): instancia agentes, executa IA × IA e coleta métricas.

Essa separação permite comparar algoritmos sobre exatamente o mesmo jogo.

## Interface comum

Os agentes clássicos usam a assinatura:

```c
int agente(Board *board, char player, void *context);
```

O `context` transporta dados específicos, como métricas de busca, pesos aprendidos ou a tabela Q.

## Persistência

Arquivos gerados localmente não são versionados:

- `data/samuel_weights.dat`
- `data/genetic_weights.dat`
- `data/qtable.bin`
- `results/experiments.csv`

Os diretórios existem no repositório por meio de `.gitkeep`.

## Regra de projeto

Nenhum agente pode alterar permanentemente o tabuleiro apenas para analisar uma jogada. Algoritmos de busca devem sempre desfazer movimentos simulados antes de retornar.
