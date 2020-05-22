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
float comprar(modeloprod [], int, float *, SerialPort *arduino, char *incomingData);
int cambio(float);				//Devuelve el cambio de la compra

void Crear_Conexion(SerialPort * PuertoSerie,char *portName);
void CerrarConexion(SerialPort * PuertoSerie);

int readSerialPort(SerialPort *PuertoSerie,char *buffer, unsigned int buf_size);
int writeSerialPort(SerialPort *PuertoSerie,char *buffer, unsigned int buf_size);
int isConnected(SerialPort *);

void compraRobot(int pasillo, SerialPort *arduino, char *incomingData);

int main(int argc, char *argv[]) {
	int n=0;
	float dinero;
	modeloprod producto[N];
	
	SerialPort *arduino;
	char *portName = "\\\\.\\COM3";
	char incomingData[MAX_DATA_LENGTH];
	arduino = (SerialPort*)malloc(sizeof(SerialPort));
	Crear_Conexion(arduino, portName);
	
	cargarinventario(producto, &n);									//Abre un fichero y lee toda la informacion de cada producto
	comprar(producto, n, &dinero, arduino, incomingData);							//funcion de compra
    guardarinventario(producto, n);									//Guarda el nuevo inventario tras la compra en un fichero de texto.
	return 0;
}



void *cargarinventario(modeloprod prod[],int *pn){
	int n,posicion;
	FILE *inventario;
    char intro;
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
			prod[n].pasillo = atoi(ptrtoken);
			ptrtoken = strtok(NULL, ";");
			prod[n].precio = atof(ptrtoken);
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

void listar(modeloprod prod[],int n){
	int i;
	printf("\nRegistro del almacen.\n");
	printf("------------------------\n");
	for(i=0;i<n;i++){
		printf("#%d/ nombre: %s - pasillo: %d - precio: %.2f - cantidad: %d\n", i, prod[i].nombre, prod[i].pasillo, prod[i].precio, prod[i].cantidad);
}
	printf("\n");
}

float comprar(modeloprod prod[], int n, float *dinero, SerialPort *arduino, char *incomingData){			//faltan las funciones para llamar Arduino
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
						cambio(introducido - prod[num].precio);			//si sobra da el cambio
				break;
					}
				case anadir:
						printf("\nAñade mas dinero...	");
						scanf("%f", &extra);
						suma += extra + introducido;					//el usuario tiene la oportunidad de aportar mas dinero, puede cancelar la compra introduciendo nada o un numero negativo
						if(extra == 0 || extra < 0){
								printf("Compra cancelada.(devuelve %.2f)\n", introducido);
								cambio(introducido);					//compra cancelada devuelve lo itroducido en monedas
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
						cambio(suma-prod[num].precio);
						break;
			}
	}
}

int cambio(float dinero){								//devuelve el cambio en monedas de euro o centimos
	int i, centimo[]={200, 100, 50, 20, 10, 5, 2, 1}, finalmoneda[]={0, 0, 0, 0, 0, 0, 0, 0};
	float euros[]={2, 1, 0.5, 0.2, 0.1, 0.05, 0.02, 0.01};
	dinero=dinero*100;
	for(i=0; dinero!=0 && i<8; i++){
		if(centimo[i]<=dinero){
			finalmoneda[i]=dinero/centimo[i];
			dinero-=centimo[i]*finalmoneda[i];
	}
	}
	for(i=0; i<8; i++){
		printf("%.2f*%d	",euros[i] ,finalmoneda[i]);
}
}


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

void compraRobot(int pasillo, SerialPort *arduino, char *incomingData){
	char sendData;
	int n;
	
	while(!isConnected(arduino)){
		Sleep(100);
		Crear_Conexion(arduino, arduino->portName);
	}
		
	while(isConnected(arduino)){
		sendData = (char) pasillo;
		if(!writeSerialPort(arduino, &sendData, sizeof(char))){
			printf("Error mandando orden.\n");
		}
		printf("Realizando compra...\n");
		do{
			int n = readSerialPort(arduino, incomingData, 1);
		}while(n==0 || *incomingData != pasillo);						//el programa en C no continua hasta que arduino no complete su orden
		
		printf("Compra acabada.\n");
		return;
	}
	if(!isConnected(arduino))
		printf("ERROR de conexion.");
}
