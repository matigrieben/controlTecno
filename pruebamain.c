#include <stdio.h>
#include "funciones.h"
int main (void)
{
	int estatusLista=0, estatusRango=0, estatusPass=0, opcion_elegida, status_opcion;
	usuarios *h=NULL;

	estatusLista = ListarUsuarios(&h, "usuarios.txt"); //crear lista
	if(estatusLista) //si se cargo la lista sin errores
	{
		//LEER TARJETA
		estatusRango =paseUsuario(h, CODIGO LEIDO DE TARJETA);
		if(! estatusRango)
			printf("no se encontro el usuario, contactese con el administrador");
		else if(estatusRango)//si es admin
			estatusPass=contrasena();
			
		if(estatusPass)
		{
			opcion_elegida=opcionesMuenu();
		  
			switch(opcion_elegida)
			{
			case 1:
			  status_opcion = cargarArchivo();
			  break;
			case 2:
			  status_opcion = modificarUsuario();
			  break;
			case 3:
			  status_opcion = eliminarUsuario();
			  break;
			}
		}
		else
			printf("contraseña incorrecta");
	}		
}
