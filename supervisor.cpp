#include <iostream>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "memorykey.h"

void initSharedMemory(int *&, int &);

int main(int argc, char *argv[])
{
    int shrm_id, *leavesStack;
    
    initSharedMemory(leavesStack, shrm_id);

    /*int status;
    pid_t processes_id[4], child;
    std::string processes[] = 
        {
            "./washleaves.out",
            "./putdough.out",
            "./putstew.out",
            "./tieuphallaca.out"
        };
    
    size_t pos = 0;
    short i;
    
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
                execlp("/bin/qterminal",
                       "/bin/qterminal",
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
    
    int times_to_kill = 5;
    bool running = true;
    while(running)
    {
        printf("Waiting times to kill processes: %i\n", times_to_kill);
        std::this_thread::sleep_for(std::chrono::seconds(times_to_kill--));
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
    }*/
    
    std::cout << "-----Out function-----"<< std::endl;
    std::cout << "shrm_id: " << shrm_id << std::endl;
    std::cout << "Direction of shared memory: " << leavesStack << std::endl;
    std::cout << "Content of shared memory:\n"
              << "[0]" << *(leavesStack + 0) << std::endl
              << "[1]" << *(leavesStack + 1) << std::endl
              << "[2]" << *(leavesStack + 2) << std::endl
              << "[3]" << *(leavesStack + 3) << std::endl;
    
    *(leavesStack + 0) = 19970217;
    *(leavesStack + 1) = 1997021;
    *(leavesStack + 2) = 199702;
    *(leavesStack + 3) = 19970;
    
    std::cout << "Updated content of shared memory:\n"
              << "[0]" << *(leavesStack + 0) << std::endl
              << "[1]" << *(leavesStack + 1) << std::endl
              << "[2]" << *(leavesStack + 2) << std::endl
              << "[3]" << *(leavesStack + 3) << std::endl;
    
    //Unlinking shared memory to BPC
    shmdt(leavesStack);
    //Deleting shared memory
    shmctl(shrm_id, IPC_RMID, 0);
    return EXIT_SUCCESS;
}

void initSharedMemory(int *&lS, int &shrm_id)
{

    //Creating a unique key to init shared memory
    key_t key = ftok(SHRMFILE, SHRMKEY);
    //Init shared memory
    if(key != -1)
    {
        shrm_id = shmget(key, sizeof(int) * 4, 0777|IPC_CREAT);
        
        if(shrm_id != -1)
        {
            //Linking shared memory to BPC
            lS = (int*)shmat(shrm_id, 0, 0);
            if(lS != nullptr)
            {
                std::cout << "Key: " << key << std::endl;
                std::cout << "shrm_id: " << shrm_id << std::endl;
                std::cout << "Direction of shared memory: " << lS << std::endl;
                std::cout << "Content of shared memory:\n"
                          << "[0]" << *(lS + 0) << std::endl
                          << "[1]" << *(lS + 1) << std::endl
                          << "[2]" << *(lS + 2) << std::endl
                          << "[3]" << *(lS + 3) << std::endl;
                //Temporary test                
                *(lS + 0) = 17021997;
                *(lS + 1) = 1702199;
                *(lS + 2) = 170219;
                *(lS + 3) = 17021;
                std::cout << "Updated content of shared memory:\n"
                          << "[0]" << *(lS + 0) << std::endl
                          << "[1]" << *(lS + 1) << std::endl
                          << "[2]" << *(lS + 2) << std::endl
                          << "[3]" << *(lS + 3) << std::endl;
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

