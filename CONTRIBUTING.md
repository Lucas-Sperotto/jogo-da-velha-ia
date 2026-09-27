# Contribuindo — fluxo dos alunos

## 1. Faça um fork

Use o botão **Fork** no GitHub e clone o seu fork:

```bash
git clone https://github.com/SEU-USUARIO/jogo-da-velha-ia.git
cd jogo-da-velha-ia
```

## 2. Configure o repositório do professor

```bash
git remote add upstream https://github.com/Lucas-Sperotto/jogo-da-velha-ia.git
git remote -v
```

## 3. Atualize sua `main`

```bash
git fetch upstream
git checkout main
git merge upstream/main
git push origin main
```

## 4. Trabalhe em uma branch

```bash
git checkout -b atividade-minimax
```

Faça commits pequenos, coerentes e descritivos. Exemplos:

```text
feat(ai): implementa função de avaliação
fix(core): corrige detecção de empate
test(ai): cobre bloqueio de vitória imediata
docs: explica poda alpha-beta
```

## 5. Valide antes de entregar

```bash
make clean
make
make test
```

Não entregue binários, tabelas Q treinadas ou CSVs de experimento no Git.

## Uso de IA

Ferramentas de IA podem apoiar o desenvolvimento, mas o aluno deve ser capaz de explicar, testar e modificar todo código submetido.

## GitHub Actions

GitHub Actions não é utilizado nesta fase do laboratório. A compilação e os testes são executados localmente.
