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
        struct subscript : expression {
            void accept(ivisitor &) override;

            subscript(location loc, expression_pointer left, expression_list indexes):
                expression(loc), left(std::move(left)), indexes(std::move(indexes)) {
            }

            expression_pointer left;
            expression_list indexes;
        };

        struct ivisitor {
            virtual ~ivisitor() = default;

            virtual void visit(null &) = 0;
            virtual void visit(vident &) = 0;
            virtual void visit(cident &) = 0;
            virtual void visit(int_ &) = 0;
            virtual void visit(real &) = 0;
            virtual void visit(string &) = 0;
            virtual void visit(label &) = 0;
            virtual void visit(binary &) = 0;
            virtual void visit(unary &) = 0;
            virtual void visit(call_function &) = 0;
            virtual void visit(subscript &) = 0;
        };

        inline void null::accept(ivisitor &v) { v.visit(*this); }
        inline void vident::accept(ivisitor &v) { v.visit(*this); }
        inline void cident::accept(ivisitor &v) { v.visit(*this); }
        inline void int_::accept(ivisitor &v) { v.visit(*this); }
        inline void real::accept(ivisitor &v) { v.visit(*this); }
        inline void string::accept(ivisitor &v) { v.visit(*this); }
        inline void label::accept(ivisitor &v) { v.visit(*this); }
        inline void binary::accept(ivisitor &v) { v.visit(*this); }
        inline void unary::accept(ivisitor &v) { v.visit(*this); }
        inline void call_function::accept(ivisitor &v) { v.visit(*this); }
        inline void subscript::accept(ivisitor &v) { v.visit(*this); }
    }
}

