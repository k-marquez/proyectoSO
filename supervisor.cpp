#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#include "memorykey.h"

void initSharedMemory(key_t ,int *&, int &);
template<typename T>
void initSharedMemory(key_t ,T *&, int &, size_t);
void initSemaphores(key_t , int &);
void initProcesses(pid_t [], std::string [], float, int);
void clear_terminal(void);
void killing_processes(pid_t [], std::string []);

int main(int argc, char *argv[])
{
    int id_shr_memory,
        id_shr_busy_memory,
        *stacks,
        status,
        ids_semaphores,
        limit_stew,
        times_to_kill = 100;
    float stew;
    bool *status_processes;
    struct sembuf operation;
	
    pid_t processes_id[4], child;
    std::string processes[] = 
    {
        "./washleaves.out",
        "./putdough.out",
        "./putstew.out",
        "./tieuphallaca.out"
    };
    
    std::cout << "Kilograms of stew available: ";
    std::cin >> stew;
    std::cout << "Limit of stew (in gr) to stop washing leaves: ";
    std::cin >> limit_stew;
    
    //Creating a unique key to init shared memory
    key_t key = ftok(SHRMFILE, SHRMKEY);

    //Init shared memory
    initSharedMemory(key, stacks, id_shr_memory, 5);

    //Init semaphores
    initSemaphores(key, ids_semaphores);
    
    //Creating a unique key to init shared memory for status processes
    key = ftok(SHRMFILE, SHRMBUSYKEY);
    
    //Init shared memory
    initSharedMemory(key, status_processes, id_shr_busy_memory, 4);
    
    std::cout << "Invoke processes:" << std::endl;
    initProcesses(processes_id, processes, stew, limit_stew);
    
    while(true)
    {
        std::cout << "Waiting "<< times_to_kill
                  <<" seconds to kill processes" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        times_to_kill -= 3;
        if(times_to_kill <= 0)
        {
            //Killing process
            killing_processes(processes_id, processes);
            //Unlinking shared memory
            shmdt(stacks);
            //Deleting shared memory
            shmctl(id_shr_memory, IPC_RMID, 0);
            //Unlinking shared memory for busy status
            shmdt(status_processes);
            //Deleting shared memory for busy status
            shmctl(id_shr_busy_memory, IPC_RMID, 0);
            //Deleting semaphores
            semctl(ids_semaphores,4,IPC_RMID,0);
            
            return EXIT_SUCCESS; 
        }
        
        //Clear terminal
        clear_terminal();
        
        std::cout << "Stacks:\n"
                  << "Stack of clean leaves:\t\t\t"
                  << *(stacks + 0) << "\twashleaves:\t"
                  << (*(status_processes + 0) ? "Busy" : "Not busy")
                  << "\nStack of leaves with dough:\t\t"
                  << *(stacks + 1) << "\tputdough:\t"
                  << (*(status_processes + 1) ? "Busy\n" : "Not busy")
                  << "\nStack of hallacas to be tied:\t\t"
                  << *(stacks + 2) << "\tputstew:\t"
                  << (*(status_processes + 2) ? "Busy\n" : "Not busy")
                  << "\nStack of tied hallacas:\t\t\t"
                  << *(stacks + 3) << "\ttieuphallaca:\t"
                  << (*(status_processes + 3) ? "Busy\n" : "Not busy")
                  << std::endl;
    }
    
    return EXIT_SUCCESS;
}

template<typename T>
void initSharedMemory(key_t key, T *&lS, int &id_shr_memory, size_t num_slots)
{
    //Init shared memory
    if(key != -1)
    {
        id_shr_memory = shmget(key, sizeof(T) * num_slots, 0777|IPC_CREAT);
        
        if(id_shr_memory != -1)
        {
            //Linking shared memory to BPC
            lS = (T*)shmat(id_shr_memory, 0, 0);
            if(lS != nullptr)
            {
                for(size_t i = 0; i < num_slots; i++)
                    *(lS + i) = 0;
            }
            else
                std::cout << "Error linking shared memory to BPC" << std::endl;
        }
        else
            std::cout << "Error creating shared memory" << std::endl;
    }
    else
        std::cout << "Error creating shared memory id" << std::endl;
}


void initSemaphores(key_t key, int &ids_semaphores)
{
    ids_semaphores = semget(key, 4, 0600 | IPC_CREAT);
    
    //Init value for semaphores
    semctl(ids_semaphores, 0, SETVAL, 1);
    semctl(ids_semaphores, 1, SETVAL, 1);
    semctl(ids_semaphores, 2, SETVAL, 1);
    semctl(ids_semaphores, 3, SETVAL, 1);
}

void clear_terminal(void)
{
    pid_t child;
    switch(child = fork())
    {
        case 0:
        {
            int r_num = 1 + rand() % 3;
            std::this_thread::sleep_for(std::chrono::seconds(r_num));
            execlp("/usr/bin/clear", "clear", NULL);
            exit(1);
        }
        default:
        {
            int stat_val;
            waitpid(child, & stat_val, 0);
        }
    }
}

void initProcesses(pid_t processes_id[],
                   std::string processes[],
                   float stew,
                   int limit_stew)
{
    size_t pos;
    pid_t child;
    for (short i = 0; i < 4; i++)
    {
        pos = processes[i].find("./");
        std::cout << "\t" << processes[i].substr(pos) << std::endl;
        switch (child = fork())
        {
            case -1:
            {
                exit(-1);
            }
            case 0:
            {
                if(i != 2)
                {                
                    execlp("/usr/bin/x-terminal-emulator",
                           "/usr/bin/x-terminal-emulator",
                           "-e",
                           processes[i].c_str(),
                           NULL);
                }
                else
                {
                    execlp("/usr/bin/x-terminal-emulator",
                           "/usr/bin/x-terminal-emulator",
                           "-e",
                           (
                                processes[i] +
                                " " + std::to_string(stew) +
                                " " + std::to_string(limit_stew) +
                                " " + std::to_string(processes_id[0])
                           ).c_str(),
                           NULL);
                }
            }
            default:
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                processes_id[i] = child;
            }
        }
    }
}

void killing_processes(pid_t processes_id[], std::string processes[])
{
    size_t pos;
    for(short i = 0; i < 4; i++)
    {
        pos = processes[i].find("./");
        std::cout << "Killing process "
            << processes[i].substr(pos)
            << std::endl;        
        kill(processes_id[i],SIGTERM);
    }
}
