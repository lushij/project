#include <stdio.h>
#include <stdint.h>

#define hexcase 0
#define chrsz 8

uint32_t rol(uint32_t num, uint32_t cnt) {
    return (num << cnt) | (num >> (32 - cnt));
}

uint32_t safe_add(uint32_t x, uint32_t y) {
    uint32_t lsw = (x & 0xFFFF) + (y & 0xFFFF);
    uint32_t msw = (x >> 16) + (y >> 16) + (lsw >> 16);
    return (msw << 16) | (lsw & 0xFFFF);
}

void core_sha1(uint32_t *blockArray, uint32_t *result) {
    uint32_t x[80];
    uint32_t a = 1732584193;
    uint32_t b = -271733879;
    uint32_t c = -1732584194;
    uint32_t d = 271733878;
    uint32_t e = -1009589776;

    for (int i = 0; i < 80; i++) {
        if (i < 16)
            x[i] = blockArray[i];
        else
            x[i] = rol(x[i - 3] ^ x[i - 8] ^ x[i - 14] ^ x[i - 16], 1);
        
        uint32_t t;
        if (i < 20)
            t = (b & c) | ((~b) & d);
        else if (i < 40)
            t = b ^ c ^ d;
        else if (i < 60)
            t = (b & c) | (b & d) | (c & d);
        else
            t = b ^ c ^ d;

        t = safe_add(safe_add(rol(a, 5), t), safe_add(safe_add(e, x[i]), (i < 20) ? 1518500249 : (i < 40) ? 1859775393 : (i < 60) ? -1894007588 : -899497514));
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = t;
    }

    result[0] = safe_add(a, result[0]);
    result[1] = safe_add(b, result[1]);
    result[2] = safe_add(c, result[2]);
    result[3] = safe_add(d, result[3]);
    result[4] = safe_add(e, result[4]);
}

void hex_sha1(char *s, char *output) {
    uint32_t result[5];
    uint32_t blockArray[16];

    for (int i = 0; i < 5; i++)
        result[i] = 0;

    int i = 0;
    while (*s) {
        blockArray[i >> 2] |= *s++ << (24 - (i & 3) * 8);
        i++;
    }
    blockArray[i >> 2] |= 0x80 << (24 - (i & 3) * 8);
    blockArray[15] = i * chrsz;

    core_sha1(blockArray, result);

    for (int i = 0; i < 5; i++) {
        output[i * 8 + 0] = (result[i] >> 28) & 0xF;
        output[i * 8 + 1] = (result[i] >> 24) & 0xF;
        output[i * 8 + 2] = (result[i] >> 20) & 0xF;
        output[i * 8 + 3] = (result[i] >> 16) & 0xF;
        output[i * 8 + 4] = (result[i] >> 12) & 0xF;
        output[i * 8 + 5] = (result[i] >> 8) & 0xF;
        output[i * 8 + 6] = (result[i] >> 4) & 0xF;
        output[i * 8 + 7] = result[i] & 0xF;
    }
}

int main() {
    char input[] = "abc";
    char output[41]; // SHA-1 produces 40-character output plus '\0'

    hex_sha1(input, output);

    printf("SHA-1 hash of 'abc': ");
    for (int i = 0; i < 40; i++) {
        printf("%c", output[i] + ((output[i] < 10) ? '0' : 'a' - 10));
    }
    printf("\n");

    return 0;
}

