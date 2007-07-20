#include <inttypes.h>
#include <avr/interrupt.h>
#include <bios.h>

#define APP_TYPE    0xf001
#define APP_VERSION 0x0001

#define F_SAMP 1000
#define PRESCALER 1

/*void can_receive(Can_Message_t *msg) {
	printf("candata!\n");
}*/

volatile uint8_t overflow_adc;
volatile uint8_t overflow_uart;
volatile uint8_t adc;

ISR(TIMER1_COMPA_vect) {
	if (ADCSRA & _BV(ADSC))	// Previous conversion is not yet ready!
		overflow_adc++;
	else if (!(UCSRA & _BV(UDRE)))	// Previous data has not been sent!
		overflow_uart++;
	else ADCSRA |= _BV(ADSC);	// Start a new conversion
}		

ISR(ADC_vect) {				// A conversion is done
	adc = ADCH;				// Store result
	UCSRB |= _BV(UDRIE);	// Enable UART interrupt to send result
}

ISR(USART_UDRE_vect) {		// UART is ready for a new byte
	UCSRB &= ~_BV(UDRIE);	// Disable this interrupt until a new conversion is done
	UDR = adc;				// Send previous result
}

int main(void)
{
	sei();
	
	UBRRL = (F_CPU / 16 / F_BAUD) - 1;
	UCSRB = _BV(TXEN)|_BV(UDRIE);
	
	ADMUX = _BV(REFS0)|_BV(ADLAR)|_BV(MUX2);
	ADCSRA = _BV(ADEN)|_BV(ADSC)|_BV(ADIE)|_BV(ADPS2)|_BV(ADPS1);
	
	OCR1A = F_CPU / F_SAMP / PRESCALER - 1;
	TIMSK |= _BV(OCIE1A);
	TCCR1B = _BV(WGM12)|_BV(CS10);

	Can_Message_t txMsg;
	txMsg.Id = (CAN_NMT_APP_START << CAN_SHIFT_NMT_TYPE) | (NODE_ID << CAN_SHIFT_NMT_SID);
	txMsg.DataLength = 4;
	txMsg.RemoteFlag = 0;
	txMsg.ExtendedFlag = 1;
	txMsg.Data.words[0] = APP_TYPE;
	txMsg.Data.words[1] = APP_VERSION;
	
	//set up callback for candata
	//bios->can_callback = &can_receive;
	// Send CAN_NMT_APP_START
	bios->can_send(&txMsg);
	
	while (1);
	
	return 0;
}
