#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<string.h>
#include <ctype.h> //libreria para usar toupper(), que convierte las letras minusculas a mayusculas.
#define MAX 1000 //Nuestro arreglo puede almacenar hasta 1000 palabras

typedef struct{
	char texto[30];
	int categoria;
	int dificultad; //operador "." sirve para acceder a los campos de una variable de tipo estructura
}Palabra; //Tipo de dato "Palabra", que reune 3 elementos

typedef struct{ 
	int elementos[MAX];//Guarda las posiciones de las palabras
	int frente, atras;
}Cola; //No almacenamos directamente las palabras, sino sus posiciones dentro del banco de palabras

typedef struct sPlayer {
    char name[50];
    int jugadas, ganadas, perdidas;
    int totalScore, bestScore;
} tPlayer;

typedef struct sNodoLista { //mantiene jugadores en memoria para buscarlos por nombre y actualizar datos
    tPlayer datos;
    struct sNodoLista *sig;    
} tNodoLista;

tNodoLista *inicio=NULL;

typedef struct sNodoArbol { //Para ordenar por puntaje solo cuando se solicite
    tPlayer datos;
    struct sNodoArbol *izq,*der;
} tNodoArbol;

tNodoArbol *raiz=NULL;

typedef struct sPartida {
    char nombre[50];
    char palabra[30];
    int gano;
    int intentos;
    int puntos;
    struct sPartida *sig;
} tPartida;

tPartida *topHistorial = NULL;

int apilarPartida(const char *nombre, const char *palabra, int gano, int intentos, int puntos);
void mostrarHistorial(void);
void liberarHistorial(void);
void cargarHistorial(void);

tNodoArbol *insertarArbol(tPlayer p);
void printRanking(tNodoArbol *actual, int *posicion);
void liberarArbol(tNodoArbol *raiz);
void mostrarClasif(tNodoLista *listaPlayers);
void liberarLista(void);
void guardarJugadores(void);
tNodoLista *buscarJugador(const char *nombre);
void registrarRes(const char *nombre, int gano, int puntos);
tNodoLista *cargarJugadores(void);

int cargarPalabras(Palabra palabras[]);
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
int calcularPuntos(int dificultad,int intentos,int gano);
void mostrarEstadisticas(int totalPartidas,int ganadas,int perdidas, int puntosTotales);

int main(int argc, char *argv[]) {
	Palabra palabras[MAX];
	Cola colas[3]; //Estamos creando 3 variables de tipo cola dentro de un arreglo.
	int total,opc=0,i;
	srand(time(NULL));
	cargarJugadores();
	cargarHistorial();
	total=cargarPalabras(palabras);
	for(i=0;i<3;i++){
		inicializarCola(&colas[i]);
	}	
	if(total!=0){
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
					mostrarClasif(inicio);
					break;
				case 4:
					printf("\n===== HISTORIAL DE PARTIDAS =====\n");
                    mostrarHistorial();
                    printf("=================================\n");
                    break;
				case 5:{
						tNodoLista *actual = inicio;
						int totalPartidas = 0;
						int ganadas = 0;
						int perdidas = 0;
						int puntosTotales = 0;

						while (actual != NULL) {
							totalPartidas += actual->datos.jugadas;
							ganadas += actual->datos.ganadas;
							perdidas += actual->datos.perdidas;
							puntosTotales += actual->datos.totalScore;
							actual = actual->sig;
						}
						printf("\n ======= ESTADISTICAS GENERALES =======\n");
						mostrarEstadisticas(totalPartidas, ganadas, perdidas,puntosTotales);
						printf(" ======================================\n");
						break;
					}
				case 6:
					printf("\nGracias por jugar!\n");
					break;
				default:
					printf("\nOpcion no valida.\n");
					break;
			}
		}while(opc!=6);
	} else {
		printf("No hay palabras disponibles.\n");
	}
	liberarLista();
	liberarHistorial();
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
	char nombreJugador[50];
	int puntos,gano;//variables para guardar el resultado de la partida y los puntos obtenidos
	printf("\n=====================================\n");
    printf("           JUEGO DEL AHORCADO\n");
    printf("=====================================\n");
    
	printf("\n===== NUEVA PARTIDA =====\n");
    printf("Nombre del jugador: "); scanf(" %49[^\n]",nombreJugador); /*[^\n permite leer nombre que contienen espacios]*/
	
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
	printf("\nDificultad de la palabra: ");
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
	gano=(strcmp(palabras[posicion].texto,descubiertas)==0);//Compara la palabra original con la descubierta
	puntos=calcularPuntos(palabras[posicion].dificultad,intentos,gano);
	printf("\n===== RESULTADO DE LA PARTIDA =====\n");
	printf("Jugador: %s\n",nombreJugador);
	if(gano==1){
		printf("Resultado: VICTORIA\n");
	}else{
		printf("Resultado: DERROTA\n");
	}
	printf("Palabra: %s\n",palabras[posicion].texto);
	printf("Intentos restantes: %i\n",intentos);
	printf("Puntos obtenidos: %i\n",puntos);
	registrarRes(nombreJugador, gano, puntos);
	apilarPartida(nombreJugador, palabras[posicion].texto, gano, intentos, puntos);

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
	printf("\nTotal de palabras: %i\n",total);
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

