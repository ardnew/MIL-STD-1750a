#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "encdec.h"

const hex_t invalid_hex = (hex_t){ .size = 0, .byte = 0 };

// Perform an integer bitwise comparison by casting the type of the second
// argument to the type of the first.
#define _biteq(src, cmp, msk) ((typeof(src))0 == ((src ^ (*(typeof(src) *)&cmp)) & msk))

// Encode a native IEEE-754 as 16-bit MIL-STD-1750A floating-point value.
ms1750a16_t encode16(const ieee754_t value)
{
  ms1750a16_t result;
  if (isnan(value)) {
    result = 0x7F81;
  } else if (isinf(value)) {
    result = value < 0 ? 0xFF80 : 0x7F80;
  } else if (value == 0) {
    result = 0;
  } else {
    // Note the 16-bit operations are performed using a 32-bit type
    // to detect over/underflow conditions and adjust the result.
    ms1750a32_t exponent = (ms1750a32_t)ceil(log2(fabs(value)));
    ms1750a32_t mantissa = (ms1750a32_t)round(
      (ieee754_t)value / pow(2.0, (ieee754_t)exponent - 9.0)
    );
    if (_biteq(0x8000, mantissa, UINT_MAX)) {
      mantissa /= 2;
      exponent += 1;
    }
    result = ((mantissa & 0x3FF) << 6) | (exponent & 0x3F);
  }
  return result;
}

// Encode a native IEEE-754 as 32-bit MIL-STD-1750A floating point value.
ms1750a32_t encode32(const ieee754_t value)
{
  ms1750a32_t result;
  if (isnan(value)) {
    result = 0x7F800001;
  } else if (isinf(value)) {
    result = value < 0 ? 0xFF800000 : 0x7F800000;
  } else if (value == 0) {
    result = 0;
  } else {
    ms1750a32_t exponent = (ms1750a32_t)ceil(log2(fabs(value)));
    ms1750a32_t mantissa = (ms1750a32_t)round(
      (ieee754_t)value / pow(2.0, (ieee754_t)exponent - 23.0)
    );
    if (_biteq(0x800000, mantissa, UINT_MAX)) {
      mantissa /= 2;
      exponent += 1;
    }
    result = ((mantissa & 0xFFFFFF) << 8) | (exponent & 0xFF);
  }
  return result;
}

// Decode a native IEEE-754 from 16-bit MIL-STD-1750A floating-point value.
ieee754_t decode16(const ms1750a16_t value)
{
  ieee754_t result;
  if (value == 0) {
    result = 0;
  } else if (_biteq(0x7F81, value, SHRT_MAX)) {
    result = NAN;
  } else if (_biteq(0x7F80, value, SHRT_MAX)) {
    result = value < 0 ? -INFINITY : INFINITY;
  } else {
    ms1750a16_t exponent = value & 0x3F;
    ms1750a16_t mantissa = value / 0x40;
    if (exponent & 0x20) {
      exponent -= 0x40;
    }
    result = (ieee754_t)mantissa * pow(2.0, (ieee754_t)exponent - 9.0);
  }
  return result;
}

// Decode a native IEEE-754 from 32-bit MIL-STD-1750A floating-point value.
ieee754_t decode32(const ms1750a32_t value)
{
  ieee754_t result;
  if (value == 0) {
    result = 0;
  } else if (_biteq(0x7F800001, value, INT_MAX)) {
    result = NAN;
  } else if (_biteq(0x7F800000, value, INT_MAX)) {
    result = value < 0 ? -INFINITY : INFINITY;
  } else {
    ms1750a32_t exponent = value & 0xFF;
    ms1750a32_t mantissa = value / 0x100;
    if (exponent & 0x80) {
      exponent -= 0x100;
    }
    result = ((ieee754_t)mantissa * pow(2.0, (ieee754_t)exponent - 23.0));
  }
  return result;
}

// Parse a hexadecimal string as a byte array with big-endian byte order.
//
// The parse and format functions are inverse operations of each other.
// Output from one can be used immediately as input for the other.
hex_t parse(const char str[])
{
  size_t offset = 0;
  size_t length = strlen(str);
  if (length >= 2 &&
    (str[0] == '0' || str[0] == '\\') &&
    (str[1] == 'x' || str[1] == 'X')) {
    offset = 2; // IBM prefix "0x", "\x", etc.
  } else if (length >= 1 && str[0] == '$') {
    offset = 1; // Pascal prefix "$"
  }
  length -= offset;
  hex_t result = { .size = (length + 1)/2 };
  if (!result.size) { return invalid_hex; }
  result.byte = malloc(result.size);
  if (!result.byte) { return invalid_hex; }
  memset(result.byte, 0, result.size);
  size_t head = 0;
#define _ord(c) (((c)&0xF) + ((c)>>6) | (((c)>>3)&0x8))
  if (length & 1) { // odd number of hex digits
    result.byte[head++] = _ord(str[offset]);
  }
  for (size_t i = 0; i < result.size; i++) {
    result.byte[head + i] |= _ord(str[offset + head + i*2]) << 4; 
    result.byte[head + i] |= _ord(str[offset + head + i*2 + 1]);
  }
  return result;
#undef _ord
}

// Format a byte array with big-endian byte order as a hexadecimal string.
//
// The parse and format functions are inverse operations of each other.
// Output from one can be used immediately as input for the other.
char *format(const hex_t hex, const char prefix[])
{
  static const char digit[] = "0123456789ABCDEF";
  size_t offset = prefix ? strlen(prefix) : 0;
  size_t length = offset + hex.size*2 + 1;
  char *result = malloc(length);
  if (!result) { return 0; } // memory allocation failed
  memset(result, 0, length);
  size_t head = prefix ? strlcpy(result, prefix, offset + 1) : 0;
  for (size_t i = 0; i < hex.size; ++i) {
    // Always append a null terminator (2 hex + 1 null == 3 bytes).
    // Every iteration (except the final) will overwrite the null terminator
    // appended with the previous.
    result[head + i*2] = digit[hex.byte[i] >> 4];
    result[head + i*2 + 1] = digit[hex.byte[i] & 0x0F];
  }
  return result;
}
