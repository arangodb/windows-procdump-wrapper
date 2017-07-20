#include "CoreWriter.h"

#include "Pathcch.h"
#include <iostream>

CoreWriter::CoreWriter() : _procdumpPath(),
	_processId(0),
	_handleId(0),
	_jitDebugInfo()
{
}


CoreWriter::~CoreWriter()
{
}

std::wstring CoreWriter::getDirectory() {

	HANDLE processHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, _processId);
	if (processHandle == nullptr) {
		throw std::invalid_argument("Couldn't open process " + std::to_string(_processId) + ": " + std::to_string(GetLastError()));
	}

	constexpr int s = 2048;
	DWORD size = s;
	TCHAR path[s];
	
	if (!QueryFullProcessImageName(processHandle, 0, path, &size)) {
		throw std::runtime_error("Cannot query process: " + GetLastError());
	}

	if (PathCchRemoveFileSpec(path, size) == S_FALSE) {
		std::wstring errorMsg = L"Couldn't extract basedir from ";
		errorMsg += path;
		throw std::runtime_error(std::string(errorMsg.begin(), errorMsg.end()));
	}

	return std::wstring(path);
}

void CoreWriter::parseArguments(int argc, char** argv) {
	if (argc != 5) {
		throw std::invalid_argument("Need exactly 4 parameters!");
	}

	std::string tmp;
	
	tmp = argv[1];
	_procdumpPath = std::wstring(tmp.begin(), tmp.end());
	_processId = atol(argv[2]);
	_handleId = atol(argv[3]);
	tmp = argv[4];
	_jitDebugInfo = std::wstring(tmp.begin(), tmp.end());
}

void CoreWriter::run(int argc, char** argv)
{
	parseArguments(argc, argv);

	auto dir = getDirectory();
	std::wstring tmpString(_procdumpPath.begin(), _procdumpPath.end());
	LPCTSTR lpApplicationName = tmpString.c_str();

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	std::wstring cmdLine(_procdumpPath.begin(), _procdumpPath.end());
	cmdLine += L" -accepteula -j \"";
	cmdLine += dir;
	cmdLine += L"\" ";
	cmdLine += std::to_wstring(_processId);
	cmdLine += L" ";
	cmdLine += std::to_wstring(_handleId);
	cmdLine += L" ";
	cmdLine += _jitDebugInfo;
	
	std::wcout << "Executing" << dir << " : " << tmpString << " " << &cmdLine[0] << std::endl;

	if (!CreateProcess(
		lpApplicationName,
		&cmdLine[0],
		NULL,
		NULL,
		false,
		0,
		NULL,
		NULL,
		&si,
		&pi)) {
		

		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		std::wstring werrorMsg = L"Failed to create procdump process: ";
		werrorMsg += (LPTSTR)lpMsgBuf;
		
		std::string errorMsg(werrorMsg.begin(), werrorMsg.end());
		throw std::runtime_error(errorMsg);
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD exitCode;

	GetExitCodeProcess(pi.hProcess, &exitCode);
	CloseHandle(pi.hProcess);

	if (exitCode != 0) {
		std::string errorMessage = "Failed to execute procdump: ";
		errorMessage += std::to_string(GetLastError());
		errorMessage += " => ";
		errorMessage += std::to_string(exitCode);
		throw std::runtime_error(errorMessage);
	}
}