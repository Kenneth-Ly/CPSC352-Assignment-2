// ===================================================
//Attached: Assignment 2
// ===================================================
//Program: Assignment 2 - SleepingTA
// ===================================================
//Programmer: Ethan Nguyen, Brian Muramatsu, Kenny Ly
//Class: CPSC 351-02 18007
// ===================================================

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

pthread_t *Students;
pthread_t TA;

int ChairsCount = 0;
int CurrentIndex = 0;
const int MAX_CHAIRS = 3;

pthread_mutex_t ChairsMutex;
sem_t TA_Sleep;
sem_t Student_Sem[MAX_CHAIRS];
sem_t TA_Sem;

void *TA_Activity(void *);
void *Student_Activity(void *threadID);

int main(int argc, char *argv[]) 
{
    int number_of_students;
    srand(time(NULL));

    pthread_mutex_init(&ChairsMutex, nullptr);
    sem_init(&TA_Sleep, 0, 0); 
    sem_init(&TA_Sem, 0, 0);

    for (int i = 0; i < MAX_CHAIRS; ++i) 
    {
        sem_init(&Student_Sem[i], 0, 0);
    }

    if (argc < 2) 
    {
        cout << "Number of Students not specified. Using default (5) students.\n";
        number_of_students = 5;
    } else 
    {
        number_of_students = atoi(argv[1]);
        cout << "Number of Students specified. Creating " << number_of_students << " threads.\n";
    }

    Students = new pthread_t[number_of_students];

    pthread_create(&TA, nullptr, TA_Activity, nullptr);

    for (int id = 0; id < number_of_students; ++id) 
    {
        pthread_create(&Students[id], nullptr, Student_Activity, (void *)(intptr_t)id);
    }

    for (int id = 0; id < number_of_students; ++id) 
    {
        pthread_join(Students[id], nullptr);
    }

    pthread_join(TA, nullptr);

    delete[] Students;
    pthread_mutex_destroy(&ChairsMutex);
    sem_destroy(&TA_Sleep);
    sem_destroy(&TA_Sem);
    for (int i = 0; i < MAX_CHAIRS; ++i) 
    {
        sem_destroy(&Student_Sem[i]);
    }

    return 0;
}

void *TA_Activity(void *) 
{
    while (true) 
    {
        sem_wait(&TA_Sleep);

        while (true) 
        {
            pthread_mutex_lock(&ChairsMutex);

            if (ChairsCount == 0) 
            {
                pthread_mutex_unlock(&ChairsMutex);
                break;
            }

            sem_post(&Student_Sem[CurrentIndex]);
            ChairsCount--;
            CurrentIndex = (CurrentIndex + 1) % MAX_CHAIRS;
            pthread_mutex_unlock(&ChairsMutex);

            cout << "TA is helping a student.\n";
            sleep(3);
            sem_post(&TA_Sem);
        }
    }
}

void *Student_Activity(void *threadID) 
{
    int id = (intptr_t)threadID;
    while (true) 
    {
        cout << "Student " << id << " needs help from the TA.\n";

        pthread_mutex_lock(&ChairsMutex);

        if (ChairsCount < MAX_CHAIRS) 
        {
            int chairIndex = (CurrentIndex + ChairsCount) % MAX_CHAIRS;
            ChairsCount++;
            cout << "Student " << id << " is waiting in chair " << chairIndex << ".\n";
            pthread_mutex_unlock(&ChairsMutex);

            sem_post(&TA_Sleep);
            sem_wait(&Student_Sem[chairIndex]);

            cout << "Student " << id << " is getting help from the TA.\n";
            sem_wait(&TA_Sem);
            cout << "Student " << id << " left the TA room.\n";
            break;
        } else 
        {
            pthread_mutex_unlock(&ChairsMutex);
            cout << "Student " << id << " will come back later.\n";
            sleep(1);
        }
    }

    return nullptr;
}

/***************************************************
OUTPUT:

Number of Students not specified. Using default (5) students.
Student 0 needs help from the TA.
Student 0 is waiting in chair 0.
TA is helping a student.
Student 1 needs help from the TA.
Student 1 is waiting in chair 1.
Student 0 is getting help from the TA.
Student 2 needs help from the TA.
Student 2 is waiting in chair 2.
Student 3 needs help from the TA.
Student 3 is waiting in chair 0.
Student 4 needs help from the TA.
Student 4 will come back later.
Student 4 needs help from the TA.
Student 4 will come back later.
Student 4 needs help from the TA.
Student 4 will come back later.
TA is helping a student.
Student Student 1 is getting help from the TA.
0 left the TA room.
Student 4 needs help from the TA.
Student 4 is waiting in chair 1.
TA is helping a student.
Student 1Student 2 is getting help from the TA.
 left the TA room.
TA is helping a student.
Student Student 3 is getting help from the TA.
2 left the TA room.
TA is helping a student.
Student 3 left the TA room.
Student 4 is getting help from the TA.
Student 4 left the TA room.

***************************************************/



