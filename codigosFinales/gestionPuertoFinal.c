#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255
#define N 100
#define L 50

typedef struct{	
	char nombre[L];
	int pasillo;
	float precio;
	int cantidad;
}modeloprod;
//Las propiedades del producto son su nombre, el pasillo donde esta guardado, precio y cantidad en el almacen

typedef struct
{
    HANDLE handler;
    int connected;
    COMSTAT status;
    DWORD errors;
    char * portName;
} SerialPort;

int menu(void);

void nuevo(modeloprod [], int*, SerialPort *arduino, char*);			//Crea un nuevo producto
void eliminar(modeloprod [], int *, SerialPort *arduino, char*);		//Elimina un producto existente
void editar(modeloprod [], int, SerialPort *arduino, char*);			//Edita nombre, precio, pasillo y cantidad de un producto existente
void listar(modeloprod [], int);										//Muestra el inventario completo
void buscar(modeloprod [], int);										//Busca un producto por su nombre
int comparar(char [], char []);


void cargarinventario(modeloprod [], int *);
void guardarinventario(modeloprod [], int);

void Crear_Conexion(SerialPort * PuertoSerie,char *portName);
void CerrarConexion(SerialPort * PuertoSerie);

int readSerialPort(SerialPort *PuertoSerie,char *buffer, unsigned int buf_size);
int writeSerialPort(SerialPort *PuertoSerie,char *buffer, unsigned int buf_size);
int isConnected(SerialPort *);

void reponer(int pasillo, SerialPort *arduino, char *incomingData);
void cambioPasillo(int pasillo,int nuevoPasillo,SerialPort *arduino,char *incomingData);


int main(int argc, char *argv[]) {
	int n=0, opcion, pasillo;
	modeloprod producto[N];
	
	SerialPort *arduino;
	char *portName = "\\\\.\\COM3";
	char incomingData[MAX_DATA_LENGTH];
	arduino = (SerialPort*)malloc(sizeof(SerialPort));
	Crear_Conexion(arduino, portName);
	
	cargarinventario(producto, &n);		//Abre un fichero y lee toda la informacion de cada producto
	do{
		opcion=menu();
		switch(opcion){
			case 1: 
				nuevo(producto, &n, arduino, incomingData);				//recordar q no negativos
				break;
			case 2:
				eliminar(producto, &n, arduino, incomingData);	
				break;
			case 3:
				editar(producto, n, arduino, incomingData);
				break;
			case 4:
				listar(producto, n);
				break;
			case 5:
				buscar(producto, n);
				break;
		}
		system("pause");
		system("cls");
	}while(opcion!=6);
    guardarinventario(producto, n);		//Guarda el inventario en un fichero de texto.
	return 0;
}


void cargarinventario(modeloprod prod[],int *pn){
	int n;
	FILE *inventario;
    char *ptrtoken = malloc(sizeof(char)), Cadena_aux[L];
    
	n=0;												//Iniciamos un contador de productos
	
	inventario = fopen("inventario.txt","rt");
	
	if(inventario == NULL)
		printf("El fichero no existe\n");
	else{
		while(fgets(Cadena_aux, L, inventario) != NULL){		//lee primera linea del fichero y empieza un bucle while que no acaba hasta llegar al final del fichero
			ptrtoken = strtok(Cadena_aux, ";");			//divide la cadena con el separador ;
       			strcpy(prod[n].nombre, ptrtoken);			//una vez dividida copia el primer trozo de la cadena que es el nombre del producto
			ptrtoken = strtok(NULL, ";");				//vuelve a dividir la cadena hasta llegar al siguiente ; que se convertira en NULL
			prod[n].pasillo = atoi(ptrtoken);			//convertimos la cadena en un entero
			ptrtoken = strtok(NULL, ";");				//vuelve a dividir la cadena hasta llegar al siguiente ; que se convertira en NULL
			prod[n].precio = atof(ptrtoken);
			ptrtoken = strtok(NULL, ";");
			prod[n].cantidad = atoi(ptrtoken);
			n++;
		}
		fclose(inventario);
	}
    *pn=n;
}

