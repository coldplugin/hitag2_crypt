#include "hitag2_crypt.h"

uint64_t hitag2_reverse(uint64_t old, int num) {
    uint64_t reversed;
    int i;

    reversed = 0;
    num--;
    for(i = 0; i <= num; i++) {
        reversed |= ((old >> (num-i)) & 1) << i;
    }

    return reversed;
}
uint16_t hitag2_makeWord(uint64_t x, int a, int b, int c, int d) {
	uint16_t wrd;

	wrd = ((x >> a)&1) | (((x >> b)&1) << 1) | (((x >> c)&1) << 2) | (((x >> d)&1) << 3);
	return wrd;
}

uint32_t hitag2_hash(uint64_t x) {
	uint32_t tmp;
	tmp =
		((0x2C79 >> hitag2_makeWord(x, 1, 2, 4, 5)) & 1) |
        (((0x6671 >> hitag2_makeWord(x, 7, 11, 13, 14)) & 1) << 1) |
        (((0x6671 >> hitag2_makeWord(x, 16, 20, 22, 25)) & 1) << 2) |
        (((0x6671 >> hitag2_makeWord(x, 27, 28, 30, 32)) & 1) << 3) |
        (((0x2C79 >> hitag2_makeWord(x, 33, 42, 43, 45)) & 1) << 4);
	tmp = (0x7907287B >> tmp) & 1;
	return tmp;
}

uint8_t hitag2_bit(uint64_t * sd) {
	uint64_t x = *sd;
	x = (x >> 1) | (((x ^ (x >> 2) ^ (x >> 3) ^ (x >> 6) ^ (x >> 7) ^ (x >> 8) ^ (x >> 16) ^ (x >> 22) ^
	(x >> 23) ^ (x >> 26) ^(x >> 30) ^ (x >> 41) ^ (x >> 42) ^ (x >> 43) ^ (x >> 46) ^ (x >> 47)) & 1) << 47);
	*sd = x;
	return hitag2_hash(x);
}

uint8_t hitag2_uint8_t(uint64_t *sd) {
	uint8_t newuint8_t;
	int i;

	newuint8_t = 0;
	for(i=0; i<8; i++) {
		newuint8_t |= hitag2_bit(sd) << (7-i);
	}

	return newuint8_t;
}
void hitag2_crypt(uint8_t * data, uint8_t * result, int bits, uint64_t * sd) {
	int uint8_ts;
	int rbits;
	int i;
	int j;

	uint8_ts = bits >> 3;
	rbits = bits & 0x7;
	for(i=0; i < uint8_ts; i++)
		result[i] = data[i] ^ hitag2_uint8_t(sd);
	if(rbits) {
		result[i] = 0;
		for(j=0; j<rbits; j++)
			result[i] |= (hitag2_bit(sd) ^ ((data[i] >> (7-i))&1)) << (7-i);
	}
}

void hitag2_init(uint8_t reversing, uint32_t iskLow, uint16_t iskHi, uint32_t random, uint32_t cardId, uint64_t * seed) {
	int i;

	if(reversing) {
		iskLow = hitag2_reverse(iskLow, 32);
		iskHi = hitag2_reverse(iskHi, 16);
		random = hitag2_reverse(random, 32);
		cardId = hitag2_reverse(cardId, 32);
	}
	
	*seed = ((uint64_t)iskHi << 32) | cardId;
	for(i = 0; i < 32; i++) {
		*seed >>= 1;
		*seed |= (uint64_t)(hitag2_hash(*seed) ^ ((random >> i) & 1) ^ ((iskLow >> i) & 1)) << 47;
	}
}
