/*------------------------------------------------------------------
 *  CME3205 - Operating Systems Assignment 1
 *  Deu Hospital Simulation Using Semaphores and Mutexes 
 *  Student ID: 2014510109
 *------------------------------------------------------------------
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#define NUM_DISEASES 10

/* Global Variables declaration */
int REGISTRATION_SIZE = 10;
int RESTROOM_SIZE = 10;
int CAFE_NUMBER = 10;
int GP_NUMBER = 10;
int PHARMACY_NUMBER = 10;
int BLOOD_LAB_NUMBER = 10;

//NUMBER OF OR, SURGOENS AND NURSES AVAILABLE
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;

//NUMBER OF SURGEONS AND NURSES REQUIRED FOR AN OPERATION
int SURGEON_LIMIT = 5;
int NURSE_LIMIT = 5;

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

//INCREASE RATES
int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;

//NUMBER OF PATIENTS REQUIRING MEDICAL ATTENTION AND HOSPITAL WALLET
int PATIENT_NUMBER = 500; //1000;
int HOSPITAL_WALLET = 0;

const char *funcName;

//PATIENT STRUCT
typedef struct patientProp
{
    int patientID;
    pthread_t thread_id;
    int disease;
    int bill;
    int getSurgery;   //1 if yes, 0 if no
    int getMedicine;  //1 if yes, 0 if no
    int getBloodTest; //1 if yes, 0 if no
    int hunger_meter;
    int Restroom_Meter;
} patientProp_t;

sem_t semREGISTRATION, semGP, semBLOODLAB, semPHARMACY, semOR, semCAFE, semRESTROOM, semMUTEX;
char *illnesses[NUM_DISEASES] = {"Heart", "Lung", "Brain", "Eye", "Skin",
                                 "Ear", "Bone", "Throat", "Kidney", "Liver"};

void *patientRoutine();
void regRoutine(patientProp_t *);
void gpRoutine(patientProp_t *);
void bloodLabRoutine(patientProp_t *);
void pharmacyRoutine(patientProp_t *);
void orRoutine(patientProp_t *);
void raiseHungerRestroomMeter(patientProp_t *);
void cafeRoutine(patientProp_t *);
void restroomRoutine(patientProp_t *);
void hunger_or_restroomChecker(patientProp_t *);

void *patientRoutine(void *param)
{
    patientProp_t *patient = (patientProp_t *)param; //generate a patient

    //assign disease to patient
    patient->disease = rand() % 10;

    sleep((rand() % WAIT_TIME) / 1000);
    raiseHungerRestroomMeter(patient);
    hunger_or_restroomChecker(patient);

    sem_wait(&semREGISTRATION);
    regRoutine(patient); /* For registration */
    sem_post(&semREGISTRATION);

    sleep((rand() % WAIT_TIME) / 1000);
    raiseHungerRestroomMeter(patient);
    hunger_or_restroomChecker(patient);

    sem_wait(&semGP);
    gpRoutine(patient); /* For GP Doctor */
    sem_post(&semGP);

    sleep((rand() % WAIT_TIME) / 1000);
    raiseHungerRestroomMeter(patient);
    hunger_or_restroomChecker(patient);

    if (patient->getBloodTest)
    {
        sem_wait(&semBLOODLAB);
        bloodLabRoutine(patient); /* blood work needed, go to blood lab  */
        sem_post(&semBLOODLAB);
    }

    sleep((rand() % WAIT_TIME) / 1000);
    raiseHungerRestroomMeter(patient);
    hunger_or_restroomChecker(patient);

    if (patient->getMedicine)
    {
        sem_wait(&semPHARMACY);
        pharmacyRoutine(patient); /* drug needed, go to pharmacy*/
        sem_post(&semPHARMACY);
    }

    sleep((rand() % WAIT_TIME) / 1000);
    raiseHungerRestroomMeter(patient);
    hunger_or_restroomChecker(patient);

    if (patient->getSurgery)
    {
        sem_wait(&semOR);
        orRoutine(patient); /* surgery needed, go to OR */
        sem_post(&semOR);
    }

    printf("%s: Hospital Wallet:%d\n", __func__, HOSPITAL_WALLET);
    printf("%s: Patient:%d's Bill:%d\n", __func__, patient->patientID, patient->bill);

    sem_wait(&semMUTEX); /* Paying Hospital Bill */
    HOSPITAL_WALLET += patient->bill;
    patient->bill = 0;
    sem_post(&semMUTEX);

    printf("----- Patient:%d pays bill. Updated Hospital Wallet:%d\n", patient->patientID, HOSPITAL_WALLET);
    pthread_exit(NULL);
}

