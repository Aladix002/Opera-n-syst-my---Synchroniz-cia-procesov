//IOS projekt 2
//Filip Botlo, xbotlo01
//30.4. 2023
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>   
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <ctype.h>

//konstantne pocty/casy
#define NUM_OF_QUEUES 3 
#define NUM_OF_ARGS 6
#define MAX_Z_WAIT 10000
#define MAX_BREAK 100
#define MAX_OPEN 10000
#define MAX_QUEUE 100

//cisla pre CASES v messages
#define CLOSING 1
#define Z_STARTED 10
#define Z_ENTERING 11
#define Z_CALLED 12
#define Z_GOING_HOME 13
#define U_STARTED 20
#define U_SERVING 21
#define U_FINISHED 22
#define U_BREAK 23
#define U_BREAK_FINISHED 24
#define U_GOING_HOME 25

//parametre
int NZ, NU, TZ, TU, F;

//zdielana pamat
typedef struct {
  int messages;
  int customers;
  int order[MAX_QUEUE];
  int cnt_front[MAX_QUEUE];
  int call_fronta[NUM_OF_QUEUES][MAX_QUEUE];
  int fronty[NUM_OF_QUEUES];
  bool is_open;
  int memory_id;
  sem_t sem_mutex;
  sem_t sem_zakaznik_wait[NUM_OF_QUEUES];
  sem_t sem_sluzby[NUM_OF_QUEUES];
} SharedResources;

FILE *file;

//spracovavanie parametrov 
void parametre(int argc, char *argv[]){
	int temp;
    char *vstup;
	if (argc!= NUM_OF_ARGS){
 		fprintf(stderr, "Zadaj 6 parametrov\n");
        exit(1);
 	}
 	else {
        for(int i = 1; i < NUM_OF_ARGS; i++){
            temp = strtol(argv[i], &vstup, 10);
            if (*vstup != '\0') {
            fprintf(stderr,"ERROR: Argument %d nie je cislo\n", i);
            exit(1);
        }
            switch(i){
                case 1:
                NZ = temp;
                break;
                case 2:
                NU = temp;
                break;
                case 3:
                    if(temp >= 0 && temp <= 10000){
                        TZ = temp;
                        break;
                    }
                    else{
                        fprintf(stderr,"ERROR: Zvol si cislo v rozmedzi 0 az 10000\n");
                        exit(1);
                    }
                case 4:
                    if(0 <= temp && temp <= 100){
                        TU = temp;
                        break;
                    }
                    else{
                        fprintf(stderr,"ERROR: Zvol si cislo v rozmedzi 0 az 100\n");
                        exit(1);
                    }
                case 5:
                    if(0 <= temp && temp <= 10000){
                        F = temp;
                        break;
                    }
                    else{
                        fprintf(stderr,"ERROR: Zvol si cislo v rozmedzi 0 az 10000\n");
                        exit(1);
                    }
                }
            }
	    }
}

//Odstranenie zdielanej pamate
void cleanupResources(SharedResources * shrd) {
    
    sem_destroy(&shrd->sem_mutex);
    sem_destroy(&shrd->sem_zakaznik_wait[0]);
    sem_destroy(&shrd->sem_zakaznik_wait[1]);
    sem_destroy(&shrd->sem_zakaznik_wait[2]);
    sem_destroy(&shrd->sem_sluzby[0]);
    sem_destroy(&shrd->sem_sluzby[1]);
    sem_destroy(&shrd->sem_sluzby[2]);
    fclose(file);
    int id = shrd->memory_id;
    shmdt(shrd);
    shmctl(id, IPC_RMID, NULL);
}

//Inicializacia zdielanej pamati
void initResources(SharedResources * shared) {

    file = fopen("proj2.out", "w"); 
    if (file == NULL){
        cleanupResources(shared);
        fprintf(stderr,"ERROR: Nepodarilo sa otvorit subor \n");
        exit(1);
    }
    //nastavenie premennych a semaforov na pociatocne hodnoty
    shared->messages = 0;
    shared->customers = 0;
    shared->is_open = true;

    int semaphores[7];
    semaphores[0] = sem_init(&shared->sem_mutex, shared->memory_id, 1);
    semaphores[1] = sem_init(&shared->sem_zakaznik_wait[0], shared->memory_id, 0);
    semaphores[2] = sem_init(&shared->sem_zakaznik_wait[1], shared->memory_id, 0);
    semaphores[3] = sem_init(&shared->sem_zakaznik_wait[2], shared->memory_id, 0);
    semaphores[4] = sem_init(&shared->sem_sluzby[0], shared->memory_id, 0);
    semaphores[5] = sem_init(&shared->sem_sluzby[1], shared->memory_id, 0);
    semaphores[6] = sem_init(&shared->sem_sluzby[2], shared->memory_id, 0);
    for(int i = 0; i < 7; i++){
        if(semaphores[i] < 0){
            cleanupResources(shared);
            fprintf(stderr,"ERROR: Neda sa vytvorit semafor\n");
            exit(1);
        }
    }
}

