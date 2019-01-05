#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <time.h>
#include <stdbool.h>

int n, C, temp;

int passengersInCar = 0;
bool carIsWaiting = false;

pthread_mutex_t rollerCoasterLock;
pthread_cond_t carLoad;
pthread_cond_t carUnload;
pthread_cond_t carFull;

void takeRide(){
    pthread_mutex_lock(&rollerCoasterLock);
    while(!carIsWaiting || passengersInCar==C)
        pthread_cond_wait(&carLoad, &rollerCoasterLock);
    usleep(1000*1000);
    printf("One person sat into car.\n");
    passengersInCar++;
    temp--;
    if (passengersInCar == C) {
		//Notifies the car that it is full:
        pthread_cond_signal(&carFull);
	}
    //Waits until the car gets full:
    pthread_cond_wait(&carFull,&rollerCoasterLock);	
    //Waits the end of the ride:
    pthread_cond_wait(&carUnload,&rollerCoasterLock);
    usleep(1000*1000);
    printf("One person got out from car.\n");
    pthread_mutex_unlock(&rollerCoasterLock);	
}

void load(){
    pthread_mutex_lock(&rollerCoasterLock);
    //Waits for the car if it is riding:
    while (passengersInCar > 0) {
        pthread_cond_wait(&carUnload, &rollerCoasterLock);
	}
    carIsWaiting = true;
    //Notifies the passengers the car is ready to load them:
    pthread_cond_broadcast(&carLoad);
    printf("Car is waiting for passengers...\n");

    pthread_cond_wait(&carFull,&rollerCoasterLock);	
    carIsWaiting = false;

    pthread_cond_broadcast(&carFull);

    printf("All passengers are in car. Car is moved on.\n");
    pthread_mutex_unlock(&rollerCoasterLock);
}

void unload(){
    pthread_mutex_lock(&rollerCoasterLock);

    printf("Car made one full rotation. Passengers started landing...\n");
    //Unload the passengers:
    pthread_cond_broadcast(&carUnload);
    passengersInCar = 0;
    pthread_mutex_unlock(&rollerCoasterLock);
}

void* passenger(void* p){
    takeRide();
    pthread_exit(NULL);
}
void* car(void* p){
    while(temp>=C){
        load();
        usleep(1000*1000);
        unload();
    }
    usleep(5000*1000);
    printf("Not enough passengers for next ride.\n");
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    n = atoi(argv[1]);
    C = atoi(argv[2]);
    temp = n;

    pthread_mutex_init(&rollerCoasterLock, NULL);
    pthread_cond_init(&carLoad, NULL);
    pthread_cond_init(&carUnload, NULL);
    pthread_cond_init(&carFull, NULL);

    pthread_t carThread;
    pthread_t passengers[n];

    for(int i=0;i<n;i++)
        pthread_create(&passengers[i],NULL,passenger,NULL);
    
    pthread_create(&carThread,NULL,car,NULL);
    pthread_join(carThread,NULL);
}
