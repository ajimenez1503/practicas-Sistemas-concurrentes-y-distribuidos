/* (C) Programmed by:
   Antonio Jimenez Martínez
   Andrés Ortiz Corrales
   Mariano Palomo Villafranca

Practica Sistemas Sistemas Concurrentes y Districuidos: Practica 1
Version:0.1

EJERCICIO productor-consumidor

El productor calcula o produce una secuencia de items
de datos (uno a uno), y el consumidor lee o consume dichos items
(tambien uno a uno).
El tiempo que se tarda en producir un item de datos puede ser variable y en general distinto al que se tarda en consumirlo.

▸ Suele ser conveniente implementar el productor y el consumidor
  como dos hebras independientes, ya que esto permite tener
  ocupadas las CPUs disponibles el maximo de tiempo,
▸ Las esperas asociadas a la lectura y la escritura pueden empeorar
   la eficiencia. Esto puede mejorarse usando un vector que pueda
  contener muchos items de datos producidos y pendientes de leer.

La implementacion debe asegurar que :
▸ cada item producido es leido (ningun item se pierde)
▸ ningun item se lee mas de una vez. Lo cual implica:
	▸ el productor tendr que esperar antes de poder escribir en el vector
   cuando haya creado un item pero el vector este completamente
   ocupado por items pendientes de leer
	▸ el consumidor debe esperar cuando vaya a leer un item del vector
    pero dicho vector no contenga ningun item pendiente de leer.

▸ LIFO (pila acotada), se usa una unica variable entera no negativa:
	▸Indice en el vector de la primera celda libre. Esta variable se incrementa al escribir, y se
	decrementa al leer.

*/

// Compilar como  g++ productor-consumidor\(lifo\).cpp -lpthread
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
using namespace std ;

sem_t
puede_escribir,// inicializado a 1
puede_leer,  // inicializado a 0
acceder_buffer; //para poder cambiar pos

int pos=0 ; // posicon donde escribir o leer
int v[10] ; // buffer
int num_item=50;
static int contador=0;

int producir_dato()
{
    return contador ++;
}
void consumir_dato(int dato)
{
    cout<<"Dato recibidos: "<<dato<<endl;
}
//############################################

void* productor( void* p )
{

    for(unsigned i=0; i<num_item; i++)
    {
        int dato=producir_dato();
        sem_wait( &puede_escribir);//esperamos que pueda escribir y acceder al buffer
        sem_wait( &acceder_buffer) ;
        v[pos]=dato;
        pos++;//incrementamos pos
        sem_post( &acceder_buffer) ;
        sem_post( &puede_leer) ;//permimitimos que pueda leer.
    }
    return NULL;
}


void* consumidor( void* p )
{

    for(unsigned i=0; i<num_item; i++)
    {
        int dato;
        sem_wait( &puede_leer) ;//esperamos que pueda leer y acceder al buffer
        sem_wait( &acceder_buffer) ;
        pos--;//decrementamos pos
        dato=v[pos];
        sem_post( &acceder_buffer) ;
        sem_post( &puede_escribir) ;//permimitimos que pueda escribir

        consumir_dato(dato);
    }
    return NULL ;
}


int main()
{
    pthread_t hebra_escritora, hebra_lectora ;
    sem_init( &puede_escribir, 0, 10); // inicialmente se puede escribir
    sem_init( &puede_leer,0, 0); // inicialmente no se puede leer
    sem_init( &acceder_buffer,0,1);

	//Creamos las hebras
    pthread_create( &hebra_escritora, NULL,  productor, NULL );
    pthread_create( &hebra_lectora,NULL,consumidor,NULL );
	
	//lanzamos las hebras
    pthread_join( hebra_escritora, NULL ) ;
    pthread_join( hebra_lectora,NULL ) ;

	//Destruimos los semaforos
    sem_destroy( &puede_escribir );
    sem_destroy( &puede_leer );
    sem_destroy( &acceder_buffer);
}
