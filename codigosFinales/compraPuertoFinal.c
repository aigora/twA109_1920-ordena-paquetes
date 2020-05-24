#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255
#define N 5000
#define L 50

typedef struct{	
	char nombre[L];
	int pasillo;
	float precio;
	int cantidad;
}modeloprod;

typedef struct{
	int cantidad; 
	float euros;
}monedas;				//las monedas tienen un valor y una cantidad


typedef struct
{
    HANDLE handler;
    int connected;
    COMSTAT status;
    DWORD errors;
    char * portName;
} SerialPort;

//Los nombres de las diferentes etapas del proceso de compra
typedef enum{
	inicio,
	comprobar,			//comprueba si hay la cantidad necearia de dinero
	anadir,				//si falta por añadir dinero pide más
}estado;

void *cargarinventario(modeloprod [], int *);
void guardarinventario(modeloprod [], int);

void listar(modeloprod [], int);
float comprar(modeloprod [], monedas [], int, int, float *, SerialPort *arduino, char *incomingData);

void cargarmonedas(monedas [], int *);
int cambio(float, monedas [], int );				//Devuelve el cambio de la compra
void guardarmonedas(monedas [], int);

void Crear_Conexion(SerialPort * PuertoSerie,char *portName);
void CerrarConexion(SerialPort * PuertoSerie);

int readSerialPort(SerialPort *PuertoSerie,char *buffer, unsigned int buf_size);
int writeSerialPort(SerialPort *PuertoSerie,char *buffer, unsigned int buf_size);
int isConnected(SerialPort *);

void compraRobot(int pasillo, SerialPort *arduino, char *incomingData);

int main(int argc, char *argv[]) {
	int n=0, num=0;
	float dinero;
	
	modeloprod producto[N];
	monedas euros[N];
	
	SerialPort *arduino;
	
	char *portName = "\\\\.\\COM3";
	char incomingData[MAX_DATA_LENGTH];
	arduino = (SerialPort*)malloc(sizeof(SerialPort));
	Crear_Conexion(arduino, portName);
	
	cargarinventario(producto, &n);									//Abre un fichero y lee toda la informacion de cada producto
	cargarmonedas(euros, &num);										//abre el fichero con la infor,acion de las monedas y su cantidad
	comprar(producto, euros, n, num, &dinero, arduino, incomingData);							//funcion de compra
    guardarinventario(producto, n);								//Guarda el nuevo inventario tras la compra en un fichero de texto.
	guardarmonedas(euros, num);									//guarda los cambios en las canidades de las monedas
	return 0;
}



void *cargarinventario(modeloprod prod[],int *pn){
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
			prod[n].precio = atof(ptrtoken);			//convierte la cadena en float
			ptrtoken = strtok(NULL, ";");
			prod[n].cantidad = atoi(ptrtoken);
			n++;
		}
		fclose(inventario);
	}
    *pn=n;
}

void guardarinventario(modeloprod prod[],int n){
	int i;
	FILE *inventario;
	
	inventario = fopen ("inventario.txt","wt");			//guarda las modificaciones
	
	if(inventario == NULL)
		printf("No se ha podido guardar el inventario.\n");
	else{
		for(i=0; i<n; i++){						//como sigue la estructura de archivo csv hay que volver a escribir los ;
		 	fputs(prod[i].nombre, inventario);
		 	fprintf(inventario,";");
           		fprintf(inventario,"%d;", prod[i].pasillo);
            		fprintf(inventario,"%.2f;", prod[i].precio);
            		fprintf(inventario,"%d\n", prod[i].cantidad);
		}	
		fclose(inventario);
	}
}

void listar(modeloprod prod[],int n){
	int i;
	printf("\nRegistro del almacen.\n");
	printf("------------------------\n");
	for(i=0;i<n;i++){
		printf("#%d/ nombre: %s - precio: %.2f - cantidad: %d\n", i, prod[i].nombre, prod[i].precio, prod[i].cantidad);
}
	printf("\n");
}

