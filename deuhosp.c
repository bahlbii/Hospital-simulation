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
int OR_NUMBER = 10; //max operating rooms
int SURGEON_NUMBER = 30; //max surgeons
int NURSE_NUMBER = 30; //max nurses that are available.


int main(int argc, char *argv[])
{

}