#include "story.hpp"

#include "error.hpp"

#include <type_traits>

template<class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};
template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

const Action& Story::action() const
{
    return _actions.at(_index);
}

void Story::next()
{
    std::visit([this]<class T>(T&&) {
        if constexpr (std::is_same<T, Text>() ||
                std::is_same<T, ShowBackground>()) {
            _index++;
        } else {
            throw Error{} << "cannot use next on action type " <<
                typeid(T).name();
        }
    }, action());
}

void Story::select(int index)
{
    std::visit(Overloaded{
        [this, index](const Choice& choice) {
            if (index < 0 ||
                    static_cast<size_t>(index) >= choice.selections.size()) {
                throw Error{} << "wrong selection " << index <<
                    ", there are " << choice.selections.size() << " choices";
            }
            _index = choice.selections.at(index).transition;
        },
        []<class T>(T&&) {
            throw Error{} << "cannot select, action type " << typeid(T).name();
        }
    }, action());
}

void Story::initializeTestStory()
{
    _actions = {
        ShowBackground{0},
        Text{0, "Как хорошо, что все зверики собрались у меня сегодня!"},
        Text{0, "Вы рады?"},
        ShowBackground{1},
        Text{1, "Где я?"},
        ShowBackground{2},
        Text{2, "Когда принесут покушать?"},
        ShowBackground{0},
        Text{0, "Всему своё время, мои маленькие проказники!"},
        Text{0, "Смотрите, я приготовила для вас замечательный ужин. Здесь есть угощение для каждого из вас. Ну же, не стесняйтесь! Садитесь к столу!"},
        ShowBackground{1},
        Text{1, "Мне почему-то очень грустно..."},
        Text{0, "..."},
        ShowBackground{2},
        Text{2, "А попить принесут?"},
        Choice{{
            {"Попытаться утешить мышку", 16},
            {"Наказать мышку", 18},
        }},
        ShowBackground{0},
        Text{0, "Не торопись, мышка, всему своё время!"},
        ShowBackground{3},
        Text{0, "Полакомимся мышиным соком!"},
        ShowBackground{1},
        Text{1, "Приятного аппетита..."},
    };
}