//Menu visual de las opciones del programa
int menu(){
	int opcion;
	do{
		printf("\nMenu de inventario\n");
		printf("===============\n");
		printf("1 - Nuevo producto\n");
		printf("2 - Eliminar producto\n");
		printf("3 - Editar producto\n");
		printf("4 - Listar productos\n");
		printf("5 - Buscar producto\n");
		printf("6 - Salir y guardar\n");
		printf("Introduce opcion: ");
		scanf("%d",&opcion);
		printf("\n");
	}while((opcion<1) || (opcion>6));
	return opcion;
}

//añade un nuevo producto al inventario
void nuevo(modeloprod prod[], int *pn, SerialPort *arduino, char *incomingData){
	char num;
	
	if(*pn<N){
		printf("Nombre: ");
		fflush(stdin);
		
		gets(prod[*pn].nombre);
		
		printf("Pasillo donde se almacena: ");
		scanf("%d", &prod[*pn].pasillo);
		while(prod[*pn].pasillo<=0){
			printf("ERROR! Repita pasillo donde se almacena: ");
			scanf("%d", &prod[*pn].pasillo);
		}
		
		printf("Precio del producto: ");
		scanf("%f", &prod[*pn].precio);
		while(prod[*pn].precio<=0){
			printf("ERROR! Repita precio del producto: ");
			scanf("%f", &prod[*pn].precio);
		}
		
	 	printf("Cantidad ha introducir en el almacen: ");
		scanf("%d", &prod[*pn].cantidad);
		while(prod[*pn].cantidad<=0){
			printf("ERROR! Repita cantidad ha introducir en el almacen: ");
			scanf("%d", &prod[*pn].cantidad);
		}
		reponer(prod[*pn].pasillo, arduino, incomingData);		//empezamos funcion de reponer para añadir el nuevo producto
		(*pn)++;							//añadimos un nuevo producto al total
	}
	else
		printf ("Almacen completo\n");
	}

//funcion para eliminar productos del inventario
void eliminar(modeloprod prod[], int *pn, SerialPort *arduino, char *incomingData){			
	int i,numero;
	char opcion;
	
	listar(prod,*pn);							//Mostramos primero que productos hay en el almacen
	
	printf("Introduzca numero del producto a eliminar:	");
	scanf("%d",&numero);
	printf("\n");
	
	if (numero < *pn){
		printf("#%d/ nombre: %s - pasillo: %d - precio: %.2f - cantidad: %d\n\n",numero, prod[numero].nombre);
		printf("Desea eliminar este producto del registro(Y/N):	");
		fflush(stdin);
		scanf("%c",&opcion);
		fflush(stdin);
		if((opcion == 'Y') || (opcion == 'y')){
			for(i=numero; i<(*pn)-1; i++){
				prod[i] = prod[i+1];			//Movemos los productos posteriores a este para eliminar el elegido
			}
			reponer(prod[*pn].pasillo, arduino, incomingData);		//funcion de reponer para quitar producto
			(*pn)--;							//Reducimos cantidad toral de productos tras haber quitado uno del registro
			printf("Producto eliminado.\n");
		}
	}else
		printf("El producto no existe en el inventario.\n");
}

void editar(modeloprod prod[], int n, SerialPort *arduino, char *incomingData){
	float nprecio;
	int numero, opcion, nuevoPasillo, ncantidad;
	
	listar(prod, n);
	
	printf("Introduzca numero del producto a editar:	");
	scanf("%d",&numero);
	printf("\n");
	
	if (numero < n){
		printf("#%d/ nombre: %s - pasillo: %d - precio: %.2f - cantidad: %d\n", numero, prod[numero].nombre, prod[numero].pasillo, prod[numero].precio, prod[numero].cantidad);
		do{
			do{
				printf("\nPropiedad a editar\n");
				printf("===============\n");
				printf("1 - Nombre producto\n");
				printf("2 - Pasillo del producto\n");
				printf("3 - Precio del producto\n");
				printf("4 - Cantidad del producto\n");
				printf("5 - Cancelar\n");
				printf("Introduce propiedad: ");
				scanf("%d",&opcion);
				printf("\n");
			}while((opcion<1) || (opcion>5));
			switch(opcion){
				case 1: 
					printf("Introduce nuevo nombre del producto: ");
					fflush(stdin);
					gets(prod[numero].nombre);;
					break;
				case 2:
					printf("Introduce a que nuevo pasillo quieres mover el producto: ");
					scanf("%d", &nuevoPasillo);
					while(nuevoPasillo<=0 || nuevoPasillo == prod[numero].pasillo){
						printf("ERROR! Repita nuevo pasillo del producto: ");
						scanf("%d", &nuevoPasillo);
					}
					cambioPasillo(prod[numero].pasillo, nuevoPasillo, arduino, incomingData);	//Creamos funcion para mover el producto a un pasillo distino y llamamos a Arduino
					break;
				case 3:
					printf("Introduce nuevo precio del producto: ");
					scanf("%f", &nprecio);
					while(nprecio<=0 || nprecio == prod[numero].precio){
						printf("ERROR! Repite el nuevo precio del producto: ");
						scanf("%f", &nprecio);	
					}
					prod[numero].precio = nprecio;
					break;
				case 4:
					printf("Introduce cantidad a añadir al pasillo.");
					scanf("%d", &ncantidad);
					prod[numero].cantidad+=ncantidad;
					reponer(prod[numero].pasillo, arduino, incomingData);						//Creamos funcion para mover el producto a un pasillo distino y llamamos a Arduino
					break;
		}
	}while(opcion!=5);
	}else
		printf("El producto no existe en el inventario.\n");
}


