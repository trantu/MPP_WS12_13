static int len = 0;

#pragma vector = USART1RX_VECTOR
__interrupt void USART1RX(void)
{
	char c = U1RXBUF;
	char str[14] = {0};
	
	if (c == '\r') {
		snprintf(str, sizeof(str)-1, "%d\r\n", len);
		print_buf(str);
		lcd_clear(WHITE);
		lcd_paint();
		len = 0;
	} else {
		str[0] = c;
		str[1] = '\0';
		lcd_string(BLACK, (len % 21)*6, (len / 21)*9, str);
		lcd_paint();
		len++;
		
	}
}
