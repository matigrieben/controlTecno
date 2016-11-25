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
#include <sys/types.h>

int main(void)
{
	int estatusLista = 0, estatusRango = 0, estatusPass = 0, opcion_elegida, status_opcion, uart0_filestream = -1, flag = 1;
	usuarios *h = NULL;
	struct termios options;
	//unsigned char rx_buffer[100];
	char vector[27];
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
			printf("Esperando Tag:\n");
			while(flag)
			{
				stringTag(uart0_filestream, vector);
				estatusRango = paseUsuario(h, vector); //vector=codigo a buscar
				if(!estatusRango) printf("No se encontro el usuario, contactese con el administrador\n");
				else if(estatusRango == -1) printf("No existen usuarios en lista\n");
				else if(estatusRango == 2) {} //pasar la imagen por soket
				else if(contrasena())
				{
					switch(opcionesMenu())
					{
						case 1:
							if(!nuevoUsuario(uart0_filestream))
							{
								//imprimirListaUsuarios(h);
								liberarListaUsuarios(&h);
							  	if(!ListarUsuarios(&h, "usuarios.txt"))
							  	{
							  		printf("Error al ingresar en lista\n");
							  	 	flag = 0;
							  	 	liberarListaUsuarios(&h);
							  	}
							  	//else imprimirListaUsuarios(h);
							}
							break;
						case 2:
							modificarUsuario(uart0_filestream, &h);
							SubirUsuarios_Archivo(&h);
							break;
						case 3:
							eliminarUsuario(&h);
							SubirUsuarios_Archivo(&h);
							break;
						case 4:
							if(nuevaPass()) 
							printf("Cambio de contrasena exitoso!\n");
							else printf("Error en el cambio de contrasena!\n");
							break;
						case 5:
							flag = 0;
							liberarListaUsuarios(&h);
							break;
						default: printf("No ingreso una opcion valida!\n");
					}
				}
				else printf("Contraseña incorrecta\n");
				printf("Esperando Tag:\n");
				tcflush(uart0_filestream, TCIFLUSH);	//elimina buffer
			}
		}	
	}
	close(uart0_filestream);
	printf("Programa finalizado!\n");
	return 0;	
}
