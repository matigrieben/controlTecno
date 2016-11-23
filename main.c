#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(void)
{
	int estatusLista = 0, estatusRango = 0, estatusPass = 0, opcion_elegida, status_opcion, uart0_filestream = -1, rx_length = 0, hola = 0, contador = 0, flag = 1;
	usuarios *h=NULL;
	struct termios options;
	unsigned char rx_buffer[100];
	char vector[27];
	int si = 1;
	//cvStartWindowThread();
	uart0_filestream = open("/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (uart0_filestream == -1)
	{
		printf("Error - Imposible abrir el puerto serie\n");
	}
	else
	{
		tcgetattr(uart0_filestream, &options);
		options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
		options.c_iflag = IGNPAR;
		options.c_oflag = 0;
		options.c_lflag = 0;
		tcflush(uart0_filestream, TCIFLUSH);
		tcsetattr(uart0_filestream, TCSANOW, &options);

		estatusLista = ListarUsuarios(&h, "usuarios.txt"); //crear lista
		if(estatusLista) //si se cargo la lista sin errores
		{
			while(flag)
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
								vector[26] = '\0'; 	
								//printf("hola %d    bytes read : %s\n", hola, vector);
								rx_length = 0;
								hola = 0;
								contador = 0;
								estatusRango = paseUsuario(h, vector, &si); //vector=codigo a buscar
								if(!estatusRango) printf("No se encontro el usuario, contactese con el administrador");
								else if(estatusRango == -1) printf("No existen usuarios en lista");
								else if(estatusRango == 2) {} //pasar la imagen por soket
								else if(contrasena())
								{
									switch(opcionesMenu())
									{
									case 1:
									  if(!nuevoUsuario(uart0_filestream))
									  {
									  	 liberarListaUsuarios(h);
									  	 if(!ListarUsuarios(&h, "usuarios.txt"))
									  	 {
									  	 	printf("Error al ingresar en lista\n");
									  	 	flag = 0;
									  	 	liberarListaUsuarios(h);
									  	 }
									  }
									break;
									/*case 2:
									  status_opcion = modificarUsuario();
									  break;*/
									/*case 3:
									  status_opcion = eliminarUsuario();
									  break;*/
									  case 4:
									  if(nuevaPass()) 
										printf("Cambio de contrasena exitoso!\n");
									  else printf("Error en el cambio de contrasena!\n");
									  break;
									  case 5:
									  flag = 0;
									  liberarListaUsuarios(h);
									  break;
									  default: printf("No ingreso una opcion valida!\n");
									}
								}
								else printf("Contraseña incorrecta");
								tcflush(uart0_filestream, TCIFLUSH);	//elimina buffer
							}
					}
				}
			}
		}	
	}
	close(uart0_filestream);
	printf("Programa finalizado!\n");
	return 0;	
}
