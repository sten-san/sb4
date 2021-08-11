#pragma once
#include <optional>
#include "sb4/include/token.hpp"
#include "sb4/include/string.hpp"

namespace sb4 {
    namespace reserved_map {
        constexpr inline std::tuple<ustring_view, token_type> words[] = {
            { u"AND", token_type::band },
            { u"OR", token_type::bor },
            { u"XOR", token_type::bxor },
            { u"NOT", token_type::bnot },
            { u"DIV", token_type::idiv },
            { u"MOD", token_type::mod },
            { u"IF", token_type::if_ },
            { u"THEN", token_type::then },
            { u"ELSE", token_type::else_ },
            { u"ELSEIF", token_type::elseif },
            { u"ENDIF", token_type::endif },
            { u"CASE", token_type::case_ },
            { u"WHEN", token_type::when },
            { u"OTHERWISE", token_type::otherwise },
            { u"ENDCASE", token_type::endcase },
            { u"GOTO", token_type::goto_ },
            { u"GOSUB", token_type::gosub },
            { u"RETURN", token_type::return_ },
            { u"ON", token_type::on },
            { u"LOOP", token_type::loop },
            { u"ENDLOOP", token_type::endloop },
            { u"FOR", token_type::for_ },
            { u"NEXT", token_type::next },
            { u"WHILE", token_type::while_ },
            { u"WEND", token_type::wend },
            { u"REPEAT", token_type::repeat },
            { u"UNTIL", token_type::until },
            { u"BREAK", token_type::break_ },
            { u"CONTINUE", token_type::continue_ },
            { u"DEF", token_type::def },
            { u"END", token_type::end },
            { u"DEFOUT", token_type::defout },
            { u"VAR", token_type::var },
            { u"DIM", token_type::dim },
            { u"DATA", token_type::data },
            { u"READ", token_type::read },
            { u"RESTORE", token_type::restore },
            { u"PRINT", token_type::print },
            { u"TPRINT", token_type::tprint },
            { u"INPUT", token_type::input },
            { u"LINPUT", token_type::linput },
            { u"CALL", token_type::call },
            { u"SWAP", token_type::swap },
            { u"OUT", token_type::out },
            { u"COMMON", token_type::common },
            { u"EXEC", token_type::exec },
            { u"CONST", token_type::const_ },
            { u"ENUM", token_type::enum_ },
        };

        constexpr inline std::tuple<ustring_view, token_type> symbols[] = {
            { u"<<<", token_type::llshift },
            { u">>>", token_type::lrshift },
            { u"<<+", token_type::rlshift },
            { u">>+", token_type::rrshift },
            { u"<<", token_type::lshift },
            { u">>", token_type::rshift },
            { u"<=", token_type::lequal },
            { u">=", token_type::gequal },
            { u"&&", token_type::land },
            { u"||", token_type::lor },
            { u"==", token_type::equal },
            { u"!=", token_type::nequal },
            { u"<", token_type::less },
            { u">", token_type::greater },
            { u"!", token_type::lnot },
            { u"=", token_type::assign },
            { u"+", token_type::plus },
            { u"-", token_type::minus },
            { u"*", token_type::mult },
            { u"/", token_type::fdiv },
            { u"(", token_type::lparen },
            { u")", token_type::rparen },
            { u"[", token_type::lsub },
            { u"]", token_type::rsub },
            { u",", token_type::comma },
            { u":", token_type::colon },
            { u";", token_type::semicolon },
            { u"?", token_type::print },
        };

        constexpr inline ustring_view variable_suffix = u"%#$";

        constexpr std::optional<ustring_view> to_string(token_type v) {
            for (auto [s, t] : words) {
                if (t == v) {
                    return s;
                }
            }

            for (auto [s, t] : symbols) {
                if (t == v) {
                    return s;
                }
            }

            return std::nullopt;
        }
    }
}

