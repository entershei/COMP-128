#include <iostream>
#include "users.h"

vector<uint8_t> generate_Ki() {
    return {};
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
        uint8_t decrepted_m = station.receive_byte(encrypted_m);

        uint8_t station_message = rand() % 256;
        uint8_t encrypted_station_m = station.send_byte(station_message);
        uint8_t decrypted_station_m = client.receive_byte(encrypted_station_m);
    }

    return 0;
}