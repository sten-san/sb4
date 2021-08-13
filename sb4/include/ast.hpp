#pragma once
#include <utility>
#include <memory>
#include <vector>
#include <cstdint>
#include "sb4/include/token.hpp"
#include "sb4/include/string.hpp"
#include "sb4/include/location.hpp"

namespace sb4 {
    using std::int32_t;

    namespace ast {
        struct ivisitor;

        struct node {
            virtual ~node() = default;

            node(location loc):
                loc(loc) {
            }

            virtual void accept(ivisitor &) = 0;

            location loc;
        };

        struct statement : node {
            statement(location loc):
                node(loc) {
            }
        };
        struct expression : node {
            expression(location loc):
                node(loc) {
            }
        };

        using statement_pointer = std::unique_ptr<statement>;
        using expression_pointer = std::unique_ptr<expression>;

        using statement_list = std::vector<statement_pointer>;
        using expression_list = std::vector<expression_pointer>;

        namespace expr {
            struct null : expression {
                void accept(ivisitor &) override;

                null(location loc):
                    expression(loc) {
                }
            };
            struct vident : expression {
                void accept(ivisitor &) override;

                vident(location loc, ustring_view name):
                    expression(loc), name(name) {
                }

                ustring name;
            };
            struct cident : expression {
                void accept(ivisitor &) override;

                cident(location loc, ustring_view name):
                    expression(loc), name(name) {
                }

                ustring name;
            };
            struct int_ : expression {
                void accept(ivisitor &) override;

                int_(location loc, int32_t value):
                    expression(loc), value(value) {
                }

                int32_t value;
            };
            struct real : expression {
                void accept(ivisitor &) override;

                real(location loc, double value):
                    expression(loc), value(value) {
                }

                double value;
            };
            struct string : expression {
                void accept(ivisitor &) override;

                string(location loc, ustring_view value):
                    expression(loc), value(value) {
                }

                ustring value;
            };
            struct label : expression {
                void accept(ivisitor &) override;

                label(location loc, ustring_view value):
                    expression(loc), value(value) {
                }

                ustring value;
            };
            struct binary : expression {
                void accept(ivisitor &) override;

                binary(location loc, expression_pointer left, expression_pointer right, token_type type):
                    expression(loc), left(std::move(left)), right(std::move(right)), type(type) {
                }

                expression_pointer left, right;
                token_type type;
            };
            struct unary : expression {
                void accept(ivisitor &) override;

                unary(location loc, expression_pointer right, token_type type):
                    expression(loc), right(std::move(right)), type(type) {
                }

                expression_pointer right;
                token_type type;
            };
            struct call_function : expression {
                void accept(ivisitor &) override;

                call_function(location loc, ustring_view name, expression_list args):
                    expression(loc), name(name), args(std::move(args)) {
                }

                ustring name;
                expression_list args;
            };
            struct call_bfunction : expression {
                void accept(ivisitor &) override;

                call_bfunction(location loc, token_type type, expression_list args):
                    expression(loc), type(type), args(std::move(args)) {
                }

                token_type type;
                expression_list args;
            };
            struct subscript : expression {
                void accept(ivisitor &) override;

                subscript(location loc, expression_pointer left, expression_list indexes):
                    expression(loc), left(std::move(left)), indexes(std::move(indexes)) {
                }

                expression_pointer left;
                expression_list indexes;
            };
        }

        namespace stmt {
            struct print : statement {
                void accept(ivisitor &) override;

                enum class argument_type {
                    expression,
                    newline,
                    tab,
                };

                struct argument {
                    argument_type type;
                    expression_pointer expr;
                };

                print(location loc):
                    print(loc, {}) {
                }
                print(location loc, std::vector<argument> args):
                    statement(loc), args(std::move(args)) {
                }

                void add_expression(expression_pointer expr) {
                    args.push_back({ argument_type::expression, std::move(expr) });
                }
                void add_newline() {
                    args.push_back({ argument_type::newline, nullptr });
                }
                void add_tab() {
                    args.push_back({ argument_type::tab, nullptr });
                }

                std::vector<argument> args;
            };
        }

        struct ivisitor {
            virtual ~ivisitor() = default;

            virtual void visit(expr::null &) = 0;
            virtual void visit(expr::vident &) = 0;
            virtual void visit(expr::cident &) = 0;
            virtual void visit(expr::int_ &) = 0;
            virtual void visit(expr::real &) = 0;
            virtual void visit(expr::string &) = 0;
            virtual void visit(expr::label &) = 0;
            virtual void visit(expr::binary &) = 0;
            virtual void visit(expr::unary &) = 0;
            virtual void visit(expr::call_function &) = 0;
            virtual void visit(expr::call_bfunction &) = 0;
            virtual void visit(expr::subscript &) = 0;

            virtual void visit(stmt::print &) = 0;
        };

        inline void expr::null::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::vident::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::cident::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::int_::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::real::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::string::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::label::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::binary::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::unary::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::call_function::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::call_bfunction::accept(ivisitor &v) { v.visit(*this); }
        inline void expr::subscript::accept(ivisitor &v) { v.visit(*this); }

        inline void stmt::print::accept(ivisitor &v) { v.visit(*this); }
    }
}

