#include <utility>

#ifndef COMP_128_AUTHORIZATION_H
#define COMP_128_AUTHORIZATION_H

#include <vector>
#include "tables.h"

using namespace std;

// A3, A8
struct authorization {
    explicit authorization(vector<uint8_t> Ki) : Ki(move(Ki)) {}

    vector<uint8_t> get_SRES() {
        return SRES;
    }

    vector<uint8_t> get_Ks() {
        return Ks;
    }

protected:
    void generate_SRES_and_Kc() {
        vector<uint8_t> x(32);
        vector<bool> bits(128);
        for (int i = 16; i < 32; ++i) {
            x[i] = random[i - 16];
        }

        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 16; ++j) {
                x[j] = Ki[j];
            }

            for (int j = 0; j < 5; ++j) {
                for (int k = 0; k < (1 << j); ++k) {
                    for (int l = 0; l < (1 << (4 - j)); ++l) {
                        uint8_t m = l + k * (1 << (5 - j));
                        uint8_t n = m + (1 << (4 - j));
                        uint y = (x[m] + 2 * x[n]) % (1 << (9 - j));
                        uint z = (2 * x[m] + x[n]) % (1 << (9 - j));
                        x[m] = T[j][y];
                        x[n] = T[j][z];
                    }
                }
            }

            for (int j = 0; j < 32; ++j) {
                for (int k = 0; k < 4; ++k) {
                    bits[4 * j + k] = (x[j] >> (3 - k)) & 1;
                }
            }

            if (i < 7) {
                for (int j = 0; j < 16; ++j) {
                    x[j + 16] = 0;

                    for (int k = 0; k < 8; ++k) {
                        uint8_t next_bit = ((8 * j + k) * 17) % 128;
                        x[j + 16] |= bits[next_bit] << (7 - k);
                    }
                }
            }
        }

        SRES.resize(4);
        Ks.resize(8);

        for (int i = 0; i < 4; ++i) {
            SRES[i] = (x[2 * i] << 4) | x[2 * i + 1];
        }

        for (int i = 0; i < 6; ++i) {
            Ks[i] = (x[2 * i + 18] << 6) | (x[2 * i + 19] << 2) | (x[2 * i + 20] >> 2);
        }
        Ks[6] = (x[2 * 6 + 18] << 6) | (x[2 * 6 + 19] << 2);
        Ks[7] = 0;
    }

    vector<uint8_t> Ki; // 16 byte
    vector<uint8_t> random; // 16 byte
    vector<uint8_t> SRES; // 4 byte
    vector<uint8_t> Ks;  // 8 byte
};

struct station_authorization : authorization {
    explicit station_authorization(vector<uint8_t> ki) : authorization(move(ki)) {
        generate_random();
        generate_SRES_and_Kc();
    }

    vector<uint8_t> get_random() {
        return random;
    }

private:
    const int RAND_SIZE = 16;

    void generate_random() {
        random.resize(RAND_SIZE);
        random_device rd;
        mt19937 mt(rd());
        uniform_int_distribution<uint8_t> distribution(0, 255);

        for (int i = 0; i < RAND_SIZE; ++i) {
            random[i] = distribution(mt);
        }
    }
};

struct client_authorization : authorization {
    explicit client_authorization(vector<uint8_t> ki, vector<uint8_t> random) : authorization(move(ki)) {
        this->random = move(random);
        generate_SRES_and_Kc();
    }
};

#endif //COMP_128_AUTHORIZATION_H
