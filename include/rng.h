#ifndef RNG_H
#define RNG_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Inicializa o gerador pseudoaleatório com uma seed explícita.
 *
 * A implementação usa SplitMix64, cuja sequência é definida apenas por
 * aritmética unsigned de 64 bits. Isso torna a sequência reproduzível
 * entre plataformas que ofereçam uint64_t.
 *
 * @param seed Seed de 64 bits. O valor zero também é válido.
 */
void rng_seed(uint64_t seed);

/**
 * @brief Gera e instala uma seed automática baseada no relógio do sistema.
 *
 * @return Seed efetivamente instalada.
 */
uint64_t rng_seed_auto(void);

/**
 * @brief Retorna a seed usada na inicialização da sequência atual.
 *
 * @return Seed atual.
 */
uint64_t rng_get_seed(void);

/**
 * @brief Produz o próximo valor pseudoaleatório de 64 bits.
 *
 * @return Valor no intervalo [0, UINT64_MAX].
 */
uint64_t rng_next_u64(void);

/**
 * @brief Produz o próximo valor pseudoaleatório de 32 bits.
 *
 * @return Valor no intervalo [0, UINT32_MAX].
 */
uint32_t rng_next_u32(void);

/**
 * @brief Sorteia uniformemente um índice no intervalo [0, bound).
 *
 * Usa rejeição para evitar o viés de módulo.
 *
 * @param bound Limite superior exclusivo. Se zero, retorna zero.
 * @return Índice pseudoaleatório menor que bound.
 */
size_t rng_index(size_t bound);

/**
 * @brief Produz um double uniforme no intervalo [0, 1).
 *
 * @return Valor pseudoaleatório com 53 bits de precisão útil.
 */
double rng_unit(void);

#endif
