#pragma once
#include <Windows.h>
#include <filesystem>
#include <vector>
#include "winapi_context.h"

struct StartupInfo
{
	StartupInfo() = default;
	StartupInfo(StartupInfo&& moveFrom) noexcept
		: startupInfo{ moveFrom.startupInfo }
	{
		moveFrom.startupInfo.hStdError	= nullptr;
		moveFrom.startupInfo.hStdInput	= nullptr;
		moveFrom.startupInfo.hStdOutput = nullptr;
	}
	StartupInfo(StartupInfo& copyFrom) = delete;
	STARTUPINFOW startupInfo{};
	~StartupInfo()
	{
		// handles need to be closed when we are done with them.
		CloseHandle(startupInfo.hStdError);
		CloseHandle(startupInfo.hStdInput);
		CloseHandle(startupInfo.hStdOutput);
	}
	operator STARTUPINFO* ()
	{
		return &startupInfo;
	}
};

struct ProcessInfo
{
	PROCESS_INFORMATION processInfo{};
	ProcessInfo() = default;
	ProcessInfo(ProcessInfo&& moveFrom) noexcept
		: processInfo{moveFrom.processInfo}
	{
		moveFrom.processInfo.hProcess	= nullptr;
		moveFrom.processInfo.hThread	= nullptr;
	}
	ProcessInfo(ProcessInfo& copyFrom) = delete;
	~ProcessInfo()
	{
		// handles need to be closed when we are done with them.
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	operator PROCESS_INFORMATION* ()
	{
		return &processInfo;
	}

};

struct Process
{
	StartupInfo startupInfo{};
	ProcessInfo procInfo{};
};

class ProcessBuilder
{
	WinapiContext context{};
	std::filesystem::path _exe{};
	std::wstring _arguments{};
public:
	explicit ProcessBuilder(WinapiContext context = WinapiContext())
		: context{ context }
	{}
	ProcessBuilder& executable(const std::filesystem::path& executable)
	{
		if (!validateExe(executable))
		{
			throw std::invalid_argument("given executable is not valid");
		}
		_exe = executable;
		return *this;
	}
	ProcessBuilder& addArgument(const std::wstring_view argument)
	{
		_arguments.append(L" ");
		_arguments.append(sanitiseArgument(argument));
		return *this;
	}
	ProcessBuilder& argument(const std::wstring& arguments)
	{
		_arguments = arguments;
		return *this;
	}
	Process start()
	{
		const wchar_t* app{ _exe.empty() ? nullptr : _exe.c_str() };
		wchar_t* args{ _arguments.empty() ? nullptr : _arguments.data() };
		Process process;
		if (context.CreateProcessW(app, args, nullptr, nullptr, false, 0, nullptr, nullptr, process.startupInfo, process.procInfo) == 0)
		{
			throw std::runtime_error{ "CreateProcessW failed" };
		}
		return process;
	}
private:
	bool validateExe(const std::filesystem::path& exe) const
	{
		return exe.has_filename()
			&& std::filesystem::exists(exe);
	}
	std::wstring sanitiseArgument(const std::wstring_view argument) const
	{
		constexpr auto Quote{ L'"' };
		std::wstring buffer{ argument };
		if (const auto begin{ std::cbegin(buffer) }; *begin != Quote)
		{
			buffer.insert(begin, Quote);
		}
		if (const auto end{ std::cend(buffer) }; *std::prev(end) != Quote)
		{
			buffer.insert(end, Quote);
		}
		return buffer;
	}
};