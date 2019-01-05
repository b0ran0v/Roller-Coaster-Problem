#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int n, C, leftPassengers;
int count = 0;
sem_t Queue, Boarding, Riding, Unloading, Check_in; //semaphores

//passenger function
void* passenger(void* p){
    sem_wait(&Queue);
    sem_wait(&Check_in);

    count++;
    leftPassengers--;
    usleep(1000*1000);
    printf("One person sat into car.\n");
    if(count==C)
        sem_post(&Boarding);
    sem_post(&Check_in);
    sem_wait(&Riding);
    sem_post(&Unloading);
    printf("One person got out from car.\n");
}

//car function
void* car(void *p){ 
    while(leftPassengers>C){
        count = 0;
        printf("Car is waiting for passengers...\n");
        for(int i=0;i<C;i++){
            sem_post(&Queue);          
        }
        sem_wait(&Boarding);
        printf("All passengers are in car. Car is moved on.\n");
        usleep(2000*1000);
        printf("Car made one full rotation. Passengers started landing...\n");
        usleep(1000*1000);
        for(int i=0;i<C;i++){
            sem_post(&Riding);
            sem_wait(&Unloading);
            usleep(1000*1000);
        }
        printf("All passengers are off.\n");
    }
    printf("Not enough passengers for next ride.\n");
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    n = atoi(argv[1]);
    C = atoi(argv[2]);
    leftPassengers = n;

    sem_init(&Queue,0,0);
    sem_init(&Boarding,0,0);
    sem_init(&Riding,0,0);
    sem_init(&Unloading,0,0);
    sem_init(&Check_in,0,1);

    pthread_t car_thread;
    pthread_t passengers[n];

    //creating passenger threads
    for(int i=0;i<n;i++)
        pthread_create(&passengers[i],NULL,passenger,NULL);

    //creating car thread
    pthread_create(&car_thread,NULL,car,NULL);  
    pthread_join(car_thread,NULL);
}
