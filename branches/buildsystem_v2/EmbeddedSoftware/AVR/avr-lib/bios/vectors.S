#include "vectors.h"

	.macro	vector number
	.if (. - __boot_vectors < _VECTORS_SIZE)
	.weak	__vector_boot_\number
	.set	__vector_boot_\number, __vector_default_\number
	XJMP	__vector_boot_\number
	.endif
	.endm

	.macro	vector_default number
	.if (. - __vector_defaults < _VECTORS_SIZE)
	.weak	__vector_default_\number
	.set	__vector_default_\number, .
	.word	0xffff
#if __AVR_MEGA__
	.word	0xffff		; interrupt vectors is 2 words in architectures with bigger flash
#endif
	.endif
	.endm

	.section .boot_vectors,"ax",@progbits
	.global	__boot_vectors
	.func	__boot_vectors
__boot_vectors:
	XJMP	__init
	vector	1
	vector	2
	vector	3
	vector	4
	vector	5
	vector	6
	vector	7
	vector	8
	vector	9
	vector	10
	vector	11
	vector	12
	vector	13
	vector	14
	vector	15
	vector	16
	vector	17
	vector	18
	vector	19
	vector	20
	vector	21
	vector	22
	vector	23
	vector	24
	vector	25
	vector	26
	vector	27
	vector	28
	vector	29
	vector	30
	vector	31
	vector	32
	vector	33
	vector	34
	vector	35
	vector	36
	vector	37
	vector	38
	vector	39
	vector	40
	vector	41
	vector	42
	vector	43
	vector	44
	vector	45
	vector	46
	vector	47
	vector	48
	vector	49
	vector	50
	vector	51
	vector	52
	vector	53
	vector	54
	vector	55
	vector	56
	.endfunc
	
	.section .app_vectors,"ax",@progbits
	.global __vector_default
	.set	__vector_default, __boot_vectors
	.func	__vector_defaults
__vector_defaults:
	.word	0xffff		; Application reset vector
#if __AVR_MEGA__
	.word	0xffff		; reset vector is 2 words in architectures with bigger flash
#endif
	vector_default	1
	vector_default	2
	vector_default	3
	vector_default	4
	vector_default	5
	vector_default	6
	vector_default	7
	vector_default	8
	vector_default	9
	vector_default	10
	vector_default	11
	vector_default	12
	vector_default	13
	vector_default	14
	vector_default	15
	vector_default	16
	vector_default	17
	vector_default	18
	vector_default	19
	vector_default	20
	vector_default	21
	vector_default	22
	vector_default	23
	vector_default	24
	vector_default	25
	vector_default	26
	vector_default	27
	vector_default	28
	vector_default	29
	vector_default	30
	vector_default	31
	vector_default	32
	vector_default	33
	vector_default	34
	vector_default	35
	vector_default	36
	vector_default	37
	vector_default	38
	vector_default	39
	vector_default	40
	vector_default	41
	vector_default	42
	vector_default	43
	vector_default	44
	vector_default	45
	vector_default	46
	vector_default	47
	vector_default	48
	vector_default	49
	vector_default	50
	vector_default	51
	vector_default	52
	vector_default	53
	vector_default	54
	vector_default	55
	vector_default	56
	.endfunc