//Spravy na vystup
void messages(SharedResources * shared,int message, int person_id, int service){
    shared->messages++;
    switch(message){
        //Sprava posty
        case CLOSING:
            fprintf(file, "%d: closing\n", shared->messages);
            fflush(file);
            break;
        //Spravy zakaznikov
        case Z_STARTED:
            fprintf(file, "%d: Z %d: started\n", shared->messages, person_id);
            fflush(file);
            break;
        case Z_GOING_HOME:
            fprintf(file, "%d: Z %d: going home\n", shared->messages, person_id);
            fflush(file);
            break;
        case Z_ENTERING:
            fprintf(file, "%d: Z %d: entering office for a service %d\n", shared->messages, person_id, service);
            fflush(file);
            break;
        case Z_CALLED:
            fprintf(file, "%d: Z %d: called by office worker\n", shared->messages, person_id);
            fflush(file);
            break;
        //Spravy uradnikov
        case U_STARTED:
            fprintf(file, "%d: U %d: started\n", shared->messages, person_id);
            fflush(file);
            break;
        case U_SERVING:
            fprintf(file, "%d: U %d: serving service of type %d\n", shared->messages, person_id, service);
            fflush(file);
            break;
        case U_FINISHED:
            fprintf(file, "%d: U %d: service finished\n", shared->messages, person_id);
            fflush(file);
            break;
        case U_BREAK:
            fprintf(file, "%d: U %d: taking break\n", shared->messages, person_id);
            fflush(file);
            break;
        case U_BREAK_FINISHED:
            fprintf(file, "%d: U %d: break finished\n", shared->messages, person_id);
            fflush(file);
            break;
        case U_GOING_HOME:
            fprintf(file, "%d: U %d: going home\n", shared->messages, person_id);
            fflush(file);
            break;
    }
}

//Zaciatok procesov uradnikov a zakaznikov
void start_process(SharedResources * shared, int id, int osoba){
    srand((time(NULL) * id * getpid()));
    sem_wait(&shared->sem_mutex);
    messages(shared, osoba, id, 0);
    sem_post(&shared->sem_mutex);
}


//Koniec procesov uradnikov a zakaznikov
void end_process(SharedResources * shared, int id, int osoba){
    sem_wait(&shared->sem_mutex);
    messages(shared, osoba, id, 0);
    sem_post(&shared->sem_mutex);
    exit(0);
}

//Zapisanie udajov o frontach v zdielanej pamati
void queue_updates(SharedResources * shared, int zakaznik_id, int my_queue){
    shared->customers++;
    shared->fronty[my_queue]++;
    shared->call_fronta[my_queue][shared->cnt_front[my_queue]] = zakaznik_id;
    shared->cnt_front[my_queue]++;
}

//Vyhodnoti prestavku pre uradnika
bool takke_break(SharedResources *shared, int urad_id) {
    bool should_break = false;
    sem_wait(&shared->sem_mutex);
    //skontroluje ci su na poste prave customers, ak nie, ma cas na prestavku
    if (shared->customers == 0) {
        messages(shared, U_BREAK, urad_id, 0);
        sem_post(&shared->sem_mutex);
        usleep((rand() % (TU + 1)) * 1000);
        sem_wait(&shared->sem_mutex);
        messages(shared, U_BREAK_FINISHED, urad_id, 0);
        should_break = true;
    }
    sem_post(&shared->sem_mutex);
    return should_break;
}

//Vyhodnoti dalsieho zakaznika v rade na obsluhu
bool serve_next_customer(int urad_id, SharedResources *shared) {
    bool queue = false;
    //prechaza fronty a ak je v nejakej zakaznik vrati true
    for (int i = 0; i < NUM_OF_QUEUES && !queue; i++) {
        sem_wait(&shared->sem_mutex);
        int sem_value;
        sem_getvalue(&shared->sem_sluzby[i], &sem_value);
        if (shared->fronty[i] > 0) {
            queue = true;
            //nasla frontu tak zavola zakaznika podla poradia
            messages(shared, Z_CALLED, shared->call_fronta[i][shared->order[i]], i + 1);
            shared->order[i]++;
            shared->fronty[i]--;
            //obsluha zakaznika a nasledne uvolnenie seba a fronty na dalsi service
            sem_post(&shared->sem_mutex);
            messages(shared, U_SERVING, urad_id, i + 1);
            sem_post(&shared->sem_sluzby[i]);
            sem_post(&shared->sem_zakaznik_wait[i]);
            usleep((rand() % (10 + 1)) * 1000);
            messages(shared, U_FINISHED, urad_id, 0);
        } 
        else {
            sem_post(&shared->sem_mutex);
        }
    } return queue;
}

