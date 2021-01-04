#include "InputManager.h"
static InputManager* _instance;
// --------------------------------------------------------------------------
InputManager::InputManager(GLFWwindow* windo)
{
	Init(windo);
}
// --------------------------------------------------------------------------
InputManager::~InputManager()
{
}
// --------------------------------------------------------------------------
void InputManager::Init(GLFWwindow* windo)
{
	if (windo != nullptr) {
		window = windo;
		InputManager::GetInstance(this);

		setIsEnabled(true);

		glfwSetKeyCallback(window, InputManager::KeyCallback);
		glfwSetMouseButtonCallback(window, InputManager::MouseCallback);
		glfwSetCursorPosCallback(window, InputManager::cursor_position_callback);
	}
}
// --------------------------------------------------------------------------
void InputManager::GetCursorPos(int* xx, int* yy)
{
	glfwGetCursorPos(window, (double*)xx, (double*)yy);
}
// -------------------------------------------------------------
void InputManager::Update(float dt)
{
	for (size_t i = 0; i < _axis.size(); i++)
	{
		if (GetKey(_axis[i].key_up))
			_axis[i].is_down = eAK_Up;
		else if(GetKey(_axis[i].key_down))
			_axis[i].is_down = eAK_Down;
		else 
			_axis[i].is_down = eAk_Non;

		if (_axis[i].is_down == eAk_Non && _axis[i].value == 0)
			continue;
		if (_axis[i].is_down == eAK_Up && _axis[i].value == 1)
			continue;
		if (_axis[i].is_down == eAK_Down && _axis[i].value == -1)
			continue;

		if (_axis[i].is_down == eAK_Up && _axis[i].value != 1)
		{
			_axis[i].value += _axis[i].speed * dt;
			if (_axis[i].value > 1)
				_axis[i].value = 1;
		}
		else if (_axis[i].is_down == eAK_Down && _axis[i].value != -1)
		{
			_axis[i].value -= _axis[i].speed * dt;
			if (_axis[i].value < -1)
				_axis[i].value = -1;
		}
		else
		{
			if (_axis[i].value > 0)
			{
				_axis[i].value -= _axis[i].speed * dt;
				if (_axis[i].value < 0)
					_axis[i].value = 0;
			}
			else if (_axis[i].value < 0)
			{
				_axis[i].value += _axis[i].speed * dt;
				if (_axis[i].value > 0)
					_axis[i].value = 0;
			}
		}
	}
}
// -------------------------------------------------------------
void InputManager::SetCursor(bool enabled)
{
	if (cursorEnabled == enabled) return;
	cursorEnabled = enabled;
	if(enabled)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
// -------------------------------------------------------------
float InputManager::GetAxis(const char* aName)
{
	for (size_t i = 0; i < _axis.size(); i++)
	{
		if (_axis[i].name == aName)
			return _axis[i].value;
	}
	return 0;
}
void InputManager::Clear()
{
	if(!_keysDown.empty())
		_keysDown.clear();
	if (!_keysUp.empty())
		_keysUp.clear();

	if (!_mouseDown.empty())
		_mouseDown.clear();
	if (!_mouseUp.empty())
		_mouseUp.clear();
}
int InputManager::IsAnyKeyDown()
{
	if (!_keysDown.empty())
	{
		return _keysDown[0];
	}
	return -1;
}
// -------------------------------------------------------------
bool InputManager::GetMouseButton(int button)
{
	bool result = false;
	if (_isEnabled) {
		std::map<int, bool>::iterator it = _mouse_buttons.find(button);
		if (it != _mouse_buttons.end()) {
			result = _mouse_buttons[button];
		}
	}
	return result;
}
// -------------------------------------------------------------
bool InputManager::GetMouseButtonDown(int button)
{
	if (_isEnabled) {
		for (size_t i = 0; i < _mouseDown.size(); i++)
		{
			if (_mouseDown[i] == button)
				return true;
		}
	}
	return false;
}
// -------------------------------------------------------------
bool InputManager::GetMouseButtonUp(int button)
{
	if (_isEnabled) {
		for (size_t i = 0; i < _mouseUp.size(); i++)
		{
			if (_mouseUp[i] == button)
				return true;
		}
	}
	return false;
}
// -------------------------------------------------------------
bool InputManager::GetKey(int key)
{
	bool result = false;
	if (_isEnabled) {
		std::map<int, bool>::iterator it = _keys.find(key);
		if (it != _keys.end()) {
			result = _keys[key];
		}
	}
	return result;
}
// -------------------------------------------------------------
bool InputManager::GetKeyUp(int key)
{
	if (_isEnabled) {
		for (size_t i = 0; i < _keysUp.size(); i++)
		{
			if (_keysUp[i] == key)
				return true;
		}
	}
	return false;
}
// -------------------------------------------------------------
bool InputManager::GetKeyDown(int key) 
{
	if (_isEnabled) {
		for (size_t i = 0; i < _keysDown.size(); i++)
		{
			if (_keysDown[i] == key)
				return true;
		}
	}
	return false;
}
// -------------------------------------------------------------
void InputManager::setIsKeyDown(int key, bool isDown) {
	std::map<int, bool>::iterator it = _keys.find(key);
	if (it != _keys.end()) {
		// register the key in "_keysDown" or "_keysUp" if pressed for the first time.
		if (isDown == true && _keys[key] == false)
			_keysDown.push_back(key);
		else if (isDown == false && _keys[key] == true)
			_keysUp.push_back(key);

		_keys[key] = isDown;
	}
	else
	{
		// register the key in "_keysDown" if pressed for the first time.
		if (isDown == true)
			_keysDown.push_back(key);

		_keys.insert(std::pair<int, bool>(key, isDown));
	}
	/*if (isDown)
		_keys.insert(std::pair<int, bool>(key, isDown));
	else
		_keys.erase(key);*/
}
// -------------------------------------------------------------
void InputManager::setIsMouseDown(int button, bool isDown)
{
	std::map<int, bool>::iterator it = _mouse_buttons.find(button);
	if (it != _mouse_buttons.end()) {
		// register the key in "_keysDown" or "_keysUp" if pressed for the first time.
		if (isDown == true && _mouse_buttons[button] == false)
			_mouseDown.push_back(button);
		else if (isDown == false && _mouse_buttons[button] == true)
			_mouseUp.push_back(button);

		_mouse_buttons[button] = isDown;
	}
	else
	{
		// register the key in "_keysDown" if pressed for the first time.
		if (isDown == true)
			_mouseDown.push_back(button);

		_mouse_buttons.insert(std::pair<int, bool>(button, isDown));
	}
}
// -------------------------------------------------------------
float InputManager::GetMouseXPos() { return mouseX; }
float InputManager::GetMouseYPos() { return mouseY; }
float InputManager::GetMouseXDelta() { return mouseXdelta; }
float InputManager::GetMouseYDelta() { return mouseYdelta; }
// -------------------------------------------------------------
void InputManager::SetMousePos(float xpos, float ypos)
{
	mouseXdelta = mouseX - xpos;
	mouseYdelta = mouseY - ypos;

	mouseX = xpos;
	mouseY = ypos;
}
// -------------------------------------------------------------
void InputManager::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Send key event to InputManager instance
	if(InputManager::GetInstance() != nullptr) {
		InputManager::GetInstance()->setIsKeyDown(key, action != GLFW_RELEASE);
	}
}
// -------------------------------------------------------------
void InputManager::MouseCallback(GLFWwindow *window, int button, int action, int mods)
{
	if (InputManager::GetInstance() != nullptr) {
		InputManager::GetInstance()->setIsMouseDown(button, action != GLFW_RELEASE);
	}
}
// -------------------------------------------------------------
void InputManager::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (InputManager::GetInstance() != nullptr) {
		InputManager::GetInstance()->SetMousePos((float)xpos, (float)ypos);
	}
}
// -------------------------------------------------------------
InputManager* InputManager::GetInstance(InputManager* _ins)
{
	if (_ins != nullptr) // set
	{
		_instance = _ins;
	}
	return _instance;
}
// -------------------------------------------------------------
void InputManager::AddAxis(std::string aName, int aKeyUp, int ekeyDown)
{
	if (!CheckAxis(aName))
	{
		_axis.push_back(eInputAxis(aName, aKeyUp, ekeyDown));
	}
}
// -------------------------------------------------------------
void InputManager::RemoveAxis(std::string & aName)
{
	for (size_t i = 0; i < _axis.size(); i++)
	{
		if (_axis[i].name == aName)
		{
			_axis.erase(_axis.begin() + i);
		}
	}
}
// -------------------------------------------------------------
bool InputManager::CheckAxis(std::string aName)
{
	for (size_t i = 0; i < _axis.size(); i++)
	{
		if (_axis[i].name == aName)
			return true;
	}
	return false;
}
// -------------------------------------------------------------