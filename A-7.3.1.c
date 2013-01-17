static unsigned char hours = 0;
static unsigned char mins = 0;
static unsigned char secs = 0;

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
  char buffer[16];
	secs++;
	if (secs > 59) {
		mins++;
		secs %= 60;
		if (mins > 59) {
			mins %= 60;
			hours++;
			if (hours > 23) {
				hours %= 24;	
			}
		}
	}
	SHT11_Read_Sensor();
	print_buf(temp_char);
	print_buf(humi_char);
	sprintf(buffer, " %02d:%02d:%02d\r\n", hours, mins, secs);
	print_buf(buffer);
}