void regRoutine(patientProp_t *patient)
{

    printf("%s: Registering Patient:%d\n", __func__, patient->patientID);
    patient->bill += REGISTRATION_COST;
    sleep(REGISTRATION_TIME / 1000);
    raiseHungerRestroomMeter(patient);
}

void gpRoutine(patientProp_t *patient)
{
    printf("%s: Attending the Patient:%d\n", __func__, patient->patientID);
    printf("%s          ----> Patient:%d Diagnosis: %s\n", __func__, patient->patientID, illnesses[patient->disease]);

    /* generate 0 = pharmacy, 1 = blood or 3 = surgery */
    int gotoDeptmnt;
    gotoDeptmnt = rand() % 3; // gp sends patient to one of the 3 main departments

    sleep(GP_TIME / 1000);

    if (gotoDeptmnt == 0)
    {
        patient->getMedicine = 1;
        printf("%s          ----> Sending Patient:%d to PHARMACY\n", __func__, patient->patientID);
    }
    else if (gotoDeptmnt == 1)
    {
        patient->getBloodTest = 1;
        printf("%s          ----> Sending Patient:%d to BLOOD LAB\n", __func__, patient->patientID);
    }
    else
    {
        patient->getSurgery = 1;
        printf("%s          ----> Sending Patient:%d to SURGERY\n", __func__, patient->patientID);
    }

    raiseHungerRestroomMeter(patient);
}

void bloodLabRoutine(patientProp_t *patient)
{

    printf("%s: Blood Test for Patient:%d\n", __func__, patient->patientID);
    patient->bill += BLOOD_LAB_COST;
    sleep(BLOOD_LAB_TIME / 1000);
    raiseHungerRestroomMeter(patient);

    sleep((rand() % WAIT_TIME) / 1000);
    raiseHungerRestroomMeter(patient);
    hunger_or_restroomChecker(patient);

    //send patient to GP after blood work for follow up
    sem_wait(&semGP);
    printf("%s: ----> Patient:%d going back to GP for follow-up.\n", __func__, patient->patientID);
    gpRoutine(patient);
    sem_post(&semGP);
}

void pharmacyRoutine(patientProp_t *patient)
{

    printf("%s: Patient:%d at Pharmacy\n", __func__, patient->patientID);
    patient->bill += rand() % PHARMACY_COST;
    sleep(PHARMACY_TIME / 1000);
    raiseHungerRestroomMeter(patient);
    patient->getMedicine = 0;
}

