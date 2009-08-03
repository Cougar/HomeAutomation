/**
 * dtmf decoder.
 * http://en.wikipedia.org/wiki/DTMF
 * http://en.wikipedia.org/wiki/Goertzel_algorithm
 * 
 * @date	2007-03-14
 * @author	Anders Runeson arune at sf dot net
 *   
 */

/*-----------------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------------*/
#include "dtmfdecode.h"

/*-----------------------------------------------------------------------------
 * Globals
 *---------------------------------------------------------------------------*/
static uint8_t		buf[BUFFER_SIZE+1];	//buffer for storing samples
static uint8_t		bufPointAD = 0;		//bufferpointer for bufferwriter (AD-converter)
static uint8_t		bufPoint = 0;		//bufferpointer for bufferreader (algoritm)
static int32_t		r[MAX_BINS];		//frequncy amplitude
static int32_t		coefs[MAX_BINS];	
static int32_t		q1[MAX_BINS];
static int32_t		q2[MAX_BINS];
static uint16_t		freqs[MAX_BINS] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};
static uint8_t		receiverState = RSPAUSE;
static uint8_t 		noSignalCnt = 0;
static uint8_t 		signalCnt = 0;
static uint8_t		toneBuf[TONE_BUFFER_SIZE+1];	//buffer for storing received tones
static uint8_t		toneBufPoint = 0;				//bufferpointer for tonebuffer

 /*-----------------------------------------------------------------------------
 * Prerequisites
 *---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
 * Public Functions
 *---------------------------------------------------------------------------*/


//inline int32_t max(int32_t a, int32_t b) { return a > b ? a : b; }
//inline int32_t min(int32_t a, int32_t b) { return a < b ? a : b; }

uint8_t DTMFIN_Init(void) {

//init ad and timer1
	/* Enable ADC4 */
	ADMUX |= (1<<MUX2);
	ADMUX &= ~((1<<MUX3)|(1<<MUX0)|(1<<MUX1));
	
	/* Set AD prescaler */
	ADCSRA |= (1<<ADPS1)|(1<<ADPS0);
	ADCSRA &= ~((1<<ADPS2));
	
	/* Enable AVcc as Voltage Reference */
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	
	/* Left adjust the result (only use 8bit) */
	ADMUX |= (1<<ADLAR);
	
	/* Wake up ADC and enable it */
	PRR &= ~(1<<PRADC);
	ADCSRA |= (1<<ADEN)|(1<<ADIE);

	//timer
	TCCR1B = (1<<CS10);
	TCNT1 = CLK_RELOAD;
	TIFR1	|= (1<<TOV1);
	TIMSK1 |= (1<<TOIE1);

/* This is where we calculate the correct co-efficients.
 * coef = 2.0 * cos( (2.0 * PI * k) / (float)GOERTZEL_N)) ;
 * Where k = (int) (0.5 + ((float)GOERTZEL_N * target_freq) / SAMPLING_RATE));
 *
 * More simply: coef = 2.0 * cos( (2.0 * PI * target_freq) / SAMPLING_RATE );
 */
	int n;

	for(n = 0; n < MAX_BINS; n++) {
	coefs[n] = COEFFSPREC * 2.0 * cos(2.0 * 3.141592654 * freqs[n] / SAMPLING_RATE);
	//printf("%i\t", coefs[n]);
	}
	//printf("\n");
	
	return 1;
}

/*----------------------------------------------------------------------------
 *	post_testing
 *----------------------------------------------------------------------------
 * This is where we look at the bins and decide if we have a valid signal.
 */
uint8_t post_testing(void) {
	uint8_t		 row, col, see_digit;
	uint8_t		 peak_count, max_index;
	int32_t		maxval, t;
	uint8_t		 i;

	uint8_t row_col_codes[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}};
