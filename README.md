# Laboratório de Inteligência Artificial — Jogo da Velha

Laboratório didático em **C** para estudar, implementar e comparar diferentes formas de Inteligência Artificial usando o Jogo da Velha como ambiente controlado.

O mesmo motor de jogo é compartilhado por todos os agentes, permitindo comparar estratégias sem alterar as regras ou a representação do problema.

## Modos disponíveis

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

## Objetivos didáticos

O projeto percorre uma sequência intencional:

**regras → baseline aleatório → heurísticas → busca adversarial → poda → self-play → evolução → aprendizado por reforço → experimentação.**

Os estudantes podem observar quando uma estratégia deixa de ser apenas uma coleção de regras e passa a pesquisar ou aprender a partir da experiência.

## Compilação

Requisitos: GCC, Make e um terminal com suporte a UTF-8.

```bash
make
./jogo_velha
```

Para executar todos os testes:

```bash
make test
```

Para limpar binários e objetos:

```bash
make clean
```

O projeto é compilado com:

```text
-std=c11 -O2 -Wall -Wextra -Wpedantic
```

## Estrutura

```text
jogo-da-velha-ia/
├── include/              interfaces públicas
├── src/
│   ├── game.c            regras e interface do jogo
│   ├── registry.c        registro uniforme dos agentes
│   ├── experiment.c      IA × IA e experimentos
│   ├── rng.c             PRNG central e reproduzível
│   └── agents/           algoritmos de IA
├── tests/                testes locais
├── docs/                 material didático
├── data/                 modelos treinados locais
├── results/              CSVs de experimentos locais
├── Makefile
├── CONTRIBUTING.md
└── LICENSE
```

## Agentes

| Agente | Ideia principal | Aprende? |
|---|---|---:|
| Aleatório | baseline sem estratégia | não |
| Heurístico | regras explícitas | não |
| Minimax | busca completa adversarial | não |
| Alpha-Beta | Minimax com poda | não |
| Samuel-style | pesos + self-play | sim |
| Genético | evolução de pesos | sim |
| Q-Learning | recompensa e tabela Q | sim |

O **Minimax** e o **Alpha-Beta** são as referências ótimas: em Jogo da Velha corretamente implementado, não podem ser forçados a perder.

## Experimentos

A opção 10 alterna os lados dos agentes e registra resultados em `results/experiments.csv`. Para Minimax e Alpha-Beta também são coletadas métricas de nós visitados e podas.

Cada experimento recebe uma **seed**. O valor `0` no menu gera uma seed automaticamente; qualquer inteiro positivo de até 64 bits pode ser informado para repetir a mesma sequência pseudoaleatória. A seed efetivamente usada é exibida e gravada no CSV.

Consulte [docs/experiments.md](docs/experiments.md).

## Fork dos alunos

Cada aluno deve trabalhar em seu próprio fork. O fluxo completo de `origin`, `upstream`, branches, commits e testes está em [CONTRIBUTING.md](CONTRIBUTING.md).

## Material didático

- [Arquitetura](docs/architecture.md)
- [Algoritmos](docs/algorithms.md)
- [Referência Completa dos Métodos](docs/referencia_metodos.md)
- [Experimentos](docs/experiments.md)

## Arquivos gerados

Pesos treinados, tabelas Q, binários e CSVs não são versionados. Eles são produzidos localmente e estão cobertos pelo `.gitignore`.

## GitHub Actions

O workflow `.github/workflows/ci.yml` é executado automaticamente em pushes e pull requests para `main`, além de permitir execução manual. O CI valida:

- GCC com `-Werror` e todas as suítes;
- Clang com `-Werror` e todas as suítes;
- AddressSanitizer (ASan) e UndefinedBehaviorSanitizer (UBSan);
- Valgrind Memcheck nas nove suítes de teste, incluindo a validação do RNG.

A validação local continua disponível com `make` e `make test`.

## Licença

MIT License. Consulte [LICENSE](LICENSE).

## Autor

**Prof. Lucas Kriesel Sperotto**  
Universidade do Estado de Mato Grosso — UNEMAT
