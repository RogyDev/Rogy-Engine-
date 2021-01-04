#ifndef INPUT_MNGER_H
#define INPUT_MNGER_H

#include <GLFW\glfw3.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "KeyCode.h"

enum eAxisKeyStatus
{
	eAK_Down,
	eAK_Up,
	eAk_Non
};
struct eInputAxis
{
	std::string name;
	int key_up;
	int key_down;
	float value;
	float speed;
	eAxisKeyStatus is_down;

	eInputAxis(std::string ename, int ekeyUp, int ekeyDown)
	{
		name = ename;
		key_up = ekeyUp;
		key_down = ekeyDown;
		value = 0;
		speed = 4.0f;
		is_down = eAk_Non;
	}
};

class InputManager
{
public:
	InputManager(GLFWwindow* windo = nullptr);
	~InputManager();

	void Init(GLFWwindow* windo);

	void Clear();

	int IsAnyKeyDown();

	void GetCursorPos(int* xx, int* yy);

	bool GetKey(int key);
	bool GetKeyDown(int key);
	bool GetKeyUp(int key);

	bool GetMouseButton(int button);
	bool GetMouseButtonDown(int button);
	bool GetMouseButtonUp(int button);

	float GetAxis(const char* aName);

	float GetMouseXPos();
	float GetMouseYPos();

	float GetMouseXDelta();
	float GetMouseYDelta();

	void Update(float dt);

	void SetCursor(bool enabled);

	static InputManager* GetInstance(InputManager* _ins = nullptr);

	void AddAxis(std::string aName, int aKeyUp, int ekeyDown);
	void RemoveAxis(std::string& aName);

	void SetMousePos(float xpos, float ypos);
	void ResetMouseDelta()
	{
		mouseYdelta = 0;
		mouseXdelta = 0;
	}

	size_t GetAxisCount()
	{
		return _axis.size();
	}

	std::vector<eInputAxis>& GetAllAxis()
	{
		return _axis;
	}

	// Serialization
	// ---------------------------------
	template <class Archive>
	void serializeSave(Archive & ar)
	{
		ar(std::string("1.0")); // version
		ar(_axis.size());
		for (size_t i = 0; i < _axis.size(); i++)
		{
			ar(_axis[i].name);
			ar(_axis[i].speed);
			ar(_axis[i].key_up);
			ar(_axis[i].key_down);
		}
	}

	template <class Archive>
	void serializeLoad(Archive & ar)
	{
		std::string ver;  ar(ver);
		size_t axisCount; ar(axisCount);

		for (size_t i = 0; i < axisCount; i++)
		{
			std::string aName;
			int keyUP, keyDown;
			float Speed;
			ar(aName);
			ar(Speed);
			ar(keyUP);
			ar(keyDown);
			AddAxis(aName, keyUP, keyDown);
			_axis[_axis.size() - 1].speed = Speed;
		}
	}

private:
	GLFWwindow* window;

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void MouseCallback(GLFWwindow *window, int button, int action, int mods);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	void setIsKeyDown(int key, bool isDown);
	void setIsMouseDown(int button, bool isDown);

	float mouseX, mouseY;
	float mouseXdelta, mouseYdelta;

	std::map<int, bool> _keys;
	std::vector<eInputAxis> _axis;
	std::vector<int> _keysDown;
	std::vector<int> _keysUp;

	std::map<int, bool> _mouse_buttons;
	std::vector<int> _mouseDown;
	std::vector<int> _mouseUp;
	

	bool _isEnabled;
	bool CheckAxis(std::string aName);
	bool cursorEnabled = true;
public:
	bool getIsEnabled() { return _isEnabled; }
	void setIsEnabled(bool value) { _isEnabled = value; }
};


#endif // ! INPUT_MNGER_H