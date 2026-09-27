#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

/**
 * @brief Converte uma string inteira de forma estrita e valida uma faixa inclusiva.
 *
 * Espaços em branco no início e no fim são aceitos. Qualquer caractere
 * adicional após o inteiro torna a entrada inválida.
 *
 * @param text Texto a interpretar.
 * @param min Valor mínimo aceito.
 * @param max Valor máximo aceito.
 * @param out Destino do valor validado.
 * @return 1 se a conversão for válida; 0 caso contrário.
 */
int parse_int_range(const char *text, int min, int max, int *out);

/**
 * @brief Converte uma string para uint64_t de forma estrita.
 *
 * Espaços em branco no início e no fim são aceitos. Sinal negativo,
 * overflow, decimal ou qualquer sufixo não branco são rejeitados.
 *
 * @param text Texto a interpretar.
 * @param out Destino do valor validado.
 * @return 1 se a conversão for válida; 0 caso contrário.
 */
int parse_u64(const char *text, uint64_t *out);

#endif
