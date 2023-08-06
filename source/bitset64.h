#include <cstdint>

struct Bitset64 {
    uint64_t value = 0;

    Bitset64() = default;

    explicit Bitset64(uint64_t other_value) : value(other_value) {
    }

    struct BitsetReference {
        Bitset64& ref;
        uint64_t bit;

        BitsetReference& operator=(bool res) {
            if (res) {
                ref.value |= (1ull << bit);
            } else {
                ref.value &= ~(1ull << bit);
            }
            return *this;
        }

        operator bool() const {
            return ref.value & (1ull << bit);
        }
    };

    inline bool operator[](uint64_t index) const {
        return (value & (1ull << index));
    }

    inline BitsetReference operator[](uint64_t index) {
        return BitsetReference{*this, index};
    }

    inline Bitset64& operator|=(const Bitset64& other) {
        value |= other.value;
        return *this;
    }

    [[nodiscard]] inline uint64_t to_ullong() const {
        return value;
    }

    [[nodiscard]] inline uint64_t _Find_first() const {
        if (!value) {
            return 64;
        }
        return __builtin_ctzll(value);
    }

    [[nodiscard]] inline uint64_t _Find_next(uint64_t index) const {
        if (index >= 63) {
            return 64;
        }
        if (!(value & ~((1ull << (index + 1)) - 1))) {
            return 64;
        }
        return __builtin_ctzll(value & ~((1ull << (index + 1)) - 1));
    }
};

struct Bitset8 {
    uint8_t value = 0;

    Bitset8() = default;

    explicit Bitset8(uint8_t other_value) : value(other_value) {
    }

    struct BitsetReference {
        Bitset8& ref;
        uint8_t bit;

        BitsetReference& operator=(bool res) {
            if (res) {
                ref.value |= (1ull << bit);
            } else {
                ref.value &= ~(1ull << bit);
            }
            return *this;
        }

        operator bool() const {
            return ref.value & (1ull << bit);
        }
    };

    inline bool operator[](uint64_t index) const {
        return (value & (1ull << index));
    }

    inline BitsetReference operator[](uint8_t index) {
        return BitsetReference{*this, index};
    }

    inline Bitset8& operator|=(const Bitset8& other) {
        value |= other.value;
        return *this;
    }

    [[nodiscard]] inline uint64_t to_ullong() const {
        return value;
    }

    [[nodiscard]] inline uint64_t _Find_first() const {
        if (!value) {
            return 8;
        }
        return __builtin_ctzll(value);
    }

    [[nodiscard]] inline uint64_t _Find_next(uint64_t index) const {
        if (index >= 7) {
            return 8;
        }
        if (!(value & ~((1 << (index + 1)) - 1))) {
            return 8;
        }
        return __builtin_ctzll(value & ~((1 << (index + 1)) - 1));
    }
};
