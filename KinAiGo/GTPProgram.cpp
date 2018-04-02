#include "GTPProgram.h"
#include <KinBase\KinBase.h>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
using namespace std::chrono;

KinAiGo::GTPProgram::GTPProgram(std::string Path, std::string cmd)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE StdInRead;
	HANDLE StdOutWrite;
	必真(CreatePipe(&StdInRead, &Send, &saAttr, 0), "CreatePipe Failed");
	必真(CreatePipe(&Receive, &StdOutWrite, &saAttr, 0), "CreatePipe Failed");

	//Create Process

	STARTUPINFOA siStartInfo;

	std::memset(&ProgramInfo, 0, sizeof(ProgramInfo));
	std::memset(&siStartInfo, 0, sizeof(siStartInfo));

	siStartInfo.cb = sizeof(siStartInfo);
	siStartInfo.hStdError = StdOutWrite;
	siStartInfo.hStdOutput = StdOutWrite;
	siStartInfo.hStdInput = StdInRead;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
	Path += " " + cmd;

	std::vector<char> CmdStr(Path.begin(), Path.end());
	CmdStr.push_back('\0');

	if (CreateProcessA(NULL,
		&CmdStr[0],
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&siStartInfo,
		&ProgramInfo))
	{
		OutputDebugString(TEXT("KinGo: GTP CreateSuccess"));
	}
	else
	{
		OutputDebugString(TEXT("KinGo: GTP Failed"));
	}

	CloseHandle(StdInRead);
	CloseHandle(StdOutWrite);
}

KinAiGo::GTPProgram::~GTPProgram()
{
	ReadFlag = 0;
	while (!OverFlag)
	{
		std::this_thread::yield();
		(*this) << "quit";
	}
	(*this) << "quit";

	CloseHandle(ProgramInfo.hProcess);
	CloseHandle(ProgramInfo.hThread);

	OutputDebugString(TEXT("KinGo: GTP Release"));
}

KinAiGo::GTPProgram & KinAiGo::GTPProgram::operator<<(const std::string info2)
{
	DWORD dwWritten;
	auto info = info2 + "\n";
	必真(WriteFile(Send, info.c_str(), info.size(), &dwWritten, NULL), "WritePipe Error");
	return *this;
}


void KinAiGo::GTPProgram::AutoRead()
{
	if (ReadFlag == 0)
	{
		ReadFlag = 1;
		std::thread(std::bind(&GTPProgram::AutoReadPipe, this)).detach();
	}
}

void KinAiGo::GTPProgram::AutoReadPipe()
{
	OverFlag = 0;
	char chBuf[4096];
	DWORD dwRead;
	while (ReadFlag)
	{
		dwRead = 0;
		必真(ReadFile(Receive, chBuf, 4096, &dwRead, NULL), "ReadPipeError");
		ReadBuffer += std::string(chBuf, dwRead);

		while (true)
		{
			auto Pos = ReadBuffer.find('\n');
			if (Pos == std::string::npos)
			{
				break;
			}
			auto Info = ReadBuffer.substr(0, Pos);
			if (Info[Info.size() - 1] == '\r')
			{
				Info.pop_back();
			}
			OnRecieved(Info);

			ReadBuffer.erase(0, Pos+1);
		}

	}
	OverFlag = 1;
}
