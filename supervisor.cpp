#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "memorykey.h"

void initSharedMemory(key_t ,int *&, int &);
void initSemaphores(key_t , int &);

int main(int argc, char *argv[])
{
    int id_shr_memory, *stacks, status, ids_semaphores;
    size_t pos = 0;
    short i;
    struct sembuf operation;
	
    pid_t processes_id[4], child;
    std::string processes[] = 
    {
        "./washleaves.out",
        "./putdough.out",
        "./putstew.out 1 300",
        "./tieuphallaca.out"
    };
    
    //Creating a unique key to init shared memory
    key_t key = ftok(SHRMFILE, SHRMKEY);

    //Init shared memory
    initSharedMemory(key, stacks, id_shr_memory);
    initSemaphores(key, ids_semaphores);
    
    std::cout << "Invoke processes:" << std::endl;
    for (i = 0; i < 4; i++)
    {
        pos = processes[i].find("./");
        std::cout << "\t" << processes[i].substr(pos) << std::endl;
        switch (child = fork())
        {
            case -1:
            {
                return -1;
            }
            case 0:
            {
                execlp("/usr/bin/x-terminal-emulator",
                       "/usr/bin/x-terminal-emulator",
                       "-e",
                       processes[i].c_str(),
                       NULL);
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            default:
            {
                processes_id[i] = child;
            }
        }
    }
    
    int times_to_kill = 100;
    bool running = true;
    
    while(running)
    {
        std::cout << "Waiting "<< times_to_kill<<" seconds to kill processes"<< std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        times_to_kill--;
        if(times_to_kill <= 0)
        {
            for(i = 0; i < 4; i++)
            {
                pos = processes[i].find("./");
                std::cout << "Killing process "
                          << processes[i].substr(pos)
                          << std::endl;
                kill(processes_id[i],SIGTERM);
            }
            running = false;
        }
        //Temporaly
        std::cout << "Stacks:\n"
                  << "Stack of leaves washed, roasted and cut:\t" << *(stacks + 0) << std::endl
                  << "Stack of leaves with dough:\t\t\t" << *(stacks + 1) << std::endl
                  << "Stack of hallacas to be tied:\t\t\t" << *(stacks + 2) << std::endl
                  << "Stack of tied hallacas:\t\t\t\t" << *(stacks + 3) << std::endl;
    }
    
    //Unlinking shared memory to BPC
    shmdt(stacks);
    //Deleting shared memory
    shmctl(id_shr_memory, IPC_RMID, 0);
    //Deleting semaphores
    semctl(ids_semaphores,4,IPC_RMID,0);
    return EXIT_SUCCESS;
}

void initSharedMemory(key_t key, int *&lS, int &id_shr_memory)
{
    //Init shared memory
    if(key != -1)
    {
        id_shr_memory = shmget(key, sizeof(int) * 4, 0777|IPC_CREAT);
        
        if(id_shr_memory != -1)
        {
            //Linking shared memory to BPC
            lS = (int*)shmat(id_shr_memory, 0, 0);
            if(lS != nullptr)
            {
                *(lS + 0) = *(lS + 1) = *(lS + 2) = *(lS + 3) = 10;
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