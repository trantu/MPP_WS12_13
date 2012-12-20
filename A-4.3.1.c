print_value();
//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
		LED_ON(LED_GELB);
		
		while(P1IN & TASTE_LINKS) {
			LED_OFF(LED_GELB);
			LED_ON(LED_GRUEN);
			if (!(P1IN & BIT_YM)) {
				wert--;
				print_value();
				while(!(P1IN & BIT_YM)) {
				}
			}
		}
		
		LED_OFF(LED_GRUEN);
		LED_ON(LED_GELB);

		while(P1IN & TASTE_RECHTS) {
			LED_OFF(LED_GELB);
			LED_ON(LED_ROT);
			if (!(P1IN & BIT_YM)) {
				wert++;
				print_value();	
				while(!(P1IN & BIT_YM)) {
				}				
			}
		}
		LED_OFF(LED_ROT);

		if (!(P1IN & BIT_YM)) {
			LED_TOGGLE(LED_GELB);
			while(!(P1IN & BIT_YM)) {
			}
		}
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

void print_value(void)
{
		char string[256];
		sprintf(string, "DEC %03d\nHEX %03x", wert, wert);
		lcd_clear(WHITE);	// Grafikspeicher auf dem MSP430 löschen
		lcd_string(BLACK, 15, 25, string);	// Textausgabe
		lcd_paint();		// Grafikspeicher auf das LCD Display ausgeben
}
