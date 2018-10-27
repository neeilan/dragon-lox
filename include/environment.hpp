#ifndef DRAGON_LOX_ENVIRONMENT_HPP
#define DRAGON_LOX_ENVIRONMENT_HPP

#include <map>
#include <memory>
#include <string>

#include "token.hpp"
#include "runtime_err.hpp"

template <class T>
class Environment
{
public:
    explicit Environment()
            : enclosing(nullptr){};

    explicit Environment(Environment<T> *enclosing)
            : enclosing(enclosing) {}

    explicit Environment(const Environment &env)
    {
        this->enclosing = env.enclosing;
        this->values = env.values;
    }

    void define(std::string name, const T value)
    {
        values[name] = value;
    }

    void assign(const Token name, const T value)
    {
        if (values.count(name.lexeme) > 0)
        {
            values[name.lexeme] = value;
            return;
        }

        if (enclosing)
        {
            enclosing->assign(name, value);
            return;
        }

        // throw RuntimeErr(name, "Undefined assignment target '" + name.lexeme + "'.");
    }

    T get(const Token name)
    {
        if (values.count(name.lexeme) > 0)
        {
            return values[name.lexeme];
        }

        if (enclosing)
        {
            return enclosing->get(name);
        }

        assert(false && "Undefined variable");
        // throw RuntimeErr(name, "Undefined variable '" + name.lexeme + "'.");
    }

    T get_at(int distance, const Token name)
    {
        return (ancestor(distance)->values)[name.lexeme];
    }

    void assign_at(int distance, const Token name, const T value)
    {
        ancestor(distance)->values.at(name.lexeme) = value;
    }

private:
    Environment<T> *enclosing = nullptr;
    std::map<std::string, T> values;

    Environment *ancestor(int distance)
    {
        Environment<T> *curr_environment = this;

        for (int i = 0; i < distance; i++)
        {
            curr_environment = curr_environment->enclosing;
        }

        return curr_environment;
    }
};

#endif //DRAGON_LOX_ENVIRONMENT_HPP
