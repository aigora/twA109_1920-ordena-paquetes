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
//Las propiedades del producto son su nombre, el pasillo donde esta guardado, precio y cantidad en el almacen

int menu(void);

void nuevo(modeloprod [], int*);			//Crea un nuevo producto
void eliminar(modeloprod [], int *);		//Elimina un producto existente
void editar(modeloprod [], int);			//Edita nombre, precio, pasillo y cantidad de un producto existente
void listar(modeloprod [], int);			//Muestra el inventario completo
void buscar(modeloprod [], int);			//Busca un producto por su nombre
int comparar(char [], char []);


void cargarinventario(modeloprod [], int *);
void guardarinventario(modeloprod [], int);

int main(int argc, char *argv[]) {
	int n=0, opcion;
	modeloprod producto[N];
	cargarinventario(producto, &n);		//Abre un fichero y lee toda la informacion de cada producto
	do{
		opcion=menu();
		switch(opcion){
			case 1: 
				nuevo(producto, &n);
				break;
			case 2: 
				eliminar(producto, &n);	
				break;
			case 3:
				editar(producto, n);
				break;
			case 4:
				listar(producto, n);
				break;
			case 5:
				buscar(producto, n);
				break;
		}
	}while(opcion!=6);
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

void nuevo(modeloprod prod[], int *pn){			//falta funcione para llamar arduino
	if(*pn<N){
		printf("Nombre: ");
		fflush(stdin);
		gets(prod[*pn].nombre);
		printf("Pasillo donde se almacena: ");
		scanf("%d", &prod[*pn].pasillo);
		printf("Precio del producto: ");
		scanf("%f", &prod[*pn].precio);
		printf("Cantidad que hay actualmente en el almacen: ");
		scanf("%d", &prod[*pn].cantidad);
		(*pn) = (*pn) + 1;
	}
	else
		printf ("Almacen completo\n");
}

void eliminar(modeloprod prod[], int *pn){			//falta funcione para llamar arduino
	int i,numero;
	char opcion;
	
	listar(prod,*pn);							//Mostramos primero que prodcutos hay en el almacen
	
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
			*pn=*pn-1;							//Reducimos cantidad toral de productos tras haber quitado uno del registro
			printf("Producto eliminado.\n");
		}
	}else
		printf("El producto no existe en el inventario.\n");
}

void editar(modeloprod prod[], int n){
	float nprecio;
	int numero, opcion;
	
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
				/*case 2: 
					cambiopasillo(producto, n);										//Creamos funcion para mover el producto a un pasillo distino y llamamos a Arduino
					break;*/
				case 3:
					printf("Introduce nuevo precio del producto: ");
					scanf("%f", &nprecio);
					prod[numero].precio = nprecio;
					break;
				/*case 4:
					reponer(producto, n);											//Creamos funcion para mover el producto a un pasillo distino y llamamos a Arduino
					break;*/
		}
	}while(opcion!=5);
	}else
		printf("El producto no existe en el inventario.\n");
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
	for(i=0; c1[i] != '\0' && c2[i] != '\0' && bandera == 1; i++){
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
		 	fprintf(inventario,"\n");
            fprintf(inventario,"%d\n", prod[i].pasillo);
            fprintf(inventario,"%.2f\n", prod[i].precio);
            fprintf(inventario,"%d\n", prod[i].cantidad);
		}	
		fclose(inventario);
	}
}
