#pragma once
#include <utility>
#include <iterator>
#include <tuple>
#include <cstddef>
#include "sb4/include/string.hpp"
#include "sb4/include/location.hpp"

namespace sb4 {
    using std::size_t;

    enum class token_type {
        unknown,

        // V, #V
        vident,
        cident,

        // &B00, 0, &H00
        int_2,
        int_10,
        int_16,

        // 1.0, 1e0
        real,
        real_exp,

        // "string", @LABEL
        string,
        label,

        // +, -, *, /, DIV, MOD
        plus,
        minus,
        mult,
        fdiv,
        idiv,
        mod,

        // &&, ||, !
        land,
        lor,
        lnot,

        // AND, OR, NOT, XOR
        band,
        bor,
        bnot,
        bxor,

        // ==, !=, <, <=, >, >=
        equal,
        nequal,
        less,
        lequal,
        greater,
        gequal,

        // <<, >>
        lshift,
        rshift,

        // <<<, >>>
        llshift,
        lrshift,

        // <<+, >>+
        rlshift,
        rrshift,

        // =
        assign,

        // ,
        comma,

        // :, ;
        colon,
        semicolon,

        // if, then, else, elseif, endif
        if_,
        then,
        else_,
        elseif,
        endif,

        // case, when, otherwise, endcase
        case_,
        when,
        otherwise,
        endcase,

        // goto, gosub, on
        goto_,
        gosub,
        on,

        // return
        return_,

        // loop, endloop
        loop,
        endloop,

        // for, next
        for_,
        next,

        // while, wend
        while_,
        wend,

        // repeat, until
        repeat,
        until,

        // break, continue
        break_,
        continue_,

        // common, def, end, out, defout
        common,
        def,
        end,
        out,
        defout,

        // var, dim
        var,
        dim,

        // data, read, restore
        data,
        read,
        restore,

        // print, tprint
        print,
        tprint,

        // input, linput
        input,
        linput,

        // call
        call,

        // swap
        swap,

        // exec
        exec,

        // const, enum
        const_,
        enum_,

        // (, )
        lparen,
        rparen,

        // [, ]
        lsub,
        rsub,

        eol,
        eof,
    };

    enum class token_class {
        ident,
        reserved,
        rvalue,
        int_,
        real,
        unary,
        binary,
        separator,
    };

    namespace constants {
        constexpr inline token_type ident_types[] = {
            token_type::vident,
            token_type::cident,
            token_type::label,
        };

        constexpr inline token_type reserved_types[] = {
            token_type::if_,
            token_type::then,
            token_type::else_,
            token_type::elseif,
            token_type::endif,
            token_type::case_,
            token_type::when,
            token_type::otherwise,
            token_type::endcase,
            token_type::goto_,
            token_type::gosub,
            token_type::on,
            token_type::return_,
            token_type::loop,
            token_type::endloop,
            token_type::for_,
            token_type::next,
            token_type::while_,
            token_type::wend,
            token_type::repeat,
            token_type::until,
            token_type::break_,
            token_type::continue_,
            token_type::common,
            token_type::def,
            token_type::end,
            token_type::out,
            token_type::defout,
            token_type::var,
            token_type::dim,
            token_type::data,
            token_type::read,
            token_type::restore,
            token_type::print,
            token_type::tprint,
            token_type::input,
            token_type::linput,
            token_type::call,
            token_type::swap,
            token_type::exec,
            token_type::const_,
            token_type::enum_,
        };

        constexpr inline token_type rvalue_types[] = {
            token_type::int_2,
            token_type::int_10,
            token_type::int_16,
            token_type::real,
            token_type::real_exp,
            token_type::string,
            token_type::label,
        };

        constexpr inline token_type int_types[] = {
            token_type::int_2,
            token_type::int_10,
            token_type::int_16,
        };

        constexpr inline token_type real_types[] = {
            token_type::real,
            token_type::real_exp,
        };

        constexpr inline token_type unary_types[] = {
            token_type::minus,
            token_type::lnot,
            token_type::bnot,
        };

        constexpr inline token_type binary_types[] = {
            token_type::plus,
            token_type::minus,
            token_type::mult,
            token_type::idiv,
            token_type::fdiv,
            token_type::mod,
            token_type::land,
            token_type::lor,
            token_type::band,
            token_type::bor,
            token_type::bxor,
            token_type::equal,
            token_type::nequal,
            token_type::less,
            token_type::lequal,
            token_type::greater,
            token_type::gequal,
            token_type::lshift,
            token_type::rshift,
            token_type::llshift,
            token_type::lrshift,
            token_type::rlshift,
            token_type::rrshift,
        };

        constexpr inline token_type separator_types[] = {
            token_type::colon,
            token_type::eol,
            token_type::eof,
        };

        constexpr inline std::tuple<size_t, const token_type *> types[] = {
            { std::size(ident_types), ident_types },
            { std::size(reserved_types), reserved_types },
            { std::size(rvalue_types), rvalue_types },
            { std::size(int_types), int_types },
            { std::size(real_types), real_types },
            { std::size(unary_types), unary_types },
            { std::size(binary_types), binary_types },
            { std::size(separator_types), separator_types },
        };
    }

    constexpr bool belong(token_type type, token_class class_) noexcept {
        auto index = size_t(class_);
        if (std::size(constants::types) <= index) {
            return false;
        }

        auto [size, first] = constants::types[index];
        auto last = first + size;

        while (first != last) {
            if (*first++ == type) {
                return true;
            }
        }

        return false;
    }

    struct token {
        token():
            token(snull, token_type::unknown, location(1, 1)) {
        }

        template <typename = nullptr_t>
        token(ustring &&raw, token_type type, location loc):
            raw(std::move(raw)), type(type), loc(loc) {
        }
        token(ustring_view raw, token_type type, location loc):
            raw(raw), type(type), loc(loc) {
        }

        bool belong(token_class class_) const noexcept {
            return sb4::belong(type, class_);
        }

        ustring raw;
        token_type type;
        location loc;
    };
}