//	{0x1, 0x2, 0x3, 0xA},
//	{0x4, 0x5, 0x6, 0xB},
//	{0x7, 0x8, 0x9, 0xC},
//	{0xE, 0x0, 0xF, 0xD}};
	

	/* Find the largest in the row group. */
	row = 0;
	maxval = 0;
	for ( i=0; i<4; i++ ) {
		if ( r[i] > maxval ) {
			maxval = r[i];
			row = i;
		}
	}

	/* Find the largest in the column group. */
	col = 4;
	maxval = 0;
	for ( i=4; i<8; i++ ) {
		if ( r[i] > maxval ) {
			maxval = r[i];
			col = i;
		}
	}
	
	/* Check for minimum energy */
	//if ( r[row]/SAMPLEPREC < 4.0e5 )	/* 2.0e5 ... 1.0e8 no change */
	if ( r[row] < 4.0e5 ) {
		/* energy not high enough */
		return 0;
	//} else if ( r[col]/SAMPLEPREC < 4.0e5 ) 
	} else if ( r[col] < 4.0e5 ) {
		/* energy not high enough */
		return 0;
	} 
	see_digit = 1;

	/* Twist check
	 * CEPT => twist < 6dB
	 * AT&T => forward twist < 4dB and reverse twist < 8dB
	 *  -ndB < 10 log10( v1 / v2 ), where v1 < v2
	 *  -4dB < 10 log10( v1 / v2 )
	 *  -0.4  < log10( v1 / v2 )
	 *  0.398 < v1 / v2
	 *  0.398 * v2 < v1
	 */
	if ( r[col] > r[row] ) {
		/* Normal twist */
		max_index = col;
		if ( r[row] < (r[col] >>2) )	/* twist > 4dB, error */
		see_digit = 0;
	} else {
		/* Reverse twist */
		max_index = row;
		if ( r[col] < (r[row] >>2) )	/* twist > 8db, error */
		see_digit = 0;
	}
	
	/* Signal to noise test
	 * AT&T states that the noise must be 16dB down from the signal.
	 * Here we count the number of signals above the threshold and
	 * there ought to be only two.
	 */
	t = r[max_index] >>2;

	peak_count = 0;
	for ( i=0; i<8; i++ ) {
		if ( r[i] > t ) {
			peak_count++;
		}
	}
	if ( peak_count > 2 ) {
		see_digit = 0;
	}

	if ( see_digit ) {
		i = (row_col_codes[row][col-4]);
		return i;
	} else {
		return 0;
	}
}

uint8_t majority(uint8_t* returntone) {
	uint8_t		counttones[TONE_BUFFER_SIZE] = {0,0,0,0,0,0,0,0};
	uint8_t		tones[TONE_BUFFER_SIZE] = {0,0,0,0,0,0,0,0};
	
	for (uint8_t i = 0; i<toneBufPoint; i++) {
		for (uint8_t j = 0; j<TONE_BUFFER_SIZE; j++) {
			if (tones[j] == toneBuf[i]) {
				counttones[j]++;
				break;
			} else if (tones[j] == 0) {
				tones[j] = toneBuf[i];
				counttones[j]=1;
				break;
			}
		}
	}
	
	uint8_t maxindex=0;
	for (uint8_t j = 0; j<TONE_BUFFER_SIZE; j++) {
		if (counttones[j] > counttones[maxindex]) {
			maxindex = j;
		}
	}
	
	if (counttones[maxindex] > (toneBufPoint>>1)) {
		*returntone = tones[maxindex];
		return 1;	//success
	}
	
	*returntone = 'x';
	return 1;	//during testing
	//return 0;	//failed
}

