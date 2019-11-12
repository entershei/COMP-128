#ifndef COMP_128_COMMUNICATION_H
#define COMP_128_COMMUNICATION_H

#include <vector>

using namespace std;

struct communication {
    explicit communication(vector<uint8_t> Kc) : Kc(move(Kc)) {}

    uint8_t send_byte(uint8_t m) {
        // todo
    }

    uint8_t receive_byte(uint8_t m) {
        // todo
    }

private:
    vector<uint8_t> Kc;
};

#endif //COMP_128_COMMUNICATION_H
