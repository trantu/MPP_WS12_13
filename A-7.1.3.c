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

#pragma vector = PORT1_VECTOR
__interrupt void PORT1(void)
{
	if (P1IFG & TASTE_LINKS) {
		BIT_CLR(P1IFG, TASTE_LINKS);
		print_buf("Hallo World\r\n");		
	}
}