void zakaznik_process(int zakaznik_id, SharedResources *shared) {
    //inicializacia a sleep pred entering 
    start_process(shared, zakaznik_id, Z_STARTED);
    usleep((rand() % (TZ + 1))*1000);
    //kontrola ci neni zavreta, ak ano ide domov
    sem_wait(&shared->sem_mutex);
    if (shared->is_open == 0) {
        messages(shared, Z_GOING_HOME, zakaznik_id, 0);
        sem_post(&shared->sem_mutex);
        exit(0);
    }
    //vyber nahodnej rady a podla toho sa updatnu informacie v shared memory ludoch v rade
    int my_queue = rand() % NUM_OF_QUEUES;
    messages(shared, Z_ENTERING, zakaznik_id, my_queue + 1);
    queue_updates(shared, zakaznik_id, my_queue);
    //po enternuti v danej rade sluzi len 1 uradnik a moze prist len 1 zakaznik
    sem_post(&shared->sem_mutex);
    sem_wait(&shared->sem_sluzby[my_queue]);
    sem_wait(&shared->sem_zakaznik_wait[my_queue]);
    //odchod customera
    shared->customers--;
    end_process(shared, zakaznik_id, Z_GOING_HOME);
}

void uradnik_process(int urad_id, SharedResources *shared)
{
    start_process(shared, urad_id, U_STARTED);

    while (true) {
    sem_wait(&shared->sem_mutex);
    //kontrola ci neni zavrete a vsetci prec, potom konci loop
    if (!shared->is_open && shared->customers == 0) {
        sem_post(&shared->sem_mutex);
        break;
    }
    sem_post(&shared->sem_mutex);   
    //uradnik kontroluje ci si moze dat pauzu
    if (takke_break(shared, urad_id)) {
        continue;
    }
    //vyberanie z radu koho obsluzit, ak niekto je
    if (!serve_next_customer(urad_id, shared)) {
        continue;
    }
}   //odchod z prace (fajront)
    end_process(shared, urad_id, U_GOING_HOME);
}

//Errory pri tvoreni zdielanej pamati
void checkError(int value, SharedResources * shared) {
    if (value < 0 || shared == (void *)-1) {
        fprintf(stderr, "ERROR: Nepodarilo sa vytvorit Shared Memory\n");
        exit(1);
    }
}

//Tvorba procesov uradnikov a zakaznikov
void makeChildren(SharedResources * shrdmmr){
        for (int i = 0; i < NU + NZ; i++) {
    pid_t pid = fork();
    if (pid == 0) {
        if (i < NZ) {
            zakaznik_process(i + 1, shrdmmr);
        } else {
            uradnik_process(i - NZ + 1, shrdmmr);
        }
        exit(0);
        }
        else if (pid < 0) {
            fprintf(stderr, "ERROR: Nepodarilo sa vytvorit proces\n");
            exit(1);
        };
    }
}

//Zatvorenie posty
void closing(SharedResources * shrdmmr){
    sem_wait(&shrdmmr->sem_mutex);
    messages(shrdmmr, CLOSING, 0, 0);
    shrdmmr->is_open = false;
    sem_post(&shrdmmr->sem_mutex);
}

//Cakanie na dokoncenie procesov aj po uzavreti posty
void waitForChildProcesses(int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        wait(NULL);
    }
}

int main(int argc, char **argv) {
    parametre(argc, argv);
    // Vytvorenie zdielanej pamati pre SharedResources
    int shared = shmget(IPC_PRIVATE, sizeof(SharedResources), IPC_CREAT | 0666);
    // Priradenie zdielanej pamäte do procesu
    SharedResources *shrdmmr = shmat(shared, NULL, 0);
    checkError(shared, shrdmmr);
    // Nastavenie id zdielanej PAMATI v SharedResources a inic zdrojov
    shrdmmr->memory_id = shared;
    initResources(shrdmmr);
    makeChildren(shrdmmr);

    //Cas kolko je posta otvorena
    int post_open = (rand() % (F/2)) + F/2;
    usleep(post_open*1000);
    
    closing(shrdmmr);
    waitForChildProcesses(NU + NZ);
    cleanupResources(shrdmmr);
    exit(0);
}