//muestra en pantalla todos los productos
void listar(modeloprod prod[],int n){
	int i;
	printf("\nRegistro del almacen.\n");
	printf("------------------------\n");
	for(i=0;i<n;i++){
		printf("#%d/ nombre: %s - pasillo: %d - precio: %.2f - cantidad: %d\n", i, prod[i].nombre, prod[i].pasillo, prod[i].precio, prod[i].cantidad);
}
	printf("\n");
}

void buscar(modeloprod prod[],int n){
	int i, contador = 0;						//Iniciamos contador de caracteres que son coincidentes
	char cadena[L];
	
	printf("Introduce nombre para buscar producto: ");
	fflush(stdin);
	gets(cadena);
	printf("\n");
	for(i=0; i<n; i++)
		if(comparar(cadena, prod[i].nombre) == 1){
			contador++;
			printf("#%d/ nombre: %s - pasillo: %d - precio: %.2f - cantidad: %d\n", i, prod[i].nombre, prod[i].pasillo, prod[i].precio, prod[i].cantidad);
		}
		printf("\n");
	if(contador == 0)
		printf("No se han encontrado resultados.\n");
	else
		printf("%d resultados encontrados.\n", contador);
}

int comparar(char c1[], char c2[]){
	int bandera = 1, i;							//Suponemos al principio que son iguales
	for(i=0; c1[i] != '\0' && c2[i] != '\0' && bandera == 1; i++){		//comparamos cada caracter de ambas cadenas
		if(c1[i] != c2[i])
			bandera = 0;
	}
	return bandera;
}

void guardarinventario(modeloprod prod[],int n){
	int i;
	FILE *inventario;
	
	inventario = fopen ("inventario.txt","wt");
	
	if(inventario == NULL)
		printf("No se ha podido guardar el inventario.\n");
	else{
		for(i=0; i<n; i++){
		 	fputs(prod[i].nombre, inventario);
		 	fprintf(inventario,";");
           		fprintf(inventario,"%d;", prod[i].pasillo);
            		fprintf(inventario,"%.2f;", prod[i].precio);
            		fprintf(inventario,"%d\n", prod[i].cantidad);
		}	
		fclose(inventario);
	}
}

