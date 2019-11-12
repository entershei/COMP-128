#include <utility>

#ifndef COMP_128_AUTHORIZATION_H
#define COMP_128_AUTHORIZATION_H

#include <vector>

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
    vector<uint8_t> generate_SRES() {

    }

    vector<uint8_t> generate_Kc() {

    }

    vector<uint8_t> Ki; // 16 byte
    vector<uint8_t> random; // 16 byte
    vector<uint8_t> SRES; // 4 byte
    vector<uint8_t> Ks;  // 8 byte
};

struct station_authorization : authorization {
    explicit station_authorization(vector<uint8_t> ki) : authorization(move(ki)) {
        generate_random();
        SRES = generate_SRES();
        Ks = generate_Kc();
    }

    vector<uint8_t> get_random() {
        return random;
    }

private:
    vector<uint8_t> generate_random() {
        return {};
    }
};

struct client_authorization : authorization {
    explicit client_authorization(vector<uint8_t> ki, vector<uint8_t> random) : authorization(move(ki)) {
        random = move(random);
        SRES = generate_SRES();
        Ks = generate_Kc();
    }
};

#endif //COMP_128_AUTHORIZATION_H
