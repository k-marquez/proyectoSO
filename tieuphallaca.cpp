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

template<typename T>
void initSharedMemory(key_t ,T *&, int &, size_t);
void initSemaphores(key_t , int &);

class TieUpHallaca
{
    private:
        unsigned int count_hallacas;
        std::string activity;
        bool status;
        struct sembuf lock, unlock;
    public:
        TieUpHallaca()
        {
            this->count_hallacas = 0;
            this->activity = "";
            this->status = false;
            //Lock semaphore three
            this->lock.sem_num = 2;
            this->lock.sem_op = -1;
            this->lock.sem_flg = 0;
            //Unlock semaphore three
            this->unlock.sem_num = 2;
            this->unlock.sem_op = 1;
            this->unlock.sem_flg = 0;
        }
        
        unsigned int get_count_hallacas(void)
        {
            return this->count_hallacas;
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

        std::chrono::seconds get_busy_time(void)
        {
            return std::chrono::seconds(7);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(7 * this->get_count_hallacas());
        }
        
        void decrement_leaves_with_stew(int *&lS, int ids_semaphores)
        {
            //Take a hallaca leaf with stew from the stack
            semop(ids_semaphores, &(this->lock), 1);
            *(lS + 2) -= 1;
            semop(ids_semaphores, &(this->unlock), 1);
        }
        
        void increment_count_hallacas(int *&lS, int ids_semaphores)
        {
            this->count_hallacas++;
            *(lS + 3) += 1;
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool *&sP, bool status)
        {
            *(sP + 3) = this->status = status;
        }
        
        void run(int *&lS, bool *& sP, int ids_semaphores)
        {
            short time_for_waiting = 2;
            while(true)
            {
                this->set_status(sP, true);
                this->set_activity("tying up");
                while(*(lS + 2) > 0)
                {
                    this->decrement_leaves_with_stew(lS, ids_semaphores);
                    std::cout << "I am "<< this->get_activity()
                              << " Hallacas!" << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(7));
                    
                    this->increment_count_hallacas(lS, ids_semaphores);
                }

                this->set_status(sP, false);
                this->set_activity("");
                while(*(lS + 2) <= 0)
                {
                    std::cout << "I am not busy, waiting for hallacas to tie up. Hurry up!"
                              << std::endl;
                    std::this_thread::sleep_for
                        (
                            std::chrono::seconds(time_for_waiting++)
                        );
                }
                time_for_waiting = 2;
            }
        }
};

int main(int argc, char *argv[])
{
    int id_shr_memory, id_shr_busy_memory, *stacks, status, ids_semaphores;
    bool *status_processes;
    //Creating a unique key to init shared memory
    key_t key = ftok(SHRMFILE, SHRMKEY);

    //Init shared memory for stacks
    initSharedMemory(key, stacks, id_shr_memory, 5);
    
    //Init semaphores
    initSemaphores(key, ids_semaphores);
    
    key = ftok(SHRMFILE, SHRMBUSYKEY);
    
    //Init shared memory for status processes
    initSharedMemory(key, status_processes, id_shr_busy_memory, 4);
    
    TieUpHallaca lewis = TieUpHallaca();
    lewis.run(stacks, status_processes, ids_semaphores);

    //Unlinking shared memory to BPC
    shmdt(stacks);
    //Unlinking shared memory to BPC
    shmdt(status_processes);

    return EXIT_SUCCESS;
}

template<typename T>
void initSharedMemory(key_t key, T *&lS, int &id_shr_memory, size_t num_slots)
{
    //Init shared memory
    if(key != -1)
    {
        id_shr_memory = shmget(key, sizeof(T) * num_slots, 0777);
        
        if(id_shr_memory != -1)
        {
            //Linking shared memory to BPC
            lS = (T*)shmat(id_shr_memory, 0, 0);
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