int calcularPuntos(int dificultad,int intentos,int gano){
	int puntos=0;
	if(gano==0){
		return 0;
	}
	switch(dificultad){
		case 1:
			puntos=50+(intentos*5);
			break;
		case 2:
			puntos=100+(intentos*10);
			break;
		case 3:
			puntos=150+(intentos*15);
			break;
	}
	return puntos;
}/*intentos indica cuantos intentos le quedan al jugador, 1 si gano,0 si perdio*/

void mostrarEstadisticas(int totalPartidas,int ganadas,int perdidas, int puntosTotales){
	float porcentaje, promedio;
	printf("Total de partidas: %i\n",totalPartidas);
	printf("Partidas ganadas: %i\n",ganadas);
	printf("Partidas perdidas: %i\n",perdidas);
	if(totalPartidas>0){//Evita dividir entre 0 cuando todavia no se ha jugado ninguna partida
		porcentaje=(ganadas*100.0f)/totalPartidas;
		promedio=(float)puntosTotales/totalPartidas;
	}else{
		porcentaje=0;
		promedio=0;
	}
	printf("Porcentaje de victorias: %.2f%%\n",porcentaje);
	printf("Promedio de puntos: %.2f\n",promedio);
}

tNodoLista *cargarJugadores(void){
    FILE *arch;
	//ya esta el inicio = NULL arriba.
	char linea[200];
	tPlayer player;

    arch = fopen("jugadores.txt", "rt");

    if(arch != NULL){
        while(fgets(linea, sizeof(linea), arch)!= NULL){

			//sscanf -> string scanf, devuelve cantidad de variables que logró llenar
			if (sscanf(linea, "%49[^|]|%d|%d|%d|%d|%d", 
								player.name, &player.jugadas, &player.ganadas, 
								&player.perdidas, &player.totalScore, 
								&player.bestScore)==6)
			{
				tNodoLista *nuevo = (tNodoLista *)malloc(sizeof(tNodoLista));
				if(nuevo!=NULL){
					nuevo->datos = player;
					nuevo->sig = inicio; //insertar al inicio de la lista
					inicio = nuevo;
				}
			} 
		}
    } else {
        arch = fopen("jugadores.txt", "wt");
	}
	if (arch!=NULL)
		fclose(arch);
	return inicio;
}

tNodoLista *buscarJugador(const char *nombre){
	tNodoLista *aux = inicio;
	while(aux!=NULL && strcmp(aux->datos.name, nombre)!=0)
		aux=aux->sig;
	return aux; //NULL si no lo encontró
}

