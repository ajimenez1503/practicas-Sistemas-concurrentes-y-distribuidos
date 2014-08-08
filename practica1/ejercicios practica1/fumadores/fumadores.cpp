/* (C) Programmed by:
   Antonio Jimenez Martínez
   Andrés Ortiz Corrales
   Mariano Palomo Villafranca

Practica Sistemas Sistemas Concurrentes y Districuidos: Practica 1
Version:0.1

EJERCICIO Fumadores
Considerar un estanco en el que hay tres fumadores y un estanquero.
-Cada fumador representa una hebra que realiza una actividad fumar.
-Cada fumador debe esperar antes de fumar a que se den ciertas
condiciones (tener suministros para fumar), que dependen de la
actividad del proceso que representa al estanquero.
-El estanquero produce suministros para que los fumadores puedan
fumar.
-Para asegurar concurrencia real, es importante tener en cuenta que
se debe permitir que varios fumadores fumen simultaneamente.
-Antes de fumar es necesario tres ingredientes: tabaco, papel y cerillas.
-Uno de los fumadores tiene solamente papel, otro tiene solamente
tabaco, y el otro tiene solamente cerillas.
-El estanquero coloca aleatoriamente dos ingredientes diferentes de
los tres que se necesitan para hacer un cigarro, desbloquea al
fumador que tiene el tercer ingrediente y despues se bloquea.
-El fumador desbloqueado toma los dos ingredientes del mostrador,
desbloquea al estanquero para que pueda seguir sirviendo
ingredientes y fuma.
-El estanquero, cuando se desbloquea, vuelve a poner dos
ingredientes aleatorios en el mostrador, y se repite el ciclo.

Problema para 3 fumadores.

*/
// Compilar como  g++ fumadores.cpp -lpthread


#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <time.h> // incluye ”time(....)”
#include <unistd.h> // incluye ”sleep(....)”
#include <stdlib.h> // incluye ”rand(...)"
using namespace std;

int fumada1=0,fumada2=0,fumada3=0;
sem_t fu1,fu2,fu3,estanco; // Semaforos
int ingredientes=-1; // 0 es papel y cerillas, 1 es papel y tabaco, 2 es tabaco y cerillas;

// **************************************************** //
// Funcion que simula la accion de fumar                //
// como un retardo aleatorio de la hebra                //
// **************************************************** //
void fumar()
{
    //cout<<"fumar:"<<fumada<<endl;
    //sleep(rand()%5);
}

void* estanquero( void* p)
{
    for(int i=0; i<300; i++) {
        sem_wait(&estanco);//esperamos que el estanquero pueda poner ingrediente.
        ingredientes=rand()%3;
        if(ingredientes==0)
            sem_post( &fu1) ;//permitimos que fume el fumador1
        if(ingredientes==1)
            sem_post( &fu2) ;//permitimos que fume el fumador2
        if(ingredientes==2)
            sem_post( &fu3) ;//permitimos que fume el fumador3
    }
}

void* fumador1( void* p)
{
    for(;;)
    {
        sem_wait(&fu1);//espera q este disponible el semaforo del fumador 1
        fumada1++;
        cout<<"f1:"<<fumada1<<endl;
        sem_post(&estanco);//permitimos que el estanquero pueda poner ingrediente.
        fumar();
    }

}
void* fumador2( void* p)
{

    for(;;)
    {
        sem_wait(&fu2);//espera q este disponible el semaforo del fumador 2
        fumada2++;
        cout<<"f2:"<<fumada2<<endl;
        sem_post(&estanco);//permitimos que el estanquero pueda poner ingrediente.
        fumar();
    }

}
void* fumador3( void* p)
{

    for(;;)
    {
        sem_wait(&fu3);//espera q este disponible el semaforo del fumador 3
        fumada3++;
        cout<<"f3:"<<fumada3<<endl;
        sem_post(&estanco);//permitimos que el estanquero pueda poner ingrediente.
        fumar();
    }

}
int main()
{
    srand ( time(NULL) ); // inicializa la semilla aleatoria
    pthread_t f1,f2,f3,est ;//inicializamos las hebras
    sem_init( &fu1, 0, 0);//inicialmente no puede fumar
    sem_init( &fu2,0, 0);//inicialmente no puede fumar
    sem_init( &fu3,0, 0);//inicialmente no puede fumar
    sem_init( &estanco,0,1);//inicialmente el estanquero puede poner ingrediente

	//creamos las hebras
    pthread_create( &est, NULL,estanquero, NULL );
    pthread_create( &f1, NULL,fumador1  , NULL );
    pthread_create( &f2, NULL,fumador2  , NULL );
    pthread_create( &f1, NULL,fumador3  , NULL );

	//lanzamos las hebras
    pthread_join(est, NULL );

	//destruimos los semaforos
    sem_destroy( &fu1);
    sem_destroy( &fu2);
    sem_destroy( &fu3);
    sem_destroy( &estanco);

	//mostramos las fumadas total
    cout<<endl<<endl<<fumada1+fumada2+fumada3<<endl;
}

