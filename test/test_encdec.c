#include <assert.h>
#include <stdio.h>

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "encdec.h"

#define ABSTOL 0.0001

void test_encode16(void) {
  // assert(encode16(NAN) == 0x7F81);
  // assert(encode16(INFINITY) == 0x7F80);
  // assert(encode16(-INFINITY) == 0xFF80);
  assert(encode16(0.0) == 0);
  assert(encode16(12.4) == 0x6344);
}

void test_encode32(void) {
  // assert(encode32(NAN) == 0x7F800001);
  // assert(encode32(INFINITY) == 0x7F800000);
  // assert(encode32(-INFINITY) == 0xFF800000);
  //assert(encode32(0.0) == 0);
  assert(encode32(-25.63) == 0x997AE105);
}

void test_decode16(void) {
  // assert(isnan(decode16(0x7F80)));
  // assert(isinf(decode16(0xFF80)) && decode16(0xFF80) < 0);
  // assert(isinf(decode16(0x7F80)) && decode16(0x7F80) > 0);
  assert(fabs(decode16(0) - 0.0) < ABSTOL);
  assert(decode16(0x6344) - 12.4);
}

void test_decode32(void) {
  // assert(isnan(decode32(0x7F800000)));
  // assert(isinf(decode32(0xFF800000)) && decode32(0xFF800000) < 0);
  // assert(isinf(decode32(0x7F800000)) && decode32(0x7F800000) > 0);
  assert(fabs(decode32(0) - 0.0) < ABSTOL);
  assert(decode32(0x997AE105) - -25.63);
}

void test_parse(void) {
  char str1[] = "0x1A2B";
  hex_t result1 = parse(str1);
  assert(result1.size == 2);
  assert(result1.byte[0] == 0x1A);
  assert(result1.byte[1] == 0x2B);

  char str2[] = "$1A2B";
  hex_t result2 = parse(str2);
  assert(result2.size == 2);
  assert(result2.byte[0] == 0x1A);
  assert(result2.byte[1] == 0x2B);

  char str3[] = "1A2B";
  hex_t result3 = parse(str3);
  assert(result3.size == 2);
  assert(result3.byte[0] == 0x1A);
  assert(result3.byte[1] == 0x2B);

  char str4[] = "A";
  hex_t result4 = parse(str4);
  assert(result4.size == 1);
  assert(result4.byte[0] == 0x0A);

  char str5[] = "ABC";
  hex_t result5 = parse(str5);
  assert(result5.size == 2);
  assert(((result5.byte[0] ^ 0x0A) & 0xFF) == 0);
  assert(((result5.byte[1] ^ 0xBC) & 0xFF) == 0);
}

void test_format(void) {
  hex_t hex = { .size = 2, .byte = (char[]){0x1A, 0x2B} };
  char prefix[] = "0x";
  char *result = format(hex, prefix);
  assert(strcmp((char *)result, "0x1A2B") == 0);
  free(result);

  result = format(hex, NULL);
  assert(strcmp((char *)result, "1A2B") == 0);
  free(result);
}

int main(void) {
  test_encode16();
  test_encode32();
  test_decode16();
  test_decode32();
  test_parse();
  test_format();

  printf("All tests passed.\n");
  return 0;
}
