#pragma once
#include <cstddef>

namespace sb4 {
    using std::size_t;

    struct location {
        constexpr location(size_t row, size_t col) noexcept:
            row(row), col(col) {
        }
        constexpr location() noexcept:
            location(0, 0) {
        }

        size_t row = 0;
        size_t col = 0;
    };
}