float comprar(modeloprod prod[], monedas euros[],int n, int numMoneda, float *dinero, SerialPort *arduino, char *incomingData){			//faltan las funciones para llamar Arduino
	float extra, introducido, suma=0;										//Introducido es el dinero que mete al principio el usuario, si falta dinero pide mas y se guarda en extra, y suma es la suma de ambos
	int num;
	
	estado nextestado;
	nextestado = inicio;													//primer caso del switch es el inicio
	
	listar(prod, n);
	printf("\nIntroduce numero del producto.\n");
	scanf("%d", &num);														//usuario elige producto
	
	while((num<0) || (num>(n-1))){											//no puede introducir negativos u opcion incorrecta
		printf("ERROR");
		listar(prod, n);
		printf("\nVuelve a introduceir numero del producto.\n");
		scanf("%d", &num);
	}
	switch(nextestado){
		case inicio:											//inicio de la compra
			if(prod[num].cantidad > 0){							//comprueba si hay existencias
				printf("\nIntroduzca dinero...");				
				scanf("%f", &introducido);						//si hay introduce dinero
				printf("Procesando compra...\n");
				*dinero = introducido;
				nextestado = comprobar;							//pasa siguiente estado donde se comprueba la cantidad introducida
			}else{
				printf("No hay profucto %s\n", prod[num].nombre);
				break;
			}
		case comprobar:
			switch(prod[num].precio == introducido){			
				case 1:													//si lo introducido es igual al precio del producto no hay problema
					compraRobot(prod[num].pasillo, arduino, incomingData);
					printf("Gracias por su compra.(Cantidad restante %d)\n", prod[num].cantidad-=1);
				break;
				case 0:													//no lo es comprueba cuanto dinero falta o sobra
					if(prod[num].precio > introducido){
						nextestado = anadir;							//si falta salta al estado dode pide mas dinero
					}else{
						compraRobot(prod[num].pasillo, arduino, incomingData);
						printf("Gracias por su compra.(vueltas=%.2f, Cantidad restante %d)\n", (introducido-prod[num].precio), prod[num].cantidad-=1);
						cambio((introducido - prod[num].precio), euros, numMoneda);			//si sobra da el cambio
				break;
					}
				case anadir:
						printf("\nAñade mas dinero...	");
						scanf("%f", &extra);
						suma += extra + introducido;					//el usuario tiene la oportunidad de aportar mas dinero, puede cancelar la compra introduciendo nada o un numero negativo
						if(extra == 0 || extra < 0){
								printf("Compra cancelada.(devuelve %.2f)\n", introducido);
								cambio(introducido, euros, numMoneda);					//compra cancelada devuelve lo itroducido en monedas
								return;
							}
						while(suma < prod[num].precio){
							if(extra>0 && suma<=prod[num].precio){		//si sigue faltando pide más dinero
								printf("Mas dinero...	");
								scanf("%f", &extra);
								suma += extra;
						}
						}
						compraRobot(prod[num].pasillo, arduino, incomingData);
						printf("Gracias por su compra.(vueltas=%.2f, restantes %d)", (suma-prod[num].precio), prod[num].cantidad-=1);
						cambio((suma-prod[num].precio), euros, numMoneda);
						break;
			}
	}
}


//definicion de las funciones para estbalecer comunicacion puerto serie
void Crear_Conexion(SerialPort *PuertoSerie, char *portName){

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


//funcion que permite recoger con el reobot el producto comprado
void compraRobot(int pasillo, SerialPort *arduino, char *incomingData){
	char sendData;
	int n, inicio = -1;
	
	while(!isConnected(arduino)){
		Sleep(100);
		Crear_Conexion(arduino, arduino->portName);
	}
		
	while(isConnected(arduino)){
		sendData = (char) pasillo;					//manda donde se encuentra el producto
		if(!writeSerialPort(arduino, &sendData, sizeof(char))){
			printf("Error mandando orden.\n");
		}
		printf("Realizando compra...\n");
		do{
			int n = readSerialPort(arduino, incomingData, 1);
		}while(*incomingData != pasillo);			//el programa en C no continua hasta que el robot vuelva al inicio
		
		printf("Compra acabada.\n");
		return;
	}
	if(!isConnected(arduino))
		printf("ERROR de conexion.");
}

void cargarmonedas(monedas euros[],int *pn){
	int n;
	FILE *monedasEuro;
	char *ptrtoken = malloc(sizeof(char)), Cadena_aux[L];
	
	n=0;									
	
	monedasEuro = fopen("monedas.txt","rt");
	
	if(monedasEuro == NULL)
		printf("El fichero no existe\n");
	else{
		while(fgets(Cadena_aux, L, monedasEuro) != NULL){		//lee primera linea del fichero y empieza un bucle while que no acaba hasta llegar al final del fichero
			ptrtoken = strtok(Cadena_aux, ";");			//divide la primera linea por el seprador del csv el ;
			euros[n].euros = atof(ptrtoken);
			ptrtoken = strtok(NULL, ";");
			euros[n].cantidad = atoi(ptrtoken);
			n++;							//no hemos definido la dimension del fichero por si se quieren meter billetes
		}
		fclose(monedasEuro);
		}
    *pn=n;
}
	
int cambio(float dinero, monedas euros[], int n){								//devuelve el cambio en monedas de euro y centimos
	int i, finalmoneda[]={0, 0, 0, 0, 0, 0, 0, 0};
	
	dinero=dinero*100;
	
	for(i=0; dinero!=0 && i<n; i++){											//devuelve el cambio con el menor numero posible de monedas
		if((euros[i].euros * 100) <= dinero && euros[i].cantidad > 0){			//solo devuelve monedas en el cambio si las hay
			finalmoneda[i] = dinero/(euros[i].euros * 100);
			dinero-=(euros[i].euros * 100)*finalmoneda[i];
	}
	}
	for(i=0; i<n; i++){
		printf("%.2f*%d	",euros[i].euros ,finalmoneda[i]);
		euros[i].cantidad-=finalmoneda[i];
}
	guardarmonedas(euros, n);
}

void guardarmonedas(monedas euros[], int n){
	int i;
	FILE *monedasFinal;
	
	monedasFinal = fopen ("monedas.txt","wt");
	
	if(monedasFinal == NULL)
		printf("No se ha podido guardar el inventario.\n");
	else{
		for(i=0; i<n; i++){
            fprintf(monedasFinal,"%.2f;", euros[i].euros);
            fprintf(monedasFinal,"%d\n", euros[i].cantidad);
		}	
		fclose(monedasFinal);
	}
}
