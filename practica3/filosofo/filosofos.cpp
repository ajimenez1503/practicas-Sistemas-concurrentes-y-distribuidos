/* (C) Programmed by:
   Antonio Jimenez Martínez
   Andrés Ortiz Corrales
   Mariano Palomo Villafranca

Practica Sistemas Sistemas Concurrentes y Districuidos: Practica 3
Version:0.1

EJERCICIO filosofos

Se pretende realizar una implementacion del problema de la cena
de los filosofos en MPI utilizando el siguiente esquema:

Tenemos 5 procesos filosofos y 5 procesos tenedor (10 procesos en
total). Supondremos que los procs. filosofos se identifican con
numero pares y los tenedores con numeros impares. 

-En principio, cada filosofo realiza repetidamente la siguiente
secuencia de acciones:
	-Pensar (sleep aleatorio).
	-Tomar los tenedores (primero el tenedor izquierdo y despues el derecho).
	-Comer (sleep aleatorio).
	-Soltar tenedores (en el mismo orden).
Las acciones pensar y comer pueden implementarse mediante un
mensaje por pantalla seguido de un retardo durante un tiempo
aleatorio. Las acciones de tomar tenedores y soltar tenedores
pueden implementarse enviando mensajes de peticion y de
liberacion a los procesos tenedor situados a ambos lados de cada filosofo.

-Un tenedor solamente podra ser asignado a uno de los dos filosofos
que realicen la peticion. Hasta que el tenedor no sea liberado no
podra ser asignado de nuevo. Cada proceso tenedor tendra que
ejecutar repetidamente la siguiente secuencia de acciones:
	-Esperar mensajes de peticion de tenedor y recibir uno.
	-Esperar mensaje de liberacion.

*/
//compilar:  mpicxx filosofos.cpp
//ejecutar:  mpirun -np 10 a.out

#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>

//coger etiqueta 1, para soltar etiqueta 0
using namespace std;

void Filosofo(int id, int nprocesos);
void Tenedor (int id, int nprocesos);

int main(int argc,char** argv ) {
    int rank,size;
    srand(time(0));
    MPI_Init( &argc, &argv );//iniciamos MPI
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if( size!=10) {//error si el numero de procesos no es 10
        if(rank == 0)
            cout<<"El numero de procesos debe ser 10"<<endl<<flush;
        MPI_Finalize( );
        return 0;
    }
    if ((rank%2) == 0)
        Filosofo(rank,size); // Los procesos pares son Filosofos
    else Tenedor(rank,size);  // Los procesos impares son Tenedores
    MPI_Finalize( );//finalizamos MPI
    return 0;
}

void Filosofo(int id, int nprocesos) {
    int izq=(id+1)%nprocesos;//calculamos el identificador del tenedor izq 
    int der=(id-1+nprocesos)%nprocesos;//calculamos el identificador del tenedor derecho
    int value=1;
    while(1) {
        if(id==0) {
            //Solicita tenedor derecho
            cout<<"Filosofo "<<id<< " coge tenedor der ..."<<der <<endl<<flush;
            MPI_Ssend( &value, 1, MPI_INT, der, 1, MPI_COMM_WORLD);
			
			 //Solicita tenedor izquierdo
            cout<<"Filosofo "<<id<< " solicita tenedor izq ..."<<izq <<endl<<flush;
            MPI_Ssend( &value, 1, MPI_INT, izq, 1, MPI_COMM_WORLD);

        }
        else {
            //Solicita tenedor izquierdo
            cout<<"Filosofo "<<id<< " solicita tenedor izq ..."<<izq <<endl<<flush;
            MPI_Ssend( &value, 1, MPI_INT, izq, 1, MPI_COMM_WORLD);

            //Solicita tenedor derecho
            cout<<"Filosofo "<<id<< " coge tenedor der ..."<<der <<endl<<flush;
            MPI_Ssend( &value, 1, MPI_INT, der, 1, MPI_COMM_WORLD);

        }
        cout<<"Filosofo "<<id<< " COMIENDO"<<endl<<flush;
       // sleep((rand() % 3)+1);  //comiendo
        //suelta el tenedor izquierdo
        cout<<"Filosofo "<<id<< " suelta tenedor izq ..."<<izq <<endl<<flush;
        MPI_Ssend( &value, 1, MPI_INT, izq, 0, MPI_COMM_WORLD);
        //suelta el tenedor derecho
        cout<<"Filosofo "<<id<< " suelta tenedor der ..."<<der <<endl<<flush;
        MPI_Ssend( &value, 1, MPI_INT, der, 0, MPI_COMM_WORLD);
        cout<<"Filosofo "<<id<< " PENSANDO"<<endl<<flush;
       // sleep((rand()%3)+1 );//pensando
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
        Filo=status.MPI_SOURCE;//calcula id filosofo a partir de status
        cout<<"el  filosofo  "<<Filo<<" coge el tenedor "<<id<<endl<<flush;
        // Espera a que el filosofo suelte el tenedor...
        MPI_Recv( &buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD,&status);
        cout<<"el  filosofo  "<<Filo<<" suelta el tenedor "<<id<<endl<<flush;
    }
}
