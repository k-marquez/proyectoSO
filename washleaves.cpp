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

// Amount of leaves to generates in the process
const unsigned int INCREMENT_OF_LEAVES = 5;

void initSharedMemory(key_t ,int *&, int &);
void initSemaphores(key_t , int &);
static void signal_handler(int);

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
	        this->lock.sem_flg = 0;
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
            semop(ids_semaphores, &(this->lock), 1);
            *(lS + 0) += 1 + rand() % INCREMENT_OF_LEAVES;
            semop(ids_semaphores, &(this->unlock), 1);
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

    //Manage signal SIGUSR1
    signal(SIGUSR1,signal_handler);

    //Creating a unique key to init shared memory
    key_t key = ftok(SHRMFILE, SHRMKEY);

    //Init shared memory
    initSharedMemory(key, stacks, id_shr_memory);
    initSemaphores(key, ids_semaphores);
    
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
            if(lS == nullptr)
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
}

void signal_handler(int sig_recieve) {
    signal(SIGUSR1,signal_handler);
    std::cout<< "Recibí la señal de parar de lavar..!"<<std::endl;
    std::cout << "Stop process washing leaves! "  << sig_recieve << std::endl;
    sleep(20);
}
