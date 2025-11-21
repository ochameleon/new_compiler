#pragma once

#include "diagnostic.hpp"
#include "severity.hpp"
#include "../text/text_location.hpp"

using namespace std;

namespace my {

class DiagnosticList {
private:
    vector<Diagnostic> diagnostics_ = {};
public:
    string to_string() {
        ostringstream buffer;
        for(int i = 0; i < diagnostics_.size(); ++i) {
            buffer << diagnostics_.at(i).to_string() << "\n";
        }
        return buffer.str();
    }

    void report(const TextLocation& location, const string& message, const Severity& severity) {
        Diagnostic diagnostic = Diagnostic(location, message, severity);
        diagnostics_.push_back(diagnostic);
    }
};

}