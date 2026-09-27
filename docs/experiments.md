# Experimentos

A opção **9 — IA × IA** executa uma partida visual entre dois agentes escolhidos.

A opção **10 — Executar experimento** executa várias partidas e alterna quem começa para reduzir o viés do primeiro jogador.

São registrados:

- partidas;
- vitórias de A;
- vitórias de B;
- empates;
- total e média de jogadas;
- nós visitados por Minimax/Alpha-Beta;
- podas realizadas pelo Alpha-Beta;
- seed pseudoaleatória usada no experimento.

Os resultados são anexados a `results/experiments.csv`. A primeira coluna é `seed`.

## Reprodutibilidade

O projeto usa um PRNG central baseado em **SplitMix64** (`src/rng.c`). Os agentes não chamam `rand()` diretamente.

Na opção **10 — Executar experimento**, é possível informar:

- `0` para gerar uma seed automaticamente;
- qualquer inteiro positivo representável em 64 bits para usar uma seed explícita.

Antes das partidas, o executor reinicializa o PRNG com essa seed. Assim, com a mesma versão do programa, os mesmos agentes/modelos e a mesma seed, as decisões estocásticas do experimento são reproduzidas.

A seed também é aplicada antes da inicialização dos agentes no menu de experimentos. Isso permite repetir treinamentos estocásticos quando os modelos locais ainda não existem. Porém, arquivos persistidos em `data/` também fazem parte do estado experimental: para reproduzir um treinamento do zero, use o mesmo estado inicial desses arquivos (por exemplo, removendo/arquivando modelos anteriores de forma controlada).

> CSVs produzidos por versões anteriores não possuem a coluna `seed`. Como `results/` é local e ignorado pelo Git, recomenda-se arquivar ou remover o CSV antigo antes de iniciar uma nova série com o esquema atual.

## Experimentos sugeridos

1. Aleatório × Heurístico — estabelecer baseline.
2. Minimax × Aleatório — verificar invencibilidade do Minimax.
3. Minimax × Alpha-Beta — comparar decisões e custo de busca.
4. Samuel-style × Heurístico — observar efeito do self-play.
5. Genético × Heurístico — avaliar evolução dos pesos.
6. Q-Learning × Minimax — medir quão perto o agente aprendido chega da política ótima.

Para comparar métodos de aprendizagem, repita treinamentos com sementes diferentes e não conclua a partir de uma única execução.
