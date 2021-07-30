#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <charconv>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <cstddef>

namespace sb4 {
    using std::size_t;
    using std::int32_t;
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

    template <typename Int = int32_t>
    Int to_int(ustring_view s, int base) {
        std::string t(s.size(), ' ');
        std::transform(s.begin(), s.end(), t.begin(), [](auto x) {
            return char(x);
        });

        Int v = 0;
        auto r = std::from_chars(t.data(), t.data() + t.size(), v, base);

        if (r.ec != std::errc{} || r.ptr != t.data() + t.size()) {
            if (r.ec == std::errc::result_out_of_range) {
                throw std::out_of_range("overflow");
            }
            throw std::invalid_argument("conversion failed");
        }

        return v;
    }

    double to_real(ustring_view s) {
        std::string t(s.size(), ' ');
        std::transform(s.begin(), s.end(), t.begin(), [](auto x) {
            return char(x);
        });

        char *end = nullptr;

        errno = 0;
        double v = std::strtod(t.c_str(), &end);

        if (size_t(end - t.c_str()) != t.size()) {
            throw std::invalid_argument("conversion failed");
        }

        if (errno == ERANGE) {
            throw std::out_of_range("overflow/underflow");
        }

        return v;
    }
}

