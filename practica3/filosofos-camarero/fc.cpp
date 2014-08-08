/* (C) Programmed by:
   Antonio Jimenez Martínez
   Andrés Ortiz Corrales
   Mariano Palomo Villafranca

Practica Sistemas Sistemas Concurrentes y Districuidos: Practica 3
Version:0.1

EJERCICIO filosofos camarero

Se pretende realizar una implementacion del problema de la cena
de los filosofos en MPI utilizando el siguiente esquema:

Tenemos 5 procesos filosofos, 5 procesos tenedor y un camarero centrar que permite sentar en la mesa (11 procesos en
total). Supondremos que los procs. Filosofos se identifican con
numero pares y los tenedores con numeros impares. El camarero se identifica con el identificador 10.


-En principio, cada filosofo realiza repetidamente la siguiente
secuencia de acciones:
	-Pensar (sleep aleatorio).
	-Sentarse
	-Tomar los tenedores (primero el tenedor izquierdo y despues el derecho).
	-Comer (sleep aleatorio).
	-Soltar tenedores (en el mismo orden).
	-Levantarse

Las acciones pensar y comer pueden implementarse mediante un
mensaje por pantalla seguido de un retardo durante un tiempo
aleatorio. Las acciones de tomar tenedores y soltar tenedores
pueden implementarse enviando mensajes de peticion y de
liberacion a los procesos tenedor situados a ambos lados de cada filosofo.

Cada filosofo pedira permiso para sentarse o levantarse enviando
un mensaje al camarero, el cual llevara una cuenta del numero de
filosofos que hay sentados a la mesa en cada momento.

-Un tenedor solamente podra ser asignado a uno de los dos filosofos
que realicen la peticion. Hasta que el tenedor no sea liberado no
podra ser asignado de nuevo. Cada proceso tenedor tendra que
ejecutar repetidamente la siguiente secuencia de acciones:
	-Esperar mensajes de peticion de tenedor y recibir uno.
	-Esperar mensaje de liberacion.

*/
//compilar:  mpicxx fc.cpp
//ejecutar:  mpirun -np 11 a.out

#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#define procesocamarero 10
#define numprocesos 10
//coger etiqueta 1, para soltar etiqueta 0
//etiqueta para sentarse 2, para levantarse 3
using namespace std;

void Filosofo(int id, int nprocesos);
void Tenedor (int id, int nprocesos);
void camarero ();


int main(int argc,char** argv ) {
    int rank,size;
    srand(time(0));
    MPI_Init( &argc, &argv );//iniciamos MPI
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if( size!=11) {//error si el numero de procesos no es 11
        if(rank == 0)
            cout<<"El numero de procesos debe ser 11"<<endl<<flush;
        MPI_Finalize( );
        return 0;
    }
    if (rank%2==0 && rank!=procesocamarero)
        Filosofo(rank,numprocesos); // Los pares son Filosofos
    else {
        if(rank!=procesocamarero)
            Tenedor(rank,numprocesos);  // Los impares diferentes de 10 son Tenedores
        else
            camarero();
    }
    MPI_Finalize( );//finalizamos MPI
    return 0;
}

void Filosofo(int id, int nprocesos) {
    MPI_Status status;
    int izq=(id+1)%nprocesos;
    int der=(id-1+nprocesos)%nprocesos;
    int value=1;
    while(1) {
        //filosofo solicita sentarse
        cout<<"Filosofo "<<id<< " solicita sentarse"<<endl<<flush;
        MPI_Send( &value, 1, MPI_INT, procesocamarero, 2, MPI_COMM_WORLD);
        MPI_Recv( &value, 1, MPI_INT, procesocamarero, 2, MPI_COMM_WORLD,&status);//el filosofo espera

        //Solicita tenedor izquierdo
        cout<<"Filosofo "<<id<< " solicita tenedor izq ..."<<izq <<endl<<flush;
        MPI_Ssend( &value, 1, MPI_INT, izq, 1, MPI_COMM_WORLD);

        //Solicita tenedor derecho
        cout<<"Filosofo "<<id<< " solicita tenedor der ..."<<der <<endl<<flush;
        MPI_Ssend( &value, 1, MPI_INT, der, 1, MPI_COMM_WORLD);

        cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
        //sleep((rand() % 3)+1);  //comiendo
        //suelta el tenedor izquierdo
        cout<<"Filosofo "<<id<< " suelta tenedor izq ..."<<izq <<endl<<flush;
        MPI_Ssend( &value, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
        //suelta el tenedor derecho
        cout<<"Filosofo "<<id<< " suelta tenedor der ..."<<der <<endl<<flush;
        MPI_Ssend( &value, 1, MPI_INT, der, 0, MPI_COMM_WORLD);

        //folosofo se levanta
        cout<<"Filosofo "<<id<< "se LEVANTA"<<endl<<flush;
        MPI_Ssend( &value, 1, MPI_INT,  procesocamarero, 3, MPI_COMM_WORLD);

        cout<<"Filosofo "<<id<< " PENSANDO"<<endl<<flush;
        //sleep((rand()%3)+1 );//pensando
    }
}

void Tenedor(int id, int nprocesos) {
    int buf;
    MPI_Status status;
    int Filo;
    while(1) {
        // Espera un peticion desde cualquier filosofo vecino ...
        MPI_Recv( &buf, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,&status);
        // Recibe la peticion del filosofo ...
        Filo=status.MPI_SOURCE;
        cout<<"el  filosofo  "<<Filo<<" coge el tenedor "<<id<<endl<<flush;
        // Espera a que el filosofo suelte el tenedor...
        MPI_Recv( &buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD,&status);
        cout<<"el  filosofo  "<<Filo<<" suelta el tenedor "<<id<<endl<<flush;
    }
}


void camarero () {
    //con el contador comprobamos el numero de filosofos sentados
    int contador=0,value,s;//source
    MPI_Status status;
    while(1) {
        status.MPI_TAG=0;
        if(contador<4)//el maximo de filosofos comiendo es 4
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);//puede sentarse o levnatarse
        else
            MPI_Probe(MPI_ANY_SOURCE,3, MPI_COMM_WORLD, &status);//solo puede levantars

        if (status.MPI_TAG==2) {//comprobamos etiqueta, etiqeta 2 sentarse
            s=status.MPI_SOURCE;
            MPI_Recv( &value, 1, MPI_INT, s, 2, MPI_COMM_WORLD,&status);
            contador++;
            MPI_Send( &value, 1, MPI_INT, s, 2, MPI_COMM_WORLD);
            cout<< "el filosofo "<< s << " se sienta. Hay "<<contador<<" filosofos sentados. "<<endl<<flush;

        }
        if (status.MPI_TAG==3) {//comprobamos etiqueta, etiqueta 3 se sienta.
            s=status.MPI_SOURCE;
            MPI_Recv( &value, 1, MPI_INT, s, 3, MPI_COMM_WORLD,&status);
            contador--;
            cout<< "el filosofo "<< s << " se levanta. Hay "<<contador<<" filosofos sentados. "<<endl<<flush;

        }
    }
}
