//---------------------------------------------------------------------------
// Fulhack bongo deluxe 5.0
// Om man definierar en ISR() i bios måste man byta VECTORDEF(x) mot
// RESERVEDVECTORDEF(x) för motsvarande x i listan nedan. Samma interrupt
// kan ha olika vektornummer i olika AVR så detta måste göras om för varje
// AVR-typ.

#define VECTORDEF(_x_) \
void _VECTOR(_x_) (void) __attribute__ ((naked)) __attribute__ ((section (".app_vectors"))); \
void _VECTOR(_x_) (void) { __asm__ __volatile__ ("reti" ::); }
#define RESERVEDVECTORDEF(_x_) \
void _reserved_##_x_ (void) __attribute__ ((naked)) __attribute__ ((section (".app_vectors"))); \
void _reserved_##_x_ (void) { __asm__ __volatile__ ("reti" ::); }

RESERVEDVECTORDEF(reset)
#if (2 < _VECTORS_SIZE)
VECTORDEF(1)
#endif
#if (4 < _VECTORS_SIZE)
VECTORDEF(2)
#endif
#if (6 < _VECTORS_SIZE)
VECTORDEF(3)
#endif
#if (8 < _VECTORS_SIZE)
VECTORDEF(4)
#endif
#if (10 < _VECTORS_SIZE)
VECTORDEF(5)
#endif
#if (12 < _VECTORS_SIZE)
VECTORDEF(6)
#endif
#if (14 < _VECTORS_SIZE)
VECTORDEF(7)
#endif
#if (16 < _VECTORS_SIZE)
VECTORDEF(8)
#endif
#if (18 < _VECTORS_SIZE)
RESERVEDVECTORDEF(9)
#endif
#if (20 < _VECTORS_SIZE)
VECTORDEF(10)
#endif
#if (22 < _VECTORS_SIZE)
VECTORDEF(11)
#endif
#if (24 < _VECTORS_SIZE)
VECTORDEF(12)
#endif
#if (26 < _VECTORS_SIZE)
VECTORDEF(13)
#endif
#if (28 < _VECTORS_SIZE)
VECTORDEF(14)
#endif
#if (30 < _VECTORS_SIZE)
VECTORDEF(15)
#endif
#if (32 < _VECTORS_SIZE)
VECTORDEF(16)
#endif
#if (34 < _VECTORS_SIZE)
VECTORDEF(17)
#endif
#if (36 < _VECTORS_SIZE)
VECTORDEF(18)
#endif
#if (38 < _VECTORS_SIZE)
VECTORDEF(19)
#endif
#if (40 < _VECTORS_SIZE)
VECTORDEF(20)
#endif
#if (42 < _VECTORS_SIZE)
VECTORDEF(21)
#endif
#if (44 < _VECTORS_SIZE)
VECTORDEF(22)
#endif
#if (46 < _VECTORS_SIZE)
VECTORDEF(23)
#endif
#if (48 < _VECTORS_SIZE)
VECTORDEF(24)
#endif
#if (50 < _VECTORS_SIZE)
VECTORDEF(25)
#endif
#if (52 < _VECTORS_SIZE)
VECTORDEF(26)
#endif
#if (54 < _VECTORS_SIZE)
VECTORDEF(27)
#endif
#if (56 < _VECTORS_SIZE)
VECTORDEF(28)
#endif
#if (58 < _VECTORS_SIZE)
VECTORDEF(29)
#endif
#if (60 < _VECTORS_SIZE)
VECTORDEF(30)
#endif
#if (62 < _VECTORS_SIZE)
VECTORDEF(31)
#endif
#if (64 < _VECTORS_SIZE)
VECTORDEF(32)
#endif
#if (66 < _VECTORS_SIZE)
VECTORDEF(33)
#endif

#if (_VECTORS_SIZE >= 66)
#error Add more entries!
#endif
