#ifndef encdec_h
#define encdec_h

// This package provides functions to encode and decode
// 16-bit and 32-bit floating-point values using MIL-STD-1750A.
//
// The MIL-STD-1750A encoder supports multiple input data types
// including native C floating-point (IEEE-754 32/64-bit) and
// hexadecimal strings with big-endian byte order.
// Conversely, the decoder is capable of producing output with
// any of these same data types.
//
// It also includes functions for parsing and formatting values
// encoded as hexadecimal strings for verification and display.

#include <stdint.h>

// Use the widest precision available to minimize error propogation.
typedef double ieee754_t;

// MIL-STD-1750A encoded floating-point types.
typedef int16_t ms1750a16_t;
typedef int32_t ms1750a32_t;

// Hexadecimal string types for 16-bit and 32-bit values.
// We require these to be big-endian for portability.
typedef struct { size_t size; char *byte; } hex_t;

// Return value indicating an error when parsing a hexadecimal string.
extern const hex_t invalid_hex;

// Encode a native IEEE-754 as 16-bit MIL-STD-1750A floating-point value.
ms1750a16_t encode16(const ieee754_t value);
// Encode a native IEEE-754 as 32-bit MIL-STD-1750A floating point value.
ms1750a32_t encode32(const ieee754_t value);

// Decode a native IEEE-754 from 16-bit MIL-STD-1750A floating-point value.
ieee754_t decode16(const ms1750a16_t value);
// Decode a native IEEE-754 from 32-bit MIL-STD-1750A floating-point value.
ieee754_t decode32(const ms1750a32_t value);

// Parse a hexadecimal string as a byte array with big-endian byte order.
hex_t parse(const char str[]);

// Format a byte array with big-endian byte order as a hexadecimal string.
char *format(const hex_t hex, const char prefix[]);

#endif // encdec_h
