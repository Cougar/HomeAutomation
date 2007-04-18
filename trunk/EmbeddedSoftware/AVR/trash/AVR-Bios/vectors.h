#include <avr/io.h>

#ifdef _VECTOR
#undef _VECTOR
#else
#warning _VECTOR previously undefined. vectors.h should be included after sfr_defs.h (if it is included).
#endif
#define _VECTOR(N) __vector_boot_ ## N

#if __AVR_MEGA__
  #define XJMP jmp
#else
  #define XJMP rjmp
#endif
