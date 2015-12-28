#ifndef __KEYBOARD_DEVICE_HPP__
#define __KEYBOARD_DEVICE_HPP__

#include "core/unique_ptr.hpp"
#include "device.hpp"
#include "keyboard_device_impl.hpp"

namespace mhe {

class MHE_EXPORT KeyboardDevice : public Device
{
public:
enum
{
    // numbers
    key_0 = 48,
    key_1 = 49,
    key_2 = 50,
    key_3 = 51,
    key_4 = 52,
    key_5 = 53,
    key_6 = 54,
    key_7 = 55,
    key_8 = 56,
    key_9 = 57,
    // small letters
    key_a = 97,
    key_b = 98,
    key_c = 99,
    key_d = 100,
    key_e = 101,
    key_f = 102,
    key_g = 103,
    key_h = 104,
    key_i = 105,
    key_j = 106,
    key_k = 107,
    key_l = 108,
    key_m = 109,
    key_n = 110,
    key_o = 111,
    key_p = 112,
    key_q = 113,
    key_r = 114,
    key_s = 115,
    key_t = 116,
    key_u = 117,
    key_v = 118,
    key_w = 119,
    key_x = 120,
    key_y = 121,
    key_z = 122,
    // capital letters
    key_A = 65,
    key_B = 66,
    key_C = 67,
    key_D = 68,
    key_E = 69,
    key_F = 70,
    key_G = 71,
    key_H = 72,
    key_I = 73,
    key_J = 74,
    key_K = 75,
    key_L = 76,
    key_M = 77,
    key_N = 78,
    key_O = 79,
    key_P = 80,
    key_Q = 81,
    key_R = 82,
    key_S = 83,
    key_T = 84,
    key_U = 85,
    key_V = 86,
    key_W = 87,
    key_X = 88,
    key_Y = 89,
    key_Z = 90,
    // different keys
    key_return = 13,
    key_space = 32,
    key_up = 128 + 0x52,
    key_down = 128 + 0x51,
    key_right = 128 + 0x50,
    key_left = 128 + 0x4f,
    // f1 - f15
    key_f1 = 128 + 0x3a,
    key_f2 = 128 + 0x3b,
    key_f3 = 128 + 0x3c,
    key_f4 = 128 + 0x3d,
    key_f5 = 128 + 0x3e,
    key_f6 = 128 + 0x3f,
    key_f7 = 128 + 0x40,
    key_f8 = 128 + 0x41,
    key_f9 = 128 + 0x42,
    key_f10 = 128 + 0x43,
    key_f11 = 128 + 0x44,
    key_f12 = 128 + 0x45,
    key_f13 = 128 + 0x46,
    key_f14 = 128 + 0x47,
    key_f15 = 128 + 0x48,
};
public:
    KeyboardDevice(const std::string& name);
    bool is_key_down(int sym) const;
    bool is_key_pressed(int sym) const;
    bool is_key_released(int sym) const;
private:
    void check_impl(Device::events_vector& events, const WindowSystem&) override;
    bool is_event_active_impl(bool current, uint32_t id) const override;

    unique_ptr<KeyboardDeviceImpl> impl_;
    bool keys_[300];
    bool was_pressed_[300];
    bool was_released_[300];
};

}

#endif
