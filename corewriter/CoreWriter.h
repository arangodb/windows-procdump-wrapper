#pragma once

#include <string>
#include <Windows.h>

class CoreWriter
{
public:
	CoreWriter();
	~CoreWriter();

	void run(int argc, char** argv);

private:
	std::wstring getDirectory();

	void parseArguments(int argc, char ** argv);
	
private:
	std::wstring _procdumpPath;
	DWORD _processId;
	DWORD _handleId;
	std::wstring _jitDebugInfo;
};