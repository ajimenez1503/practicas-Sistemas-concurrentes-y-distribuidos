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

FIFO (cola circular), se usan dos variables enteras no negativas:
	▸Indice en el vector de la primera celda ocupada. Esta variable se incrementa al leer.
	▸Indice en el vector de la primera celda libre. Esta variable se incrementa al escribir.

*/
// Compilar como  g++ productor-consumidor\(fifo\).cpp -lpthread
    

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
using namespace std ;

sem_t//semaforos
puede_escribir,// inicializado a 1
puede_leer,  // inicializado a 0
acceder_buffer; //exclusion mutua

int pos_r=0 ; // posicon donde leer
int pos_w=0; // posicion donde escribir
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

void* productor( void* p )
{
    for(unsigned i=0; i<num_item; i++)
    {

        sem_wait( &puede_escribir);//esperamos que pueda escribir y acceder al buffer
        sem_wait( &acceder_buffer) ;
        int dato=producir_dato();
        v[pos_w]=dato;
        //cout<<"La posicion write es: "<<pos_w<<endl;
        pos_w++;
        if(pos_w>=10)//incrementamos pos_W
            pos_w=pos_w%10;
        sem_post(&acceder_buffer) ;//permimitimos que pueda leer.
        sem_post(&puede_leer) ;
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

        dato=v[pos_r];
        //cout<<"La posicion read es: "<<pos_r<<endl;
        pos_r++;
        if(pos_r>=10)
            pos_r=pos_r%10;//decrementamos pos_r

        sem_post( &acceder_buffer) ;//permimitimos que pueda escribir
        sem_post( &puede_escribir) ;

        consumir_dato(dato);
    }
    return NULL ;
}


int main()
{
    pthread_t hebra_escritora, hebra_lectora ;//creamos una hebra para leer y otra para escribir
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
