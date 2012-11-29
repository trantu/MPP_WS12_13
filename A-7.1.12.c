// init.c
void init_UART1(void)
  {
	//	P3SEL .....;			// USART RX und TX dem Modul zuweisen
	P3SEL = BIT6 | BIT7;
	//	U1CTL .....; 			// Reset
	U1CTL = SWRST;
	//	U1CTL .....; 			// Format 8N1
	U1CTL |= CHAR; // 8 Bits Daten, keine Paritaet und 1-Bit Stop ist default
	//	U1TCTL.....;			// Taktquelle SMCLK
	U1TCTL |= SSEL0 + SSEL1; // Taktquelle SMCLK
	//	U1BR0 .....;  			// Teiler Low-Teil, da 7372800/64 ca. 115200
	U1BR0 = 64; //7372800/64 ca. 115200
	U1BR1 = 0;
	//	U1BR1 .....;			// Teiler High-Teil
	//	U1MCTL ....; 			// Modulationskontrolle
	U1MCTL = 0;
	//	ME2 .......;			// Enable USART1 TXD/RXD
	ME2 = BIT5 + BIT4;
	//	U1CTL .....;	 		// Reset
	U1CTL &= ~SWRST;
	//	IE2 .......;			// Enable Interrupt
	//IE2 = BIT5 + BIT4;
	}
  
  //main.c
  BIT_CLR(P1DIR, TASTE_LINKS);
BIT_SET(P1IES, TASTE_LINKS); // HL an linker taste loest interrupt aus
BIT_SET(P1IE, TASTE_LINKS); // interrupts fuer linke taste erlauben
BIT_CLR(P1IFG, TASTE_LINKS); // interrupt flag loeschen
_bis_SR_register(GIE);
//===Hier die Endlosschleife quasi das Betriebssystem=====================
  while(1){
	}	// Ende der Endlosschleife
}		// Ende Main
//====Ende des Hauptprogramms ============================================

#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	if (P1IFG & TASTE_LINKS) {
		BIT_CLR(P1IFG, TASTE_LINKS);
		U1TXBUF = '?';
	}
}
