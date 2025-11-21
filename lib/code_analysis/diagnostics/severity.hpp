#pragma once

#include <string>

using namespace std;

namespace my {

enum class Severity {
    Note,
    Error,
    Warning
};

inline string to_string(Severity s) {
    switch (s) {
        case Severity::Note:
            return "Note";
        case Severity::Warning:
            return "Warning";
        case Severity::Error:
            return "Error";
    }
    return "";
}

}