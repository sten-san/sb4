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
        return is_alnum(c) || is_digit(c);
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

    constexpr bool roughly_equal(ustring_view l, ustring_view r) {
        if (l.size() != r.size()) {
            return false;
        }

        for (ustring_view::size_type i = 0; i < l.size(); ++i) {
            if (to_upper(l[i]) != to_upper(r[i])) {
                return false;
            }
        }

        return true;
    }

    template <typename Pred>
    constexpr size_t left_to(ustring_view s, Pred &&pred) {
        size_t count = 0;

        for (auto c : s) {
            if (!pred(c)) {
                break;
            }
            ++count;
        }

        return count;
    }
}

