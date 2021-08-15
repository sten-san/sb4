#pragma once
#include <algorithm>
#include <utility>
#include <memory>
#include <stdexcept>
#include <cstdint>
#include "sb4/include/ast.hpp"
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
            using namespace sb4::ast;

            auto lead = [&]() -> expression_pointer {
                auto token = lex_.cur();

                // parse unary
                if (lex_.consume(token_class::unary)) {
                    return std::make_unique<expr::unary>(
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
                    lead = std::make_unique<expr::subscript>(
                        op.loc, std::move(lead), parse_enclosed_expression_list()
                    );

                    if (lex_.consume(token_type::rsub)) {
                        continue;
                    }

                    throw std::runtime_error("']' not found");
                }

                if (lex_.consume(token_class::binary)) {
                    lead = std::make_unique<expr::binary>(
                        op.loc, std::move(lead), parse_expression(to_rank(op.type)), op.type
                    );
                    continue;
                }
            }

            return lead;
        }

        ast::expression_pointer parse_atomic() {
            using namespace sb4::ast;

            auto token = lex_.cur();

            if (lex_.consume(token_type::cident)) {
                return std::make_unique<expr::cident>(
                    token.loc, token.raw
                );
            }

            if (lex_.consume(token_class::int_)) {
                return std::make_unique<expr::int_>(
                    token.loc, detail::to_int(token.raw, token.type)
                );
            }

            if (lex_.consume(token_class::real)) {
                return std::make_unique<expr::real>(
                    token.loc, detail::to_real(token.raw, token.type)
                );
            }

            if (lex_.consume(token_type::string)) {
                return std::make_unique<expr::string>(
                    token.loc, detail::to_string(token.raw)
                );
            }

            if (lex_.consume(token_type::label)) {
                return std::make_unique<expr::label>(
                    token.loc, token.raw
                );
            }

            if (lex_.consume(token_type::vident)) {
                if (!lex_.consume(token_type::lparen)) {
                    return std::make_unique<expr::vident>(
                        token.loc, token.raw
                    );
                }

                auto list = parse_enclosed_expression_list();
                if (lex_.consume(token_type::rparen)) {
                    return std::make_unique<expr::call_function>(
                        token.loc, token.raw, std::move(list)
                    );
                }

                throw std::runtime_error("')' not found");
            }

            if (lex_.consume(token_class::bfunction)) {
                if (!lex_.consume(token_type::lparen)) {
                    throw std::runtime_error("'(' not found");
                }

                auto list = parse_enclosed_expression_list();
                if (lex_.consume(token_type::rparen)) {
                    return std::make_unique<expr::call_bfunction>(
                        token.loc, token.type, std::move(list)
                    );
                }

                throw std::runtime_error("')' not found");
            }

            throw std::runtime_error("parse atomic failed");
        }

        ast::expression_pointer parse_label() {
            auto token = lex_.cur();
            if (!lex_.consume(token_type::label)) {
                throw std::runtime_error("<label> not found");
            }

            return std::make_unique<ast::expr::label>(
                token.loc, token.raw
            );
        }

        template <typename F, typename G, typename H, typename I>
        token_type parse_expression_list(F &&first, G &&later, H &&is_delimiter, I &&is_terminal) {
            using namespace sb4::ast;

            auto make_null = [&]() {
                return std::make_unique<expr::null>(lex_.cur().loc);
            };

            auto push = [&, f = true](token_type del, auto expr) mutable {
                if (std::exchange(f, false)) {
                    first(std::move(expr));
                }
                else {
                    later(del, std::move(expr));
                }
            };

            auto del = lex_.cur().type;

            int count = 0;
            bool next = true;
            while (!is_terminal(lex_.cur().type) && next) {
                if (is_delimiter(lex_.cur().type)) {
                    push(del, make_null());
                }
                else {
                    push(del, parse_expression());
                }
                ++count;

                if ((next = is_delimiter(del = lex_.cur().type))) {
                    lex_.advance();
                }
            }

            if (0 < count && next) {
                push(del, make_null());
            }

            return del;
        }

        // ("," | <expression>)* ("]" | ")" | ":" | <eol> | <eof>)
        // ("]" | ")" | ":" | <eol> | <eof>) is not consume
        ast::expression_list parse_enclosed_expression_list() {
            using namespace sb4::ast;

            expression_list list;

            auto first = [&](auto expr) {
                list.push_back(std::move(expr));
            };
            auto later = [&](token_type, auto expr) {
                list.push_back(std::move(expr));
            };
            auto is_delimiter = [&](token_type del) {
                return del == token_type::comma;
            };
            auto is_terminal = [&](token_type term) {
                return belong(term, token_class::terminal);
            };

            parse_expression_list(first, later, is_delimiter, is_terminal);

            return list;
        }

        // ("," | <expression>)* (<separator> || (<reserved> && !<bfunction>))
        // (<separator> || (<reserved> && !<bfunction>)) is not consume
        ast::expression_list parse_unenclosed_expression_list() {
            using namespace sb4::ast;

            expression_list list;

            auto first = [&](auto expr) {
                list.push_back(std::move(expr));
            };
            auto later = [&](token_type, auto expr) {
                list.push_back(std::move(expr));
            };
            auto is_delimiter = [&](token_type del) {
                return del == token_type::comma;
            };

            parse_expression_list(first, later, is_delimiter, is_unenclosed_terminal);

            return list;
        }

        constexpr static inline bool is_unenclosed_terminal(token_type term) {
            return
                belong(term, token_class::separator) ||
                (belong(term, token_class::reserved) && !belong(term, token_class::bfunction));
        }

    private:
        ast::statement_pointer parse_statement() {
            if (auto v = parse_if()) {
                return v;
            }
            if (auto v = parse_goto()) {
                return v;
            }
            if (auto v = parse_print()) {
                return v;
            }

            throw std::runtime_error("parse statement failed");
        }

        template <typename ...Args>
        ast::statement_list parse_statements(Args ...until) {
            ast::statement_list list;
            while ((skip_separator(), !(lex_.equal(until...) || is_terminal()))) {
                list.push_back(parse_statement());
            }
            return list;
        }

        ast::statement_pointer parse_if(bool elseif = false) {
            using namespace sb4::ast;

            auto loc = lex_.cur().loc;
            if (!lex_.consume(elseif ? token_type::elseif : token_type::if_)) {
                return nullptr;
            }

            auto if_ = std::make_unique<stmt::if_>(loc);
            if_->cond = parse_expression();

            struct scope {
                scope(bool &v): v(v), save(v) {}
                ~scope() { v = save; }
                bool &v, save;
            } _(context_.oneline);

            context_.oneline = true;
            [&] {
                // if <expr> goto <label>
                if (auto goto_ = lex_.cur().loc; lex_.consume(token_type::goto_)) {
                    if_->then.push_back(std::make_unique<stmt::goto_>(
                        goto_, parse_label()
                    ));
                    return;
                }

                // then
                if (!lex_.consume(token_type::then)) {
                    throw std::runtime_error("<then> not found");
                }

                // if <expr> then <label>
                if (auto loc = lex_.cur().loc; lex_.equal(token_type::label)) {
                    if_->then.push_back(std::make_unique<stmt::goto_>(
                        loc, parse_label()
                    ));
                    return;
                }

                context_.oneline &= !lex_.consume(token_type::eol);
                if_->then = parse_statements(
                    token_type::elseif, token_type::else_, token_type::endif
                );
            }();

            // elseif
            if (auto v = parse_if(true)) {
                if_->else_.push_back(std::move(v));
                return if_;
            }

            // else
            if (lex_.consume(token_type::else_)) {
                // else <label>
                if (auto loc = lex_.cur().loc; lex_.equal(token_type::label)) {
                    if_->else_.push_back(std::make_unique<stmt::goto_>(
                        loc, parse_label()
                    ));
                }
                else {
                    context_.oneline &= !lex_.consume(token_type::eol);
                    if_->else_ = parse_statements(token_type::endif);
                }
            }

            // endif
            if (!lex_.consume(token_type::endif) && !context_.oneline) {
                throw std::runtime_error("<endif> not found");
            }

            return if_;
        }

        ast::statement_pointer parse_goto() {
            return nullptr;
        }

        // <print> ("," | ";" | <expression>)*
        ast::statement_pointer parse_print() {
            using namespace sb4::ast;

            auto loc = lex_.cur().loc;
            if (!lex_.consume(token_type::print)) {
                return nullptr;
            }

            auto print = std::make_unique<stmt::print>(loc);

            auto first = [&](auto expr) {
                print->add_expression(std::move(expr));
            };
            auto later = [&](token_type del, auto expr) {
                if (del == token_type::comma) {
                    print->add_tab();
                }
                print->add_expression(std::move(expr));
            };
            auto is_delimiter = [&](token_type del) {
                return del == token_type::comma || del == token_type::semicolon;
            };

            auto last = parse_expression_list(first, later, is_delimiter, is_unenclosed_terminal);
            if (is_delimiter(last)) {
                print->args.pop_back();
            }
            else {
                print->add_newline();
            }

            return print;
        }

    private:
        bool is_terminal(token_type type) const {
            if (context_.oneline && type == token_type::eol) {
                return true;
            }
            return type == token_type::eof;
        }

        bool is_terminal() const {
            return is_terminal(lex_.cur().type);
        }

        void skip_separator() {
            while (lex_.equal(token_class::separator)) {
                if (is_terminal()) {
                    break;
                }
                lex_.advance();
            }
        }

    private:
        lexer lex_;

        struct {
            bool oneline = false;
        } context_;
    };
}

