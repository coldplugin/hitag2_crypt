#include <stdint.h>

hitag2_reverse(uint64_t old, int num);
uint16_t hitag2_makeWord(uint64_t x, int a, int b, int c, int d);
uint32_t hitag2_hash(uint64_t x);
uint8_t hitag2_bit(uint64_t * sd);
uint8_t hitag2_uint8_t(uint64_t *sd);
void hitag2_crypt(uint8_t * data, uint8_t * result, int bits, uint64_t * sd);
void hitag2_init(uint8_t reversing, uint32_t iskLow, uint16_t iskHi, uint32_t random, uint32_t cardId, uint64_t * seed);
