/*

Run the project: gcc test.c -o test.o -lpthread   ->  ./test.o

-> gcc is compiler command (compiler name)
-> test.c is a source file name.
-> -o is an option to create objcect file.
-> test is the name of object (binary) file.
-> -lpthread is an option for pthread.h

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define REGISTRATION_SIZE 10 // The number of registration desks that are available.
#define RESTROOM_SIZE 10     // The number of restrooms that are available.
#define CAFE_NUMBER 10       // The number of cashiers in cafe that are available.
#define GP_NUMBER 10         // The number of General Practitioner that are available.
#define PHARMACY_NUMBER 10   // The number of cashiers in pharmacy that are available.
#define BLOOD_LAB_NUMBER 10  // The number of assistants in blood lab that are available.
#define PATIENT_NUMBER 100   // The number of patients that will be generated over the course of this program. Default Value: 1000
#define OP_ROOM 10           // The number of operating rooms.
#define PHARMACY 1           // The patient's state of going to the Pharmacy.
#define BLOOD_LAB 2          // The patient's state of going to the Blood Lab.
#define SURGERY 3            // The patient's state of going to the Surgery.


// Function definitions.
void makeDisease();
void hungerRate();
void restroomRate();
int randomNumber(int minValue, int maxValue);
void randWait(int value);
void patientWaitTime(int minValue, int maxValue);
void regWaitTime(int p_id);
void regTime(int p_id);
void gpWaitTime(int p_id);
void gpTime(int p_id);
void phrWaitTime(int p_id);
void phrTime(int p_id);
void bloodLabWaitTime(int p_id);
void bloodLabTime(int p_id);
void srgWaitTime(int p_id);
void srgTime(int p_id);
void goCafe(int p_id);
void goRestroom(int p_id);
void *patientRegistration(void *patient_id);

// The number of operating rooms, surgeons and nurses that are available.
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;

// The maximum number of surgeons and nurses that can do a surgery. A random value is
// calculated for each operation between 1 and given values to determine the required
// number of surgeons and nurses.
int SURGEON_LIMIT = 5;
int NURSE_LIMIT = 5;

// The account of hospital where the money acquired from patients are stored.
int HOSPITAL_WALLET = 0;

int WAIT_TIME = 100;
int REGISTRATION_TIME = 100;
int GP_TIME = 200;
int PHARMACY_TIME = 100;
int BLOOD_LAB_TIME = 200;
int SURGERY_TIME = 500;
int CAFE_TIME = 100;
int RESTROOM_TIME = 100;

int REGISTRATION_COST = 100;
int PHARMACY_COST = 200; // Calculated randomly between 1 and given value.
int BLOOD_LAB_COST = 200;
int SURGERY_OR_COST = 200;
int SURGERY_SURGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; // Calculated randomly between 1 and given value.

// The global increase rate of hunger and restroom needs of patients. It will increase
// randomly between 1 and given rate below.
int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;

int HUNGER_METER = 100;   // Initialized between 1 and 100 at creation.
int RESTROOM_METER = 100; // Initialized between 1 and 100 at creation.

// Semaphore and Mutex definitions.
sem_t registeration;
sem_t restroom;
sem_t cafe;
sem_t generalPrac;
sem_t pharmacy;
sem_t bloodLab;
sem_t orNumber;

pthread_mutex_t surg;  // 0: surgery time started, 1: surgery time finished.


int state[PATIENT_NUMBER]; // Keeps the states of each patients.
int hungerMeter[PATIENT_NUMBER]; // Keeps the hunger rate of each patients.
int restroomMeter[PATIENT_NUMBER]; // Keeps the restroom rate of each patients.
int patient_number[PATIENT_NUMBER];  // The array that holding patients id.

int main(int argc, char *argv[])
{

  pthread_t p_tid[PATIENT_NUMBER];

  for (int i = 0; i < PATIENT_NUMBER; i++) {patient_number[i] = i;} // Array for thread of patient id.

  makeDisease();  // Random disease assignment process.
  hungerRate();   // Random hunger rate assignment process.
  restroomRate(); // Random restroom rate assignment process.

  // Initialize the semaphores with initial values.
  sem_init(&registeration, 0, REGISTRATION_SIZE);
  sem_init(&restroom, 0, RESTROOM_SIZE);
  sem_init(&cafe, 0, CAFE_NUMBER);
  sem_init(&generalPrac, 0, GP_NUMBER);
  sem_init(&pharmacy, 0, PHARMACY_NUMBER);
  sem_init(&bloodLab, 0, BLOOD_LAB_NUMBER);
  sem_init(&orNumber, 0, OP_ROOM);

  // Create the patient threads.
  for (int i = 0; i < PATIENT_NUMBER; i++){
    pthread_create(&p_tid[i], NULL, patientRegistration, (void *)&patient_number[i]);
    patientWaitTime(0, WAIT_TIME+1); // Patient's arrival time at the hospital.
  }

  // When all of the patients are finished, kill each of patient threads.
  for(int i = 0; i < PATIENT_NUMBER; i++){pthread_join(p_tid[i], NULL);}

  system("PAUSE");
  return 0;
}

void makeDisease(){
  for (int i = 0; i < PATIENT_NUMBER; i++) // The loop that makes an random disease for each patient.
  {
    int disease = randomNumber(1, 4); // Generate random number (1,2,3)
    if (disease == 1)
    {
      state[i] = PHARMACY;
    }
    else if (disease == 2)
    {
      state[i] = BLOOD_LAB;
    }
    else if (disease == 3)
    {
      state[i] = SURGERY;
    }
  }
}

void hungerRate(){
  for (int i = 0; i < PATIENT_NUMBER; i++) {
    int rate = randomNumber(1, HUNGER_METER+1);
    hungerMeter[i] = rate;
  }
}

void restroomRate(){
  for (int i = 0; i < PATIENT_NUMBER; i++) 
  {
    int rate = randomNumber(1, 101);
    restroomMeter[i] = randomNumber(1, RESTROOM_METER+1);
  }
}

int randomNumber(int minValue, int maxValue) 
{
  int r = rand() % (maxValue - minValue) + 1; // Generate a random number between min and max values.
  return r;
}

void randWait(int value)
{
  usleep(value * 1000); // Milliseconds 
}

void patientWaitTime(int minValue, int maxValue)
{
  int value = randomNumber(minValue, maxValue);
  usleep(value * 1000); // Milliseconds 
}

void regWaitTime(int p_id){
  int time = randomNumber(0, WAIT_TIME+1);
  randWait(time);
  printf("Patient %d is waiting to registration. Elapsed Time: %d milliseconds\n", p_id, time);
}

void regTime(int p_id){
  int time = randomNumber(0, REGISTRATION_TIME+1);
  randWait(time);
  HOSPITAL_WALLET += REGISTRATION_COST;
  printf("Patient %d is currently registrating. Elapsed Time: %d milliseconds\n", p_id, time);
}

void gpWaitTime(int p_id){
  int time = randomNumber(0, WAIT_TIME+1);
  randWait(time);
  printf("Patient %d is waiting to GP. Elapsed Time: %d milliseconds\n", p_id, time);
}

void gpTime(int p_id){
  int time = randomNumber(0, GP_TIME+1);
  randWait(time);
  printf("Patient %d describes her complaint to the GP. Elapsed Time: %d milliseconds\n", p_id, time);
}

void phrWaitTime(int p_id){
  int time = randomNumber(0, WAIT_TIME+1);
  randWait(time);
  printf("Patient %d is waiting to Pharmacy. Elapsed Time: %d milliseconds\n", p_id, time);
}

void phrTime(int p_id){
  int time = randomNumber(0, PHARMACY_TIME+1);
  randWait(time);
  HOSPITAL_WALLET += randomNumber(1, PHARMACY_COST+1);
  printf("Patient %d has taken her medication from the pharmacy and is checking out of the hospital. Elapsed Time: %d milliseconds\n", p_id, time);
}

void bloodLabWaitTime(int p_id){
  int time = randomNumber(0, WAIT_TIME+1);
  randWait(time);
  printf("Patient %d is waiting to Blood Lab. Elapsed Time: %d milliseconds\n", p_id, time);
}

void bloodLabTime(int p_id){
  int time = randomNumber(0, BLOOD_LAB_TIME+1);
  randWait(time);
  HOSPITAL_WALLET += BLOOD_LAB_COST;
  printf("Patient %d giving blood. Elapsed Time: %d milliseconds\n", p_id, time);
}

void srgWaitTime(int p_id){
  int time = randomNumber(0, WAIT_TIME+1);
  randWait(time);
  printf("Patient %d is waiting to SURGERY. Elapsed Time: %d milliseconds\n", p_id, time);
}

void srgTime(int p_id){
  sem_wait(&orNumber);
  pthread_mutex_lock(&surg);
  int surgeonLimit = randomNumber(1, SURGEON_LIMIT + 1);
  int nurseLimit = randomNumber(1, NURSE_LIMIT + 1);
  if(SURGEON_NUMBER - surgeonLimit >= 0 && NURSE_NUMBER - nurseLimit >= 0){
    SURGEON_NUMBER -= surgeonLimit;
    NURSE_NUMBER -= nurseLimit;
    int time = randomNumber(0, SURGERY_TIME+1);
    randWait(time);
    printf("Patient %d is undergoing surgery. Elapsed Time: %d milliseconds\n", p_id, time);
    HOSPITAL_WALLET += SURGERY_OR_COST + (surgeonLimit * SURGERY_SURGEON_COST) + (nurseLimit * SURGERY_NURSE_COST);
  }
  pthread_mutex_unlock(&surg);
  sem_post(&orNumber);
  SURGEON_NUMBER += surgeonLimit;
  NURSE_NUMBER += nurseLimit;
  printf("Patient %d is out of surgery and going to the General Practitioner.\n", p_id);
}

void goCafe(int p_id){
  if(hungerMeter[p_id] >= HUNGER_METER){
    sem_wait(&cafe);
    printf("Patient %d achieved %d/100 hunger rate. He/She is eating at the cafe.\n", p_id, hungerMeter[p_id]);
    HOSPITAL_WALLET += randomNumber(1, CAFE_COST+1);
    hungerMeter[p_id] = 0;
    sem_post(&cafe);
  }
  else{
    hungerMeter[p_id] += randomNumber(1, HUNGER_INCREASE_RATE+1);
    if(hungerMeter[p_id] > 100){ // When approaching 100, the final random number may exceed 100.
      hungerMeter[p_id] = 100;
    }
    printf("Patient %d hunger rate: %d/100 \n", p_id, hungerMeter[p_id]);
  }
}

void goRestroom(int p_id){
  if(restroomMeter[p_id] >= RESTROOM_METER){
    sem_wait(&restroom);
    printf("Patient %d achieved %d/100 restroom rate. He/She is at the restroom.\n", p_id, restroomMeter[p_id]);
    restroomMeter[p_id] = 0;
    sem_post(&restroom);
  }
  else{
    restroomMeter[p_id] += randomNumber(1, RESTROOM_INCREASE_RATE+1);
    if(restroomMeter[p_id] > 100){ // When approaching 100, the final random number may exceed 100.
      restroomMeter[p_id] = 100;
    }
    printf("Patient %d restroom rate: %d/100 \n", p_id, restroomMeter[p_id]);
  }

}

void *patientRegistration(void *patient_id)
{
  srand(time(NULL));
  int p_id = *(int *)patient_id;
  
  while (1)
  {
    int value; 
    if(sem_getvalue(&registeration, &value) == 0){ 
      regWaitTime(p_id); // If the recording rooms are full, the patient begins to wait.
      goCafe(p_id);  // Check the hunger rate during the waiting time
      goRestroom(p_id);  // Check the restroom rate during the waiting time
    }
    sem_wait(&registeration); 
    regTime(p_id);  // The patient entered the registration room.

    if(sem_getvalue(&generalPrac, &value) == 0){
      gpWaitTime(p_id); // If the GP are full, the patient begins to wait.
      goCafe(p_id); 
      goRestroom(p_id); 
    }
    sem_wait(&generalPrac);
    gpTime(p_id); // The patient is talking to the doctor.

    if (state[p_id] == PHARMACY)
    {
      if(sem_getvalue(&pharmacy, &value) == 0){
        phrWaitTime(p_id); // The patient is waiting in line at the pharmacy.
        goCafe(p_id);  
        goRestroom(p_id);  
      }
      sem_wait(&pharmacy);
      phrTime(p_id); // The patient entered the pharmacy
      sem_post(&pharmacy);
      break;
    }
    else if (state[p_id] == BLOOD_LAB)
    {
      if(sem_getvalue(&bloodLab, &value) == 0){
        bloodLabWaitTime(p_id); // The patient is waiting to the blood lab.
        goCafe(p_id);  
        goRestroom(p_id);
      }
      sem_wait(&bloodLab);
      bloodLabTime(p_id); // The patient is giving blood in the blood lab.
      sem_post(&bloodLab);

      // After giving blood, the patient will either go home or be directed to the pharmacy.
      int goHomeOrPharmacy = randomNumber(1, 3);  
      state[p_id] = goHomeOrPharmacy;
      if (state[p_id] == PHARMACY)
      {
        if(sem_getvalue(&pharmacy, &value) == 0){
          phrWaitTime(p_id);
          goCafe(p_id);  
          goRestroom(p_id);
        }
        sem_wait(&pharmacy);
        phrTime(p_id);
        sem_post(&pharmacy);
        break;
      }
      else
      {
        printf("General Doctor is sending Patient %d to home.\n", p_id);
        break;     
      }
    }
    else if (state[p_id] == SURGERY)
    {
      if(sem_getvalue(&orNumber, &value) == 0){
        srgWaitTime(p_id); // The patient is waiting to enter the surgery.
      }
      srgTime(p_id); // The patient is in surgery.

       // After surgery, the patient will either go home or be directed to the pharmacy.
      int goHomeOrPharmacy = randomNumber(1, 3);
      state[p_id] = goHomeOrPharmacy;
      if (state[p_id] == PHARMACY)
      {
        if(sem_getvalue(&pharmacy, &value) == 0){
          phrWaitTime(p_id);
          goCafe(p_id);  
          goRestroom(p_id); 
        }
        sem_wait(&pharmacy);
        phrTime(p_id);
        sem_post(&pharmacy);
        break;
      }
      else
      {
        printf("General Doctor is sending Patient %d to home.\n", p_id);
        break;
      }
    }
  }
  sem_post(&generalPrac);
  sem_post(&registeration);
  printf("Total Hospital Vallet: %d \n", HOSPITAL_WALLET);
}

