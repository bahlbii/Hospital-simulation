#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

//GLOBAL VARIABLES
int REGISTRATION_SIZE = 10; //max registration desks available.
int RESTROOM_SIZE = 10;     //max restrooms that are available.
int CAFE_NUMBER = 10;       //max cashiers in cafe that are available.
int GP_NUMBER = 10;         //max General Practitioner that are available.
int PHARMACY_NUMBER = 10;   //max cashiers in pharmacy that are available.
int BLOOD_LAB_NUMBER = 10;  //max assistants in blood lab that are available.

//NUMBER OF OR, SURGOENS AND NURSES AVAILABLE
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;

//NUMBER OF SURGEONS AND NURSES REQUIRED FOR AN OPERATION
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
int BLOOD_LAB_COST = 200;    //fixed
int PHARMACY_COST = 200;     //randomly generated
int SURGERY_OR_COST = 200;
int SURGERY_SERGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; //randomly generated

//increase rate of hunger and restroom needs
int HUNGER_INCREASE_RATE = 10; //randomly generated
int RESTROOM_INCREASE_RATE = 10;

//patient properties
int Hunger_Meter;   //initialized between 1 and 100 at creation
int Restroom_Meter; //initialized between 1 and 100 at creation

//function prototypes
void *registration();
void *patient(void *n);
void *restroom();
void *cafe();
void *pharmacy();
void *bloodlab();
void *operation();
void *costCalculator();
void *display();

//counters
int patientsGenerated = 0; //0;

//semaphore declarations
sem_t gpSem, patientSem, registrationSem, restroomSem, cafeSem,
    pharmacySem, bloodLabSem, operatingRoomSem, mutexSem;
//mutex declaration
pthread_mutex_t lock;

//diseases array
const char *disease[10] = {"heart", "lung", "brain", "eye", "skin",
                           "ear", "bone", "mental", "kidney", "liver"};

int main(int argc, char *argv[])
{
    //semaphore initializations
    sem_init(&patientSem, 0, 0);
    sem_init(&registrationSem, 0, 10);
    sem_init(&gpSem, 0, 10);
    sem_init(&restroomSem, 0, 10);
    sem_init(&cafeSem, 0, 10);
    sem_init(&pharmacySem, 0, 10);
    sem_init(&bloodLabSem, 0, 10);
    sem_init(&operatingRoomSem, 0, 10);

    //mutex to protect critical sections
    sem_init(&mutexSem, 0, 1);
    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

    //declare threads
    pthread_t patient_thrd, registration_thrd, gp_thrd, restroom_thrd, cafe_thrd,
        pharmacy_thrd, bloodLab_thrd, operatingRoom_thrd;

    //create threads
    pthread_create(&registration_thrd, NULL, registration, NULL); //registration thread
    pthread_create(&gp_thrd, NULL, registration, NULL); //gp thread
    pthread_create(&restroom_thrd, NULL, restroom, NULL); //restroom thread
    pthread_create(&cafe_thrd, NULL, cafe, NULL); //cafe thread
    pthread_create(&pharmacy_thrd, NULL, pharmacy, NULL); //pharmacy thread
    pthread_create(&bloodLab_thrd, NULL, bloodlab, NULL); //bloodlab thread
    pthread_create(&operatingRoom_thrd, NULL, operation, NULL); //or thread

    int i;
    for (i = 0; i <= patientsGenerated; i++)
    {
        pthread_create(&patient_thrd, NULL, patient, (void *)(intptr_t)(patientsGenerated));
        //sleep(2);
    }

    pthread_exit(NULL);
    //return 0;
}

void *patient(void *n)
{
    while (true)
    {
        printf("Patient[%d] generated.\n", (intptr_t)(patientsGenerated + 1));
        //sem_wait(&patientSem);

        //protect critical sector
        pthread_mutex_lock(&lock); //sem_wait(&mutexSem);;
        patientsGenerated++;
        REGISTRATION_SIZE--;
        pthread_mutex_unlock(&lock); //sem_post(&mutexSem);

        sem_wait(&patientSem);
        sem_post(&registrationSem);
    }
    pthread_exit(NULL);
}

void *registration()
{
    while (true)
    {   
        //sem_wait(&registrationSem);
        sleep(1);
        printf("Patient[%d] registered successfully.\n ****idle reg desks: %d\n", patientsGenerated, REGISTRATION_SIZE);

        int value1;
        sem_getvalue(&registrationSem, &value1);
        printf("registrationSem value1: %d\n\n", value1);

        //protect critical sector
        pthread_mutex_lock(&lock);//sem_wait(&mutexSem);
        REGISTRATION_SIZE++;
        pthread_mutex_unlock(&lock);//sem_post(&mutexSem);

        sem_wait(&registrationSem);
        sem_post(&patientSem); //1
    }
    pthread_exit(NULL);
}

void *restroom()
{
}

void *cafe()
{
}

void *pharmacy()
{
}

void *bloodlab()
{
}

void *operation()
{
}

void *costCalculator()
{
}

void *display()
{
}
