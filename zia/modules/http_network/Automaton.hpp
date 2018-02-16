//
// Created by doom on 11/02/18.
//

#ifndef ZIA_PROGRESSIVEPARSER_HPP
#define ZIA_PROGRESSIVEPARSER_HPP

#include <type_traits>
#include <unordered_map>
#include <functional>

namespace parsing
{
    template <typename StateT, typename SymbolT = char>
    class Automaton
    {
    public:
        Automaton(StateT initState, StateT errorState, StateT finalState,
                  std::unordered_map<StateT, std::function<StateT(SymbolT)>> transitions) noexcept :
            _initialState(initState), _errorState(errorState),
            _finalState(finalState), _curState(initState),
            _transitions(std::move(transitions))
        {
        }

        void restart() noexcept
        {
            _curState = _initialState;
        }

        enum Result
        {
            ParseSuccess,
            ParseError,
            InProgress,
        };

        template <typename Iterator>
        std::pair<Result, Iterator> feed(Iterator begin, Iterator end) noexcept
        {
            for (auto cur = begin; cur != end; ++cur) {
                _curState = _transitions[_curState](static_cast<SymbolT>(*cur));

                if (_curState == _finalState)
                    return {ParseSuccess, ++cur};
                if (_curState == _errorState)
                    return {ParseError, ++cur};
            }
            return {InProgress, end};
        }

    private:
        const StateT _initialState;
        const StateT _errorState;
        const StateT _finalState;
        StateT _curState;
        std::unordered_map<StateT, std::function<StateT(SymbolT)>> _transitions;
    };
}

#endif //ZIA_PROGRESSIVEPARSER_HPP
