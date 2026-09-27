# Laboratório de Inteligência Artificial — Jogo da Velha

Projeto didático desenvolvido em **C** para estudo e comparação de diferentes técnicas de Inteligência Artificial utilizando o Jogo da Velha como ambiente experimental.

O objetivo não é apenas desenvolver um programa capaz de jogar Jogo da Velha, mas utilizar o jogo como laboratório para compreender diferentes formas de construção de agentes inteligentes.

---

## Objetivos

O projeto permitirá estudar progressivamente:

* representação de estados;
* geração de movimentos;
* regras e validação de jogadas;
* estratégias aleatórias;
* funções heurísticas;
* busca adversarial;
* algoritmo Minimax;
* poda Alpha-Beta;
* funções de avaliação;
* self-play;
* aprendizado inspirado nos trabalhos de Arthur Samuel;
* algoritmos genéticos;
* aprendizado por reforço;
* Q-Learning;
* experimentação e comparação de algoritmos.

---

## Menu principal

```text
╔══════════════════════════════════╗
║       LABORATÓRIO DE IA          ║
║          JOGO DA VELHA           ║
╠══════════════════════════════════╣
║                                  ║
║  1. Humano × Humano              ║
║  2. Humano × Aleatório           ║
║  3. Humano × Heurístico          ║
║  4. Humano × Minimax             ║
║  5. Humano × Alpha-Beta          ║
║  6. IA estilo Arthur Samuel      ║
║  7. Algoritmo Genético           ║
║  8. Q-Learning                    ║
║                                  ║
║  9. IA × IA                      ║
║ 10. Executar experimento         ║
║                                  ║
║  0. Sair                         ║
╚══════════════════════════════════╝
```

---

# Modos de jogo

## 1. Humano × Humano

Modo básico utilizado para validar o funcionamento do motor do jogo.

Dois jogadores humanos realizam as jogadas alternadamente.

Esse modo permitirá testar:

* representação do tabuleiro;
* entrada de dados;
* validação de movimentos;
* detecção de vitória;
* detecção de empate.

---

## 2. Humano × Aleatório

O computador escolhe aleatoriamente uma das posições disponíveis.

Esse agente não possui estratégia.

Ele será utilizado como **baseline** para comparação com os demais algoritmos.

---

## 3. Humano × Heurístico

O computador utiliza regras previamente definidas.

Exemplos:

1. vencer quando houver uma jogada vencedora;
2. bloquear uma vitória do adversário;
3. ocupar o centro;
4. ocupar um canto;
5. escolher uma posição disponível.

Esse modo introduz o conceito de **função heurística**.

---

## 4. Humano × Minimax

Implementação do algoritmo Minimax.

O computador analisa a árvore de possibilidades do jogo considerando que o adversário também realizará sempre sua melhor jogada.

Estados terminais podem receber valores como:

```text
Vitória da IA     +10
Empate              0
Vitória humana    -10
```

A profundidade poderá ser considerada para fazer com que a IA prefira vitórias mais rápidas e adie derrotas inevitáveis.

---

## 5. Humano × Alpha-Beta

Implementação do Minimax utilizando **poda Alpha-Beta**.

O resultado estratégico deve ser equivalente ao Minimax, porém determinadas partes da árvore deixam de ser examinadas quando já é possível determinar que elas não poderão alterar a decisão.

O programa deverá registrar:

* estados visitados;
* estados podados;
* profundidade;
* tempo de execução.

Isso permitirá comparar Minimax e Alpha-Beta experimentalmente.

---

## 6. IA estilo Arthur Samuel

Modo inspirado nos primeiros trabalhos de Arthur Samuel sobre aprendizado de máquina em jogos.

O agente utilizará uma função de avaliação baseada em características do estado.

Exemplo:

```text
Avaliação =
w1 × possibilidade_de_vencer
+
w2 × possibilidade_de_bloquear
+
w3 × controle_do_centro
+
w4 × controle_de_cantos
+
w5 × criação_de_ameaças
```

Os pesos poderão ser ajustados através de partidas realizadas pelo próprio computador.

O objetivo é investigar estratégias de **self-play** e aprendizado.

---

## 7. Algoritmo Genético

Uma estratégia será representada por um cromossomo contendo parâmetros ou pesos da função de avaliação.

Exemplo:

```text
[w1, w2, w3, w4, w5]
```

Uma população de agentes será criada.

Cada geração executará:

