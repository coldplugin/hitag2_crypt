typedef unsigned char byte;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

hitag2_reverse(uint64 old, int num);
uint16 hitag2_makeWord(uint64 x, int a, int b, int c, int d);
uint32 hitag2_hash(uint64 x);
byte hitag2_bit(uint64 * sd);
byte hitag2_byte(uint64 *sd);
void hitag2_crypt(byte * data, byte * result, int bits, uint64 * sd);
void hitag2_init(byte reversing, uint32 iskLow, uint16 iskHi, uint32 random, uint32 cardId, uint64 * seed);
