/*
* 	Authors: Juan San Miguel
*/

// Productor Consumidor solucionado con semáforos
// Asignatura: Sistemas Distribuidos y Concurrentes
// Universidad Rey Juan Carlos

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <semaphore.h>

#define MAX_BUFFER 10
#define MAX_DATA_PRODUCER 50
#define SLEEP_TIME 50000


sem_t sem_mutex;
sem_t sem_full;
sem_t sem_empty;

int n_elems;
int buffer[MAX_BUFFER];


void producer (void) {

  int pos = 0 ;
  int aux_pos = 0;

  for (int i=0; i<MAX_DATA_PRODUCER; i++) {

    sem_wait(&sem_empty); // espera un hueco libre
    sem_wait(&sem_mutex); // entra a la región crítica

    buffer[pos] = i;
    aux_pos = pos;
    pos = (pos + 1) % MAX_BUFFER;
    n_elems ++;

    sem_post(&sem_mutex); // sale de la región crítica
    sem_post(&sem_full); // hay un elemento más para consumir

    printf("Producer[%d] %d \n", aux_pos, i);
    usleep(SLEEP_TIME);
  }
  pthread_exit(0);
}

void consumer (void) {

  int data = 0;
  int pos = 0;
  int aux_pos = 0;

  for (int i=0; i<MAX_DATA_PRODUCER; i++) {

    sem_wait(&sem_full); // espera un elemento disponible 
    sem_wait(&sem_mutex); // entra a la región crítica 

    data = buffer[pos];
    aux_pos = pos;
    pos = (pos + 1) % MAX_BUFFER;
    n_elems--;

    sem_post(&sem_mutex);
    sem_post(&sem_empty);  // hay un hueco libre más

    printf("Consumer[%d] %d \n", aux_pos, data);
    usleep(SLEEP_TIME*3);
  }
  pthread_exit(0);
}




int main(void)
{

  pthread_t th_consumer, th_producer;

  sem_init(&sem_mutex, 0, 1);
  sem_init(&sem_full, 0, 0);
  sem_init(&sem_empty, 0, MAX_BUFFER);


  pthread_create(&th_producer, NULL, (void *)&producer, NULL);
  pthread_create(&th_consumer, NULL, (void *)&consumer, NULL);

  pthread_join(th_consumer, NULL);
  pthread_join(th_producer, NULL);
 
  sem_destroy(&sem_mutex);
  sem_destroy(&sem_full);
  sem_destroy(&sem_empty);

  return 0;

}