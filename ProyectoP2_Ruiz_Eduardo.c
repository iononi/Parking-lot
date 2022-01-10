#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct car
{
	char marca[30];
	char nombre[30];
	char placa[11];
	int modelo;
	int id_user;
};

struct user
{
	char nombre[120];
	char tel[11];
	int id;
	int cant_auto;
	struct car coche;
};

struct ePila
{
	int tope;
	struct car mi_pila[3];
};

struct eCola
{
	struct ePila tope;
	struct eCola *sig;
};

struct eLista
{
	int dia;
	struct eCola *frente, *final;
	struct eLista *sig;
};

int alta_user(FILE **user, FILE **car, struct user *usuario);
int es_pila_vacia(struct ePila *pila);
int es_pila_llena(struct ePila *pila);
int push(struct ePila *pila, struct car coche);
struct car *pop(struct ePila *pila);
struct eLista *buscar_lista(struct eLista *inicio, int dia);
struct eLista *crear_lista(int dia);
int insertar_lista(struct eLista **inicio, int dia);
struct eCola *crear_cola();
int insertar_cola_en_lista(struct eLista *inicio);
int user_esp(FILE **user, FILE **car, struct user *p, int id);
void leer_usuarios(FILE **user, struct user *q);
struct eCola *lote_disponible(struct eCola *frente, struct ePila *pila);
int estacionar(int id, char *nombre, int dia, FILE **car, struct user *p, struct eLista *inicio);
void print_cola_pila(struct eCola *frente, struct ePila *pila, struct ePila **pila_aux);
void print_lista(struct eLista *inicio, struct ePila *pila_aux, int cont);
int encontro_en_pila(int i, char *nombre, struct ePila *pila, struct ePila *pila_aux);
int del_car(int id, char *nombre, struct eLista *inicio, struct eCola *frente, struct ePila *pila, struct ePila *pila_aux);
struct car *buscar_en_pila(struct ePila *pila, struct ePila *pila_aux, char *placa);
struct car *buscar_carro(struct eLista *inicio, struct eCola *frente, char *placa, struct ePila *pila_aux);



int main(int argc, char const *argv[])
{
	struct eLista *inicio = NULL;
	struct user usuario;
	struct ePila Pila_aux;
	FILE *user, *car;
	char aux[30];

	int op, id, dia, cont = 1;

	Pila_aux.tope = -1;

	for (int i = 7; i >= 1; i-=2)
	{
		if (!insertar_lista(&inicio, i))
			break;
	}

	do
	{
		printf("-==== MENU DE OPCIONES ====-\n");
		printf("[1] Alta de usuario.\n");//funciona
		printf("[2] Estacionar carro.\n");//funciona
		printf("[3] Desaparcar carro.\n");//funciona, pero mueve los coches de un piso a otro
		printf("[4] Localizar carro.\n");
		printf("[5] Mostrar estacionamiento.\n"); //funciona
		printf("[6] Mostrar todos los usuarios registrados.\n");
		printf("[7] Mostrar datos de usuario especifico.\n");//funciona
		printf("[8] Salir.\n\n");
		printf("Opcion >> ");
		scanf("%d", &op);

		switch(op)
		{
			case 1:	if (alta_user(&user, &car, &usuario))
						printf("\nHa sido dado de alta satisfactoriamente.\n\n");
					else
						printf("\nLo sentimos, ha ocurrido un error al darlo de alta. Por favor, intentelo de nuevo.\n\n");
			break;
			case 2:	printf("\nIngrese su ID: ");
					scanf("%d", &id);
					printf("Ingrese el nombre del automovil: "); fflush(stdin);
					fgets(aux, 30, stdin); strtok(aux, "\n");
					printf("Ingrese la cantidad de dias que guardara su automovil: ");
					scanf("%d", &dia);
					if (estacionar(id, aux, dia, &car, &usuario, inicio))
						printf("\nSu automovil ha sido estacionado.\n\n");
					else
						printf("\nHa ocurrido un error al estacionar su automovil. Por favor, intentelo nuevamente.\n\n");
			break;
			case 3:	printf("\nIngrese su ID: ");
					scanf("%d", &id);
					printf("Ingrese el nombre de su automovil: "); fflush(stdin);
					fgets(aux, 30, stdin); strtok(aux, "\n");
					if (!del_car(id, aux, inicio, inicio->frente, &(inicio->frente->tope), &Pila_aux))
						printf("\nNo se ha podido desaparcar su carro.\n\n");
					else
						printf("\nSu carro ha sido desaparcado.\n\n");
			break;
			case 4: printf("\nIngrese las placas de su automovil: "); fflush(stdin);
					fgets(aux, 11, stdin); strtok(aux, "\n");
					struct car *coche = buscar_carro(inicio, inicio->frente, aux, &Pila_aux);
					if (!coche)
						printf("\nNo se ha encontrado ningun coche con las placas '%s'.\n\n", aux);
					else
					{
						printf("\nEstos son los datos del coche\n\n");
						printf("Marca: %s\n", coche->marca);
						printf("Nombre: %s\n", coche->nombre);
						printf("Placas: %s\n", coche->placa);
						printf("Modelo: %d\n", coche->modelo);
						printf("Codigo del propietario: %d\n\n", coche->id_user);
					}
			break;
			case 5:	print_lista(inicio, &Pila_aux, cont);
			break;
			case 6:	leer_usuarios(&user, &usuario);
			break;
			case 7:	printf("\nIngrese su ID: ");
					scanf("%d", &id);
					switch(user_esp(&user, &car, &usuario, id))
					{
						case -1: printf("\nERROR: no se pudo abrir el archivo de usuarios.\n\n");
						break;
						case -2: printf("\nERROR: no se pudo abrir el archivo de carros.\n\n");
						break;
						case 0: printf("\nNo se encontro el usuario con el ID '%d'.\n\n", id);
						break;
					}
			break;
			case 8:	printf("\nGracias por usar mi programa.\n\n");
			break;
			default: printf("\nIngrese una opcion valida, por favor.\n\n"); fflush(stdin);
			break;
		}
	} while (op != 8);

	return 0;
}

