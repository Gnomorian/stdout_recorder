#pragma once
#include <Windows.h>
#include <filesystem>
#include <vector>
#include "winapi_context.h"

struct Process
{
	STARTUPINFOW startupInfo{};
	PROCESS_INFORMATION procInfo{};
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
		if (context.CreateProcessW(app, args, nullptr, nullptr, false, 0, nullptr, nullptr, &process.startupInfo, &process.procInfo) == 0)
		{
			throw std::runtime_error{ "CreateProcessW failed" };
		}
		return std::move(process);
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