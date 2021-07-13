#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <cstddef>

namespace sb4 {
    using std::size_t;
    using uchar = char16_t;
    using ustring = std::u16string;
    using ustring_view = std::u16string_view;
    using namespace std::string_view_literals;

    constexpr inline ustring_view snull = ustring_view();

    namespace constants {
        constexpr inline ustring_view space = u" \t\v\f";
        constexpr inline ustring_view newline = u"\r\n";
    }

    template <typename String>
    constexpr String substr(
        const String &s,
        typename String::size_type pos = 0,
        typename String::size_type n = String::npos) {
        return s.substr(std::min(pos, s.size()), n);
    }

    constexpr bool is_upper(uchar c) noexcept {
        return u'A' <= c && c <= u'Z';
    }

    constexpr bool is_lower(uchar c) noexcept {
        return u'a' <= c && c <= u'z';
    }

    constexpr bool is_alpha(uchar c) noexcept {
        return is_upper(c) || is_lower(c);
    }

    constexpr bool is_digit(uchar c) noexcept {
        return u'0' <= c && c <= u'9';
    }

    constexpr bool is_alnum(uchar c) noexcept {
        return is_alpha(c) || is_digit(c);
    }

    constexpr bool is_alnumbar(uchar c) noexcept {
        return is_alnum(c) || c == u'_';
    }

    constexpr bool is_space(uchar c) noexcept {
        return constants::space.find(c) != ustring_view::npos;
    }

    constexpr bool is_newline(uchar c) noexcept {
        return constants::newline.find(c) != ustring_view::npos;
    }

    constexpr uchar to_upper(uchar c) noexcept {
        if (is_lower(c)) {
            return c - u'a' + u'A';
        }
        return c;
    }

    constexpr uchar to_lower(uchar c) noexcept {
        if (is_upper(c)) {
            return c - u'A' + u'a';
        }
        return c;
    }

    constexpr bool roughly_equal_c(uchar l, uchar r) noexcept {
        return to_upper(l) == to_upper(r);
    }

    constexpr bool roughly_equal(ustring_view l, ustring_view r) {
        if (l.size() != r.size()) {
            return false;
        }

        for (ustring_view::size_type i = 0; i < l.size(); ++i) {
            if (!roughly_equal_c(l[i], r[i])) {
                return false;
            }
        }

        return true;
    }
}

