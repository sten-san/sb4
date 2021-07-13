#pragma once
#include <cstddef>

namespace sb4 {
    using std::size_t;

    struct location {
        constexpr location(size_t row, size_t col) noexcept:
            row(row), col(col) {
        }
        constexpr location() noexcept:
            location(1, 1) {
        }

        constexpr void next_row() noexcept {
            ++row; col = 1;
        }
        constexpr void next_col() noexcept {
            ++col;
        }

        size_t row, col;
    };
}

