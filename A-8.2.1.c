// waehle ACLK
BIT_SET(TBCTL, TBSSEL0);
BIT_CLR(TBCTL, TBSSEL1);

// waehle Set/Reset
BIT_SET(TBCCTL1, OUTMOD1 + OUTMOD0);

// setze divider auf 8
//BIT_SET(TBCTL, ID0 | ID1);

// wahle count up mode
BIT_SET(TBCTL, MC0);
BIT_CLR(TBCTL, MC1);

// setze taktanzahl fuer eine halbe sekunde
TBCCR0 = 1024; // eine sekunde: takt=32000 / divider=8
TBCCR1 = 512;

TBR = 1;

// setze Pin 6.4, 6.5, 6.6 als output
BIT_CLR(P5DIR, BIT4 + BIT5 + BIT6);
// Beschleunigungssensor g-Range 1.5g, Sensitivity 800mV/g
BIT_CLR(P5OUT, BIT4);
BIT_CLR(P5OUT, BIT5);
// sleep mode off
BIT_CLR(P5OUT, BIT6);

// waehle P6.0, P6.1, P6.2 als input
P6SEL = BIT0 + BIT1 + BIT2;
// schalte wandler an, waehle "multiple sample and conversion", SHT = 0 -> 4 cycles
ADC12CTL0 = ADC12ON;
// ADC12 clock, sequence of channels, timer B1
ADC12CTL1 = CONSEQ0 + SHS1 + SHS0;
// waehle channel A0, A1, A2
ADC12MCTL0 = INCH_0;
ADC12MCTL1 = INCH_1;
ADC12MCTL2 = INCH_2;
// loesche interrupt flag
BIT_CLR(ADC12IFG, BIT0 + BIT1 + BIT2);
// erlaube interrupt
BIT_SET(ADC12IE, BIT0 + BIT1 + BIT2);

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

#pragma vector = ADC12_VECTOR
__interrupt void ADC12(void)
{
	char buffer[32];
	int x, y, z;
	LED_TOGGLE(LED_GRUEN);
	if (ADC12IFG & (BIT0 + BIT1 + BIT2))
	{
		LED_TOGGLE(LED_ROT);
		x = ADC12MEM0;
		y = ADC12MEM1;
		z = ADC12MEM2;
		
		lcd_clear(WHITE);
		sprintf(buffer, "ax=%d", x);
		lcd_string(BLACK, 16, 16, buffer);
		sprintf(buffer, "ay=%d", y);
		lcd_string(BLACK, 16, 25, buffer);
		sprintf(buffer, "az=%d", z);
		lcd_string(BLACK, 16, 33, buffer);
		
		lcd_paint();
		BIT_CLR(ADC12IFG, BIT0 + BIT1 + BIT2);
	}	
}
