/**
 * dtmf generator.
 * AVR AppNote314
 * 
 * @date	2007-03-15
 * @author	Anders Runeson arune at sf dot net
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "dtmfgenerate.h"

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
uint16_t x_SWh = 0x00;               // step width of high frequency
uint16_t x_SWl = 0x00;               // step width of low frequency
uint16_t  i_CurSinValA = 0;           // position freq. A in LUT (extended format)
uint16_t  i_CurSinValB = 0;           // position freq. B in LUT (extended format)
//***************************  x_SW  ***************************************
//Table of x_SW (excess 8): x_SW = ROUND(FP*N_SAMPLES*f*256/Fck)
//**************************************************************************
//high frequency (coloun)	8MHz128		20MHz256
//1209hz  ---> x_SW = 		79			1014
//1336hz  ---> x_SW = 		87			1121
//1477hz  ---> x_SW = 		96			1239
//1633hz  ---> x_SW = 		107			1370
uint16_t auc_frequencyH [4] = {1370, 1239, 1121, 1014};

//low frequency (row)
//697hz  ---> x_SW = 		46			585
//770hz  ---> x_SW = 		50			646
//852hz  ---> x_SW = 		56			715
//941hz  ---> x_SW = 		61			789
uint16_t auc_frequencyL [4] = {789, 715, 646, 585};
/*************************** SIN TABLE *************************************
 * Samples table : one period sampled on 128 samples and
 * quantized on 7 bit
 **************************************************************************/
//nu i flash? prog_
//skala om till 7bit amplitud		prog_
int8_t auc_SinParam [64] = {
63, 63, 63, 63, 63, 63, 62, 62, 62, 61, 61, 61, 60, 60, 59, 59, 58, 58, 57, 
56, 56, 55, 54, 53, 52, 52, 51, 50, 49, 48, 47, 46, 45, 43, 42, 41, 40, 39, 
38, 36, 35, 34, 32, 31, 30, 28, 27, 26, 24, 23, 21, 20, 18, 17, 15, 14, 12, 
11,  9,  8,  6,  5,  3,  2 };


/*
 * Timer0 overflow interrupt
 * occurs at 78.125kHz (20MHz/256)
 */
ISR(SIG_OVERFLOW0) {
	uint8_t  i_TmpSinValA;               // position freq. A in LUT (actual position)
	uint8_t  i_TmpSinValB;               // position freq. B in LUT (actual position)

	// move Pointer about step width aheaed
	i_CurSinValA += x_SWh;
	i_CurSinValB += x_SWl;
	// normalize Temp-Pointer
	i_TmpSinValA	=	(uint8_t)(((i_CurSinValA+4) >> FPSHIFT)&(N_SAMPLES-1));
	i_TmpSinValB	=	(uint8_t)(((i_CurSinValB+4) >> FPSHIFT)&(N_SAMPLES-1));
	// calculate PWM value: high frequency value + 3/4 low frequency value
	//OCR0A = (auc_SinParam[i_TmpSinValA] + (auc_SinParam[i_TmpSinValB]-(auc_SinParam[i_TmpSinValB]>>2)));
	int8_t tmpVal;
	if (i_TmpSinValB<N_SAMPLES/4) {
		tmpVal = auc_SinParam[ i_TmpSinValB ];
		tmpVal -= tmpVal/4;
	} else if (i_TmpSinValB<N_SAMPLES/2) {
		tmpVal = -auc_SinParam[ (N_SAMPLES/2-1)-i_TmpSinValB ];
		tmpVal -= tmpVal/4;
	} else if (i_TmpSinValB<N_SAMPLES*3/4) {
		tmpVal = -auc_SinParam[ i_TmpSinValB-(N_SAMPLES/2) ];
		tmpVal -= tmpVal/4;
	} else {
		tmpVal = auc_SinParam[ (N_SAMPLES-1)-i_TmpSinValB ];
		tmpVal -= tmpVal/4;
	}
	if (i_TmpSinValA<N_SAMPLES/4) {
		tmpVal += auc_SinParam[ i_TmpSinValA ];
	} else if (i_TmpSinValA<N_SAMPLES/2) {
		tmpVal += -auc_SinParam[ (N_SAMPLES/2-1)-i_TmpSinValA ];
	} else if (i_TmpSinValA<N_SAMPLES*3/4) {
		tmpVal += -auc_SinParam[ i_TmpSinValA-(N_SAMPLES/2) ];
	} else {
		tmpVal += auc_SinParam[ (N_SAMPLES-1)-i_TmpSinValA ];
	}

	OCR0A = tmpVal+128;
	//OCR0A = 128; 
}


uint8_t DTMFOUT_Init (void) {
	TIMSK0 = (1<<TOIE0);				// Timer0 interrupt overflow enabled
	TCCR0A = ((1<<COM0A1)|(1<<WGM00)|(1<<WGM01));	// non inverting / 8Bit PWM
	TCCR0B = (1<<CS00);					// CLK/1
	DDRD |= (1<<PD6);					// PD5 (OC0A) as output
	
	x_SWh=1014;
	x_SWl=789;
	return 1;
}

void DTMFOUT_Send (uint8_t tone) {
	/*
	{0x1, 0x2, 0x3, 0xA},
	{0x4, 0x5, 0x6, 0xB},
	{0x7, 0x8, 0x9, 0xC},
	{0xE, 0x0, 0xF, 0xD}};
	*/
	
		
}

//funktion man anropar med 0/1/2/3/4/5/6/7/8/9/a/b/c/d/*/# och den öppnar porten (ddrd|=(1>>pd6)), 
//aktiverar timer overfl int, lägger rätt värden i x_SWh och x_SWl, 
//efter en viss tid ska tysnad läggas ut, stänger av timer ovfl int och sätter porten högimpedans

//en funktion som kallas under tiden ska returnera status, om man sänder ton eller tysnad eller klar
//ISRen kan sköta statemashinen med ton och tystnad

//x_SWh = auc_frequencyH[0];

