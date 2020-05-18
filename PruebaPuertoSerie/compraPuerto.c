#include <stdio.h>
#include <stdlib.h>
#define N 100
#define L 50

typedef struct{	
	char nombre[L];
	int pasillo;
	float precio;
	int cantidad;
}modeloprod;

typedef enum{
	inicio,
	comprobar,
	añadir,
}estados;

void cargarinventario(modeloprod [], int *);
void guardarinventario(modeloprod [], int);

void listar(modeloprod [], int);
float comprar(modeloprod [], int, float *);
int cambio(float);

int main(int argc, char *argv[]) {
	int n=0;
	float dinero;
	modeloprod producto[N];
	cargarinventario(producto, &n);		//Abre un fichero y lee toda la informacion de cada producto
	comprar(producto, n, &dinero);							//funcion de compra
    guardarinventario(producto, n);		//Guarda el inventario en un fichero de texto.
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

float comprar(modeloprod prod[], int n, int cod, float *dinero){
	float extra, introducido;
	int opcion;
	char option; //boton cancelar
	estado nextestado;
	nextestado = inicio;
	do{
		listar(prod, n);
		printf("\nIntroduce numero del producto.\n");
	}while((opcion<0) || (opcion>(n-1)));
	switch(nextestado){
		case inicio:
			if(prod.cantidad[] > 0){
				scanf("%f", &money);
				printf("Procesando compra...\n");
				next_state = check_price;
			}else{
				printf("No hay profucto %s\n", prod->name);
				break;
			}
		case check_price:
			switch(prod->price == money){
				case 1:
					printf("Gracias por su compra.(restantes %d)\n", prod->quantity-=1);
				break;
				case 0:
					if(prod->price > money){
						next_state = add_money;
					}else{
						printf("Gracias por su compra.(vueltas=%.2f, restantes %d)\n", money - prod->price, prod->quantity-=1);
						toss_coin(money - prod->price);
				break;
					}
				case add_money: //añadir dinero restante o cancelar compra
						do{
							if(more_money==0){
								printf("Compra cancelada(devuelve %.2f dineros)\n", money);
								toss_coin(money);
								return;
							}else{
								printf("Mas dinero...\n");
								scanf("%f", &more_money);
								money += more_money;
						}
						}while(money < prod->price);
						printf("Gracias por su compra.(vueltas=%.2f, restantes %d)", money - prod->price, prod->quantity-=1);
						toss_coin(money - prod->price);
						break;
			}
	}
}
