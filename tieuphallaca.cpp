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

class TieUpHallaca
{
    private:
        unsigned int count_hallacas;
        std::string activity;
        bool status;
        struct sembuf lock[2], unlock[2];
    public:
        TieUpHallaca()
        {
            this->count_hallacas = 0;
            this->activity = "";
            this->status = false;
            //Lock semaphore three
            this->lock[0].sem_num = 2;
            this->lock[0].sem_op = -1;
            this->lock[0].sem_flg = 0;
            //Unlock semaphore three
            this->unlock[0].sem_num = 2;
            this->unlock[0].sem_op = 1;
            this->unlock[0].sem_flg = 0;
            //Lock semaphore four
            this->lock[1].sem_num = 3;
            this->lock[1].sem_op = -1;
            this->lock[1].sem_flg = 0;
            //Unlock semaphore four
            this->unlock[1].sem_num = 3;
            this->unlock[1].sem_op = 1;
            this->unlock[1].sem_flg = 0;
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
            semop(ids_semaphores, &(this->lock[0]), 1);
            *(lS + 2) -= 1;
            semop(ids_semaphores, &(this->unlock[0]), 1);
        }
        
        void increment_count_hallacas(int *&lS, int ids_semaphores)
        {
            this->count_hallacas++;
            semop(ids_semaphores, &(this->lock[1]), 1);
            *(lS + 3) += 1;
            semop(ids_semaphores, &(this->unlock[1]), 1);
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool status)
        {
            this->status = status;
        }
        
        void run(int *&lS, int ids_semaphores)
        {
            short time_for_waiting = 2;
            while(true)
            {
                this->set_status(true);
                this->set_activity("tying up");
                while(*(lS + 2) > 0)
                {
                    this->decrement_leaves_with_stew(lS, ids_semaphores);
                    std::cout << "I am "<< this->get_activity()
                              << " Hallacas!" << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(7));
                    
                    this->increment_count_hallacas(lS, ids_semaphores);
                }

                this->set_status(false);
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
    int id_shr_memory, *stacks, status, ids_semaphores;
    //Creating a unique key to init shared memory
    key_t key = ftok(SHRMFILE, SHRMKEY);

    //Init shared memory
    initSharedMemory(key, stacks, id_shr_memory);
    initSemaphores(key, ids_semaphores);
    TieUpHallaca lewis = TieUpHallaca();
    lewis.run(stacks, ids_semaphores);

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