int alta_user(FILE **user, FILE **car, struct user *usuario)
{
	int id, aux;
	if (!(*user = fopen("Usuarios.bin", "ab")))
		return 0;

	srand(time(NULL));
	printf("Ingrese su nombre completo: "); fflush(stdin);
	fgets(usuario->nombre, 120, stdin); strtok(usuario->nombre, "\n");
	printf("Ingrese su numero de celular (10 digitos MAX.): "); fflush(stdin);
	fgets(usuario->tel, 11, stdin); strtok(usuario->tel, "\n");
	usuario->id = rand() % 4501 + 1; //generamos numeros aleatorios entre 1 y 4500 para asginar como ID
	printf("Ingrese la cantidad de carros que posee: ");
	scanf("%d", &usuario->cant_auto);
	fwrite(usuario, sizeof(struct user), 1, *user);
	printf("\nSu codigo es: %d. Conservelo bien, pues es su identificador ante el sistema.\n", usuario->id);
	fclose(*user);

	if (!(*car = fopen("Carros.bin", "ab")))
		return 0;

	for (int i = 0; i < usuario->cant_auto; ++i)
	{
		printf("\nAutomovil %d\n\n", i + 1);
		printf("Ingrese la marca del automovil: "); fflush(stdin);
		fgets(usuario->coche.marca, 30, stdin); strtok(usuario->coche.marca, "\n");
		printf("Ingrese el nombre del automovil: "); fflush(stdin);
		fgets(usuario->coche.nombre, 30, stdin); strtok(usuario->coche.nombre, "\n");
		printf("Ingrese las placas del automovil, en formato XXX-XXX-N: "); fflush(stdin);
		fgets(usuario->coche.placa, 11, stdin); strtok(usuario->coche.placa, "\n");
		printf("Ingrese el modelo del automovil: ");
		scanf("%d", &(usuario->coche).modelo);
		usuario->coche.id_user = usuario->id;
		fwrite(&usuario->coche, sizeof(struct car), 1, *car);
	}
	fclose(*car);
	return 1;
}

int es_pila_vacia(struct ePila *pila)
{
	if (pila->tope < 0)
		return 1;
	return 0;
}

int es_pila_llena(struct ePila *pila)
{
	if (pila->tope == 2)
		return 1;
	return 0;
}

int push(struct ePila *pila, struct car coche)
{
	if (es_pila_llena(pila))
		return 0;
	(pila->tope)++;
	pila->mi_pila[pila->tope] = coche;
	return 1;
}

struct car *pop(struct ePila *pila)
{
	if (es_pila_vacia(pila))
		return NULL;
	struct car *temp = &pila->mi_pila[pila->tope];
	(pila->tope)--;
	return temp;
}

