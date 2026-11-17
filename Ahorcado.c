#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<string.h>
#include <ctype.h> //libreria para usar toupper(), que convierte las letras minusculas a mayusculas.
#define MAX 1000 //Nuestro arreglo puede almacenar hasta 1000 palabras

typedef struct{
	char texto[30];
	int categoria;
	int dificultad;
}Palabra; //Tipo de dato "Palabra", que reune 3 elementos

typedef struct{ 
	int elementos[MAX];//Guarda las posiciones de las palabras
	int frente, atras;
}Cola;

int cargarPalabras(Palabra palabras[]);
int elegirPalabras(Palabra palabras[], int total, int categoria);
void mostrarPalabra(char descubiertas[]);
int buscarLetra(char palabra[],char letra);
void inicializarPalabra(char palabra[],char descubiertas[]);
void descubrirLetra(char palabra[], char descubiertas[], char letra);
int letraRepetida(char utilizadas[],int cantidad, char letra); //Solo comprueba si la letra existe en el arreglo.
void mostrarLetrasUtilizadas(char utilizadas[],int cantidad);
void jugar(Palabra palabras[],int total,Cola colas[]);
void mostrarBancoPalabras(Palabra palabras[],int total);
void agregarPalabra(Palabra palabras[],int *total); /*cuando uso int total, la funcion recibe una copia del numero de palabras, ahora utilizamo un puntero
que nos permite modificar la variable original*/
void administrarPalabras(Palabra palabras[],int *total);
int palabraRepetida(Palabra palabras[],int total,char nueva[]); //Funcion para buscar palabras repetidas.
int validarPalabra(char palabra[]);
void inicializarCola(Cola*cola);
int isEmpty(Cola*cola);
int isFull(Cola*cola);
int enqueueCola(Cola*cola,int posicion);
int dequeue(Cola*cola);
void llenarCola(Cola*cola,Palabra palabras[], int total, int categoria);
void dibujarAhorcado(int errores);

