// waehle P6.7 als input
P6SEL |= BIT7;
// schalte wandler an, waehle "multiple sample and conversion", SHT = 0 -> 4 cycles
ADC12CTL0 = ADC12ON + MSC;
// ADC12 clock, repeat single channel
ADC12CTL1 = CONSEQ1 + SHP;
// waehle channel 7
ADC12MCTL0 = INCH_7;
// loesche interrupt flag
BIT_CLR(ADC12IFG, 1);
// erlaube interrupt
BIT_SET(ADC12IE, 1);

// erlaube conversion
ADC12CTL0 |= ENC;
// starte conversion
ADC12CTL0 |= ADC12SC;

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

#pragma vector = ADC12_VECTOR
__interrupt void ADC12(void)
{
	char buffer[16];
	if (ADC12IFG & 1)
	{
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
		BIT_CLR(ADC12IFG, 1);
	}	
}
