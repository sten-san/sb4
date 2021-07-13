#pragma once
#include <utility>
#include <iterator>
#include "sb4/include/string.hpp"
#include "sb4/include/location.hpp"

namespace sb4 {
    struct string_reader {
        template <typename = nullptr_t>
        string_reader(ustring &&raw, location loc = { 1, 1 }):
            raw_(std::move(raw)), cur_(), loc_(loc) {
            cur_ = raw_;
        }
        string_reader(ustring_view raw, location loc = { 1, 1 }):
            string_reader(ustring(raw), loc) {
        }
        template <typename InIter>
        string_reader(InIter first, InIter last, location loc = { 1, 1 }):
            string_reader(ustring(first, last), loc) {
        }

    public:
        template <typename Pred>
        ustring_view match(Pred &&pred) const {
            size_t count = 0;
            for (auto c : cur_) {
                if (pred(c)) {
                    ++count;
                    continue;
                }
                break;
            }
            return substr(cur_, 0, count);
        }

        template <typename Pred>
        size_t skip(Pred &&pred) {
            auto s = std::size(match(std::forward<Pred>(pred)));
            advance(s);
            return s;
        }

        bool equal(ustring_view s) const {
            return substr(cur_, 0, std::size(s)) == s;
        }
        bool equal(uchar c) const {
            return !empty() && cur_[0] == c;
        }

        size_t skip(ustring_view s) {
            if (equal(s)) {
                advance(std::size(s));
                return std::size(s);
            }
            return 0;
        }
        size_t skip(uchar c) {
            if (equal(c)) {
                advance();
                return 1;
            }
            return 0;
        }

        void advance(size_t count = 1) {
            while (0 < count && !empty()) {
                if (is_newline(cur_[0])) {
                    loc_.next_row();
                }
                else {
                    loc_.next_col();
                }
                cur_ = substr(cur_, 1); --count;
            }
        }

        size_t skip_ws() {
            return skip(is_space);
        }

        ustring_view view() const noexcept {
            return cur_;
        }

        size_t size() const noexcept {
            return std::size(cur_);
        }

        bool empty() const noexcept {
            return std::size(cur_) == 0;
        }

        location loc() const noexcept { return loc_; }
        size_t row() const noexcept { return loc_.row; }
        size_t col() const noexcept { return loc_.col; }

    private:
        ustring raw_;
        ustring_view cur_;
        location loc_;
    };
}

