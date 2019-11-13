#ifndef COMP_128_COMMUNICATION_H
#define COMP_128_COMMUNICATION_H

#include <vector>

using namespace std;

struct communication {
    explicit communication(const vector<uint8_t> &kc) {
        Kc = byte_to_bit_stream(kc);
        vector<uint8_t> key_stream = generate_key_stream(0);
        key_stream_for_send = key_stream;
        key_stream_for_receive = key_stream;
    }

    uint8_t send_byte(uint8_t m) {
        vector<uint8_t> m_stream = uint_to_bit_stream(m, 8);
        for (size_t i = 0; i < 8; ++i) {
            m_stream[i] = (key_stream_for_send[cur_bit_for_send] ^ m_stream[i]);
            ++cur_bit_for_send;
            if (cur_bit_for_send == 228) {
                ++cur_frame_for_send;
                key_stream_for_send = generate_key_stream(cur_frame_for_send);
                cur_bit_for_send = 0;
            }
        }
        return bit_stream_to_uint8(m_stream);
    }

    uint8_t receive_byte(uint8_t m) {
        vector<uint8_t> m_stream = uint_to_bit_stream(m, 8);
        for (size_t i = 0; i < 8; ++i) {
            m_stream[i] = (key_stream_for_receive[cur_bit_for_receive] ^ m_stream[i]);
            ++cur_bit_for_receive;
            if (cur_bit_for_receive == 228) {
                ++cur_frame_for_receive;
                key_stream_for_receive = generate_key_stream(cur_frame_for_receive);
                cur_bit_for_receive = 0;
            }
        }
        return bit_stream_to_uint8(m_stream);
    }

private:
    size_t cur_frame_for_send = 0;
    size_t cur_frame_for_receive = 0;
    size_t cur_bit_for_send = 0;
    size_t cur_bit_for_receive = 0;

    vector<uint8_t> Kc;
    vector<uint8_t> key_stream_for_send;
    vector<uint8_t> key_stream_for_receive;

    vector<uint8_t> byte_to_bit_stream(const vector<uint8_t> &vec) const {
        vector<uint8_t> res;
        for (auto &v : vec) {
            for (int i = 7; i >= 0; --i) {
                res.emplace_back(v & (1 << i));
            }
        }
    }

    vector<uint8_t> uint_to_bit_stream(size_t num, size_t size) const {
        vector<uint8_t> res(size, false);
        for (size_t i = 0; i < size; ++i) {
            res[i] = (num & (1 << i));
        }
        return res;
    }

    uint8_t bit_stream_to_uint8(const vector<uint8_t> &stream) const {
        uint8_t res = 0;
        for (size_t i = 0; i < 8; ++i) {
            res += ((uint8_t) stream[i] << i);
        }
        return res;
    }

    vector<uint8_t> generate_key_stream(size_t frame) {
        vector<uint8_t> reg1(19, false);
        vector<uint8_t> reg2(22, false);
        vector<uint8_t> reg3(23, false);

        vector<uint8_t> frame_stream = uint_to_bit_stream(frame, 22);
        for (int i = 0; i < 64; i++) {
            shift_all(reg1, reg2, reg3);
            reg1[0] = reg1[0] ^ Kc[i];
            reg2[0] = reg2[0] ^ Kc[i];
            reg3[0] = reg3[0] ^ Kc[i];
//            if (Kc[i] != 0 || reg1[0] != 0 || reg2[0] != 0 || reg3[0] != 0) {
//                cerr << "!!!!" << endl;
//            }
        }
        for (int i = 0; i < 22; i++) {
            shift_all(reg1, reg2, reg3);
            reg1[0] = reg1[0] ^ frame_stream[i];
            reg2[0] = reg2[0] ^ frame_stream[i];
            reg3[0] = reg3[0] ^ frame_stream[i];
//            if (Kc[i] != 0 || reg1[0] != 0 || reg2[0] != 0 || reg3[0] != 0) {
//                cerr << "!!!!" << endl;
//            }
        }
        for (int i = 0; i < 100; i++) {
            shift_majority(reg1, reg2, reg3);
        }

        vector<uint8_t> key(228, false);
        for (int i = 0; i < 228; i++) {
            shift_majority(reg1, reg2, reg3);
            key[i] = (reg1[18] ^ reg2[21] ^ reg3[22]);
        }
        return key;
    }

    void shift_majority(vector<uint8_t> &reg1, vector<uint8_t> &reg2, vector<uint8_t> &reg3) const {
        uint8_t maj = ((reg1[8] & reg2[10]) | (reg1[8] & reg3[10]) | (reg2[10] & reg3[10]));
        if (reg1[8] == maj)
            reg1 = shift_reg1(reg1);
        if (reg2[10] == maj)
            reg2 = shift_reg2(reg2);
        if (reg3[10] == maj)
            reg3 = shift_reg3(reg3);
    }

    void shift_all(vector<uint8_t> &reg1, vector<uint8_t> &reg2, vector<uint8_t> &reg3) const {
        reg1 = shift_reg1(reg1);
        reg2 = shift_reg2(reg2);
        reg3 = shift_reg3(reg3);
    }

    vector<uint8_t> shift_reg1(vector<uint8_t> reg) const {
        uint8_t temp = 0;
        for (int i = reg.size() - 1; i > 0; i--) {
            if (i == reg.size() - 1)
                temp = reg[13] ^ reg[16] ^ reg[17] ^ reg[18];
            reg[i] = reg[i - 1];
            if (i == 1)
                reg[0] = temp;
        }
        return reg;
    }

    vector<uint8_t> shift_reg2(vector<uint8_t> reg) const {
        uint8_t temp = 0;
        for (int i = reg.size() - 1; i > 0; i--) {
            if (i == reg.size() - 1)
                temp = reg[20] ^ reg[21];
            reg[i] = reg[i - 1];
            if (i == 1)
                reg[0] = temp;
        }
        return reg;
    }

    vector<uint8_t> shift_reg3(vector<uint8_t> reg) const {
        uint8_t temp = false;
        for (int i = reg.size() - 1; i > 0; i--) {
            if (i == reg.size() - 1)
                temp = reg[7] ^ reg[20] ^ reg[21] ^ reg[22];
            reg[i] = reg[i - 1];
            if (i == 1)
                reg[0] = temp;
        }
        return reg;
    }
};

#endif //COMP_128_COMMUNICATION_H