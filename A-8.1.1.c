
// waehle ACLK
BIT_SET(TBCTL, TBSSEL0);
BIT_CLR(TBCTL, TBSSEL1);

// setze divider auf 8
BIT_SET(TBCTL, ID0 | ID1);

// wahle count up mode
BIT_SET(TBCTL, MC0);
BIT_CLR(TBCTL, MC1);

// setze taktanzahl fuer eine sekunde
TBCCR0 = 4096; // eine sekunde: takt=32000 / divider=8

// loesche interrupt flag fuer timer
BIT_CLR(TBCCTL0, CCIFG);
BIT_SET(TBCCTL0, CCIE);

TBR = 1;

// waehle P6.7 als input
P6SEL |= BIT7;
// schalte wandler an
ADC12CTL0 = ADC12ON;
// ADC12 clock, Single-channel, Single-conversion
ADC12CTL1 = 0;
// waehle channel 7
ADC12MCTL0 = INCH_7;
// erlaube conversion
ADC12CTL0 |= ENC;

_bis_SR_register(GIE);
//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

void print_buf(const char *str)
{
	do {
		U1TXBUF = *str;
		/*
		 * (115200 / 8)**-1 = 70us pro Zeichen
		 */
		wait(7);
	} while(*str++);
}

static float volts;
#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0(void)
{
	char buffer[16];
	
	// start conversion
	ADC12CTL0 |= ADC12SC;
	
	// Zeit fuer Approximation
	wait(1);
	
	// stop conversion
	ADC12CTL0 &= ~ADC12SC;
	
	// berechne Spannung
	volts = ADC12MEM0/4096.0f;
	volts *= 3.3f; 
	
	LED_OFF(LED_ALL);
	if (volts < 1) 
	{
		LED_ON(LED_GELB);
	} 
	else if (volts < 2) 
	{
		LED_ON(LED_GRUEN);
	}
	else
	{
		LED_ON(LED_ROT);
	}
	
	sprintf(buffer, "U=%.2f V", volts);
	lcd_clear(WHITE);
	lcd_string(BLACK, 15, 25, buffer);
	lcd_paint();
}
