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

class WashLeaves
{
    private:
        unsigned int count_leaves_washed;
        std::string activity;
        bool status, stop;
        struct sembuf lock, unlock;
    public:
        WashLeaves()
        {
            this->count_leaves_washed = 0;
            this->activity = "";
            this->status = this->stop = false;
            //Lock semaphore one
            this->lock.sem_num = 0;
	        this->lock.sem_op = -1;
	        this->lock.sem_flg = IPC_NOWAIT;
	        //Unlock semaphore one
	        this->unlock.sem_num = 0;
	        this->unlock.sem_op = 1;
	        this->unlock.sem_flg = 0;
        }
        
        unsigned int get_count_leaves_washed(void)
        {
            return this->count_leaves_washed;
        }
        
        std::string get_activity(void)
        {
            return this->activity;
        }

        std::string get_status(void)
        {
            if(this->status)
                return "Busy: "+this->get_activity();
            else
                return "Not Busy";
        }

        bool get_stop(void)
        {
            return this->stop;
        }
        
        std::chrono::seconds get_busy_time(void)
        {
            if(this->get_activity() == "washing")
                return std::chrono::seconds(8);
            else if(this->get_activity() == "roasting")
                return std::chrono::seconds(4);
            else
                return std::chrono::seconds(2);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(8 * this->get_count_leaves_washed());
        }

        void increment_count_leaves_washed(int *&lS, int ids_semaphores)
        {
            this->count_leaves_washed++;
            std::cout << "Antes del semaforo"<<std::endl;
            semop(ids_semaphores, &(this->lock), 1);
            *(lS + 0) += 1;
            std::this_thread::sleep_for(std::chrono::seconds(10));
            std::cout << "Dentro del semaforo"<<std::endl;
            semop(ids_semaphores, &(this->unlock), 1);
            std::cout << "Despues del semaforo"<<std::endl;
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool status)
        {
            this->status = status;
        }

        void set_stop(bool stop)
        {
            this->stop = stop;
        }
        
        void run(int *&lS, int ids_semaphores)
        {
            this->set_status(true);
            while(!this->get_stop())
            {
                this->set_activity("washing");
                std::cout << "I am " << this->get_activity() << " leaves!"
                          << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->set_activity("roasting");
                std::cout << "I am " << this->get_activity() << " leaves!"
                          << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(4));
                
                this->set_activity("cutting up");
                std::cout << "I am " << this->get_activity() << " leaves!"
                          << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->increment_count_leaves_washed(lS, ids_semaphores);
            } 
            this->set_status(false);
        }
};

int main(int argc, char *argv[])
{
    int id_shr_memory, *stacks, status, ids_semaphores;
    //Creating a unique key to init shared memory
    key_t key = ftok(SHRMFILE, SHRMKEY);

    //Init shared memory
    initSharedMemory(key, stacks, id_shr_memory);
    initSemaphores(key, ids_semaphores);
    
    
    std::cout << "id_shr_memory: " << id_shr_memory << std::endl;
    std::cout << "Direction of shared memory: " << stacks << std::endl;
    std::cout << "Content of shared memory:\n"
              << "[0]" << *(stacks + 0) << std::endl
              << "[1]" << *(stacks + 1) << std::endl
              << "[2]" << *(stacks + 2) << std::endl
              << "[3]" << *(stacks + 3) << std::endl;
    
    WashLeaves pepito = WashLeaves();
    pepito.run(stacks, ids_semaphores);
    
    //Unlinking shared memory to BPC
    shmdt(stacks);
    
    return EXIT_SUCCESS;
}

void initSharedMemory(key_t key, int *&lS, int &id_shr_memory)
{
    //Init shared memory
    if(key != -1)
    {
        id_shr_memory = shmget(key, sizeof(int) * 4, 0777);
        
        if(id_shr_memory != -1)
        {
            //Linking shared memory to BPC
            lS = (int*)shmat(id_shr_memory, 0, 0);
            if(lS != nullptr)
            {               
                std::cout << "Key: " << key << std::endl;
                std::cout << "id_shr_memory: " << id_shr_memory << std::endl;
                std::cout << "Direction of shared memory: " << lS << std::endl;
                std::cout << "Content of shared memory:\n"
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


void initSemaphores(key_t key, int &ids_semaphores)
{
    ids_semaphores = semget(key, 4, 0600);
    
    //Init value for semaphores
    semctl(ids_semaphores, 0, SETVAL, 0);
    semctl(ids_semaphores, 1, SETVAL, 0);
    semctl(ids_semaphores, 2, SETVAL, 0);
    semctl(ids_semaphores, 3, SETVAL, 0);
}