void guardarJugadores(void){
	FILE *arch = fopen("jugadores.txt", "wt");
	tNodoLista *aux=inicio;
	if(arch!=NULL){
		while(aux!=NULL){
			fprintf(arch, "%s|%d|%d|%d|%d|%d\n", 
				aux->datos.name, aux->datos.jugadas,
				aux->datos.ganadas, aux->datos.perdidas,
				aux->datos.totalScore, aux->datos.bestScore);
			aux=aux->sig;
		}
		fclose(arch);
	} else{
		printf("No se pudieron guardar los jugadores.\n");
	}
}

void registrarRes(const char *nombre, int gano, int puntos){
	tNodoLista *encontrado = buscarJugador(nombre);

	if(encontrado!=NULL){
		encontrado->datos.jugadas++;
		if (gano==1) 
			encontrado->datos.ganadas++;
		else 
			encontrado->datos.perdidas++;

		encontrado->datos.totalScore += puntos;
		if(puntos > encontrado->datos.bestScore)
			encontrado->datos.bestScore = puntos;
	} else {
		tNodoLista *nuevo = (tNodoLista *)malloc(sizeof(tNodoLista));
		if (nuevo!=NULL){

			snprintf(nuevo->datos.name, sizeof nuevo->datos.name, "%s", nombre);
			//snprintf escribe en el arreglo destino
			//snprintf(arregloDestino, sizeof destino, formato, valor a insertar)

			nuevo->datos.jugadas=1;
			nuevo->datos.ganadas = (gano==1)? 1 : 0;
			nuevo->datos.perdidas = (gano==1) ? 0 : 1;
			nuevo->datos.totalScore = puntos;
			nuevo->datos.bestScore = puntos;
			nuevo->sig = inicio;
			inicio = nuevo;
		} else 
			printf("No hay memoria para registrar al jugador.\n");
	}
	guardarJugadores();
}

tNodoArbol *insertarArbol(tPlayer p){
	tNodoArbol *aux=raiz, *futuroPapa=NULL;
	tNodoArbol *nuevo = (tNodoArbol *)malloc(sizeof(tNodoArbol));
	if(nuevo!=NULL){
		nuevo->datos=p;
		nuevo->der = nuevo->izq = NULL;
		if(raiz == NULL){
			raiz = nuevo;
		}else{
			while(aux!=NULL){
				futuroPapa=aux;
				if(p.totalScore <= aux->datos.totalScore)
					aux = aux->izq;
				else
					aux = aux->der;
			}
			if(p.totalScore <= futuroPapa->datos.totalScore)
				futuroPapa->izq=nuevo;
			else 
				futuroPapa->der=nuevo;
		}
	} else
		printf("No se pudo reservar memoria.\n");
	return nuevo;
}

void printRanking(tNodoArbol *actual, int *posicion){
	if(actual!=NULL){
		printRanking(actual->der, posicion);
		printf("#%i. %s | Jugadas: %i | Ganadas: %i | Perdidas: %i | Puntaje total: %d | Mejor puntaje: %d\n",
			(*posicion)++, actual->datos.name, actual->datos.jugadas, actual->datos.ganadas, actual->datos.perdidas,
		actual->datos.totalScore, actual->datos.bestScore);
		printRanking(actual->izq, posicion);
	}
}

void liberarArbol(tNodoArbol *raiz){
	if(raiz!=NULL){
		liberarArbol(raiz->izq);
		liberarArbol(raiz->der);
		free(raiz);
	}
}

void mostrarClasif(tNodoLista *listaPlayers){
	tNodoLista *actual = listaPlayers;
	int posicion = 1;
	if(listaPlayers!=NULL){

		while(actual!=NULL){
			insertarArbol(actual->datos);
			actual=actual->sig;
		}

		printf("\n===== CLASIFICACION DE JUGADORES =====\n");
		printRanking(raiz, &posicion);
		printf("======================================\n");
		liberarArbol(raiz);
		raiz=NULL;
	}else
		printf("\nAun no existen registros de jugadores.\n");
}

