#include <cstdint>
#include <type_traits>
#include <bit>

using namespace std;

// Base template for endianint
template <endian E, typename T, typename Enable = std::enable_if_t<std::is_integral_v<T>>> class endianint;

static_assert(endian::native == endian::little or endian::native == endian::big,
              "Mixed endian machines are not supported");

template <endian E>
class endianint<E, uint16_t> {

    static constexpr uint16_t to_E(uint16_t val) {
        if constexpr ((E == endian::big and endian::native == endian::little) or 
                      (E == endian::little and endian::native == endian::big)) {
            return ((val & 0xFF00) >> 8u) |
                   ((val & 0x00FF) << 8u);
        }
        return val;
    }

    static constexpr uint16_t from_E(uint16_t val) {
        return to_E(val); // commutative op!
    }

public:
    uint16_t value;
    endianint() : value(0) {}
    endianint(uint16_t val) : value(to_E(val)) {}

    endianint& operator=(uint16_t val) {
        value = to_E(val);
        return *this;
    }

    operator uint16_t() const {
        return from_E(value);
    }

};

template <endian E>
class endianint<E, uint32_t> {

    static constexpr uint32_t to_E(uint32_t val) {
        if constexpr ((E == endian::big and endian::native == endian::little) or 
                      (E == endian::little and endian::native == endian::big)) {
            return ((val & 0xFF000000u) >> 24u) |
                   ((val & 0x00FF0000u) >>  8u) |
                   ((val & 0x0000FF00u) <<  8u) |
                   ((val & 0x000000FFu) << 24u);
        }
        return val;
    }

    static constexpr uint32_t from_E(uint32_t val) {
        return to_E(val); // commutative op!
    }

public:
    uint32_t value;
    endianint() : value(0) {}
    endianint(uint32_t val) : value(to_E(val)) {}

    endianint& operator=(uint32_t val) {
        value = to_E(val);
        return *this;
    }

    operator uint32_t() const {
        return from_E(value);
    }

};

template <endian E>
class endianint<E, uint64_t> {

    static constexpr uint64_t to_E(uint64_t val) {
        if constexpr ((E == endian::big and endian::native == endian::little) or 
                      (E == endian::little and endian::native == endian::big)) {
            // these should be optimized down to __builtin_bswap64 as needed.
            return ((val & 0xFF00000000000000u) >> 56u) |
                   ((val & 0x00FF000000000000u) >> 40u) |
                   ((val & 0x0000FF0000000000u) >> 24u) |
                   ((val & 0x000000FF00000000u) >>  8u) |
                   ((val & 0x00000000FF000000u) <<  8u) |      
                   ((val & 0x0000000000FF0000u) << 24u) |
                   ((val & 0x000000000000FF00u) << 40u) |
                   ((val & 0x00000000000000FFu) << 56u);
        }
        return val;
    }

    static constexpr uint64_t from_E(uint64_t val) {
        return to_E(val); // commutative op!
    }

public:
    uint64_t value;
    endianint() : value(0) {}
    endianint(uint64_t val) : value(to_E(val)) {}

    endianint& operator=(uint64_t val) {
        value = to_E(val);
        return *this;
    }

    operator uint64_t() const {
        return from_E(value);
    }
};
