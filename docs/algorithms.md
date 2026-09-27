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

O teste automatizado explora todas as respostas humanas possíveis contra o Minimax jogando de `O` e verifica que o humano não consegue forçar uma vitória.

## Alpha-Beta

Produz a mesma decisão ótima do Minimax, mas elimina ramos que não podem modificar a decisão final. O laboratório contabiliza nós visitados e podas.

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
