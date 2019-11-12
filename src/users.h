#ifndef COMP_128_USERS_H
#define COMP_128_USERS_H

#include "authorization.h"
#include "communication.h"

struct client {
    client(vector<uint8_t> Ki, vector<uint8_t> random) : authorization(move(Ki), move(random)),
                                                         communication(authorization.get_Ks()) {}

    vector<uint8_t> get_SRES() {
        return authorization.get_SRES();
    }

    uint8_t send_byte(uint8_t m) {
        return communication.send_byte(m);
    }

    uint8_t receive_byte(uint8_t m) {
        return communication.receive_byte(m);
    }

private:
    client_authorization authorization;
    communication communication;
};

struct base_station {
    explicit base_station(vector<uint8_t> Ki) : authorization(move(Ki)), communication(authorization.get_Ks()) {}

    vector<uint8_t> get_SRES() {
        return authorization.get_SRES();
    }

    vector<uint8_t> get_random() {
        return authorization.get_random();
    }

    uint8_t send_byte(uint8_t m) {
        return communication.send_byte(m);
    }

    uint8_t receive_byte(uint8_t m) {
        return communication.receive_byte(m);
    }

private:
    station_authorization authorization;
    communication communication;
};

#endif //COMP_128_USERS_H