int main(int argc, char *argv[]) {
	Palabra palabras[MAX];
	Cola colas[3]; //Estamos creando 3 variables de tipo cola dentro de un arreglo.
	int total,opc,i;
	srand(time(NULL));
	total=cargarPalabras(palabras);
	for(i=0;i<3;i++){
		inicializarCola(&colas[i]);
	}	
	if(total==0){
		printf("No hay palabras disponibles.\n");
		return 1;
	}
	do{
		printf("\n=================================\n");
		printf("         JUEGO DEL AHORCADO\n");
		printf("===================================\n");
		printf("\n1. Iniciar nueva partida");
		printf("\n2. Administrar banco de palabras");
		printf("\n3. Consultar clasificacion de jugadores");
		printf("\n4. Consultar historial de partidas");
		printf("\n5. Consultar estadisticas generales");
		printf("\n6. Salir");
		printf("\n\nSeleccione una opcion: "); scanf("%i",&opc);
		switch(opc){
			case 1:
				jugar(palabras,total,colas);
				break;
			case 2:
				administrarPalabras(palabras,&total);
				break;
			case 3:
				printf("\nClasificacion de jugadores.\n"); //Pendiente
				break;
			case 4:
				printf("\nHistorial de partidad.\n"); //Pendiente
				break;
			case 5:
				printf("\nEstadisticas generales"); // Pendiente
				break;
			case 6:
				printf("\nGracias por jugar!\n");
				break;
			default:
				printf("\nOpcion no valida.\n");
		}
	}while(opc!=6);
	return 0;
}
void jugar(Palabra palabras[],int total,Cola colas[]){
	char letra;
	int posicion, resultado,intentos, categoria;
	int intentosIniciales,errores; /*Intentos iniciales guarda los intentos que recibe el jugador
	,errores indica que tan avanzado debe estar el dibujo*/
	char descubiertas[30]; //este arreglo tendra espacio para representar las letras descubiertas de nuestra palabra
	char utilizadas[27];//Guardara las letras que el jugador vaya introduciendo
	int cantidadUtilizadas=0;//Llevara la cuenta de cuantas letras diferentes se han registrado.
	printf("\n====================================\n");
	printf("\n");
	printf("           JUEGO DEL AHORCADO\n");
	printf("\n");
	printf("=====================================\n");
	printf("\n===== SELECCIONAR CATEGORIA =====\n");
	printf("1. Animales\n");
	printf("2. Tecnologia\n");
	printf("3. Deportes\n");
	printf("Seleccione una categoria: "); 
	if(scanf("%i",&categoria)!=1||categoria<1||categoria>3){
		printf("\nCategoria no valida.\n");
		return;
	}
	//Si la cola de esta categoria esta vacia, la llenamos
	if(isEmpty(&colas[categoria-1])){/*categoria-1 porque las actegorias empiezan en 1, pero los
		indices de los arreglos empiezan en 0*/
		llenarCola(&colas[categoria-1],palabras,total,categoria);
		/*Esta funcion busca las posiciones de las palabras de esa categoria
	, las mezcla aleatoriamente y las inserta en la cola*/
	}
	//Obtener la siguiente palabra de la cola
	posicion=dequeue(&colas[categoria-1]);
	if(posicion==-1){
		printf("\nNo hay palabras disponibles en esta categoria.\n");
		return;
	}
	inicializarPalabra(palabras[posicion].texto,descubiertas);
	switch(palabras[posicion].dificultad){
		case 1:
			intentos=8;
			break;
		case 2:
			intentos=6;
			break;
		case 3:
			intentos=4;
			break;
		default:
			intentos=6;
			break;
	}
	intentosIniciales=intentos;
	printf("\nDificultadad de la palabra: ");
	switch(palabras[posicion].dificultad){
		case 1:
			printf("Facil\n");
			break;
		case 2:
			printf("Medio\n");
			break;
		case 3:
			printf("Dificil\n");
			break;
		default:
			printf("Sin clasificar\n");
	}
	while(strcmp(palabras[posicion].texto,descubiertas)!=0 && intentos>0){
		errores=((intentosIniciales-intentos)*6)/intentosIniciales;
		dibujarAhorcado(errores);
		mostrarPalabra(descubiertas);
		 mostrarLetrasUtilizadas(utilizadas,cantidadUtilizadas);
		printf("\nIntentos disponibles: %i",intentos);
		printf("\nIngresa una letra: "); scanf(" %c",&letra);
		//Convertir la letra a mayuscula
		letra=toupper((unsigned char)letra);
		//Validad que sea una letra de A-Z
		if(letra<'A'|| letra>'Z'){
		printf("\nCaracter no valido. Ingresa una letra de la A a la Z.\n");
		continue;
		}
		if(letraRepetida(utilizadas,cantidadUtilizadas,letra)==1){
			printf("\nYa utilizaste esa letra.\n");
			continue;
		}
		utilizadas[cantidadUtilizadas]=letra; //Guardar la nueva letra en el arreglo
		cantidadUtilizadas++;
		//Buscar si la letra existe en la palabra
		resultado=buscarLetra(palabras[posicion].texto,letra);
		if(resultado==1){
			printf("\nCorrecto! La letra esta en la palabra.\n");
			descubrirLetra(palabras[posicion].texto,descubiertas,letra);
		}else{
			intentos--;
			printf("\nIncorrecto. La letra no esta en la palabra.\n");
		}
	}
	errores=((intentosIniciales-intentos)*6)/intentosIniciales;
	dibujarAhorcado(errores);
	mostrarPalabra(descubiertas);//mostramos la palabra oculta
	if(strcmp(palabras[posicion].texto,descubiertas)==0){
			printf("\nFELICIDADES! HAS GANADO!\n");
		}else{
			printf("\nHAS PERDIDO!\n");
			printf("La palabra era : %s\n",palabras[posicion].texto);
		}
}

int cargarPalabras(Palabra palabras[]){
    FILE *arch;
    int total = 0;
    int datos;
    char linea[150];
    arch = fopen("palabras.txt", "rt");
    if(arch == NULL){
        printf("No se pudo abrir el archivo.\n");
        return 0;
    }
    while(total < MAX && fgets(linea, sizeof(linea), arch) != NULL){

        datos = sscanf(linea, "%29s %d %d",palabras[total].texto,&palabras[total].categoria,&palabras[total].dificultad);
        if(datos == 3){
            total++;
        }
        else if(datos == 1){
            palabras[total].categoria = 0;
            palabras[total].dificultad = 0;
            total++;
        }
    }
    fclose(arch);
    return total;
}
int elegirPalabras(Palabra palabras[],int total, int categoria){
	int i;
	int cantidad=0;
	int posiciones[MAX];
	int seleccion;
	for(i=0;i<total;i++){
		if(palabras[i].categoria==categoria){ /*Si el jugadpr selecciona 1, nuestro
			ciclo solamente guardara las posiciones 0 y 3 en el arreglo posiciones*/
			posiciones[cantidad]=i;
			cantidad++;
		}
	}
	if(cantidad==0){
		return -1; /*Si no encuentro ninguna palabra de la categoria elegida,
		la funcion devuelve -1*/
	}
	seleccion=rand()%cantidad; // Elegira aleatoriamenre una de esas posiciones
	return posiciones[seleccion];
}

