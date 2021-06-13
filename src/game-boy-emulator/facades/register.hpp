#pragma once

#include <type_traits>
#include <cstdint>


struct AF;
struct BC;
struct DE;
struct HL;
struct SP;
struct PC;
struct A;
struct B;
struct C;
struct D;
struct E;
struct F;
struct H;
struct L;

/**
 * Template helper functions which characterize register type
 */

/**
 * Condition true if byte register
 */
template <typename T>
struct is_byte_register {
    const static bool value = false;
};

template <>
struct is_byte_register<A> {
    const static bool value = true;
};

template <>
struct is_byte_register<B> {
    const static bool value = true;
};

template <>
struct is_byte_register<C> {
    const static bool value = true;
};

template <>
struct is_byte_register<D> {
    const static bool value = true;
};

template <>
struct is_byte_register<E> {
    const static bool value = true;
};

template <>
struct is_byte_register<F> {
    const static bool value = true;
};

template <>
struct is_byte_register<H> {
    const static bool value = true;
};

template <>
struct is_byte_register<L> {
    const static bool value = true;
};

template <typename T>
constexpr bool is_byte_register_v = is_byte_register<T>::value;

/**
 * Condition true of combined/word register
 */
template <typename T>
struct is_word_register {
    const static bool value = false;
};

template <>
struct is_word_register<AF> {
    const static bool value = true;
};

template <>
struct is_word_register<BC> {
    const static bool value = true;
};

template <>
struct is_word_register<DE> {
    const static bool value = true;
};

template <>
struct is_word_register<HL> {
    const static bool value = true;
};

template <>
struct is_word_register<SP> {
    const static bool value = true;
};

template <>
struct is_word_register<PC> {
    const static bool value = true;
};

template <typename T>
constexpr bool is_word_register_v = is_word_register<T>::value;

/**
 * Condition true if either word or byte register
 */
template <typename T>
struct is_register {
    const static bool value = is_byte_register_v<T> or is_word_register_v<T>;
};

template <typename T>
constexpr bool is_register_v = is_register<T>::value;


/**
 * Facade for read only access to byte or word register.
 * @tparam T
 */
template <typename T>
class Register {
    static_assert(is_register_v<T>);

public:
    using underlying_type = std::conditional_t<is_byte_register_v<T>, uint8_t, uint16_t>;

    explicit Register(underlying_type input): value(input) {}

    underlying_type get() {
        return value;
    }

private:
    underlying_type value = {0};
};

/**
 * Facade for read-write-access to a byte or word register.
 * @tparam T
 */
template <typename T>
struct MutableRegister {
    static_assert(is_register_v<T>);

public:
    using underlying_type = std::conditional_t<is_byte_register_v<T>, uint8_t, uint16_t>;

    explicit MutableRegister(underlying_type& input) : value(input) {}

    underlying_type get() {
        return value;
    }

    void set(underlying_type new_value) {
        value = new_value;
    }

    void increment() {
        value++;
    }

private:
    underlying_type& value;
};