#pragma vector = USART1RX_VECTOR
__interrupt void USART1RX(void)
{
  char c = U1RXBUF;
	BIT_CLR(IFG2, URXIFG1);

	if (c == 'E')
		LED_ON(LED_GELB);
	else if (c == 'A')
		LED_OFF(LED_GELB);
	else if (c == '\r')
		LED_TOGGLE(LED_GRUEN);
}

...
void init_UART1(void)
{
	...
	//	IE2 .......;			// Enable Interrupt
	IE2 = BIT4;
}