struct eLista *buscar_lista(struct eLista *inicio, int dia)
{
	if (inicio == NULL)
		return NULL;

	if (dia <= inicio->dia)
		return inicio;

	return buscar_lista(inicio->sig, dia);
}

struct eLista *crear_lista(int dia)
{
	struct eLista *nuevo = (struct eLista *)malloc(sizeof(struct eLista));
	if (!nuevo)
		return NULL;

	nuevo->dia = dia;
	nuevo->frente = NULL;
	nuevo->final = NULL;

	return nuevo;
}

int insertar_lista(struct eLista **inicio, int dia)
{
	struct eLista *elem = crear_lista(dia);
	if (!elem)
		return 0;

	elem->sig = *inicio;
	*inicio = elem;

	for(int i = 0; i < 1; ++i)
	{
		if (!insertar_cola_en_lista(*inicio))
			break;
		else
			continue;
	}

	return 1;
}

struct eCola *crear_cola()
{
	struct eCola *nuevo = (struct eCola *)malloc(sizeof(struct eCola));
	if (!nuevo)
		return NULL;

	struct ePila pila;
	pila.tope = -1;

	nuevo->tope = pila;
	nuevo->sig = NULL;

	return nuevo;
}

int insertar_cola_en_lista(struct eLista *inicio)
{
	struct eCola *elem = crear_cola();
	if (!elem)
		return 0;

	if (inicio->frente == NULL)
		inicio->frente = elem;
	else
		inicio->final->sig = elem;

	inicio->final = elem;

	return 1;
}

int user_esp(FILE **user, FILE **car, struct user *p, int id)
{
	int existe = 0;
	if (!(*user = fopen("Usuarios.bin", "rb")))
		return -1; //no se pudo abrir el archivo

	fread(p, sizeof(struct user), 1, *user);
	while(!feof(*user))
	{
		if (p->id == id)
		{
			existe = 1;
			printf("\nNombre: %s\n", p->nombre);
			printf("Numero de celular: %s\n", p->tel);
			printf("Codigo: %d\n", p->id);
			printf("Cantidad de automoviles: %d\n", p->cant_auto);
			break;
		}
		fread(p, sizeof(struct user), 1, *user);
	}
	fclose(*user);

	if (existe == 1)
	{
		if (!(*car = fopen("Carros.bin", "rb")))
			return -2;
		
		fread(&p->coche, sizeof(struct car), 1, *car);
		printf("\nEstos son los automoviles que posee\n\n");
		while(!feof(*car))
		{
			if (p->coche.id_user == id)
			{
				printf("Marca: %s\n", p->coche.marca);
				printf("Nombre: %s\n", p->coche.nombre);
				printf("Placa: %s\n", p->coche.placa);
				printf("Modelo: %d\n\n", p->coche.modelo);
			}
			fread(&p->coche, sizeof(struct car), 1, *car);
		}
		fclose(*car);
		return 1;
	}
	else
		return 0;
}

void leer_usuarios(FILE **user, struct user *q)
{
	if (!(*user = fopen("Usuarios.bin", "rb")))
		return;

	fread(q, sizeof(struct user), 1, *user);
	while(!feof(*user))
	{
		printf("\nNombre: %s\n", q->nombre);
		printf("Telefono celular: %s\n", q->tel);
		printf("ID: %d\n", q->id);
		printf("Cantidad de autos: %d\n\n", q->cant_auto);
		fread(q, sizeof(struct user), 1, *user);
	}
	fclose(*user);
	return;
}

struct eCola *lote_disponible(struct eCola *frente, struct ePila *pila)
{
	if (frente == NULL)
		return NULL;

	if (!es_pila_llena(pila))
		return frente;
	
	return lote_disponible(frente->sig, &(frente->sig)->tope);  
}

int estacionar(int id, char *nombre, int dia, FILE **car, struct user *p, struct eLista *inicio)
{
	int existe = 0;
	if (!(*car = fopen("Carros.bin", "rb")))
		return 0;

	fread(&p->coche, sizeof(struct car), 1, *car);
	while(!feof(*car))
	{
		if (p->coche.id_user == id && strcmp(p->coche.nombre, nombre) == 0) //se deben cumplir ambos, pues puede haber coches iguales de distintos duenios
		{
			existe = 1;
			break;
		}
		fread(&p->coche, sizeof(struct car), 1, *car);
	}

	if (existe == 1)
	{
		struct eLista *aux = buscar_lista(inicio, dia);
		if (!aux)
			return 0;

		//verificar que las pilas de dicha lista tengan espacio
		struct eCola *lote = lote_disponible(aux->frente, &(aux->frente)->tope);
		if (!lote)
		{
			fclose(*car);
			return 0;
		}
		
		if (!push(&lote->tope, p->coche))
		{
			fclose(*car);
			return 0;
		}

		fclose(*car);
		return 1;
	}
	else
		return 0;
}

