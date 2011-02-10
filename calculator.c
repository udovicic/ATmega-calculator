#include <avr/io.h>
#include <avr/delay.h>
#include <inttypes.h>

uint16_t count;

void inic();
uint8_t sseg(uint8_t x);
void disp_n(uint8_t num);
void disp(uint8_t led, uint8_t num);

int main() {
	uint8_t i;
	struct {
		unsigned last:1; //bit
	} x;
		
	inic();
	x.last = 0;
	while (1) {
		disp_n(count);
		if ((PINA & 1) != x.last) count++;
		if (PINA & 2) count+=10;
		if (PINA & 4) count+=100;
		x.last = (PINA & 1);
	}

	return 0;
}

void inic() {
	/* port initialization*/
	DDRA = 0x00;
	DDRB = 0xff;
	DDRC = 0xff;
	PORTA = 0xff;
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

void disp_n(uint8_t num) {
    /* display uint8_t on 4 7seg displays */
	uint8_t i,led[4];
	led[3]=count/1000;
	led[2]=(count%1000)/100;
	led[1]=(count%100)/10;
	led[0]=count%10;
	for (i=0;i<4;i++)
		disp(i,led[i]);
}

void disp(uint8_t led, uint8_t num) {
	/* display single number on 7seg display */
	PORTB = 1 << led;
	PORTC = sseg(num);
	_delay_ms(10);
	PORTC = 0;
	return;
}