```text
População
    ↓
Avaliação
    ↓
Seleção
    ↓
Cruzamento
    ↓
Mutação
    ↓
Nova população
```

Os indivíduos serão avaliados através de partidas.

Possíveis componentes da função de fitness:

```text
vitória
empate
derrota
número de jogadas
desempenho contra outros agentes
```

---

## 8. Q-Learning

Implementação de aprendizado por reforço.

O agente aprenderá uma função:

```text
Q(estado, ação)
```

que representa a qualidade de realizar determinada ação em determinado estado.

Uma atualização típica é:

```text
Q(s,a) =
Q(s,a) +
alpha ×
[r + gamma × max Q(s',a') - Q(s,a)]
```

O treinamento poderá utilizar milhares de partidas automáticas.

Será possível acompanhar:

* número de episódios;
* taxa de exploração;
* vitórias;
* derrotas;
* empates;
* evolução do agente.

---

## 9. IA × IA

Permite colocar dois algoritmos diferentes para jogar entre si.

Exemplos:

```text
Aleatório × Minimax

Heurístico × Minimax

Q-Learning × Minimax

Genético × Alpha-Beta

Arthur Samuel × Q-Learning
```

Esse modo será utilizado principalmente para validação e comparação dos algoritmos.

---

## 10. Executar experimento

Executa automaticamente um grande número de partidas.

Por exemplo:

```text
Algoritmo A: Q-Learning
Algoritmo B: Minimax

Partidas: 10000
```

O programa poderá apresentar:

```text
Vitórias A:       127
Vitórias B:         0
Empates:         9873
```

Outras métricas poderão ser coletadas:

* tempo médio por jogada;
* estados pesquisados;
* estados podados;
* número médio de jogadas;
* taxa de vitória;
* taxa de empate;
* taxa de derrota.

Os resultados poderão ser armazenados em arquivos CSV para posterior análise.

---

# Estrutura

```text
src/game/
```

Contém o motor do Jogo da Velha.

```text
src/ai/
```

Contém os diferentes agentes de Inteligência Artificial.

```text
experiments/
```

Infraestrutura para execução automática de partidas.

```text
data/
```

Dados persistentes produzidos pelos algoritmos de aprendizado.

```text
results/
```

Resultados dos experimentos.

```text
tests/
```

Testes automatizados.

```text
docs/
```

Material teórico relacionado aos algoritmos.

```text
assignments/
```

Atividades propostas aos alunos.

---

# Compilação

O projeto utiliza GCC.

```bash
gcc --version
```

Para compilar:

```bash
make
```

Para executar:

```bash
./jogo_velha
```

Para remover os arquivos compilados:

```bash
make clean
```

---

# Trabalho dos alunos

Cada aluno deverá realizar um **fork** deste repositório.

Clone seu próprio fork:

```bash
git clone https://github.com/SEU-USUARIO/jogo-da-velha-ia.git
```

Entre no diretório:

```bash
cd jogo-da-velha-ia
```

Configure o repositório do professor como `upstream`:

```bash
git remote add upstream URL_DO_REPOSITORIO_ORIGINAL
```

Confira:

```bash
git remote -v
```

Antes de iniciar uma nova atividade:

```bash
git fetch upstream
git checkout main
git merge upstream/main
git push origin main
```

Cada atividade deverá ser desenvolvida em uma nova branch.

Exemplo:

```bash
git checkout -b atividade-minimax
```

Após concluir:

```bash
git add .
git commit -m "feat: implementa algoritmo minimax"
git push origin atividade-minimax
```

---

# Política de uso de Inteligência Artificial

Ferramentas de Inteligência Artificial podem ser utilizadas como apoio ao desenvolvimento.

Entretanto, o aluno deverá ser capaz de:

* explicar o funcionamento do código;
* explicar o algoritmo utilizado;
* modificar o código quando solicitado;
* executar testes;
* interpretar os resultados;
* identificar possíveis erros;
* justificar suas decisões de implementação.

A utilização de código gerado por Inteligência Artificial não substitui a compreensão do conteúdo.

---

# Tecnologias

* Linguagem C
* GCC
* Make
* Git
* GitHub

Não é necessário utilizar bibliotecas externas para as primeiras etapas do projeto.

---

# Licença

Distribuído sob a licença MIT.

Consulte o arquivo `LICENSE`.

---

# Autor

**Prof. Lucas Sperotto**

Universidade do Estado de Mato Grosso — UNEMAT
