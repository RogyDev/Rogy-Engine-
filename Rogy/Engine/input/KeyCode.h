#ifndef R_KEY_CODE_H
#define R_KEY_CODE_H

#include <GLFW\glfw3.h>
#include "luainc.h"
#include "LuaBridge\LuaBridge.h"
#include <string>

namespace RKey
{
	static int KEY_UNKNOWN = GLFW_KEY_UNKNOWN;
	static int KEY_A = GLFW_KEY_A;
	static int KEY_B = GLFW_KEY_B;
	static int KEY_C = GLFW_KEY_C;
	static int KEY_D = GLFW_KEY_D;
	static int KEY_E = GLFW_KEY_E;
	static int KEY_F = GLFW_KEY_F;
	static int KEY_G = GLFW_KEY_G;
	static int KEY_H = GLFW_KEY_H;
	static int KEY_I = GLFW_KEY_E;
	static int KEY_J = GLFW_KEY_J;
	static int KEY_K = GLFW_KEY_K;
	static int KEY_L = GLFW_KEY_L;
	static int KEY_M = GLFW_KEY_M;
	static int KEY_N = GLFW_KEY_N;
	static int KEY_O = GLFW_KEY_O;
	static int KEY_P = GLFW_KEY_P;
	static int KEY_Q = GLFW_KEY_Q;
	static int KEY_R = GLFW_KEY_R;
	static int KEY_S = GLFW_KEY_S;
	static int KEY_T = GLFW_KEY_T;
	static int KEY_U = GLFW_KEY_U;
	static int KEY_V = GLFW_KEY_V;
	static int KEY_W = GLFW_KEY_W;
	static int KEY_X = GLFW_KEY_X;
	static int KEY_Y = GLFW_KEY_Y;
	static int KEY_Z = GLFW_KEY_Z;

	static int KEY_0 = GLFW_KEY_0;
	static int KEY_1 = GLFW_KEY_1;
	static int KEY_2 = GLFW_KEY_2;
	static int KEY_3 = GLFW_KEY_3;
	static int KEY_4 = GLFW_KEY_4;
	static int KEY_5 = GLFW_KEY_5;
	static int KEY_6 = GLFW_KEY_6;
	static int KEY_7 = GLFW_KEY_7;
	static int KEY_8 = GLFW_KEY_8;
	static int KEY_9 = GLFW_KEY_9;

	static int KEY_BACKSPACE = GLFW_KEY_BACKSPACE;
	static int KEY_TAB = GLFW_KEY_TAB;
	static int KEY_RETURN = GLFW_KEY_ENTER;
	//static int KEY_RETURN2 = GLFW_KEY_ENTER;
	static int KEY_ENTER = GLFW_KEY_ENTER;

	static int KEY_NUMLOCKCLEAR = GLFW_KEY_NUM_LOCK;
	static int KEY_SCROLLLOCK = GLFW_KEY_SCROLL_LOCK;

	static int KEY_RSHIFT = GLFW_KEY_RIGHT_SHIFT;
	static int KEY_LSHIFT = GLFW_KEY_LEFT_SHIFT;
	static int KEY_RCTRL = GLFW_KEY_RIGHT_CONTROL;
	static int KEY_LCTRL = GLFW_KEY_LEFT_CONTROL;
	static int KEY_RALT = GLFW_KEY_RIGHT_ALT;
	static int KEY_LALT = GLFW_KEY_LEFT_ALT;

	static int KEY_PAUSE = GLFW_KEY_PAUSE;
	static int KEY_CAPSLOCK = GLFW_KEY_CAPS_LOCK;
	static int KEY_ESCAPE = GLFW_KEY_ESCAPE;
	static int KEY_SPACE = GLFW_KEY_SPACE;
	static int KEY_PAGEUP = GLFW_KEY_PAGE_UP;
	static int KEY_PAGEDOWN = GLFW_KEY_PAGE_DOWN;
	static int KEY_END = GLFW_KEY_END;
	static int KEY_HOME = GLFW_KEY_HOME;

	static int KEY_LEFT = GLFW_KEY_LEFT;
	static int KEY_UP = GLFW_KEY_UP;
	static int KEY_RIGHT = GLFW_KEY_RIGHT;
	static int KEY_DOWN = GLFW_KEY_DOWN;

	static int KEY_PRINTSCREEN = GLFW_KEY_PRINT_SCREEN;
	static int KEY_INSERT = GLFW_KEY_INSERT;
	static int KEY_DELETE = GLFW_KEY_DELETE;