int buscarLetra(char palabra[],char letra){ //para verificar que la letra que ese inserto este dentro de la palabra.
	int i;
	int longitud;
	longitud=strlen(palabra);
	for(i=0;i<longitud;i++){
		if(palabra[i]==letra){
			return 1;
		}
	}
	return 0;
}

void mostrarPalabra(char descubiertas[]){
	int i;
	printf("\nPalabra: ");
	for(i=0;descubiertas[i]!='\0';i++){
		printf(" %c",descubiertas[i]);

	}
	printf("\n");

}

void inicializarPalabra(char palabra[],char descubiertas[]){
	int i, longitud;
	longitud=strlen(palabra);
	for(i=0;i<longitud;i++){
		descubiertas[i]='_';
	}
descubiertas[longitud]='\0';
}

void descubrirLetra(char palabra[], char descubiertas[], char letra) {
    int i;
    int longitud;
    longitud = strlen(palabra);
    for (i = 0; i < longitud; i++) {
        if (palabra[i] == letra) {
            descubiertas[i] = letra;
        }
    }
}//Recorre la palabra original y, cuando encuentra una coincidencia, coloa la letra en la misma posicion del arreglo descubiertas

int letraRepetida(char utilizadas[],int cantidad, char letra){
	int i;
	for(i=0;i<cantidad;i++){
		if(utilizadas[i]==letra){
			return 1;
		}
	}
	return 0;
}

void mostrarLetrasUtilizadas(char utilizadas[],int cantidad){
	int i;
	printf("\nLetras utilizadas: ");
	for(i=0;i<cantidad;i++){
		printf("%c ",utilizadas[i]);
	}
	printf("\n");
}

void mostrarBancoPalabras(Palabra palabras[],int total){ //No estamos leyendo nuevamente el archivo, estamos utilizando las palabras que ya cargamos.
	int i;
	printf("\n===== BANCO DE PALABRAS =====\n");
	for(i=0;i<total;i++){
		printf("\n%i. %s\n",i+1,palabras[i].texto); /*Muestra el numero de la palabra y su contenido. i+1 porque queremos que la lista empiece en 1*/
		printf("\n  Categoria:");
		switch(palabras[i].categoria){
			case 1:
				printf("Animales");
				break;
			case 2:
				printf("Tecnologia");
				break;
			case 3:
				printf("Deportes");
				break;
			default:
				printf("Sin clasificar");
		}
		printf("\n  Dificultad: ");
		switch(palabras[i].dificultad){
			case 1:
				printf("Facil");
				break;
			case 2:
				printf("Medio");
				break;
			case 3:
				printf("Dificil");
				break;
			default:
				printf("Sin clasificar");
		}

	}
	printf("\nTotal de palabraa: %i\n",total);
}

void agregarPalabra(Palabra palabras[], int *total){
    char nueva[30];
    char entrada[100];
    int i, categoria, dificultad;
    FILE *arch;
    if(*total >= MAX){
        printf("\nEl banco de palabras esta lleno.\n");
        return;
    }
    printf("\nIngrese una nueva palabra: ");
    if(scanf("%99s", entrada) != 1){
        return;
    }
    if(strlen(entrada) > 29){
        printf("\nLa palabra es demasiado larga.\n");
        return;
    }
    strcpy(nueva, entrada);
    for(i = 0; nueva[i] != '\0'; i++){
        nueva[i] = toupper((unsigned char)nueva[i]);
    }
    if(validarPalabra(nueva) == 0){
        printf("\nLa palabra contiene caracteres invalidos.\n");
        return;
    }
    if(palabraRepetida(palabras, *total, nueva) == 1){
        printf("\nLa palabra ya existe.\n");
        return;
    }
    printf("\nSeleccione la categoria:\n");
    printf("1. Animales\n");
    printf("2. Tecnologia\n");
    printf("3. Deportes\n");
    printf("Opcion: ");
    if(scanf("%d", &categoria) != 1 ||categoria < 1 || categoria > 3){
        printf("\nCategoria no valida.\n");
        return;
    }
    printf("\nSeleccione la dificultad:\n");
    printf("1. Facil\n");
    printf("2. Medio\n");
    printf("3. Dificil\n");
    printf("Opcion: ");
    if(scanf("%d", &dificultad) != 1 ||dificultad < 1 || dificultad > 3){
        printf("\nDificultad no valida.\n");
        return;
    }
    arch = fopen("palabras.txt", "at");

    if(arch == NULL){
        printf("\nNo se pudo abrir el archivo.\n");
        return;
    }
    fprintf(arch, "\n%s %d %d", nueva, categoria, dificultad);
    fclose(arch);
    strcpy(palabras[*total].texto,nueva);//Agregamos la palabra en el arreglo
    palabras[*total].categoria = categoria;
    palabras[*total].dificultad = dificultad;
    (*total)++;
    printf("\nPalabra agregada correctamente.\n");
}

