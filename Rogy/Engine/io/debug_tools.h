#ifndef R_DEBUG_TOOLS_H
#define R_DEBUG_TOOLS_H

#include <iostream>
#include <string>
#include <glm\glm.hpp>
#include <vector>

enum LogType
{
	LT_Message,
	LT_Warning,
	LT_Error
};

struct LogInfo
{
	std::string msg;
	LogType type;

	LogInfo(const std::string message, LogType ltype)
	{
		msg = message;
		type = ltype;
	}
};

class DebugTool
{
public:
	DebugTool();
	~DebugTool();

	std::vector<LogInfo> messages;

	void Log(const std::string message);
	void Warning(const std::string message);
	void Error(const std::string message);
	void Clear();

	size_t Size()
	{
		return messages.size();
	}

	bool hasMessages = false;

	// Static
	static DebugTool* GetInstance(DebugTool* _ins = nullptr);
	/*static void LOG(std::string message);
	static void WARNING(std::string message);
	static void ERROR(std::string message);*/

private:
	void addMsg(const std::string message, LogType ltype);
};

#endif // !R_DEBUG_TOOLS_H