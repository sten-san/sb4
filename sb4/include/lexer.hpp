#pragma once
#include <utility>
#include <algorithm>
#include <iterator>
#include <tuple>
#include "sb4/include/string.hpp"
#include "sb4/include/string_reader.hpp"
#include "sb4/include/token.hpp"

namespace sb4 {
    namespace constants {
        constexpr inline std::tuple<ustring_view, token_type> reserved_words[] = {
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

        constexpr inline std::tuple<ustring_view, token_type> reserved_symbols[] = {
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
    }

    struct lexer {
        template <typename Reader>
        lexer(Reader &&reader):
            reader_(std::forward<Reader>(reader)) {
            cache_[0] = token();
            cache_[1] = next_token();
            cache_[2] = next_token();
        }

    public:
        lexer &advance() {
            cache_[0] = std::move(cache_[1]);
            cache_[1] = std::move(cache_[2]);
            cache_[2] = next_token();
            return *this;
        }

        const token &prev() const noexcept {
            return cache_[0];
        }
        const token &cur() const noexcept {
            return cache_[1];
        }
        const token &next() const noexcept {
            return cache_[2];
        }

        bool empty() const noexcept {
            return cur().type == token_type::eof;
        }

    private:
        token next_token() {
            auto t = look_token();
            reader_.advance(std::size(t.raw));
            return t;
        }

        token look_token() {
            while (skip_ws() || skip_comment());

            if (reader_.empty()) {
                return token(snull, token_type::eof, reader_.loc());
            }

            if (auto v = vident(); 0 < std::size(v)) {
                for (auto [s, t] : constants::reserved_words) {
                    if (roughly_equal(v, s)) {
                        return token(s, t, reader_.loc());
                    }
                }
            }

            for (auto [s, t] : constants::reserved_symbols) {
                if (reader_.equal(s, roughly_equal)) {
                    return token(s, t, reader_.loc());
                }
            }

            if (auto v = real(); 0 < std::size(v)) {
                return token(v, token_type::real, reader_.loc());
            }

            if (auto v = real_exp(); 0 < std::size(v)) {
                return token(v, token_type::real_exp, reader_.loc());
            }

            if (auto v = int_2(); 0 < std::size(v)) {
                return token(v, token_type::int_2, reader_.loc());
            }

            if (auto v = int_10(); 0 < std::size(v)) {
                return token(v, token_type::int_10, reader_.loc());
            }

            if (auto v = int_16(); 0 < std::size(v)) {
                return token(v, token_type::int_16, reader_.loc());
            }

            if (auto v = string(); 0 < std::size(v)) {
                return token(v, token_type::string, reader_.loc());
            }

            if (auto v = label(); 0 < std::size(v)) {
                return token(v, token_type::label, reader_.loc());
            }

            if (auto v = vident(); 0 < std::size(v)) {
                return token(v, token_type::vident, reader_.loc());
            }

            if (auto v = cident(); 0 < std::size(v)) {
                return token(v, token_type::cident, reader_.loc());
            }

            if (auto v = eol(); 0 < std::size(v)) {
                return token(v, token_type::eol, reader_.loc());
            }

            auto v = reader_.match([](auto c) {
                return !is_space(c) && !is_newline(c);
            });
            return token(v, token_type::unknown, reader_.loc());
        }

    private:
        bool skip_comment() {
            bool cont = reader_.equal(u'\\');
            if (!roughly_equal(vident(), u"REM") && !cont && !reader_.equal(u'\'')) {
                return false;
            }

            // until newline
            reader_.skip([](auto c) {
                return !is_newline(c);
            });

            // ignore newline
            if (cont) {
                reader_.advance(1);
            }

            return true;
        }

        bool skip_ws() {
            return reader_.skip(is_space);
        }

    private:
        ustring_view vident() const {
            bool first = true;
            bool last = false;

            return reader_.match([&](auto c) {
                if (last) {
                    return false;
                }

                if (std::exchange(first, false)) {
                    return is_alnumbar(c) && !is_digit(c);
                }

                if (is_alnumbar(c)) {
                    return true;
                }

                last = true;
                return constants::variable_suffix.find(c) != ustring_view::npos;
            });
        }

        ustring_view cident() const {
            bool first = true;
            bool last = false;

            auto v = reader_.match([&](auto c) {
                if (last) {
                    return false;
                }

                if (std::exchange(first, false)) {
                    return c == u'#';
                }

                if (is_alnumbar(c)) {
                    return true;
                }

                last = true;
                return constants::variable_suffix.find(c) != ustring_view::npos;
            });

            if (1 < std::size(v)) {
                return v;
            }

            return snull;
        }

        ustring_view int_2() const {
            return int_(u"&B", [](auto c) {
                return u'0' <= c && c <= u'1';
            });
        }

        ustring_view int_10() const {
            return int_(snull, is_digit);
        }

        ustring_view int_16() const {
            return int_(u"&H", [](auto c) {
                c = to_upper(c);
                return is_digit(c) || (u'A' <= c && c <= u'F');
            });
        }

        template <typename Pred>
        ustring_view int_(ustring_view prefix, Pred &&pred) const {
            if (!reader_.equal(prefix, roughly_equal)) {
                return snull;
            }

            auto s = std::size(reader_.match(std::size(prefix), pred));
            if (0 < s) {
                return substr(reader_.view(), 0, std::size(prefix) + s);
            }

            return snull;
        }

        ustring_view real() const {
            auto l = reader_.match(is_digit);
            if (!reader_.equal(std::size(l), u'.')) {
                return snull;
            }
            auto r = reader_.match(std::size(l) + 1, is_digit);

            if (1 < std::size(l) + std::size(r) + 1) {
                return substr(reader_.view(), 0, std::size(l) + std::size(r) + 1);
            }
            return snull;
        }

        ustring_view real_exp() const {
            auto s = std::max(std::size(reader_.match(is_digit)), std::size(real()));
            if (s <= 0 || reader_.equal(s - 1, u'.') || !reader_.equal(s, u'E', roughly_equal_c)) {
                return snull;
            }
            ++s;

            bool sign = false;
            sign |= reader_.equal(s, u'+');
            sign |= reader_.equal(s, u'-');

            auto t = std::size(reader_.match(s + sign, is_digit));
            if (t <= 0) {
                return snull;
            }

            return substr(reader_.view(), 0, s + t + sign);
        }

        ustring_view string() const {
            bool first = true;
            bool last = false;

            return reader_.match([&](auto c) {
                if (last) {
                    return false;
                }

                if (std::exchange(first, false)) {
                    return c == u'"';
                }

                if (is_newline(c)) {
                    return false;
                }

                if (c == u'"') {
                    last = true;
                }
                return true;
            });
        }

        ustring_view label() const {
            bool first = true;

            return reader_.match([&](auto c) {
                if (std::exchange(first, false)) {
                    return c == u'@';
                }
                return is_alnumbar(c);
            });
        }

        ustring_view eol() const {
            for (auto c : constants::newline) {
                if (reader_.equal(c)) {
                    return substr(reader_.view(), 0, 1);
                }
            }
            return snull;
        }

    private:
        string_reader reader_;
        token cache_[3];
    };
}