void administrarPalabras(Palabra palabras[],int *total){
	int opc;
	do{
		printf("\n===== ADMINISTRACION DE PALABRAS =====");
		printf("\n1.Mostrar banco de palabras"); 
		printf("\n2.Agregar nueva palabra");
		printf("\n3.Regresar al menu principal");
		printf("\n\nSeleccione una opcion: "); scanf("%i",&opc);
		switch(opc){
			case 1:
				mostrarBancoPalabras(palabras,*total);
				break;
			case 2: 
				agregarPalabra(palabras,total);
				break;
			case 3:
				printf("\nRegresando al menu principal...\n");
				break;
			default:
				printf("\nOpcion no valida.\n");
		}
	}while(opc!=3);
}

int palabraRepetida(Palabra palabras[], int total, char nueva[]){
	int i;
	for(i=0;i<total;i++){
		if(strcmp(palabras[i].texto,nueva)==0){ //Devuelve 1 cuando las cadenas son iguales, si son diferente es 0
			return 1;
		}
	}
	return 0;
}

int validarPalabra(char palabra[]){
	int i;
	if(palabra[0]=='\0'){ //1 si la palabra es valida
		return 0;
	}
	for(i=0;palabra[i]!='\0';i++){
		if(palabra[i]<'A'||palabra[i]>'Z'){ //Si encuentro un caracter fuera del intervalo de A a Z, la palabra no es valida
			return 0;
		}
	}
	return 1;
}

void inicializarCola(Cola*cola){
	cola->frente=-1;
	cola->atras=-1;
}

int enqueueCola(Cola*cola,int posicion){
	int r=0;
	if(!isFull(cola)){
		r=1;
		cola->elementos[++cola->atras]=posicion;
		if(cola->frente<0){
			cola->frente++;
		}
	}
	return r;
}

int isEmpty(Cola*cola){
	if(cola->frente==-1){
		return 1;
	}
	return 0;
/* devuelve 1 si la cola esta vacia, 0 si contiene elementos*/
}

int isFull(Cola*cola){
	if(cola->atras==MAX-1){/*Como nuestro arreglo tiene 1000 espacios pero los 
		indices va de 0 a 999, cuando atras llega a 999, significa que alcanzamos
		el ultimo espacio disponible*/
		return 1;
	}
	return 0;
}

int dequeue(Cola*cola){
	int saliente=-1;
	if(!isEmpty(cola)){
		saliente=cola->elementos[cola->frente];
		cola->frente++;
		if(cola->frente>cola->atras){
			cola->frente=-1;
			cola->atras=-1;
		}
	}
	return saliente;
}

void llenarCola(Cola*cola,Palabra palabras[], int total, int categoria){
	int posiciones[MAX];
	int cantidad=0;
	int i,j,aux;
	//Buscar las palabras de la categoria
	for(i=0;i<total;i++){/*Digamos que tenemos el banco de palabras con sus categorias, si el jugador seleccion la categoria 1(animales)
		Este ciclo guardara las posicionesY despues mezclamos esas posiciones. para insertarlas en ka cika ¿*/
		if(palabras[i].categoria==categoria){
			posiciones[cantidad]=i;
			cantidad++;
		}
	}
	//Mezclar las posiciones aleatoriamente
	for(i=cantidad-1;i>0;i--){
		j=rand()%(i+1);
		aux=posiciones[i];
		posiciones[i]=posiciones[j];
		posiciones[j]=aux;
	}
	//Insertar las posiciones en la cola
	for(i=0;i<cantidad;i++){
		enqueueCola(cola,posiciones[i]);
	}
}

void dibujarAhorcado(int errores){
    printf("\n  +----+");
    printf("\n  |    |");
    if(errores >= 1) //1 o mas errores, mostramos la cabeza
        printf("\n  O    |");
    else
        printf("\n       |");
    if(errores >= 4)
        printf("\n /|\\   |");
    else if(errores >= 3)
        printf("\n /|    |");
    else if(errores >= 2)
        printf("\n  |    |");
    else
        printf("\n       |");
    if(errores >= 6)
        printf("\n / \\   |");
    else if(errores >= 5)
        printf("\n /     |");
    else
        printf("\n       |");
    printf("\n       |");
    printf("\n =======\n");
}
