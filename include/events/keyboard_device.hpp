#ifndef __KEYBOARD_DEVICE_HPP__
#define __KEYBOARD_DEVICE_HPP__

#include <boost/scoped_ptr.hpp>
#include "device.hpp"
#include "keyboard_device_impl.hpp"

namespace mhe {

class KeyboardDevice : public Device
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
	key_up = 273,
	key_down = 274,
	key_right = 275,
	key_left = 276,
	// f1 - f15
	key_f1 = 282,
	key_f2 = 283,
	key_f3 = 284,
	key_f4 = 285,
	key_f5 = 286,
	key_f6 = 287,
	key_f7 = 288,
	key_f8 = 289,
	key_f9 = 290,
	key_f10 = 291,
	key_f11 = 292,
	key_f12 = 293,
	key_f13 = 294,
	key_f14 = 295,
	key_f15 = 296,
};
public:
	KeyboardDevice(const std::string& name);
	bool is_key_pressed(int sym) const;
private:
	std::vector< boost::shared_ptr<Event> > check_impl(const WindowSystem&);

	boost::scoped_ptr<KeyboardDeviceImpl> impl_;
	bool keys_[256];
};

}

#endif
