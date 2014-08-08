/* (C) Programmed by:
   Antonio Jimenez Martínez
   Andrés Ortiz Corrales
   Mariano Palomo Villafranca

Practica Sistemas Sistemas Concurrentes y Districuidos: Practica 3
Version:0.1

EJERCICIO producto comsumidor

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

Se lanzaran 10 procesos y asumiremos que los procesos 0 . . . 4 son productores, el
proceso Buffer es el proceso 5 y el resto de procesos en el
comunicador universal (6 . . . 9) son consumidores.
Utilizanod para ello la interfaz de paso de mensajes MPI.

*/

//compilar:  mpicxx productorconsumidorfinal.cpp
//ejecutar:  mpirun -np 10 a.out

#include "mpi.h"
#include <iostream>
#include "math.h"
#include <time.h>
#include <stdlib.h>

//Etiqueta de enviar productor es 1 y taga de enviar peticion es 2.
//#define Productor 0// productor es 0 1 2 3 4
#define Buffer   5//buffer es 5
//#define Consumidor 2//consumidor es 6 7 8 9
#define ITERS 20
#define TAM 5

using namespace std;

void productor() {
    int value;
    for (unsigned int i=0; i<ITERS; i++) {
        //el productor envia el valor i a el buffer.
        value=i;
        cout<< "Productor produce valor "<<value<<endl<<flush;
        //sleep(rand() % 2 );
        MPI_Ssend( &value, 1, MPI_INT, Buffer, 1, MPI_COMM_WORLD );//Etiqueta del productor es 1
    }
}


void buffer() {//gestion del buffer
    int value[TAM], peticion, pos=0,rama;
    MPI_Status status;
    for (unsigned int i=0; i<ITERS*2; i++) {
        if (pos==0) rama=0;        //El consumidor no puede consumir
        else if (pos==TAM) rama=1; // El productor no puede producir
        else { //Ambas guardas son ciertas pos!=0 && pos!=TAM
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG==1) rama =0;//segun la etiqeta que tenga sera un rama o otra.
            else rama=1;
        }

        switch(rama) {
        case 0://recibe del productor
            MPI_Recv( &value[pos], 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,&status);
            //recibe de cualquiera de los productores pero con al etiqueta 1
            cout<< "Buffer recibe "<< value[pos] << " de Productor "<<endl<<flush;
            pos++;
            break;
        case 1://recibe peticion y envia valor
            MPI_Recv( &peticion, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD,&status);
            //recibe de cualquiera de los consumidores pero con al etiqueta 2
            MPI_Ssend( &value[pos-1], 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            //envia el que le envio la peiticion con etiquete 0
            cout<< "Buffer envía "<< value[pos-1] << " a Consumidor "<<endl<<flush;
            pos--;
            break;
        }
    }
}


void consumidor() {
    int value,peticion=1;
    float raiz;
    MPI_Status status;
    for (unsigned int i=0; i<ITERS; i++) {
        //el consumidor recibe un valor del buffer
        //primero el consumidor envia peticion para recibir
        MPI_Ssend(&peticion, 1, MPI_INT, Buffer, 2, MPI_COMM_WORLD); //etiquea del consumidor es 2
        //despues recibe valor
        MPI_Recv(&value, 1,MPI_INT, Buffer, 0, MPI_COMM_WORLD,&status );//etiquete de envio de buffer a consumidor es 0
        cout<< "Consumidor recibe valor "<<value<<" de Buffer "<<endl<<flush;
        //sleep(rand() % 2 );
        raiz=sqrt(value);
    }
}


int main(int argc, char *argv[]) {
    int rank,size;
    cout.flush();
    MPI_Init( &argc, &argv ); //iniciamos MPI
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    /* Inicializa la semilla aleatoria */
    srand ( time(NULL) );
    if (size!=10)//error si el numero de procesos no es 10
    {
        cout<< "El numero de procesos debe ser 3 "<<endl;
        return 0;
    }
    if (rank >=0 && rank<=4) productor();//procesos 0 1 3 4 son del productor
    else if (rank==5) buffer();//proceso 5 es de gestion del buffer
    else consumidor();//proceso 6 7 8 9 son del consumidor
    MPI_Finalize( );//finalizamos MPI
    return 0;
}

