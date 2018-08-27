/**
 * Distributed under the MIT License (MIT)
 * Copyright (c) 2018 icekylin
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in the
 * Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 * OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DM_FORMAT_H
#define DM_FORMAT_H
#include <stdio.h>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>

#include "memory_buffer.h"
#include "any.h"

#ifdef __has_builtin
#define FMT_HAS_BUILTIN(x) __has_builtin(x)
#else
#define FMT_HAS_BUILTIN(x) 0
#endif

#ifndef _MSC_VER
#if FMT_GCC_VERSION >= 400 || FMT_HAS_BUILTIN(__builtin_clz)
#define FMT_BUILTIN_CLZ(n) __builtin_clz(n)
#endif

#if FMT_GCC_VERSION >= 400 || FMT_HAS_BUILTIN(__builtin_clzll)
#define FMT_BUILTIN_CLZLL(n) __builtin_clzll(n)
#endif
#endif

#define FMT_USE_GRISU 1
#ifndef FMT_USE_GRISU
#define FMT_USE_GRISU 0
#endif

namespace dm {
namespace fmt {
namespace details {

template<typename T = void>
struct basic_data {
    static const uint32_t POWERS_OF_10_32[];
    static const uint64_t POWERS_OF_10_64[];
    static const uint64_t POW10_SIGNIFICANDS[];
    static const int16_t POW10_EXPONENTS[];
    static const char DIGITS[];
    static const char RESET_COLOR[];
};
typedef basic_data<> data;

template<typename T>
const char basic_data<T>::DIGITS[] = "0001020304050607080910111213141516171819"
                                     "2021222324252627282930313233343536373839"
                                     "4041424344454647484950515253545556575859"
                                     "6061626364656667686970717273747576777879"
                                     "8081828384858687888990919293949596979899";

#define FMT_POWERS_OF_10(factor)                                                                                                           \
    factor * 10,\
    factor * 100,\
    factor * 1000,\
    factor * 10000,\
    factor * 100000,\
    factor * 1000000,\
    factor * 10000000,\
    factor * 100000000,\
    factor * 1000000000

template<typename T>
const uint32_t basic_data<T>::POWERS_OF_10_32[] = {0, FMT_POWERS_OF_10(1)};

template<typename T>
const uint64_t basic_data<T>::POWERS_OF_10_64[] = {0, FMT_POWERS_OF_10(1), FMT_POWERS_OF_10(1000000000ull), 10000000000000000000ull};

template<typename T>
const uint64_t basic_data<T>::POW10_SIGNIFICANDS[] = {0xfa8fd5a0081c0288, 0xbaaee17fa23ebf76, 0x8b16fb203055ac76, 0xcf42894a5dce35ea,
    0x9a6bb0aa55653b2d, 0xe61acf033d1a45df, 0xab70fe17c79ac6ca, 0xff77b1fcbebcdc4f, 0xbe5691ef416bd60c, 0x8dd01fad907ffc3c,
    0xd3515c2831559a83, 0x9d71ac8fada6c9b5, 0xea9c227723ee8bcb, 0xaecc49914078536d, 0x823c12795db6ce57, 0xc21094364dfb5637,
    0x9096ea6f3848984f, 0xd77485cb25823ac7, 0xa086cfcd97bf97f4, 0xef340a98172aace5, 0xb23867fb2a35b28e, 0x84c8d4dfd2c63f3b,
    0xc5dd44271ad3cdba, 0x936b9fcebb25c996, 0xdbac6c247d62a584, 0xa3ab66580d5fdaf6, 0xf3e2f893dec3f126, 0xb5b5ada8aaff80b8,
    0x87625f056c7c4a8b, 0xc9bcff6034c13053, 0x964e858c91ba2655, 0xdff9772470297ebd, 0xa6dfbd9fb8e5b88f, 0xf8a95fcf88747d94,
    0xb94470938fa89bcf, 0x8a08f0f8bf0f156b, 0xcdb02555653131b6, 0x993fe2c6d07b7fac, 0xe45c10c42a2b3b06, 0xaa242499697392d3,
    0xfd87b5f28300ca0e, 0xbce5086492111aeb, 0x8cbccc096f5088cc, 0xd1b71758e219652c, 0x9c40000000000000, 0xe8d4a51000000000,
    0xad78ebc5ac620000, 0x813f3978f8940984, 0xc097ce7bc90715b3, 0x8f7e32ce7bea5c70, 0xd5d238a4abe98068, 0x9f4f2726179a2245,
    0xed63a231d4c4fb27, 0xb0de65388cc8ada8, 0x83c7088e1aab65db, 0xc45d1df942711d9a, 0x924d692ca61be758, 0xda01ee641a708dea,
    0xa26da3999aef774a, 0xf209787bb47d6b85, 0xb454e4a179dd1877, 0x865b86925b9bc5c2, 0xc83553c5c8965d3d, 0x952ab45cfa97a0b3,
    0xde469fbd99a05fe3, 0xa59bc234db398c25, 0xf6c69a72a3989f5c, 0xb7dcbf5354e9bece, 0x88fcf317f22241e2, 0xcc20ce9bd35c78a5,
    0x98165af37b2153df, 0xe2a0b5dc971f303a, 0xa8d9d1535ce3b396, 0xfb9b7cd9a4a7443c, 0xbb764c4ca7a44410, 0x8bab8eefb6409c1a,
    0xd01fef10a657842c, 0x9b10a4e5e9913129, 0xe7109bfba19c0c9d, 0xac2820d9623bf429, 0x80444b5e7aa7cf85, 0xbf21e44003acdd2d,
    0x8e679c2f5e44ff8f, 0xd433179d9c8cb841, 0x9e19db92b4e31ba9, 0xeb96bf6ebadf77d9, 0xaf87023b9bf0ee6b};

template<typename T>
const int16_t basic_data<T>::POW10_EXPONENTS[] = {-1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007, -980, -954, -927, -901,
    -874, -847, -821, -794, -768, -741, -715, -688, -661, -635, -608, -582, -555, -529, -502, -475, -449, -422, -396, -369, -343, -316,
    -289, -263, -236, -210, -183, -157, -130, -103, -77, -50, -24, 3, 30, 56, 83, 109, 136, 162, 189, 216, 242, 269, 295, 322, 348, 375,
    402, 428, 455, 481, 508, 534, 561, 588, 614, 641, 667, 694, 720, 747, 774, 800, 827, 853, 880, 907, 933, 960, 986, 1013, 1039, 1066};

template<typename T>
const char basic_data<T>::RESET_COLOR[] = "\x1b[0m";

namespace internal {
// An equivalent of `*reinterpret_cast<Dest*>(&source)` that doesn't produce
// undefined behavior (e.g. due to type aliasing).
// Example: uint64_t d = bit_cast<uint64_t>(2.718);
template<typename Dest, typename Source>
inline Dest bit_cast(const Source &source)
{
    static_assert(sizeof(Dest) == sizeof(Source), "size mismatch");
    Dest dest;
    std::memcpy(&dest, &source, sizeof(dest));
    return dest;
}

template<typename T>
struct int_traits {
    typedef typename std::conditional<std::numeric_limits<T>::digits <= 32, uint32_t, uint64_t>::type main_type;
};

template<typename T>
constexpr typename std::enable_if<std::numeric_limits<T>::is_signed, bool>::type is_negative(T value) {
    return value < 0;
}
template<typename T>
constexpr typename std::enable_if<!std::numeric_limits<T>::is_signed, bool>::type is_negative(T) {
    return false;
}

template<typename Int>
constexpr typename std::make_unsigned<Int>::type to_unsigned(Int value) {
    return static_cast<typename std::make_unsigned<Int>::type>(value);
}

struct no_thousands_sep {
    typedef char char_type;

    template<typename Char>
    void operator()(Char *) { }
};

inline unsigned count_digits(uint64_t n) {
    // Based on http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog10
    // and the benchmark https://github.com/localvoid/cxx-benchmark-count-digits.
    int t = (64 - FMT_BUILTIN_CLZLL(n | 1)) * 1233 >> 12;
    return to_unsigned(t) - (n < data::POWERS_OF_10_64[t]) + 1;
}

inline unsigned count_digits(uint32_t n) {
    int t = (32 - FMT_BUILTIN_CLZ(n | 1)) * 1233 >> 12;
    return to_unsigned(t) - (n < data::POWERS_OF_10_32[t]) + 1;
}

template<typename UInt, typename Char, typename ThousandsSep>
inline Char *format_decimal(Char *buffer, UInt value, unsigned num_digits, ThousandsSep thousands_sep)
{
    buffer += num_digits;
    Char *end = buffer;
    while (value >= 100)
    {
        // Integer division is slow so do it for a group of two digits instead
        // of for every digit. The idea comes from the talk by Alexandrescu
        // "Three Optimization Tips for C++". See speed-test for a comparison.
        unsigned index = static_cast<unsigned>((value % 100) * 2);
        value /= 100;
        *--buffer = data::DIGITS[index + 1];
        thousands_sep(buffer);
        *--buffer = data::DIGITS[index];
        thousands_sep(buffer);
    }
    if (value < 10)
    {
        *--buffer = static_cast<char>('0' + value);
        return end;
    }
    unsigned index = static_cast<unsigned>(value * 2);
    *--buffer = data::DIGITS[index + 1];
    thousands_sep(buffer);
    *--buffer = data::DIGITS[index];
    return end;
}

template<typename UInt, typename ThousandsSep>
inline void format_decimal(dm::basic_memory_buffer &buffer, UInt value, unsigned num_digits, ThousandsSep sep)
{
    typedef typename ThousandsSep::char_type char_type;
    // Buffer should be large enough to hold all digits (digits10 + 1) and null.
    char_type buf[std::numeric_limits<UInt>::digits10 + 2];
    std::memset(buf, 0, std::numeric_limits<UInt>::digits10 + 2);
    format_decimal(buf, value, num_digits, sep);
    buffer<<buf;
}

template<typename UInt>
inline void format_decimal(dm::basic_memory_buffer &buffer, UInt value, unsigned num_digits) {
    format_decimal(buffer, value, num_digits, no_thousands_sep());
}

enum alignment {
    ALIGN_DEFAULT,
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER,
    ALIGN_NUMERIC
};

// Flags.
enum {
    SIGN_FLAG = 1,
    PLUS_FLAG = 2,
    MINUS_FLAG = 4,
    HASH_FLAG = 8
};

// An empty format specifier.
struct empty_spec { };
struct align_spec : empty_spec {
    unsigned width_;
    // Fill is always wchar_t and cast to char if necessary to avoid having
    // two specialization of AlignSpec and its subclasses.
    wchar_t fill_;
    alignment align_;

    constexpr align_spec(unsigned width, wchar_t fill, alignment align = ALIGN_DEFAULT)
        : width_(width)
        , fill_(fill)
        , align_(align)
    {
    }

    constexpr unsigned width() const
    {
        return width_;
    }
    constexpr wchar_t fill() const
    {
        return fill_;
    }
    constexpr alignment align() const
    {
        return align_;
    }

    int precision() const
    {
        return -1;
    }
};

template<typename Char>
class basic_format_specs : public align_spec {
public:
    unsigned flags_;
    int precision_;
    Char type_;

    constexpr basic_format_specs(unsigned width = 0, char type = 0, Char fill = ' ')
        : align_spec(width, fill)
        , flags_(0)
        , precision_(-1)
        , type_(type)
    {
    }

    constexpr bool flag(unsigned f) const
    {
        return (flags_ & f) != 0;
    }
    constexpr int precision() const
    {
        return precision_;
    }
    constexpr Char type() const
    {
        return type_;
    }
};

typedef basic_format_specs<char> format_specs;

template<typename Char>
struct float_spec_handler
{
    Char type;
    bool upper;

    explicit float_spec_handler(Char t)
        : type(t)
        , upper(false)
    {
    }

    void on_general()
    {
        if (type == 'G')
            upper = true;
        else
            type = 'g';
    }

    void on_exp()
    {
        if (type == 'E')
            upper = true;
    }

    void on_fixed()
    {
        if (type == 'F')
        {
            upper = true;
#if FMT_MSC_VER
            // MSVC's printf doesn't support 'F'.
            type = 'f';
#endif
        }
    }

    void on_hex()
    {
        if (type == 'A')
            upper = true;
    }

    void on_error() {
        throw dm_error("invalid type specifier");
    }
};

template<typename Char, typename Handler>
void handle_float_type_spec(Char spec, Handler &&handler) {
    switch (spec)
    {
    case 0:
    case 'g':
    case 'G':
        handler.on_general();
        break;
    case 'e':
    case 'E':
        handler.on_exp();
        break;
    case 'f':
    case 'F':
        handler.on_fixed();
        break;
    case 'a':
    case 'A':
        handler.on_hex();
        break;
    default:
        handler.on_error();
        break;
    }
}

struct dummy_int {
    int data[2];
    operator int() const
    {
        return 0;
    }
};

class fp {
private:
    typedef uint64_t significand_type;

    // All sizes are in bits.
    static constexpr const int char_size = std::numeric_limits<unsigned char>::digits;
    // Subtract 1 to account for an implicit most significant bit in the
    // normalized form.
    static constexpr const int double_significand_size = std::numeric_limits<double>::digits - 1;
    static constexpr const uint64_t implicit_bit = 1ull << double_significand_size;

public:
    significand_type f;
    int e;

    static constexpr const int significand_size = sizeof(significand_type) * char_size;

    fp(uint64_t f, int e) : f(f) , e(e) { }

    // Constructs fp from an IEEE754 double. It is a template to prevent compile
    // errors on platforms where double is not IEEE754.
    template<typename Double>
    explicit fp(Double d)
    {
        // Assume double is in the format [sign][exponent][significand].
        typedef std::numeric_limits<Double> limits;
        const int double_size = sizeof(Double) * char_size;
        const int exponent_size = double_size - double_significand_size - 1; // -1 for sign
        const uint64_t significand_mask = implicit_bit - 1;
        const uint64_t exponent_mask = (~0ull >> 1) & ~significand_mask;
        const int exponent_bias = (1 << exponent_size) - limits::max_exponent - 1;
        auto u = bit_cast<uint64_t>(d);
        auto biased_e = (u & exponent_mask) >> double_significand_size;
        f = u & significand_mask;
        if (biased_e != 0)
            f += implicit_bit;
        else
            biased_e = 1; // Subnormals use biased exponent 1 (min exponent).
        e = static_cast<int>(biased_e - exponent_bias - double_significand_size);
    }

    // Normalizes the value converted from double and multiplied by (1 << SHIFT).
    template<int SHIFT = 0>
    void normalize() {
        // Handle subnormals.
        auto shifted_implicit_bit = implicit_bit << SHIFT;
        while ((f & shifted_implicit_bit) == 0) {
            f <<= 1;
            --e;
        }
        // Subtract 1 to account for hidden bit.
        auto offset = significand_size - double_significand_size - SHIFT - 1;
        f <<= offset;
        e -= offset;
    }
};

inline fp operator*(fp x, fp y)
{
    // Multiply 32-bit parts of significands.
    uint64_t mask = (1ULL << 32) - 1;
    uint64_t a = x.f >> 32, b = x.f & mask;
    uint64_t c = y.f >> 32, d = y.f & mask;
    uint64_t ac = a * c, bc = b * c, ad = a * d, bd = b * d;
    // Compute mid 64-bit of result and round.
    uint64_t mid = (bd >> 32) + (ad & mask) + (bc & mask) + (1U << 31);
    return fp(ac + (ad >> 32) + (bc >> 32) + (mid >> 32), x.e + y.e + 64);
}

inline fp get_cached_power(int min_exponent, int &pow10_exponent)
{
    const double one_over_log2_10 = 0.30102999566398114; // 1 / log2(10)
    int index = static_cast<int>(std::ceil((min_exponent + fp::significand_size - 1) * one_over_log2_10));
    // Decimal exponent of the first (smallest) cached power of 10.
    const int first_dec_exp = -348;
    // Difference between two consecutive decimal exponents in cached powers of 10.
    const int dec_exp_step = 8;
    index = (index - first_dec_exp - 1) / dec_exp_step + 1;
    pow10_exponent = first_dec_exp + index * dec_exp_step;
    return fp(data::POW10_SIGNIFICANDS[index], data::POW10_EXPONENTS[index]);
}

template<typename Int>
void format_decimal(dm::basic_memory_buffer &buffer, Int value) {
    typedef typename internal::int_traits<Int>::main_type main_type;
    main_type abs_value = static_cast<main_type>(value);
    bool is_negative = internal::is_negative(value);
    if (is_negative) abs_value = 0 - abs_value;
    unsigned num_digits = internal::count_digits(abs_value);
    if (is_negative) buffer << "-";
    internal::format_decimal(buffer, abs_value, num_digits);
}

template <typename Int, typename Char>
void format_decimal(Char *buffer, Int value) {
    typedef typename internal::int_traits<Int>::main_type main_type;
    main_type abs_value = static_cast<main_type>(value);
    bool is_negative = internal::is_negative(value);
    if (is_negative) abs_value = 0 - abs_value;
    unsigned num_digits = internal::count_digits(abs_value);
    int offset = 0;
    if (is_negative) {
        buffer[0] = '-';
        offset = 1;
    }

    internal::format_decimal(buffer + offset, abs_value,
                             num_digits, internal::no_thousands_sep());
}

void make_point(char *buffer, int length, int pos) {
    int start = length - pos;
    std::memmove(buffer + start + 1, buffer + start, pos);
    buffer[start] = '.';
}

template <typename T>
void format_double(char *buffer, T value, const format_specs &spec) {
    internal::fp fp_value(static_cast<double>(value));
    fp_value.normalize();
    int dec_exp = 0;
    const int min_exp = -60;
    auto dec_pow = internal::get_cached_power(min_exp - (fp_value.e + internal::fp::significand_size), dec_exp);
    internal::fp product = fp_value * dec_pow;
    internal::fp one(1ull << -product.e, product.e);
    uint64_t hi = product.f >> -one.e;
    uint64_t f = product.f & (one.f - 1);
    format_decimal(buffer, hi);

    int i = internal::count_digits(hi);
    const unsigned max_digits = 6 - dec_exp + i;
    make_point(buffer, i, dec_exp);
    while (i++ < max_digits) {
        f *= 10;
        buffer[i] = ('0' + (f >> -one.e));
        f &= one.f - 1;
    }
}

template<typename T>
void format_double(dm::basic_memory_buffer &buffer, T value, const format_specs &spec) {
    if (!std::numeric_limits<double>::is_iec559)
        throw dm_error("Don't support fromat double, use snprintf");

    char buf[25] = "";
    format_double(buf, value, spec);
    buffer << buf;
}

} // namespace internal

int vformat(
    dm::basic_memory_buffer &buffer,
    const char *fmt,
    const dm::any &arg,
    int *index) {
    // begin of this fmt is '%'.
    if ('%' != *fmt) return -1;

    int len = 0;
    switch (*(fmt +1)) {
        // 单字符
        case 'c' : {
            buffer<<dm::any_cast<char>(arg);
            ++(*index);
            len = 1;
            break;
        }
        // 字符串
        case 's': {
            const char *v = dm::any_cast<const char *>(arg);
            buffer.append(v, strlen(v));
            ++(*index);
            len = 1;
            break;
        }
        // 有符号10进制整数(int)
        case 'i' :
        case 'd' : {
            internal::format_decimal(buffer, dm::any_cast<int>(arg));
            ++(*index);
            len = 1;
            break;
        }
        case 'u' : {
            internal::format_decimal(buffer, dm::any_cast<unsigned int>(arg));
            ++(*index);
            len = 1;
            break;
        }
        case 'l' : {
            if (*(fmt + 1 + 1) == 'd') {
                internal::format_decimal(buffer, dm::any_cast<long>(arg));
                ++(*index);
                len = 2;
            } else if (*(fmt + 1 + 1) == 'u') {
                internal::format_decimal(buffer, dm::any_cast<unsigned long>(arg));
                ++(*index);
                len = 2;
            } else if (*(fmt + 1 + 1) == 'f') {
                char buf[64] = "";
                snprintf(buf, 64, "%f", dm::any_cast<double>(arg));
                buffer<<buf;
                //format_double(buffer, dm::any_cast<double>(arg));
                ++(*index);
                len = 2;
                break;
            }
            break;
        }
        case 'h' : {
            // %hd短整型 
            if (*(fmt + 1 + 1) == 'd') {
                // 短整型
                internal::format_decimal(buffer, dm::any_cast<short>(arg));
                ++(*index);
                len = 2;
            } else if (*(fmt + 1 + 1) == 'u') { // 无符号短整形
                internal::format_decimal(buffer, dm::any_cast<unsigned short>(arg));
                ++(*index);
                len = 2;
            }
            break;
        }
        case 'f' : {
            internal::format_double(buffer, dm::any_cast<double>(arg),
                        internal::format_specs(10, 'f', '0'));
            ++(*index);
            len = 1;
            break;
        }
        case 'e':
        case 'E': {
            char buf[64] = "";
            snprintf(buf, 64, "%f", dm::any_cast<double>(arg));
            buffer<<buf;
            ++(*index);
            len = 1;
            break;
        }
        case 'x': {
            char buf[64] = "";
            snprintf(buf, 64, "%x", dm::any_cast<int>(arg));
            buffer<<buf;
            ++(*index);
            len = 1;
            break;
        }
        case 'X': {
            char buf[64] = "";
            snprintf(buf, 64, "%X", dm::any_cast<int>(arg));
            buffer<<buf;
            ++(*index);
            len = 1;
            break;
        }
    }
    return len;
}

void vformat(dm::basic_memory_buffer &buffer, const char *fmt, dm::any *an) {
    int index = 0;
    for (const char *pos = fmt; '\0' != *pos; ++pos) {
        if (*pos != '%') { buffer<<*pos; continue; }
        int ret = vformat(buffer, pos, an[index], &index);
        if (ret <= 0) { buffer<<*pos; continue; }
        pos += ret;
    }
}

}	  // namespace details

#define MAX_BUFFER_LEN 2048
template <typename ...Args>
void format_to(dm::basic_memory_buffer &buffer, const char *fmt, Args ...args) {
    typename dm::anys<sizeof...(Args)>::Type array{args...};
    details::vformat(buffer, fmt, array);
}
}	  // namespace fmt

namespace fmt_helper {
inline void pad2(int n, basic_memory_buffer &buffer) {
    if (n > 99) {
        fmt::details::internal::format_decimal(buffer, n);
        return;
    }
    if (n > 9) { // 10-99
        buffer << ('0' + (n / 10));
        buffer << ('0' + (n % 10));
        return;
    }
    if (n >= 0) { // 0-9
        buffer << '0';
        buffer << ('0' + n);
        return;
    }
    
    fmt::details::internal::format_decimal(buffer, n);
}

inline void append_int(int n, basic_memory_buffer &buffer) {
    fmt::details::internal::format_decimal(buffer, n);
}

inline void pad3(int n, basic_memory_buffer &buffer) {
    if (n > 99) {
        append_int(n, buffer);
        return;
    }
    if (n > 9) { // 10-99
        buffer << '0';
        buffer << ('0' + n / 10);
        buffer << ('0' + n % 10);
        return;
    }
    if (n >= 0) {
        buffer << "00";
        buffer << ('0' + n);
        return;
    }
}

}   // namespace fmt_helper

}	  // namespace dm

#endif	  // DM_FORMAT_H