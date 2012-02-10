/*
   pr49123.c from the execute part of the gcc torture tests.
 */

#include <testfwk.h>

#ifdef SDCC
#pragma std_c99
#endif

/* PR lto/49123 */

static struct S { signed int f : 1; } s;
static int v = -1;

void
testTortureExecute (void)
{
  s.f = v < 0;
  if ((unsigned int) s.f != -1U)
    ASSERT (0);
  return;
}
