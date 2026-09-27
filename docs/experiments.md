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
- podas realizadas pelo Alpha-Beta.

Os resultados são anexados a `results/experiments.csv`.

## Experimentos sugeridos

1. Aleatório × Heurístico — estabelecer baseline.
2. Minimax × Aleatório — verificar invencibilidade do Minimax.
3. Minimax × Alpha-Beta — comparar decisões e custo de busca.
4. Samuel-style × Heurístico — observar efeito do self-play.
5. Genético × Heurístico — avaliar evolução dos pesos.
6. Q-Learning × Minimax — medir quão perto o agente aprendido chega da política ótima.

Para comparar métodos de aprendizagem, repita treinamentos com sementes diferentes e não conclua a partir de uma única execução.
