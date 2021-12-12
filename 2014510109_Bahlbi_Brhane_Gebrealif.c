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
#define NUM_DISEASES 10 //ten diseases

// Global Variables declaration
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

//WAITING TIME REQUIRED FOR ACTIONS(IN MILLISECONDS)
int WAIT_TIME = 100;
int REGISTRATION_TIME = 100;
int GP_TIME = 200;
int PHARMACY_TIME = 100;
int BLOOD_LAB_TIME = 200;
int SURGERY_TIME = 500;
int CAFE_TIME = 100;
int RESTROOM_TIME = 100;

//COSTS
int REGISTRATION_COST = 100;    //fixed
int BLOOD_LAB_COST = 200;       //fixed
int PHARMACY_COST = 200;        //randomly generated
int SURGERY_OR_COST = 200;      //fixed
int SURGERY_SERGEON_COST = 100; //fixed
int SURGERY_NURSE_COST = 50;    //fixed
int CAFE_COST = 200;            //randomly generated

//INCREASE RATES
int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;

//NUMBER OF PATIENTS REQUIRING MEDICAL ATTENTION AND HOSPITAL WALLET
int PATIENT_NUMBER = 1000;
int HOSPITAL_WALLET = 0;

//PATIENT STRUCT
typedef struct Persons
{
    int patientID;
    pthread_t thread_id;
    int disease;
    int bill;
    int getSurgery;   //1 if yes, 0 if no
    int getMedicine;  //1 if yes, 0 if no
    int getBloodTest; //1 if yes, 0 if no
    int Hunger_Meter;
    int Restroom_Meter;
} Patient;

sem_t semREGISTRATION, semGP, semBLOODLAB, semPHARMACY, semOR, semCAFE, semRESTROOM, semMUTEX;
char *Diseases[NUM_DISEASES] = {"Heart", "Lung", "Brain", "Eye", "Skin",
                                "Ear", "Bone", "Throat", "Kidney", "Liver"};

//FUNCTION PROTOTYPES
void *m_pateint();
void m_registration(Patient *);
void m_gp(Patient *);
void m_bloodlab(Patient *);
void m_pharmacy(Patient *);
void m_or(Patient *);
void raiseHungerRestroomMeter(Patient *);
void m_cafe(Patient *);
void m_restroom(Patient *);
void m_hunger_checker(Patient *);
void m_wait(Patient *);

void *m_pateint(void *p)
{
    Patient *patient = (Patient *)p; //generate a patient

    //assign disease to patient
    patient->disease = rand() % 10;

    //necessary general waiting before registration action
    m_wait(patient);

    //enter registration method
    sem_wait(&semREGISTRATION);
    m_registration(patient); //
    sem_post(&semREGISTRATION);

    //necessary general waiting before gp action
    m_wait(patient);

    //enter GP method
    sem_wait(&semGP);
    m_gp(patient);
    sem_post(&semGP);

    //necessary general waiting before bloodTest action
    m_wait(patient);

    //if patient needs bloodtest, enter bloodlab method
    if (patient->getBloodTest)
    {
        sem_wait(&semBLOODLAB);
        m_bloodlab(patient); // blood work needed, go to blood lab
        sem_post(&semBLOODLAB);
    }

    //necessary general waiting before pharmacy action
    m_wait(patient);

    //if patient needs bloodtest, enter pharmacy method
    if (patient->getMedicine)
    {
        sem_wait(&semPHARMACY);
        m_pharmacy(patient); // drug needed, go to pharmacy
        sem_post(&semPHARMACY);
    }

    //necessary general waiting before OR action
    m_wait(patient);

    //if patient needs surgery, enter OR method
    if (patient->getSurgery)
    {
        sem_wait(&semOR);
        m_or(patient); /* surgery needed, go to OR */
        sem_post(&semOR);
    }

    printf("Hospital Wallet:%d\n", HOSPITAL_WALLET);
    printf("Patient:%d's Bill:%d\n", patient->patientID, patient->bill);

    /* ENTER critical region, keeping track of hospital bill */
    sem_wait(&semMUTEX);
    HOSPITAL_WALLET += patient->bill;
    patient->bill = 0;
    sem_post(&semMUTEX);
    /* EXIT critical region, keeping track of hospital bill */

    printf("Patient:%d pays bill. Updated Hospital Wallet:%d\n", patient->patientID, HOSPITAL_WALLET);
    pthread_exit(NULL);
}

void m_registration(Patient *patient)
{

    printf("Patient:%d registration.\n", patient->patientID);
    patient->bill += REGISTRATION_COST;
    sleep(REGISTRATION_TIME / 1000);
    raiseHungerRestroomMeter(patient);
}

void m_gp(Patient *patient)
{
    printf("---- GP examining Patient:%d ----> Diagnosis: %s\n", patient->patientID, Diseases[patient->disease]);

    /* generate random , if 0 = pharmacy, 1 = blood or 3 = surgery */
    int gotoDeptmnt;
    gotoDeptmnt = rand() % 3; // gp sends patient to one of the 3 main departments

    //waiting time for gp to finish examination
    sleep(GP_TIME / 1000);

    if (gotoDeptmnt == 0)
    {
        patient->getMedicine = 1;
        printf("--------> GP sending Patient:%d to PHARMACY\n", patient->patientID);
    }
    else if (gotoDeptmnt == 1)
    {
        patient->getBloodTest = 1;
        printf("--------> GP sending Patient:%d to BLOOD LAB\n", patient->patientID);
    }
    else
    {
        patient->getSurgery = 1;
        printf("--------> GP sending Patient:%d to SURGERY\n", patient->patientID);
    }

    raiseHungerRestroomMeter(patient);
}

