#ifndef LOXPP_RUNTIME_ERR_HPP
#define LOXPP_RUNTIME_ERR_HPP

#include <stdexcept>

#include "token.hpp"

class RuntimeErr : public std::runtime_error {
public:
    RuntimeErr(Token token, const std::string msg) : std::runtime_error(msg), token(token) {}

    Token token;
};

#endif //LOXPP_RUNTIME_ERR_HPP