uint8_t DTMFIN_GetData(uint8_t* returntone) {
	//uint16_t time2;	//for time measure
	//uint16_t time1;

	// calculate while there is data in buffer
	if (bufPoint<bufPointAD) {
		int32_t	q0;
		uint8_t	i;
		uint8_t sample = buf[bufPoint];
		//uint8_t sample = samples[bufPoint];
		for ( i=0; i<MAX_BINS; i++ ) {
				q0 = q1[i] * coefs[i] / COEFFSPREC;
				q0 -= q2[i];
				//q0 += (sample-127)*SAMPLEPREC;		//use this row instead of next to filter DC
				q0 += sample*SAMPLEPREC;
			q2[i] = q1[i];
			q1[i] = q0;
		}
		
		bufPoint++;
		//when buffer is full do the final calculation and call post_testing()
		if (bufPoint == BUFFER_SIZE) {
			
			//printf("\n");
			//printf("current power:\n\t697\t770\t852\t941\t1209\t1336\t1477\t1633\n");
			for ( i=0; i<MAX_BINS; i++ ) {

				q1[i] = q1[i]>>SAMPLEPRECSHIFT;
				q2[i] = q2[i]>>SAMPLEPRECSHIFT;
				r[i] = (q1[i] * q1[i]);
				r[i] += (q2[i] * q2[i]);
				r[i] -= ((coefs[i] * (q1[i])>>COEFFSPRECSHIFT) * q2[i]);
				q1[i] = 0;
				q2[i] = 0;
				
				//printf("\t%li", r[i]);
			}
			//printf("\n");
			
//time1 = TCNT1;
//time2 = TCNT1;
//printf("\n%u\t%u\n", time1, time2);

			bufPoint = 0;
			bufPointAD = 0;
			uint8_t tone = post_testing();

			if (tone) {	
				//tone &= 0x0F;
				if (toneBufPoint<TONE_BUFFER_SIZE) {
					toneBuf[toneBufPoint] = tone;
					toneBufPoint++;
				}
				 
				//printf("%x", tone);
			}
		}
	}
		
	if (toneBufPoint>0 && receiverState==RSPAUSE) {
		//gå inom buffer och kolla, majoritetsbeslut, om ok så returnera med valet
		uint8_t majtone;
		if (majority(&majtone)) {
			*returntone = majtone;
			//*returntone = toneBuf[0];
			toneBufPoint = 0;
			return 1;
		}
		
		toneBufPoint = 0;
		//uint8_t data = toneBuf[0];
		//*returntone = data;
		//return 1;
	}
	
	return 0;
}

//Timer1 overflow, should occur at 8kHz
ISR(SIG_OVERFLOW1) {
	TCNT1 = CLK_RELOAD;
	//start ad-conversion
	ADCSRA |= (1<<ADSC);
}

//ADC-overflow, occurs when an AD-conversion is done (also at 8kHz)
ISR(ADC_vect) {
	//save value if buffer is not full
	if (bufPointAD<BUFFER_SIZE) {
		uint8_t ADCvalue = ADCH;
		
		if (ADCvalue < UPPERSILENCELIMIT && ADCvalue > LOWERSILENCELIMIT) {
			if (receiverState == RSRECEIVING) {
				noSignalCnt++;
			}
		} else {
			if (receiverState == RSPAUSE) {
				signalCnt++;
			}
		}
		
		if (signalCnt > 4) {
			if (receiverState == RSPAUSE) {
				noSignalCnt = 0;
			}
			buf[bufPointAD] = ADCvalue;
			bufPointAD++;
			receiverState = RSRECEIVING;		//no longer signal pause
		}

		if (noSignalCnt > 60) {
			if (receiverState == RSRECEIVING) {
				signalCnt = 0;
			}
			bufPointAD = 0;
			receiverState = RSPAUSE;		//signal pause
		}
		
		//only save value if there is a signal
/*		if (bufPointAD>0 || ADCvalue > UPPERSILENCELIMIT || ADCvalue < LOWERSILENCELIMIT) {
			buf[bufPointAD] = ADCvalue;
			bufPointAD++;
			receiverState = RSRECEIVING;		//no longer signal pause
		} else {
			noSignalCnt++;
			if (noSignalCnt==40) {
				//printf(".");
				receiverState = RSPAUSE;		//signal pause
				noSignalCnt = 0;
			}
		}*/
	}
}
