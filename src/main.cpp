#include <iostream>
#include <bits/uniform_int_dist.h>
#include <random>
#include "users.h"

const size_t K_SIZE = 16;

struct my_random {
    my_random() : mt(rd()), distribution(0, 255) {}

    uint8_t get_random() {
        distribution(mt);
    }

private:
    random_device rd;
    mt19937 mt;
    uniform_int_distribution<uint8_t> distribution;
};

my_random my_rand;

vector<uint8_t> generate_Ki() {
    vector<uint8_t> key(K_SIZE);

    for (int i = 0; i < K_SIZE; ++i) {
        key[i] = my_rand.get_random();
    }
    return key;
}

string print_char_as_bits(char c) {
    string res;
    for (int i = 0; i < 8; ++i) {
        if (((int) c) & (1 << i)) {
            res += '1';
        } else {
            res += '0';
        }
    }
    return res;
}

int main() {
    auto Ki = generate_Ki();

    base_station station(Ki);
    client client(Ki, station.get_random());

    if (station.get_SRES() != client.get_SRES()) {
        cerr << "Authorization failed." << endl;
        return 0;
    }

    cout << "Enter your message:" << endl;

    unsigned char m;
    while (cin >> m) {
        uint8_t encrypted_m = client.send_byte(m);
        uint8_t decrypted_m = station.receive_byte(encrypted_m);

        uint8_t station_message = my_rand.get_random();
        uint8_t encrypted_station_m = station.send_byte(station_message);
        uint8_t decrypted_station_m = client.receive_byte(encrypted_station_m);

        cout << endl << "====================================================================================" << endl;

        cout << endl << "Sending message from Client to Station..." << endl;

        cout << endl << "  Client sent message   |  Station received message  |  Station decrypted message  |" << endl
             << "       " << print_char_as_bits(m) << "         |"
             << "          " << print_char_as_bits(encrypted_m) << "          |"
             << "          " << print_char_as_bits(decrypted_m) << "           |" << endl;

        if (m != decrypted_m) {
            cout << endl << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                 << endl;
        }

        cout << endl
             << "Sending message from Station to Client..."
             << endl << endl;

        cout << "  Station sent message  |  Client received message   |  Client decrypted message   |" << endl
             << "        " << print_char_as_bits(station_message) << "        |"
             << "         " << print_char_as_bits(encrypted_station_m) << "           |"
             << "          " << print_char_as_bits(decrypted_station_m) << "           |";

        if (station_message != decrypted_station_m) {
            cout << endl << endl
                 << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        }

        cout << endl << endl
             << "===================================================================================="
             << endl << endl << endl;
    }

    return 0;
}
