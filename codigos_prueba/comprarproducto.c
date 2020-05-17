#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 100
#define L 50

typedef struct{	
	char nombre[L];
	int pasillo;
	float precio;
	int cantidad;
}modeloprod;


//Los nombres de las diferentes etapas del proceso de compra
typedef enum{
	inicio,
	comprobar,			//comprueba si hay la cantidad necearia de dinero
	anadir,				//si falta por añadir dinero pide más
}estado;

void cargarinventario(modeloprod [], int *);
void guardarinventario(modeloprod [], int);

void listar(modeloprod [], int);
float comprar(modeloprod [], int, float *);
int cambio(float);				//Devuelve el cambio de la compra

int main(int argc, char *argv[]) {
	int n=0;
	float dinero;
	modeloprod producto[N];
	cargarinventario(producto, &n);									//Abre un fichero y lee toda la informacion de cada producto
	comprar(producto, n, &dinero);							//funcion de compra
    guardarinventario(producto, n);									//Guarda el nuevo inventario tras la compra en un fichero de texto.
	return 0;
}


void cargarinventario(modeloprod prod[],int *pn){
	int n,posicion;
	FILE *inventario;
    char intro;
	
	n=0;												//Iniciamos un contador de productos
	
	inventario = fopen("inventario.txt","rt");
	
	if(inventario == NULL)
		printf("El fichero no existe\n");
	else{
		fgets(prod[n].nombre,L,inventario);				//Lee el primer nombre de dimension L
        while(!feof(inventario)){
          posicion = strlen(prod[n].nombre);			//Longitud del nombre
          prod[n].nombre[posicion-1]='\0';				//Sustituir \n por \0
		  fscanf(inventario,"\n", &intro);
		  fscanf(inventario,"%d",&prod[n].pasillo);		//Lee el pasillo, cantidad y precio    
          fscanf(inventario,"%f",&prod[n].precio);		
          fscanf(inventario,"%d",&prod[n].cantidad);	
          fscanf(inventario,"%c",&intro);  
          n++;											//Cuenta los productos que hay en el fichero
          fgets(prod[n].nombre,L,inventario);
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
		 	fprintf(inventario,"\n");
            fprintf(inventario,"%d\n", prod[i].pasillo);
            fprintf(inventario,"%.2f\n", prod[i].precio);
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

float comprar(modeloprod prod[], int n, float *dinero){			//faltan las funciones para llamar Arduino
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
					printf("Gracias por su compra.(Cantidad restante %d)\n", prod[num].cantidad-=1);
				break;
				case 0:													//no lo es comprueba cuanto dinero falta o sobra
					if(prod[num].precio > introducido){
						nextestado = anadir;							//si falta salta al estado dode pide mas dinero
					}else{
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
