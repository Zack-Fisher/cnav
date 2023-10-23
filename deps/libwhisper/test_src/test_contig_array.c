#include "whisper/contig_array.h"
#include <stdio.h>

typedef struct Stuff {
  int x;
  float y;
  char z;
} Stuff;

typedef struct Stuff_two {
  int x;
  float y;
  char z;
} Stuff_two;

// no padding
DEFINE_CONTIG_ARRAY_TYPES(Stuff, 10, , , )
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_SOURCE(Stuff, 10, { printf("oh my gosh\n"); })
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff, 10)

DEFINE_CONTIG_ARRAY_TYPES(Stuff_two, 10, , , )
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_SOURCE(Stuff_two, 10, { printf("oh my gosh again\n"); })
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)
DEFINE_CONTIG_ARRAY_HEADERS(Stuff_two, 10)

void test_contig_array() {
  printf("start testing of contig_array\n");

  W_CA_Stuff ca = {0};
  Stuff s;
  s.x = 1;
  s.y = 1;
  s.z = '1';

  StuffSlot ss = w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);
  w_ca_add_Stuff(&ca, &s);    // reach the limit
  w_ca_remove_Stuff(&ca, ss); // then remove one
  w_ca_add_Stuff(&ca, &s);    // reach the limit again
  w_ca_add_Stuff(&ca, &s);    // trigger the passed in error block

  W_CA_Stuff_two ca_two = {0};
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
  w_ca_add_Stuff_two(&ca_two, (Stuff_two *)&s);
}
