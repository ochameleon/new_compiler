#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "parser.hpp"

namespace my {

class Interpreter {
private:
    Node root_;

    static bool is_lambda_literal_(const Node& node) {
        if (node.kind != Node::Kind::Call) {
            return false;
        }

        if (node.arguments.size() != 3) {
            return false;
        }

        const Node& head = node.arguments.at(0);
        return head.kind == Node::Kind::Identifier && head.name == "↦";
    }

    static std::string kind_name_(Node::Kind kind) {
        switch (kind) {
            case Node::Kind::Empty: return "Empty";
            case Node::Kind::Identifier: return "Identifier";
            case Node::Kind::StringLiteral: return "StringLiteral";
            case Node::Kind::Quote: return "Quote";
            case Node::Kind::Tuple: return "Tuple";
            case Node::Kind::Call: return "Call";
            default: return "Unknown";
        }
    }

    static std::vector<std::string> collect_parameter_names_(const Node& parameters) {
        if (parameters.kind == Node::Kind::Identifier) {
            return {parameters.name};
        }

        if (parameters.kind != Node::Kind::Tuple) {
            throw std::runtime_error("Lambda parameters must be an identifier or a tuple of identifiers");
        }

        std::vector<std::string> names;
        std::unordered_set<std::string> seen;
        for (const Node& item : parameters.arguments) {
            if (item.kind != Node::Kind::Identifier) {
                throw std::runtime_error("Tuple parameters must contain only identifiers");
            }

            if (seen.find(item.name) != seen.end()) {
                throw std::runtime_error("Duplicate lambda parameter name: " + item.name);
            }

            seen.insert(item.name);
            names.push_back(item.name);
        }
        return names;
    }

    static std::vector<Node> normalize_argument_values_(const std::vector<Node>& call_arguments, const Node& parameters) {
        if (parameters.kind == Node::Kind::Identifier) {
            if (call_arguments.size() == 1) {
                return {call_arguments.at(0)};
            }
            return {Node::tuple(call_arguments)};
        }

        if (parameters.kind != Node::Kind::Tuple) {
            throw std::runtime_error("Invalid lambda parameter structure");
        }

        if (call_arguments.size() == 1 && call_arguments.at(0).kind == Node::Kind::Tuple) {
            return call_arguments.at(0).arguments;
        }

        return call_arguments;
    }

    static std::unordered_map<std::string, Node> create_environment_(const Node& parameters, const std::vector<Node>& call_arguments) {
        std::vector<std::string> names = collect_parameter_names_(parameters);
        std::vector<Node> values = normalize_argument_values_(call_arguments, parameters);

        if (names.size() != values.size()) {
            throw std::runtime_error("Lambda argument count does not match parameter count");
        }

        std::unordered_map<std::string, Node> environment;
        for (int i = 0; i < static_cast<int>(names.size()); ++i) {
            environment.insert({names.at(i), values.at(i)});
        }
        return environment;
    }

    static Node substitute_(const Node& node, const std::unordered_map<std::string, Node>& environment) {
        if (node.kind == Node::Kind::Identifier) {
            auto it = environment.find(node.name);
            if (it != environment.end()) {
                return it->second;
            }
            return node;
        }

        if (node.kind == Node::Kind::Quote) {
            return node;
        }

        if (node.arguments.empty()) {
            return node;
        }

        std::vector<Node> rewritten;
        rewritten.reserve(node.arguments.size());
        for (const Node& child : node.arguments) {
            rewritten.push_back(substitute_(child, environment));
        }

        return Node{node.kind, node.name, rewritten};
    }

    Node evaluate_(const Node& node) const {

        if (is_lambda_literal_(node)) {
            return node;
        }

        if (node.kind == Node::Kind::Call) {
            if (node.arguments.empty()) {
                return node;
            }

            Node callable = evaluate_(node.arguments.at(0));
            std::vector<Node> evaluated_arguments;
            for (int i = 1; i < static_cast<int>(node.arguments.size()); ++i) {
                evaluated_arguments.push_back(evaluate_(node.arguments.at(i)));
            }

            if (!is_lambda_literal_(callable)) {
                return Node::call(callable, evaluated_arguments);
            }

            const Node& parameters = callable.arguments.at(1);
            const Node& body = callable.arguments.at(2);

            std::unordered_map<std::string, Node> environment = create_environment_(parameters, evaluated_arguments);
            Node substituted = substitute_(body, environment);
            return evaluate_(substituted);
        }

        if (node.kind == Node::Kind::Tuple) {
            std::vector<Node> items;
            for (const Node& child : node.arguments) {
                items.push_back(evaluate_(child));
            }
            return Node::tuple(items);
        }

        return node;
    }

public:
    explicit Interpreter(const Node& root) : root_(root) {
    }

    Node evaluate() const {
        return evaluate_(root_);
    }
};

}