void print_cola_pila(struct eCola *frente, struct ePila *pila, struct ePila **pila_aux)
{
	if (frente == NULL)
		return;
	
	while(!es_pila_vacia(pila) && !es_pila_llena(*pila_aux))
	{
		struct car *temp = pop(pila);
		printf("   %s\n", temp->nombre);
		printf("     %d\n", temp->id_user);
		printf("     %s\n", temp->placa);
		push(*pila_aux, *temp);
	}

	while(!es_pila_vacia(*pila_aux) && !es_pila_llena(pila))
	{
		struct car *dato = pop(*pila_aux);
		
		push(pila, *dato);
	}

	printf("    -----------\n");

	print_cola_pila(frente->sig, &(frente->sig->tope), &*pila_aux);
}

void print_lista(struct eLista *inicio, struct ePila *pila_aux, int cont)
{
	if (inicio == NULL)
	{
		printf("\n");
		return;
	}

	printf("\nPiso %d\n\n", cont);

	print_cola_pila(inicio->frente, &(inicio->frente)->tope, &pila_aux);

	print_lista(inicio->sig, pila_aux, ++cont);
}

int encontro_en_pila(int id, char *nombre, struct ePila *pila, struct ePila *pila_aux)
{
	if (es_pila_vacia(pila))
		return 0;

	while(!es_pila_vacia(pila) && !es_pila_llena(pila_aux))
	{
		struct car *car = pop(pila);
		if (car->id_user == id && strcmp(car->nombre, nombre) == 0)
		{
			while(!es_pila_vacia(pila_aux) && !es_pila_llena(pila))
			{
				struct car *dato = pop(pila_aux);
				push(pila, *dato);
			}

			return 1;
		}
		else
			push(pila_aux, *car);
	}
	return 0;
}

int del_car(int id, char *nombre, struct eLista *inicio, struct eCola *frente, struct ePila *pila, struct ePila *pila_aux)
{
	if (inicio == NULL)
		return 0;

	if (frente == NULL)
		return del_car(id, nombre, inicio->sig, inicio->sig->frente, &(inicio->sig->frente->tope), pila_aux);

	if (encontro_en_pila(id, nombre, pila, pila_aux))
		return 1;

	return del_car(id, nombre, inicio, frente->sig, &(frente->sig->tope), pila_aux);
}

struct car *buscar_en_pila(struct ePila *pila, struct ePila *pila_aux, char *placa)
{
	
	if (es_pila_vacia(pila))
		return NULL;

	struct car *temp = NULL;

	while(!es_pila_vacia(pila) && !es_pila_llena(pila_aux))
	{
		struct car *carrito = pop(pila);
		if (strcmp(carrito->placa, placa) == 0)
		{
			push(pila, *carrito);
			while(!es_pila_vacia(pila_aux) && !es_pila_llena(pila))
			{
				temp = pop(pila_aux);
				push(pila, *temp);
			}
			return carrito;
		}
		else
			push(pila_aux, *carrito);
	}

	return NULL;
}

struct car *buscar_carro(struct eLista *inicio, struct eCola *frente, char *placa, struct ePila *pila_aux)
{
	if (inicio == NULL)
		return NULL;

	if (frente == NULL)
		return buscar_carro(inicio->sig, inicio->sig->frente, placa, pila_aux);

	struct car *carro = buscar_en_pila(&frente->tope, pila_aux, placa);
	if (!carro)
		return buscar_carro(inicio, frente->sig, placa, pila_aux);
	if (inicio->dia == 1)
		printf("\nUbicacion: Piso 1\n");
	if (inicio->dia > 1 && inicio->dia <= 3)	
		printf("\nUbicacion: Piso 2\n");
	if (inicio->dia > 3 && inicio->dia <= 5)
		printf("\nUbicacion: Piso 3\n");
	if (inicio->dia > 5)
		printf("\nUbicacion: Piso 4\n");
	return carro;
}