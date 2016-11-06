#include <stdio.h>
#include <unistd.h>			
#include <fcntl.h>			
#include <termios.h>		

int main(void)
{
	int uart0_filestream = -1;	
	uart0_filestream = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (uart0_filestream == -1)
	{
		printf("Error - Unable to open UART\n");
		return 0;
	}
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);

unsigned char rx_buffer[100];
int rx_length = 0, hola = 0, contador = 0;
char vector[27];
	while(1)
	{
		if (uart0_filestream != -1)
		{
			rx_length = read(uart0_filestream, (void *)rx_buffer, 100);				
			if (rx_length > 0)
			{	
				hola = hola + rx_length;
				while(contador < rx_length && hola < 27)
				{
				  vector[(hola - rx_length + contador)] = rx_buffer[contador];
				  contador++;
  				}
				contador = 0;
				if(hola > 25)
					{	
						vector[27] = '\0'; 	
						printf("hola %d    bytes read : %s\n", hola, vector);
						rx_length = 0;
						hola = 0;
					        contador = 0;
					}
			}
		}
	}
close(uart0_filestream);
return 0;
}