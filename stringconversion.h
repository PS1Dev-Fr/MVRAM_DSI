#pragma once

#include <codecvt>
#include <string>

// convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string& str);

// convert wstring to UTF-8 string
std::string wstring_to_utf8(const std::wstring& str);
