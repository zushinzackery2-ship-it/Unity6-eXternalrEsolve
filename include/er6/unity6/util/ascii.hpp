#pragma once

#include <string>

namespace er6
{

inline std::string ToLowerAscii(std::string s)
{
    for (char& c : s)
    {
        if (c >= 'A' && c <= 'Z')
        {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return s;
}

} // namespace er6
