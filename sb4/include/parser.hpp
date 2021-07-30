#pragma once
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <cstdint>
#include "sb4/include/lexer.hpp"
#include "sb4/include/string.hpp"

namespace sb4 {
    using std::int32_t;

    namespace detail {
        // throw out_of_range
        int32_t to_int(ustring_view s, token_type type) {
            try {
                if (type == token_type::int_2) {
                    // skip "&B"
                    return sb4::to_int(substr(s, 2), 2);
                }

                if (type == token_type::int_10) {
                    return sb4::to_int(s, 10);
                }

                if (type == token_type::int_16) {
                    // skip "&H"
                    return sb4::to_int(substr(s, 2), 16);
                }
            }
            catch (std::invalid_argument &e) {
                return 0;
            }

            return 0;
        }

        // throw out_of_range
        double to_real(ustring_view s, token_type /*type*/) {
            try {
                return sb4::to_real(s);
            }
            catch (std::invalid_argument &e) {
                return 0;
            }

            return 0;
        }

        // "string", "string
        ustring_view to_string(ustring_view s) {
            if (s.size() == 0 || s[0] != u'"') {
                return snull;
            }

            if (auto f = s.find(u'"', 1); s.size() - 1 <= f) {
                return substr(s, 1, std::min(s.size(), f) - 1);
            }

            return snull;
        }
    }

    struct parser {
        parser(lexer lex):
            lex_(std::move(lex)) {
        }

    public:
        auto parse() {
            return parse_expression();
        }

    private:
        enum operator_rank {
            lowest,

            // ||
            lor,
            // &&
            land,
            // AND, OR, XOR
            bitwise_binary,
            // <, >, <=, >=, ==, !=
            compare,
            // <<, >>, <<<, >>>, <<+, >>+
            shift,
            // +, -
            plus,
            // *, /, DIV, MOD
            mult,
            // !, -, NOT
            unary,
            // []
            subscript,
        };

        static inline operator_rank to_rank(token_type type) {
            switch (type) {
            case token_type::lor:
                return lor;

            case token_type::land:
                return land;

            case token_type::band:
            case token_type::bor:
            case token_type::bxor:
                return bitwise_binary;

            case token_type::less:
            case token_type::greater:
            case token_type::lequal:
            case token_type::gequal:
            case token_type::equal:
            case token_type::nequal:
                return compare;

            case token_type::lshift:
            case token_type::rshift:
            case token_type::llshift:
            case token_type::lrshift:
            case token_type::rlshift:
            case token_type::rrshift:
                return shift;

            case token_type::plus:
            case token_type::minus:
                return plus;

            case token_type::mult:
            case token_type::fdiv:
            case token_type::idiv:
            case token_type::mod:
                return mult;

            case token_type::lsub:
                return subscript;

            default:
                return lowest;
            }
        }

    private:
        ast::expression_pointer parse_expression(operator_rank prev = lowest) {
            auto lead = [&]() -> ast::expression_pointer {
                auto token = lex_.cur();

                // parse unary
                if (lex_.consume(token_class::unary)) {
                    return std::make_unique<ast::unary>(
                        token.loc, parse_expression(unary), token.type
                    );
                }

                // parse paren
                if (lex_.consume(token_type::lparen)) {
                    auto inner = parse_expression(lowest);

                    if (lex_.consume(token_type::rparen)) {
                        return inner;
                    }

                    throw std::runtime_error("')' not found");
                }

                return parse_atomic();
            }();

            while (prev < to_rank(lex_.cur().type)) {
                auto op = lex_.cur();

                if (lex_.consume(token_type::lsub)) {
                    lead = std::make_unique<ast::subscript>(
                        op.loc, std::move(lead), parse_expression_list()
                    );

                    if (lex_.consume(token_type::rsub)) {
                        continue;
                    }

                    throw std::runtime_error("']' not found");
                }

                if (lex_.consume(token_class::binary)) {
                    lead = std::make_unique<ast::binary>(
                        op.loc, std::move(lead), parse_expression(), op.type
                    );
                    continue;
                }
            }

            return lead;
        }

        ast::expression_pointer parse_atomic() {
            auto token = lex_.cur();

            if (lex_.consume(token_type::cident)) {
                return std::make_unique<ast::cident>(
                    token.loc, token.raw
                );
            }

            if (lex_.consume(token_class::int_)) {
                return std::make_unique<ast::int_>(
                    token.loc, detail::to_int(token.raw, token.type)
                );
            }

            if (lex_.consume(token_class::real)) {
                return std::make_unique<ast::real>(
                    token.loc, detail::to_real(token.raw, token.type)
                );
            }

            if (lex_.consume(token_type::string)) {
                return std::make_unique<ast::string>(
                    token.loc, detail::to_string(token.raw)
                );
            }

            if (lex_.consume(token_type::label)) {
                return std::make_unique<ast::label>(
                    token.loc, token.raw
                );
            }

            if (lex_.consume(token_type::vident)) {
                if (!lex_.consume(token_type::lparen)) {
                    return std::make_unique<ast::vident>(
                        token.loc, token.raw
                    );
                }

                auto list = parse_expression_list();
                if (lex_.consume(token_type::rparen)) {
                    return std::make_unique<ast::call_function>(
                        token.loc, token.raw, std::move(list)
                    );
                }

                throw std::runtime_error("')' not found");
            }

            throw std::runtime_error("parse atomic failed");
        }

        ast::expression_list parse_expression_list() {
            ast::expression_list list;

            while (!lex_.equal(token_class::terminal)) {
                list.push_back(parse_expression());
            }

            return list;
        }

    private:
        lexer lex_;
    };
}