void m_bloodlab(Patient *patient)
{
    printf("------------> Patient:%d arrived at Blood Lab Center.\n", patient->patientID);
    patient->bill += BLOOD_LAB_COST;
    sleep(BLOOD_LAB_TIME / 1000);
    raiseHungerRestroomMeter(patient);

    //necessary general waiting before gp action
    m_wait(patient);

    //send patient to GP after blood work for follow up
    sem_wait(&semGP);
    printf("--------> Patient:%d going back to GP for follow-up.\n", patient->patientID);
    m_gp(patient);
    sem_post(&semGP);
}

void m_pharmacy(Patient *patient)
{
    printf("------------> Patient:%d arrived at Pharmacy\n", patient->patientID);
    patient->bill += rand() % PHARMACY_COST;
    sleep(PHARMACY_TIME / 1000);
    raiseHungerRestroomMeter(patient);
}

void m_or(Patient *patient)
{
    int Sselected = (rand() % (SURGEON_LIMIT + 1 - 1)) + 1; //surgeon limit between 1 and 5
    int Nselected = (rand() % (NURSE_LIMIT + 1 - 1)) + 1;   //surgeon limit between 1 and 5

    if (Sselected <= SURGEON_NUMBER && Nselected <= NURSE_NUMBER && OR_NUMBER > 0)
    {
        //---------ENTER critical region, keeping available doctors, nurses and ORs
        sem_wait(&semMUTEX);
        SURGEON_NUMBER -= Sselected;
        NURSE_NUMBER -= Nselected;
        OR_NUMBER--;
        sem_post(&semMUTEX);
        //--------- EXIT critical region, keeping available doctors, nurses and ORs

        printf("** ** Available: %d Surgeons | %d Nurses | %d ORs ** **\n------------ Patient:%d undergoing surgery. (%d Surgeons and %d nurses selected.\n",
               SURGEON_NUMBER + Sselected, NURSE_NUMBER + Nselected, OR_NUMBER + 1, patient->patientID, Sselected, Nselected);
        patient->bill += SURGERY_OR_COST + (SURGERY_SERGEON_COST * Sselected) + (SURGERY_NURSE_COST * Nselected);
        sleep(SURGERY_TIME / 1000);
        patient->getSurgery = 0;
        printf("---------- <---- Surgery complete: Sending Patient:%d to GP for follow-up\n", patient->patientID);

        /* ENTER critical region, keeping available doctors, nurses and ORs */
        sem_wait(&semMUTEX);
        SURGEON_NUMBER += Sselected;
        NURSE_NUMBER += Nselected;
        OR_NUMBER++;
        sem_post(&semMUTEX);
        /* EXIT critical region, keeping available doctors, nurses and ORs */

        //necessary general waiting before GP action
        m_wait(patient);

        //send patient to GP after surgery
        sem_wait(&semGP);
        m_gp(patient);
        sem_post(&semGP);
    }
    else
    {
        printf("Not enough personnel and ORs available");
    }
}

void m_cafe(Patient *patient)
{
    printf("Patient:%d is going to CAFETERIA.\n", patient->patientID);
    patient->bill += rand() % CAFE_COST;
    sleep(CAFE_TIME / 1000); //cafe wait time
}

void m_restroom(Patient *patient)
{
    printf("Patient:%d is going to RESTROOM.\n", patient->patientID);
    sleep(RESTROOM_TIME / 1000); //restroom waiting time
}

//raise hunger and rest
void raiseHungerRestroomMeter(Patient *patient) 
{
    patient->Hunger_Meter += (rand() % (HUNGER_INCREASE_RATE - 1 + 1)) + 1;
    patient->Restroom_Meter += (rand() % (RESTROOM_INCREASE_RATE - 1 + 1)) + 1;
}

void m_hunger_checker(Patient *patient)
{
    if (patient->Hunger_Meter >= 100)
    {
        sem_wait(&semCAFE);
        m_cafe(patient); //hungry, so go to cafe
        sem_post(&semCAFE);
        patient->Hunger_Meter = 0;
    }
    if (patient->Restroom_Meter >= 100)
    {
        sem_wait(&semRESTROOM);
        m_restroom(patient); //go to restroom
        sem_post(&semRESTROOM);
        patient->Restroom_Meter = 0;
    }
}

//a function to perform general waiting time
void m_wait(Patient *patient)
{
    sleep((rand() % WAIT_TIME) / 1000);
    raiseHungerRestroomMeter(patient);
    m_hunger_checker(patient);
}

int main(int argc, char *argv[])
{
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
    Patient *patients = (Patient *)calloc(PATIENT_NUMBER, sizeof(*patients));

    //Create one thread for each patient
    int i;
    for (i = 0; i < PATIENT_NUMBER; i++)
    {
        patients[i].patientID = i + 1;
        pthread_create(&patients[i].thread_id, NULL, &m_pateint, &patients[i]); //create thread for every patient
    }

    // join threads so that finished threads don't cause main to exit
    for (i = 0; i < PATIENT_NUMBER; i++)
        pthread_join(patients[i].thread_id, NULL);

    // free allocated memory
    free(patients);
    return 0;
}