//declaracion funciones de comunicacion puerto serie
void Crear_Conexion(SerialPort *PuertoSerie, char *portName)
{

	PuertoSerie->connected = 0;
	PuertoSerie->portName = portName;
    PuertoSerie->handler = CreateFileA((portName),
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (PuertoSerie->handler == INVALID_HANDLE_VALUE)
	{
        if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
            printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
        }
    	else
        {
            printf("ERROR!!!");
        }
    }
    else{
        DCB dcbSerialParameters = {0};
        if(!GetCommState(PuertoSerie->handler, &dcbSerialParameters))
		{
            printf("failed to get current serial parameters");
        }
        else{
            dcbSerialParameters.BaudRate = CBR_9600;
            dcbSerialParameters.ByteSize = 8;
            dcbSerialParameters.StopBits = ONESTOPBIT;
            dcbSerialParameters.Parity = NOPARITY;
            dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

            if(!SetCommState(PuertoSerie->handler, &dcbSerialParameters)){
                printf("ALERT: could not set Serial port parameters\n");
            }
            else{
                PuertoSerie->connected = 1;
                PurgeComm(PuertoSerie->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
                Sleep(ARDUINO_WAIT_TIME);
            }
        }
    }
    return;
}

void CerrarConexion(SerialPort * PuertoSerie){
    if (PuertoSerie->connected){
        PuertoSerie->connected = 0;
        CloseHandle(PuertoSerie->handler);
    }
}

int readSerialPort(SerialPort * PuertoSerie,char *buffer, unsigned int buf_size){
    DWORD bytesRead;
    unsigned int toRead = 0;

    ClearCommError(PuertoSerie->handler, &PuertoSerie->errors, &PuertoSerie->status);

    if(PuertoSerie->status.cbInQue > 0){
        if(PuertoSerie->status.cbInQue > buf_size){
            toRead = buf_size;
        }
        else toRead = PuertoSerie->status.cbInQue;
    }

    memset(buffer, 0, buf_size);

    if(ReadFile(PuertoSerie->handler, buffer, toRead, &bytesRead, NULL))
	return bytesRead;
    return 0;
}

int writeSerialPort(SerialPort *PuertoSerie,char *buffer, unsigned int buf_size)
{
    DWORD bytesSend;

    if(!WriteFile(PuertoSerie->handler, (void*) buffer, buf_size, &bytesSend, 0)){
        ClearCommError(PuertoSerie->handler, &PuertoSerie->errors, &PuertoSerie->status);
        return 0;
    }
    else return 1;
}

int isConnected(SerialPort *PuertoSerie){
    if(!ClearCommError(PuertoSerie->handler, &PuertoSerie->errors, &PuertoSerie->status))
		PuertoSerie->connected = 0;
    return PuertoSerie->connected;
}

//funciones para el robot para que añada un nuevo producto
void reponer(int pasillo, SerialPort *arduino, char *incomingData){
	char sendData;
	int n;
	
	while(!isConnected(arduino)){
		Sleep(100);
		Crear_Conexion(arduino, arduino->portName);
	}
		
	while(isConnected(arduino)){
		printf("Conectado con Arduino en el puerto %s\n", arduino->portName);
		printf("Se ha mandado la orden de reponer al Arduino en pasillo %d...\n", pasillo);
		
		sendData = (char) pasillo;
		if(!writeSerialPort(arduino, &sendData, sizeof(char))){
			printf("Error escribiendo\n");
		}
		printf("Realizando reparto...\n");
		do{
			int n = readSerialPort(arduino, incomingData, 1);
		}while(*incomingData != pasillo);						//el programa en C no continua hasta que arduino no complete su orden
		
		printf("Reparto acabado.\n");
		return;
	}
	if(!isConnected(arduino))
		printf("ERROR de conexion con Arduino.");
}

void cambioPasillo(int pasillo,int nuevoPasillo,SerialPort *arduino,char *incomingData){
	char sendData;
	int n;
	
	while(!isConnected(arduino)){
		Sleep(100);
		Crear_Conexion(arduino, arduino->portName);
	}
		
	while(isConnected(arduino)){
		printf("Conectado con Arduino en el puerto %s\n", arduino->portName);
		printf("Se ha mandado la orden de traslado al Arduino en pasillo %d...\n", pasillo);
		
		sendData = (char)pasillo;
		if(!writeSerialPort(arduino, &sendData, sizeof(char))){
			printf("Error escribiendo\n");
		}
		printf("Empezando traslado...");
		do{
			int n = readSerialPort(arduino, incomingData, 1);
		}while(*incomingData != pasillo);					//ha llegado al pasillo empezamos cambio
		
		printf("Realizando traslado...");
		sendData = (char)nuevoPasillo;
		if(!writeSerialPort(arduino, &sendData, sizeof(char))){
			printf("Error escribiendo\n");
		}
		do{
			int n = readSerialPort(arduino, incomingData, 1);
		}while(*incomingData != nuevoPasillo);					//mover paquete recogido al nuevo pasillo
		
		printf("Traslado acabado.\n");
		return;
		}
		if(!isConnected(arduino))
			printf("ERROR de conexion con Arduino.");
}
