// waehle ACLK
BIT_SET(TBCTL, TBSSEL0);
BIT_CLR(TBCTL, TBSSEL1);

// waehle Set/Reset
//BIT_SET(TBCCTL1, OUTMOD1 + OUTMOD0);

// setze divider auf 8
BIT_SET(TBCTL, ID0 | ID1);

// wahle count up mode
BIT_SET(TBCTL, MC0);
BIT_CLR(TBCTL, MC1);

// setze taktanzahl fuer eine halbe sekunde
TBCCR0 = 2048; // eine sekunde: takt=32000 / divider=8

// loesche interrupt flag fuer timer
BIT_CLR(TBCCTL0, CCIFG);

TBR = 1;

// setze Pin 5.4, 5.5, 5.6 als output
BIT_SET(P5DIR, BIT4 + BIT5 + BIT6);
// Beschleunigungssensor g-Range 1.5g, Sensitivity 800mV/g
BIT_CLR(P5OUT, BIT4);
BIT_CLR(P5OUT, BIT5);
// sleep mode off
BIT_SET(P5OUT, BIT6);

// waehle P6.0, P6.1, P6.2 als input
P6SEL = BIT0 + BIT1 + BIT2;
// schalte wandler an, waehle "multiple sample and conversion", SHT = 0 -> 4 cycles
ADC12CTL0 = ADC12ON + MSC;
// ADC12 clock, sequence of channels
ADC12CTL1 = CONSEQ0 + SHP;
// waehle channel A0, A1, A2
ADC12MCTL0 = INCH_0;
ADC12MCTL1 = INCH_1;
ADC12MCTL2 = INCH_2 + EOS;
// loesche interrupt flag
BIT_CLR(ADC12IFG, BIT2);
// erlaube interrupt
BIT_SET(ADC12IE, BIT2);
// erlaube timer interrupt
BIT_SET(TBCCTL0, CCIE);

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

#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0(void)
{
	// starte conversion
	ADC12CTL0 |= ADC12SC;
	LED_TOGGLE(LED_ROT);
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12(void)
{
	char buffer[32];
	int x, y, z;
	LED_TOGGLE(LED_GRUEN);
	if (ADC12IFG & BIT2)
	{
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
		
		sprintf(buffer, "%d\t%d\t%d\r\n", x, y, z);
		
		print_buf(buffer);
		
		lcd_paint();
		BIT_CLR(ADC12IFG, BIT2 + BIT1 + BIT0);
	}	
}
