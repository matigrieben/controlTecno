typedef struct usuarios
{
	char codigo[30];
	char nombre[30];
	char apellido[30];
	int edad;
	int documento;
	int rango; //1 admin, 2 usuario
	struct usuarios *sig;
}usuarios;

void logg(int); 
int cargarArchivo();
int ListarUsuarios(struct usuarios **, char *);
void liberarListaUsuarios(struct usuarios *);
void imprimirListaUsuarios(struct usuarios *);
int paseUsuario(struct usuarios *, char *);
void imprimirUsuarioEncontrado(struct usuarios *);
int contrasena();

int modificarUsuario();