void orRoutine(patientProp_t *patient)
{
    int needed_SURGEON = (rand() % (SURGEON_LIMIT + 1 - 1)) + 1; //surgeon limit between 1 and 5
    int needed_NURSE = (rand() % (NURSE_LIMIT + 1 - 1)) + 1;     //surgeon limit between 1 and 5

    if (needed_SURGEON <= SURGEON_NUMBER && needed_NURSE <= NURSE_NUMBER && OR_NUMBER > 0)
    {
        //critical region, keeping track of doctors, nurses and ORs availability
        sem_wait(&semMUTEX);
        SURGEON_NUMBER -= needed_SURGEON;
        NURSE_NUMBER -= needed_NURSE;
        OR_NUMBER--;
        sem_post(&semMUTEX);

        printf("%s: <---- Surgery for the Patient:%d\n", __func__, patient->patientID);
        printf("%s: %d Surgeons and %d nurses ready. (Update: %d surgeons, %d nurses and %d ORs available ) *********************** ---->\n",
               __func__, needed_SURGEON, needed_NURSE, SURGEON_NUMBER, NURSE_NUMBER, OR_NUMBER);
        patient->bill += SURGERY_OR_COST + (SURGERY_SERGEON_COST * needed_SURGEON) + (SURGERY_NURSE_COST * needed_NURSE);
        sleep(SURGERY_TIME / 1000);
        patient->getSurgery = 0;
        printf("*****%s: ----> Surgery complete: Sending Patient:%d to GP for follow-up\n", __func__, patient->patientID);

        //---------ENTER critical region, keeping available doctors, nurses and ORs
        
        SURGEON_NUMBER += needed_SURGEON;
        NURSE_NUMBER += needed_NURSE;
        OR_NUMBER++;
        sem_post(&semMUTEX);
        //--------- EXIT critical region, keeping available doctors, nurses and ORs

        sleep((rand() % WAIT_TIME) / 1000);
        raiseHungerRestroomMeter(patient);
        hunger_or_restroomChecker(patient);

        sem_wait(&semGP);   //send patient to GP after surgery
        gpRoutine(patient); /* For GP Doctor */
        sem_post(&semGP);
    }
}

void cafeRoutine(patientProp_t *patient)
{
    printf("Patient:%d is going to CAFETERIA.\n", patient->patientID);
    patient->bill += rand() % CAFE_COST;
    sleep(CAFE_TIME / 1000);
}

void restroomRoutine(patientProp_t *patient)
{
    printf("Patient:%d is going to RESTROOM.\n", patient->patientID);
    sleep(RESTROOM_TIME / 1000);
}

void raiseHungerRestroomMeter(patientProp_t *patient) //increase hunger/restroom values(after waiting time)
{
    patient->hunger_meter += rand() % HUNGER_INCREASE_RATE;
    patient->Restroom_Meter += rand() % RESTROOM_INCREASE_RATE;
}

void hunger_or_restroomChecker(patientProp_t *patient)
{
    if (patient->hunger_meter >= 100)
    {
        sem_wait(&semCAFE);
        cafeRoutine(patient); //hungry, so go to cafe
        sem_post(&semCAFE);
        patient->hunger_meter = 0;
    }
    if (patient->Restroom_Meter >= 100)
    {
        sem_wait(&semRESTROOM);
        restroomRoutine(patient); //go to restroom
        sem_post(&semRESTROOM);
        patient->Restroom_Meter = 0;
    }
}

int main(int argc, char *argv[])
{

    int status = 0;
    srand(time(NULL)); //to generate random number to access disease array

    /* Initializng all the semaphores */
    sem_init(&semREGISTRATION, 0, REGISTRATION_SIZE);
    sem_init(&semGP, 0, GP_NUMBER);
    sem_init(&semBLOODLAB, 0, BLOOD_LAB_NUMBER);
    sem_init(&semPHARMACY, 0, PHARMACY_NUMBER);
    sem_init(&semOR, 0, OR_NUMBER);
    sem_init(&semCAFE, 0, CAFE_NUMBER);
    sem_init(&semRESTROOM, 0, RESTROOM_SIZE);
    sem_init(&semMUTEX, 0, 1); //mutex

    // Allocate memory for each patients private property
    patientProp_t *patients = (patientProp_t *)calloc(PATIENT_NUMBER, sizeof(*patients));
    if (patients == NULL)
        perror("FAILURE! MEMORY ALLOCATION UNSUCCESSFULL.\n");

    //Create one thread for each patient 
    int i;
    for (i = 0; i < PATIENT_NUMBER; i++)
    {
        patients[i].patientID = i + 1;
        pthread_create(&patients[i].thread_id, NULL, &patientRoutine, &patients[i]);//create thread for every patient
    }

    /* join with each thread */
    for (i = 0; i < PATIENT_NUMBER; i++)
        status = pthread_join(patients[i].thread_id, NULL);

    /* Freeing the dynamic allocated memory */
    free(patients);
    printf("***** Hospital Wallet: %d *****\n", HOSPITAL_WALLET);

    return 0;
}