	static int KEY_KP_0 = GLFW_KEY_KP_0;
	static int KEY_KP_1 = GLFW_KEY_KP_1;
	static int KEY_KP_2 = GLFW_KEY_KP_2;
	static int KEY_KP_3 = GLFW_KEY_KP_3;
	static int KEY_KP_4 = GLFW_KEY_KP_4;
	static int KEY_KP_5 = GLFW_KEY_KP_5;
	static int KEY_KP_6 = GLFW_KEY_KP_6;
	static int KEY_KP_7 = GLFW_KEY_KP_7;
	static int KEY_KP_8 = GLFW_KEY_KP_8;
	static int KEY_KP_9 = GLFW_KEY_KP_9;

	static int KEY_KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;
	static int KEY_KP_ADD = GLFW_KEY_KP_ADD;
	static int KEY_KP_EQUAL = GLFW_KEY_KP_EQUAL;
	static int KEY_KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;
	static int KEY_KP_DIVIDE = GLFW_KEY_KP_DIVIDE;

	static int KEY_F1 = GLFW_KEY_F1;
	static int KEY_F2 = GLFW_KEY_F1;
	static int KEY_F3 = GLFW_KEY_F1;
	static int KEY_F4 = GLFW_KEY_F4;
	static int KEY_F5 = GLFW_KEY_F5;
	static int KEY_F6 = GLFW_KEY_F6;
	static int KEY_F7 = GLFW_KEY_F7;
	static int KEY_F8 = GLFW_KEY_F8;
	static int KEY_F9 = GLFW_KEY_F9;
	static int KEY_F10 = GLFW_KEY_F10;
	static int KEY_F11 = GLFW_KEY_F11;
	static int KEY_F12 = GLFW_KEY_F12;
	static int KEY_F13 = GLFW_KEY_F13;
	static int KEY_F14 = GLFW_KEY_F14;
	static int KEY_F15 = GLFW_KEY_F15;
	static int KEY_F16 = GLFW_KEY_F16;
	static int KEY_F17 = GLFW_KEY_F17;
	static int KEY_F18 = GLFW_KEY_F18;
	static int KEY_F19 = GLFW_KEY_F19;
	static int KEY_F20 = GLFW_KEY_F20;
	static int KEY_F21 = GLFW_KEY_F21;
	static int KEY_F22 = GLFW_KEY_F22;
	static int KEY_F23 = GLFW_KEY_F23;
	static int KEY_F24 = GLFW_KEY_F24;

	static int CONTROLLER_BUTTON_A = GLFW_GAMEPAD_BUTTON_A;
	static int CONTROLLER_BUTTON_B = GLFW_GAMEPAD_BUTTON_B;
	static int CONTROLLER_BUTTON_X = GLFW_GAMEPAD_BUTTON_X;
	static int CONTROLLER_BUTTON_Y = GLFW_GAMEPAD_BUTTON_Y;
	static int CONTROLLER_BUTTON_BACK = GLFW_GAMEPAD_BUTTON_BACK;
	static int CONTROLLER_BUTTON_GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE;
	static int CONTROLLER_BUTTON_START = GLFW_GAMEPAD_BUTTON_START;
	static int CONTROLLER_BUTTON_LEFTSTICK = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
	static int CONTROLLER_BUTTON_RIGHTSTICK = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
	static int CONTROLLER_BUTTON_DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP;
	static int CONTROLLER_BUTTON_DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
	static int CONTROLLER_BUTTON_DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
	static int CONTROLLER_BUTTON_DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;

	static int CONTROLLER_AXIS_LEFTX = GLFW_GAMEPAD_AXIS_LEFT_X;
	static int CONTROLLER_AXIS_LEFTY = GLFW_GAMEPAD_AXIS_LEFT_Y;
	static int CONTROLLER_AXIS_RIGHTX = GLFW_GAMEPAD_AXIS_RIGHT_X;
	static int CONTROLLER_AXIS_RIGHTY = GLFW_GAMEPAD_AXIS_RIGHT_Y;
	static int CONTROLLER_AXIS_TRIGGERLEFT = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
	static int CONTROLLER_AXIS_TRIGGERRIGHT = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;

