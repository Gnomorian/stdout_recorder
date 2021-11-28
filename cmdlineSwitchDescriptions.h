#pragma once
#include <regex>
#include <string_view>

class CommandSwitchPattern : public std::wregex
{
	using RegexOptions = std::regex_constants::syntax_option_type;
	constexpr static RegexOptions PatternOptions{ std::regex::icase | std::regex::nosubs | std::regex::ECMAScript };
	constexpr static std::wstring_view NoArgsPattern{ L"^(--<longname>)|(-[a-z]*<shortname>[a-z]*)$" };
	constexpr static std::wstring_view ArgsPattern{ L"^(--<longname>)|(-[a-z]*<shortname>)$" };
	constexpr static std::wstring_view LongNameReplace{ L"<longname>" };
	constexpr static std::wstring_view ShortNameReplace{ L"<shortname>" };
	std::wstring pattern{};
public:
	CommandSwitchPattern() = delete;
	CommandSwitchPattern(std::wstring_view longName, std::wstring_view shortName, bool hasParameters)
		: pattern{makePatternStr(longName, shortName, hasParameters)}
		, std::wregex{pattern, PatternOptions}
	{}
private:
	std::wstring makePatternStr(auto longName, auto shortName, auto hasParameters)
	{
		const auto patternTemplate{ hasParameters ? ArgsPattern : NoArgsPattern };
		std::wstring buffer{patternTemplate};
		buffer.replace(buffer.cbegin(), buffer.cend(), LongNameReplace);
		buffer.replace(buffer.cbegin(), buffer.cend(), ShortNameReplace);
		return buffer;
	}
};

struct CommandlineSwitchDescription
{
	const std::wstring_view FriendlyName{};
	const std::wstring_view HelpDescription{};
	const CommandSwitchPattern Pattern;
	const size_t NumArgs{ 0 };
	constexpr static decltype(NumArgs) InfinateArguments{ ~0ul };
};

struct StdoutSwitch : CommandlineSwitchDescription
{
	const std::wstring_view FriendlyName{L"stdout"};
	const std::wstring_view HelpDescription{L"--stdout, -o		record stdout of child application"};
	const CommandSwitchPattern Pattern{L"stdout", L"o", false};
	const size_t NumArgs{ 0 };
};

struct StderrSwitch : CommandlineSwitchDescription
{
	const std::wstring_view FriendlyName{ L"stderr" };
	const std::wstring_view HelpDescription{ L"--stderr, -e		record stderr of child application" };
	const CommandSwitchPattern Pattern{ L"stderr", L"e", false };
	const size_t NumArgs{ 0 };
};

struct ApplicationSwitch : CommandlineSwitchDescription
{
	const std::wstring_view FriendlyName{ L"application" };
	const std::wstring_view HelpDescription{ L"--application, -a	path to application to run/record stdout" };
	const CommandSwitchPattern Pattern{ L"application", L"a", true };
	const size_t NumArgs{ 1 };
};

struct FileSwitch : CommandlineSwitchDescription
{
	const std::wstring_view FriendlyName{ L"output file" };
	const std::wstring_view HelpDescription{ L"--file, -f			file to record output to" };
	const CommandSwitchPattern Pattern{ L"file", L"f", true };
	const size_t NumArgs{ 1 };
};

struct PidSwitch : CommandlineSwitchDescription
{
	const std::wstring_view FriendlyName{ L"application pid" };
	const std::wstring_view HelpDescription{ L"--pid, -i			pid of application to record output" };
	const CommandSwitchPattern Pattern{ L"pid", L"i", true };
	const size_t NumArgs{ 1 };
};

struct ParametersSwitch : CommandlineSwitchDescription
{
	const std::wstring_view FriendlyName{ L"application pid" };
	const std::wstring_view HelpDescription{ L"--parameters, -p	parameters to give to the application" };
	const CommandSwitchPattern Pattern{ L"parameters", L"p", true };
	const size_t NumArgs{ InfinateArguments };
};