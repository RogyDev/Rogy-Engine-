#include "debug_tools.h"

// ---------------------------------------------
DebugTool::DebugTool()
{
}
// ---------------------------------------------
DebugTool::~DebugTool()
{
}
// ---------------------------------------------
void DebugTool::Log(const std::string message)
{
	addMsg(message, LogType::LT_Message);
}
// ---------------------------------------------
void DebugTool::Warning(const std::string message)
{
	addMsg(message, LogType::LT_Warning);
}
// ---------------------------------------------
void DebugTool::Error(const std::string message)
{
	addMsg(message, LogType::LT_Error);
}
// ---------------------------------------------
void DebugTool::Clear()
{
	messages.clear();
}
// ---------------------------------------------
void DebugTool::addMsg(const std::string message, LogType ltype)
{
	messages.push_back(LogInfo(message, ltype));
}
// ---------------------------------------------
static DebugTool* _instance = nullptr;
DebugTool* DebugTool::GetInstance(DebugTool* _ins)
{
	if (_ins != nullptr) // set
	{
		_instance = _ins;
	}
	return _instance;
}
/*
// ---------------------------------------------
void DebugTool::LOG(const std::string message)
{
	if (GetInstance() == nullptr) return;
	GetInstance()->addMsg(message, LogType::LT_Message);
}
// ---------------------------------------------
void DebugTool::WARNING(const std::string message)
{
	if (GetInstance() == nullptr) return;
	GetInstance()->addMsg(message, LogType::LT_Warning);
}
// ---------------------------------------------
void DebugTool::ERROR(const std::string message)
{
	if (GetInstance() == nullptr) return;
	GetInstance()->addMsg(message, LogType::LT_Error);
}
// ---------------------------------------------*/