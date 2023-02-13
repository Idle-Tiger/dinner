#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

// For now, support only integer variables
class Variable {
public:
    explicit Variable(int value)
        : _value(value)
    { }

    operator int() const
    {
        return _value;
    }

private:
    int _value;
};

struct ShowBackground {
    int id = 0;
};

struct Text {
    int character = 0;
    std::string text;
};

struct Choice {
    struct Selection {
        std::string text;
        int transition = 0;
    };

    std::vector<Selection> selections;
};

using Action = std::variant<
    Choice,
    ShowBackground,
    Text>;

struct Script {
};

class Story {
public:
    [[nodiscard]] const Action& action() const;

    void next();
    void select(int index);

    void initializeTestStory();

private:
    std::vector<Action> _actions;
    std::map<std::string, Variable> _variables;
    size_t _index = 0;
};
