void ts_reset_pins(void)
{
  // alle Leitungen auf Eingang  
	TS_TIP_DIR_IN;
	TS_YP_DIR_IN; TS_YM_DIR_IN;
	TS_XP_DIR_IN; TS_XM_DIR_IN;
}

// Einlesen Touch-Event
void ts_listen(void)
{
	ts_reset_pins();
	// das Ausgangsregister vorbereitend setzen
	TS_TIP_1; // YP Y-Achse wird über einen PullUp Widerstand auf 1 gezogen
	TS_XM_0; // XM X-Achse wird auf 0 gesetzt
	// Die Ausgaenge jetzt freigeben
	TS_TIP_DIR_OUT; // YP auf 1
	TS_XM_DIR_OUT;  // XM auf 0 
	
	// erlaube Interrupt in P1.6 bei wechsel auf 0
	BIT_SET(P1IES, BIT_YM); // HL
	BIT_CLR(P1IFG, BIT_YM);
	BIT_SET(P1IE, BIT_YM);
}

// Bereite TS fuer Einlesen von X vor
void ts_cfg_x(void)
{
	ts_reset_pins();
	// die Ausgangsregister vorbereitend setzen
	TS_XP_1; // XP X-Achse rechts auf 1
	TS_XM_0; // XM X-Achse links auf 0
	// Die Ausgaenge freigeben
	TS_XP_DIR_OUT;  // XP auf 1 freigeben
	TS_XM_DIR_OUT;  // XM auf 0 freigeben
}

void ts_cfg_y(void)
{
	ts_reset_pins();
	// die Ausgangsregister vorbereitend setzen
	TS_YP_1; // YP Y-Achse oben auf 1
	TS_YM_0; // YM Y-Achse unten auf 0
	// Die Ausgaenge freigeben
	TS_YP_DIR_OUT;  // YP auf 1 freigeben
	TS_YM_DIR_OUT;  // YM auf 0 freigeben
}

int main(void)
{
	[...]
	ts_listen();


	_bis_SR_register(GIE);
//===Hier die Endlosschleife quasi das Betriebssystem=====================
	while(1){
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================


#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	char buffer[32];
	int x, y;
	static const int xmin = 660;
	static const int xwidth = 3475 - 660;
	static const int ymin = 848;
	static const int ywidth = 3343 - 848;

	LED_ON(LED_GELB);

	while (P1IFG & BIT6 || !(P1IN & BIT_YM)) {

		// loesche touch interrupt flag und schalte interrupt aus
		BIT_CLR(P1IFG, BIT6);
		BIT_CLR(P1IE, BIT_YM);
		
		// verbinde YP
		BIT_SET(P6SEL, BIT_YP);
		BIT_CLR(P6DIR, BIT_YP);

		// lade konfiguration zum Lesen von x
		ts_cfg_x();

		// schalte Wandler an		
		ADC12CTL0 = ADC12ON;
		// konfiguriere AD Wandler
		ADC12CTL1 = 0;
		ADC12MCTL0 = INCH_4;
		// starte Wandlung
		ADC12CTL0 |= ENC + ADC12SC;
		// stoppe Wandlung		
		BIT_CLR(ADC12CTL0, ADC12SC);
		
		// Warte, bis die Konversion abgeschlossen ist
		while(ADC12CTL1 & ADC12BUSY);
		x = ADC12MEM0;

		// Normalisieren auf 0...(xwidth)
		x -= xmin;
		x = ((float)1 - (((float)(xwidth - x))/((float)(xwidth)))) * 127;
		
		BIT_CLR(P6SEL, BIT_YP);
		

		// lade konfiguration zum Lesen von y
		ts_cfg_y();
		
		// verbinde XP
		BIT_SET(P6SEL, BIT_XP);
		BIT_CLR(P6DIR, BIT_XP);

		// schalte Wandler an		
		ADC12CTL0 = ADC12ON;
		// konfiguriere AD Wandler
		ADC12CTL1 = 0;
		ADC12MCTL0 = INCH_5;
		// starte Wandlung
		ADC12CTL0 |= ENC + ADC12SC;
		// stoppe Wandlung		
		BIT_CLR(ADC12CTL0, ADC12SC);
		
		// Warte, bis die Konversion abgeschlossen ist
		while(ADC12CTL1 & ADC12BUSY);
		y = ADC12MEM0;
		// Normalisieren auf 0...(ywidth)
		y -= ymin;
		y = (((float)(ywidth - y))/((float)ywidth)) * 63;

		// schalte Wandler aus
		ADC12CTL0 = 0;
		
		BIT_CLR(P6SEL, BIT_XP);

		lcd_clear(WHITE);
		sprintf(buffer, "x=%d y=%d", x, y);
		lcd_string(BLACK, 8, 15, buffer);
		lcd_paint();

		ts_listen();
		BIT_SET(P1IE, BIT_YM);
		BIT_CLR(ADC12IFG, 1);	
	}
	
	LED_OFF(LED_GELB);
}
