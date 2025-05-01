#include "utils.h"
#include <string>

namespace http::utils {
std::string_view getStringLine(std::string_view input, std::size_t& pos, std::string_view lineSeparator)
{
    auto lastPos = input.find(lineSeparator, pos);
    auto separatorSize = lineSeparator.size();
    if (lastPos == std::string::npos) {
        lastPos = input.size();
        separatorSize = 0;
    }
    auto lineSize = lastPos - pos;
    auto linePos = pos;
    pos += lineSize + separatorSize;
    return input.substr(linePos, lineSize);
}

} //namespace http::utils