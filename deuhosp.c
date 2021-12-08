#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>

//GLOBAL VARIABLES
int REGISTRATION_SIZE = 10; //max registration desks available.
int RESTROOM_SIZE = 10; //max restrooms that are available.
int CAFE_NUMBER = 10; //max cashiers in cafe that are available.
int GP_NUMBER = 10; //max General Practitioner that are available.
int PHARMACY_NUMBER = 10; //max cashiers in pharmacy that are available.
int BLOOD_LAB_NUMBER = 10; //max assistants in blood lab that are available.

//NUMBER OF OR, SURGOENS AND NURSES AVAILABLE
int OR_NUMBER = 10; 
int SURGEON_NUMBER = 30; 
int NURSE_NUMBER = 30; 

//NUMBER OF SUREONS AND NURSES REQUIRED FOR AN OPERATION
int SURGEON_LIMIT = 5;
int NURSE_LIMIT = 5;

//NUMBER OF PATIENTS REQUIRING MEDICAL ATTENTION AND HOSPITAL WALLET
int PATIENT_NUMBER = 1000;
int HOSPITAL_WALLET = 0;

//time required to execute actions(IN MILLISECONDS)
int WAIT_TIME = 100;
int REGISTRATION_TIME = 100;
int GP_TIME = 100;
int PHARMACY_TIME = 100;
int BLOOD_LAB_TIME = 100;
int SURGERY_TIME = 100;
int CAFE_TIME = 100;
int RESTROOM_TIME = 100;

//COSTS
int REGISTRATION_COST = 100; //fixed
int BLOOD_LAB_COST = 200; //fixed
int PHARMACY_COST = 200; //randomly generated
int SURGERY_OR_COST = 200;
int SURGERY_SERGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; //randomly generated

//increase rate of hunger and restroom needs
int HUNGER_INCREASE_RATE = 10; //randomly generated
int RESTROOM_INCREASE_RATE = 10;

//function prototypes
void registration();
void costCalculator();

//semaphore declarations
sem_t gpSem, patientSem, registrationSem,restroomSem, cafeSem, 
      pharmacySem, bloodLabSem, operatingRoomSem, mutexSem;


int main(int argc, char *argv[])
{
    //semaphore initializations
    sem_init(&gpSem, 0, 8);	   
	sem_init(&patientSem, 0, 0); 
	sem_init(&mutexSem, 0, 1);   

    //declare threads
	pthread_t gpSem, patientSem, registrationSem,restroomSem, cafeSem, 
      pharmacySem, bloodLabSem, operatingRoomSem, mutexSem;

    //create threads
    pthread_create(&registrationSem, NULL, registration, NULL);	//registration thread
}