	static std::string KeyToString(int keycode)
	{
		if (keycode == KEY_A) return "A";
		else if (keycode == KEY_B) return "B";
		else if (keycode == KEY_C) return "C";
		else if (keycode == KEY_D) return "D";
		else if (keycode == KEY_E) return "E";
		else if (keycode == KEY_F) return "F";
		else if (keycode == KEY_G) return "G";
		else if (keycode == KEY_H) return "H";
		else if (keycode == KEY_I) return "I";
		else if (keycode == KEY_J) return "J";
		else if (keycode == KEY_K) return "K";
		else if (keycode == KEY_L) return "L";
		else if (keycode == KEY_M) return "M";
		else if (keycode == KEY_N) return "N";
		else if (keycode == KEY_O) return "O";
		else if (keycode == KEY_P) return "P";
		else if (keycode == KEY_Q) return "Q";
		else if (keycode == KEY_R) return "R";
		else if (keycode == KEY_S) return "S";
		else if (keycode == KEY_T) return "T";
		else if (keycode == KEY_U) return "U";
		else if (keycode == KEY_V) return "V";
		else if (keycode == KEY_W) return "W";
		else if (keycode == KEY_X) return "X";
		else if (keycode == KEY_Y) return "Y";
		else if (keycode == KEY_Z) return "Z";

		else if (keycode == KEY_0) return "0";
		else if (keycode == KEY_1) return "1";
		else if (keycode == KEY_2) return "2";
		else if (keycode == KEY_3) return "3";
		else if (keycode == KEY_4) return "4";
		else if (keycode == KEY_5) return "5";
		else if (keycode == KEY_6) return "6";
		else if (keycode == KEY_7) return "7";
		else if (keycode == KEY_8) return "8";
		else if (keycode == KEY_9) return "9";

		else if (keycode == KEY_KP_0) return "KP_0";
		else if (keycode == KEY_KP_1) return "KP_1";
		else if (keycode == KEY_KP_2) return "KP_2";
		else if (keycode == KEY_KP_3) return "KP_3";
		else if (keycode == KEY_KP_4) return "KP_4";
		else if (keycode == KEY_KP_5) return "KP_5";
		else if (keycode == KEY_KP_6) return "KP_6";
		else if (keycode == KEY_KP_7) return "KP_7";
		else if (keycode == KEY_KP_8) return "KP_8";
		else if (keycode == KEY_KP_9) return "KP_9";

		else if (keycode == KEY_F1) return "F1";
		else if (keycode == KEY_F2) return "F2";
		else if (keycode == KEY_F3) return "F3";
		else if (keycode == KEY_F4) return "F4";
		else if (keycode == KEY_F5) return "F5";
		else if (keycode == KEY_F6) return "F6";
		else if (keycode == KEY_F7) return "F7";
		else if (keycode == KEY_F8) return "F8";
		else if (keycode == KEY_F9) return "F9";
		else if (keycode == KEY_F10) return "F10";
		else if (keycode == KEY_F11) return "F11";
		else if (keycode == KEY_F12) return "F12";

		else if (keycode == KEY_SPACE) return "Space";
		else if (keycode == KEY_LSHIFT) return "Left Shift";
		else if (keycode == KEY_LCTRL) return "Left Ctrl";
		else if (keycode == KEY_LALT) return "Left Alt";
		else if (keycode == KEY_RSHIFT) return "Right Shift";
		else if (keycode == KEY_RCTRL) return "Right Ctrl";
		else if (keycode == KEY_RALT) return "Right Alt";

		else if (keycode == KEY_ESCAPE) return "Escape";
		else if (keycode == KEY_DELETE) return "Delete";
		else if (keycode == KEY_HOME) return "Home";

		else if (keycode == KEY_UP) return "Up Arrow";
		else if (keycode == KEY_DOWN) return "Down Arrow";
		else if (keycode == KEY_LEFT) return "Left Arrow";
		else if (keycode == KEY_RIGHT) return "Right Arrow";

		else return "NotDefined";
	};

