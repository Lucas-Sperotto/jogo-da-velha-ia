# Algoritmos implementados

## Aleatório

Baseline sem estratégia. Escolhe uniformemente uma casa livre.

## Heurístico

Prioridades explícitas:

1. vencer imediatamente;
2. bloquear vitória imediata;
3. ocupar o centro;
4. ocupar um canto;
5. escolher a primeira casa livre.

## Minimax

Explora recursivamente o jogo até estados terminais. Vitória da IA recebe valor positivo, derrota valor negativo e empate zero. A profundidade favorece vitórias mais rápidas e derrotas mais tardias.

Os testes automatizados exploram todas as respostas possíveis do adversário contra o Minimax jogando como `X` e como `O`. Em ambos os lados, verificam que o adversário não consegue forçar uma vitória e também confirmam que a função de seleção não altera permanentemente o tabuleiro durante a análise.

## Alpha-Beta

Preserva a optimalidade do Minimax, mas elimina ramos que não podem modificar o valor da decisão final. O laboratório contabiliza nós visitados e podas.

Os testes verificam uma decisão representativa em igualdade com o Minimax, confirmam que há podas e redução de nós na árvore inicial e exploram todas as respostas possíveis do adversário contra o Alpha-Beta tanto como `X` quanto como `O`. Como podem existir várias jogadas igualmente ótimas em uma mesma posição, a propriedade essencial é preservar o valor ótimo, não necessariamente um desempate universal entre implementações diferentes.

## Samuel-style

Adaptação didática inspirada nas ideias de Arthur Samuel: uma função linear avalia características do tabuleiro e os pesos são atualizados após partidas de self-play.

Não é uma reprodução literal do programa histórico de damas de Samuel.

Características atuais:

- termo de viés;
- diferença de vitórias imediatas;
- controle do centro;
- diferença de cantos;
- potencial de linhas;
- diferença de número de peças.

## Algoritmo Genético

Cada indivíduo é um vetor com os mesmos seis pesos. A população é avaliada em partidas, preserva uma elite e produz descendentes por cruzamento e mutação.

## Q-Learning

Usa uma tabela `Q(estado, ação)`. Um tabuleiro possui `3^9 = 19.683` codificações possíveis e nove ações. A codificação é feita da perspectiva do agente, permitindo reutilizar a mesma tabela quando ele joga como `X` ou `O`.
