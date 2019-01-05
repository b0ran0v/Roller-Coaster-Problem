import sys
import threading
import time
import os

n = int(sys.argv[1]) # number of passengers
C = int(sys.argv[2]) # number of seats in car
leftPassengers = n # variable for counting rest passengers in queue


class Semaphore(threading.Semaphore):
    """
        Inherited class with changed methods from
        release, acquire into wait, signal respectively
    """

    wait = threading.Semaphore.acquire
    signal = threading.Semaphore.release

count = 0 # variable for counting passenger in car
Queue = Semaphore(0) # passengers waiting for a ride
Boarding = Semaphore(0) # controls the boarding process(for the last passenger)
Riding = Semaphore(0) # keep passengers on the car so 
Unloading = Semaphore(0) # controlling the unloading process
Check_in = Semaphore(1) # check in counter (checks for entering passengers one by one)

def Passenger():
    """
        Passenger function for Passenger Thread
    """
    global leftPassengers
    global C
    global count
    global Queue
    global Boarding
    global Riding
    global Unloading
    global Check_in
    Queue.wait() # join the queue for a ride
    Check_in.wait() # time to check in
    count+=1
    leftPassengers=leftPassengers-1
    print('One person sat into car.')
    # if he is the last one to be on board, boarding completes and the car is full
    if(count==C):
        Boarding.signal()
    Check_in.signal() # allow next passenger to check in
    Riding.wait() # I am riding in the car
    Unloading.signal() # // get off the car
    print('One person got out from car.')


def Car():
    """
        Car function for Car Thread
    """
    global leftPassengers
    global C
    global count
    global Queue
    global Boarding
    global Riding
    global Unloading
    global Check_in
    while(leftPassengers>C): # while amount of left passengers is greater than capacity of car
        count = 0
        print('Car is waiting for passengers...')
        time.sleep(1)
        for i in range(C):
            Queue.signal() # allow passengers to check in
            time.sleep(1)
        Boarding.wait() #waits for last passenger
        print('All passengers are in car. Car is moved on.')
        time.sleep(2)
        print('Car made one full rotation. Passengers started landing...')
        time.sleep(1)
        for i in range(C):
            Riding.signal() # release a passenger
            Unloading.wait() # wait until passenger is off
            time.sleep(1)
        print('All passengers are off.')
    print('Not enough passengers for next ride.')
    os._exit(1)


# creating and starting passengers thread
for i in range(n):
    p = threading.Thread(target=Passenger)
    p.start()

# creating and starting car thread
car = threading.Thread(target=Car)
car.start()

car.join()