	static void BindKeyToNamespace(lua_State *L, const char* ns_name)
	{
		luabridge::getGlobalNamespace(L)
			.beginNamespace(ns_name)

			.addVariable<int>("A", &RKey::KEY_A, false)
			.addVariable<int>("B", &RKey::KEY_B, false)
			.addVariable<int>("C", &RKey::KEY_C, false)
			.addVariable<int>("D", &RKey::KEY_D, false)
			.addVariable<int>("E", &RKey::KEY_E, false)
			.addVariable<int>("F", &RKey::KEY_F, false)
			.addVariable<int>("G", &RKey::KEY_G, false)
			.addVariable<int>("H", &RKey::KEY_H, false)
			.addVariable<int>("I", &RKey::KEY_I, false)
			.addVariable<int>("J", &RKey::KEY_J, false)
			.addVariable<int>("K", &RKey::KEY_K, false)
			.addVariable<int>("L", &RKey::KEY_L, false)
			.addVariable<int>("M", &RKey::KEY_M, false)
			.addVariable<int>("N", &RKey::KEY_N, false)
			.addVariable<int>("O", &RKey::KEY_O, false)
			.addVariable<int>("P", &RKey::KEY_P, false)
			.addVariable<int>("K", &RKey::KEY_K, false)
			.addVariable<int>("R", &RKey::KEY_R, false)
			.addVariable<int>("S", &RKey::KEY_S, false)
			.addVariable<int>("T", &RKey::KEY_T, false)
			.addVariable<int>("U", &RKey::KEY_U, false)
			.addVariable<int>("V", &RKey::KEY_V, false)
			.addVariable<int>("W", &RKey::KEY_W, false)
			.addVariable<int>("X", &RKey::KEY_X, false)
			.addVariable<int>("Y", &RKey::KEY_Y, false)
			.addVariable<int>("Z", &RKey::KEY_Z, false)

			.addVariable<int>("KEY_0", &RKey::KEY_0, false)
			.addVariable<int>("KEY_1", &RKey::KEY_1, false)
			.addVariable<int>("KEY_2", &RKey::KEY_2, false)
			.addVariable<int>("KEY_3", &RKey::KEY_3, false)
			.addVariable<int>("KEY_4", &RKey::KEY_4, false)
			.addVariable<int>("KEY_5", &RKey::KEY_5, false)
			.addVariable<int>("KEY_6", &RKey::KEY_6, false)
			.addVariable<int>("KEY_7", &RKey::KEY_7, false)
			.addVariable<int>("KEY_8", &RKey::KEY_8, false)
			.addVariable<int>("KEY_9", &RKey::KEY_9, false)

			.addVariable<int>("KEY_KP_0", &RKey::KEY_KP_0, false)
			.addVariable<int>("KEY_KP_1", &RKey::KEY_KP_1, false)
			.addVariable<int>("KEY_KP_2", &RKey::KEY_KP_2, false)
			.addVariable<int>("KEY_KP_3", &RKey::KEY_KP_3, false)
			.addVariable<int>("KEY_KP_4", &RKey::KEY_KP_4, false)
			.addVariable<int>("KEY_KP_5", &RKey::KEY_KP_5, false)
			.addVariable<int>("KEY_KP_6", &RKey::KEY_KP_6, false)
			.addVariable<int>("KEY_KP_7", &RKey::KEY_KP_7, false)
			.addVariable<int>("KEY_KP_8", &RKey::KEY_KP_8, false)
			.addVariable<int>("KEY_KP_9", &RKey::KEY_KP_9, false)

			.addVariable<int>("KEY_F1", &RKey::KEY_F1, false)
			.addVariable<int>("KEY_F2", &RKey::KEY_F2, false)
			.addVariable<int>("KEY_F3", &RKey::KEY_F3, false)
			.addVariable<int>("KEY_F4", &RKey::KEY_F4, false)
			.addVariable<int>("KEY_F5", &RKey::KEY_F5, false)
			.addVariable<int>("KEY_F6", &RKey::KEY_F6, false)
			.addVariable<int>("KEY_F7", &RKey::KEY_F7, false)
			.addVariable<int>("KEY_F8", &RKey::KEY_F8, false)
			.addVariable<int>("KEY_F9", &RKey::KEY_F9, false)
			.addVariable<int>("KEY_F10", &RKey::KEY_F10, false)
			.addVariable<int>("KEY_F11", &RKey::KEY_F11, false)
			.addVariable<int>("KEY_F12", &RKey::KEY_F12, false)

			.addVariable<int>("Space", &RKey::KEY_SPACE, false)
			.addVariable<int>("KEY_LShift", &RKey::KEY_LSHIFT, false)
			.addVariable<int>("KEY_LCtrl", &RKey::KEY_LCTRL, false)
			.addVariable<int>("KEY_LAlt", &RKey::KEY_LALT, false)
			.addVariable<int>("KEY_RShift", &RKey::KEY_RSHIFT, false)
			.addVariable<int>("KEY_RCtrl", &RKey::KEY_RCTRL, false)
			.addVariable<int>("KEY_RAlt", &RKey::KEY_RALT, false)

			.addVariable<int>("KEY_Escape", &RKey::KEY_ESCAPE, false)
			.addVariable<int>("KEY_Delete", &RKey::KEY_DELETE, false)
			.addVariable<int>("KEY_Home", &RKey::KEY_HOME, false)

			.addVariable<int>("UpArrow", &RKey::KEY_UP, false)
			.addVariable<int>("DownArrow", &RKey::KEY_DOWN, false)
			.addVariable<int>("LeftArrow", &RKey::KEY_LEFT, false)
			.addVariable<int>("RightArrow", &RKey::KEY_RIGHT, false)

			.endNamespace();
	};
}

#endif // ! R_KEY_CODE_H