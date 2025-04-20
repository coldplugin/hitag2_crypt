#include "hitag2_crypt.h"

uint64 hitag2_reverse(uint64 old, int num) {
    uint64 reversed;
    int i;

    reversed = 0;
    num--;
    for(i = 0; i <= num; i++) {
        reversed |= ((old >> (num-i)) & 1) << i;
    }

    return reversed;
}
uint16 hitag2_makeWord(uint64 x, int a, int b, int c, int d) {
	uint16 wrd;

	wrd = ((x >> a)&1) | (((x >> b)&1) << 1) | (((x >> c)&1) << 2) | (((x >> d)&1) << 3);
	return wrd;
}

uint32 hitag2_hash(uint64 x) {
	uint32 tmp;
	tmp =
		((0x2C79 >> hitag2_makeWord(x, 1, 2, 4, 5)) & 1) |
        (((0x6671 >> hitag2_makeWord(x, 7, 11, 13, 14)) & 1) << 1) |
        (((0x6671 >> hitag2_makeWord(x, 16, 20, 22, 25)) & 1) << 2) |
        (((0x6671 >> hitag2_makeWord(x, 27, 28, 30, 32)) & 1) << 3) |
        (((0x2C79 >> hitag2_makeWord(x, 33, 42, 43, 45)) & 1) << 4);
	tmp = (0x7907287B >> tmp) & 1;
	return tmp;
}

byte hitag2_bit(uint64 * sd) {
	uint64 x = *sd;
	x = (x >> 1) | (((x ^ (x >> 2) ^ (x >> 3) ^ (x >> 6) ^ (x >> 7) ^ (x >> 8) ^ (x >> 16) ^ (x >> 22) ^
	(x >> 23) ^ (x >> 26) ^(x >> 30) ^ (x >> 41) ^ (x >> 42) ^ (x >> 43) ^ (x >> 46) ^ (x >> 47)) & 1) << 47);
	*sd = x;
	return hitag2_hash(x);
}

byte hitag2_byte(uint64 *sd) {
	byte newByte;
	int i;

	newByte = 0;
	for(i=0; i<8; i++) {
		newByte |= hitag2_bit(sd) << (7-i);
	}

	return newByte;
}
void hitag2_crypt(byte * data, byte * result, int bits, uint64 * sd) {
	int bytes;
	int rbits;
	int i;
	int j;

	bytes = bits >> 3;
	rbits = bits & 0x7;
	for(i=0; i < bytes; i++)
		result[i] = data[i] ^ hitag2_byte(sd);
	if(rbits) {
		result[i] = 0;
		for(j=0; j<rbits; j++)
			result[i] |= (hitag2_bit(sd) ^ ((data[i] >> (7-i))&1)) << (7-i);
	}
}

void hitag2_init(byte reversing, uint32 iskLow, uint16 iskHi, uint32 random, uint32 cardId, uint64 * seed) {
	int i;

	if(reversing) {
		iskLow = hitag2_reverse(iskLow, 32);
		iskHi = hitag2_reverse(iskHi, 16);
		random = hitag2_reverse(random, 32);
		cardId = hitag2_reverse(cardId, 32);
	}
	
	*seed = ((uint64)iskHi << 32) | cardId;
	for(i = 0; i < 32; i++) {
		*seed >>= 1;
		*seed |= (uint64)(hitag2_hash(*seed) ^ ((random >> i) & 1) ^ ((iskLow >> i) & 1)) << 47;
	}
}
