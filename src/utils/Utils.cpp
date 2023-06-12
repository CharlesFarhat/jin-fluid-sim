//
// Created by charl on 6/9/2023.
//

#include "Utils.h"
#include <iomanip>
#include <ostream>
#include <sstream>

namespace Utils {

    std::string FloatToStr(float val, size_t precision) {
        std::ostringstream str;
        str << std::fixed << std::setprecision(precision) << std::setfill('0') << val << "f";
        return str.str();
    }

    std::string Utils::GetSrcDir() {
        return std::string(SOURCE_DIR);
    }
}