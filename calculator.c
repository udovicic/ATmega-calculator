/* Stjepan Udovičić <stjepan@udovicic.org> */

/* Calculator made for ATmega16 uP on EasyAVR 5A board */


#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

uint16_t count[3];
uint8_t state[2];
uint8_t dot; // wasted 7 bits

void inic(void);
uint8_t sseg(uint8_t);
void disp_n(uint16_t);
void disp(uint8_t, uint8_t);
uint8_t button(volatile uint8_t *, uint8_t);

int main(void) {
	
	inic();
	
	uint8_t i,j,k;
	j=0;k=0;state[1]=0;state[1]=0;dot=0;
	
	while (j<2) {
		disp_n(count[j]); // display curent number
		
		for (i=0;i<8;i++) // try to read number
			if ( (button( &PINA, i)) && k<4 ) {
				count[j]= (count[j] * 10) + i;
				k++;
			}
		for (i=0;i<8;i++) // try to read operation
			if (button( &PIND, i)) {
				if (!j) count[2]=i;
				j++;k=0;
			}
	}
	
	// exec. statement
	j=count[2];
	switch(j) {
		case 0: // +
			count[2]=count[0]+count[1];
			break;
		case 1: // -
			count[2]=count[0]-count[1];
			break;
		case 2: // *
			count[2]=count[0]*count[1];
			break;
		case 3: // /
			count[2]=(count[0]*10)/count[1];
			if (count[2] & 1)
				dot=1;
			else
				count[2]/=10;
			break;
		case 4: // exp( num1, num2)
			if (count[0]==0)
				count[2]=0;
			else if (count[1]==0)
				count[2]=1;
			else {
				count[2]=count[0];
				for (i=0;i<count[1]-1;i++) count[2]*=count[0];
			}
			break;
	}
	
	//display result
	while(1) disp_n(count[2]);

	return 0;
}
uint8_t button(volatile uint8_t *port, uint8_t pin) {
	uint8_t i = 1;
	if (*port == PINA) i=0;
	if (*port & (1 << pin )) {
		if (!(state[i] & (1 << pin))) {
			_delay_ms(10);
			if (*port & (1 << pin )) {
				state[i] |= (1 << pin);
				return 1;
			}
		}
	} else
		state[i] &= ~(1 << pin);
	return 0;
}
void inic(void) {
	/* port initialization*/
	DDRA = 0x00;
	DDRB = 0xff;
	DDRC = 0xff;
	DDRD = 0x00;
	PORTA = 0xff;
	PORTD = 0xff;
	PORTB = 0;
	PORTC = 0;
}
uint8_t sseg(uint8_t sseg_x) {
	/* converts uint8_t to 7seg code */
	switch(sseg_x) {
		case 0 : return 0x3F;
		case 1 : return 0x06;
		case 2 : return 0x5B;
		case 3 : return 0x4F;
		case 4 : return 0x66;
		case 5 : return 0x6D;
		case 6 : return 0x7D;
		case 7 : return 0x07;
		case 8 : return 0x7F;
		case 9 : return 0x6F;
		default: return 0xAA;
	}
	return 0xAA;
}
void disp_n(uint16_t num) {
    /* display uint8_t on 4 7seg displays */
	uint8_t i,led[4];
	if (num <= 9999) {
		led[3]=num/1000;
		led[2]=(num%1000)/100;
		led[1]=(num%100)/10;
		led[0]=num%10;
	} else for (i=0;i<4;i++)
		disp(i,10);
	for (i=0;i<4;i++)
		disp(i,led[i]);
}
void disp(uint8_t led, uint8_t num) {
	/* display single number on 7seg display */
	PORTB = 1 << led;
	PORTC = sseg(num);
	if (dot && led == 1) PORTC |= 0x80;
	_delay_ms(10);
	PORTC = 0;
}
