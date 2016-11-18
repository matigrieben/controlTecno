Author= "Matías Grieben"
Curse= "R1042"
TP= "proyecto"


##########################
# Compiler and arguments #
##########################

# Compilador y linker a utilizar
CC = gcc
# Argumentos de compilacion el -g activa para hacer debuging en algun futuro
C_PARAM = -c -g   
# Argumentos de linkeo
LD= -lm `pkg-config --libs opencv`


##########################
#   Src, Obj and Exec    #
##########################

# Fuentes
SRC= $(wildcard *.c)
# Objetos
OBJ= $(SRC:.c=.o)
# Headers
HEAD= $(wildcard *.h)
# Nombre del ejecutable
Exec= ejecutable



##########################
#     Documentation      #
##########################
# Comando de documentacio
DOC_GEN= doxygen
# Archivo de configuracion para la documentacion
DOC_FILE= Doxyfile
# Comando de la documentacion
DOC_LINE= $(DOC_GEN) $(DOC_FILE) 

##########################
#        Compress        #
##########################

# Archivos a comprimir
OTHER_FILES= Makefile $(DOC_FILE)
# Nombre del archivo a generar
FILE_OUT= $(Author)-$(Curse)-$(TP).tar.gz
# Comando de compilacion
COMPRESS_LINE= tar -zcvf $(FILE_OUT) $(SRC) $(HEAD) $(OTHER_FILES)

######################################################################################################
######################################################################################################

all: $(Exec)
$(Exec): $(OBJ)
	@echo "\n===================================================================================="
	@echo "  Linking" $(OBJ)    
	@echo "===================================================================================="
	$(CC)  -o $(Exec) $(OBJ) $(LD)
	@echo "\n===================================================================================="
	@echo "  Finish: OK\n\n"

%.o: %.c
	@echo "\n===================================================================================="
	@echo "  Compiling the file: " $< 
	@echo "===================================================================================="
	$(CC) $(C_PARAM) $< -o $@ 

clean:
	@echo "\n===================================================================================="
	@echo "  Cleaning: " $(OBJ) $(Exec) 
	@echo "===================================================================================="
	rm $(OBJ) $(Exec) -f
	@echo "\n"

compress:
	@echo "\n===================================================================================="
	@echo "  Compressing "
	@echo "===================================================================================="
	@$(COMPRESS_LINE)	
	@echo "\n===================================================================================="
	@echo  $(FILE_OUT)": OK\n\n"

help:
	@echo "\n===================================================================================="
	@echo "  Help: "
	@echo "    all: Compile and Linker"
	@echo "    file.o: Compile the file.c"
	@echo "    clean: remove generated files"
	@echo "    doxy: makes the documentation"
	@echo "====================================================================================\n"

doc:
	@echo "\n===================================================================================="
	@echo "  Making the documentation " 
	@echo "===================================================================================="
	$(DOC_LINE)
	@echo "\n===================================================================================="
	@echo "  Documentation: OK\n\n"
	