void liberarLista(void) {
    tNodoLista *actual = inicio;

    while (actual != NULL) {
        tNodoLista *siguiente = actual->sig;
        free(actual);
        actual = siguiente;
    }

    inicio = NULL;
}
void cargarHistorial(void){
	FILE *arch = fopen("historial.txt", "rt");
	char linea[150];

	if(arch!=NULL){
		while (fgets(linea, sizeof linea, arch) != NULL){
			tPartida registro;
			tPartida *nuevo;

			if(sscanf(linea, "%49[^|]|%29[^|]|%d|%d|%d", 
				registro.nombre, registro.palabra, &registro.gano, 
				&registro.intentos, &registro.puntos)==5){
				nuevo=(tPartida *)malloc(sizeof(tPartida));
				if(nuevo!=NULL){
					snprintf(nuevo->nombre, sizeof nuevo->nombre, "%s", registro.nombre);
					snprintf(nuevo->palabra, sizeof nuevo->palabra, "%s", registro.palabra);
					nuevo->gano = registro.gano;
					nuevo->intentos = registro.intentos;
					nuevo->puntos = registro.puntos;

					nuevo->sig = topHistorial;
					topHistorial = nuevo;
				} else
					printf("No hay memoria para cargar todo el historial.\n");
			}
		}
		fclose(arch);
	}else 
		printf("Aun no se tienen registros de partidas.\n");
}

int apilarPartida(const char *nombre, const char *palabra, int gano, int intentos, int puntos){
	tPartida *nuevo = (tPartida *)malloc(sizeof(tPartida));
	FILE *arch;
	int seGuardo=0;

	if(nuevo!=NULL){
		snprintf(nuevo->nombre, sizeof nuevo->nombre, "%s", nombre);
		snprintf(nuevo->palabra, sizeof nuevo->palabra, "%s", palabra);
		nuevo->gano = gano;
		nuevo->intentos = intentos;
		nuevo->puntos = puntos;

		arch = fopen("historial.txt", "at");
		if(arch!=NULL){
			seGuardo = fprintf(arch, "%s|%s|%d|%d|%d\n",
                nuevo->nombre, nuevo->palabra, nuevo->gano,
                nuevo->intentos, nuevo->puntos) >= 0;
			//fprintf devuelve un numero negativo si hubo error

			if(fclose(arch)!=0) //fclose devuelve 0 si el cierre salio bien, !=0 significa error
				seGuardo=0;

			if(seGuardo){
				nuevo->sig = topHistorial;
				topHistorial = nuevo;
			}else{
				printf("No se pudo guardar la partida en el archivo.\n");
				free(nuevo);
			}
		} else {
			printf("No se pudo abrir el archivo para guardar el historial.\n");
			free(nuevo);
		}
	} else 
		printf("No hay memoria para guardar la partida.\n");
	return seGuardo;
}
void mostrarHistorial(void){
	tPartida *actual = topHistorial;
	int numero = 1;

	if(actual!=NULL){
		while(actual!=NULL){
			printf("#%d | Jugador: %s | Palabra: %s | Resultado: %s"
               " | Intentos restantes: %d | Puntos: %d\n",
			   numero++,
			   actual->nombre,
			   actual->palabra,
			   actual->gano ? "Victoria" : "Derrota",
			   actual->intentos,
			   actual->puntos);
			actual=actual->sig;
		}
	} else {
		printf("Aun no hay partidas en el historial.\n");
	}
}
void liberarHistorial(void){
	tPartida *actual = topHistorial;
	while(actual!=NULL){
		tPartida *sig = actual->sig;
		free(actual);
		actual=sig;
	}
	topHistorial=NULL;
}
