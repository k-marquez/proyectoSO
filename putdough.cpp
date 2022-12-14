/**
 * @file putdough.cpp
 * @authors Kevin Márquez V023391555 Lewis Ochoa V026373847
 * @date 10/10/2022
 */

#include "memorykey.h"

template<typename T>
void initSharedMemory(key_t ,T *&, int &, size_t);
void initSemaphores(key_t , int &);

class PutDough
{
    private:
        unsigned int count_leaves_whith_dough;
        std::string activity;
        bool status;
        struct sembuf lock[2], unlock[2];

    public:
        PutDough()
        {
            this->count_leaves_whith_dough = 0;
            this->activity = "";
            this->status = false;
            //Lock semaphore one
            this->lock[0].sem_num = 0;
            this->lock[0].sem_op = -1;
            this->lock[0].sem_flg = 0;
            //Unlock semaphore one
            this->unlock[0].sem_num = 0;
            this->unlock[0].sem_op = 1;
            this->unlock[0].sem_flg = 0;
            //Lock semaphore two
            this->lock[1].sem_num = 1;
            this->lock[1].sem_op = -1;
            this->lock[1].sem_flg = 0;
            //Unlock semaphore two
            this->unlock[1].sem_num = 1;
            this->unlock[1].sem_op = 1;
            this->unlock[1].sem_flg = 0;
        }

        unsigned int get_count_leaves_with_dough(void)
        {
            return this->count_leaves_whith_dough;
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
            if(this->get_activity() == "greasing")
                return std::chrono::seconds(3);
            else
                return std::chrono::seconds(2);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(3 *
                                        this->get_count_leaves_with_dough());
        }
        
        void increment_count_leaves_with_dough(int *&lS, int ids_semaphores)
        {
            this->count_leaves_whith_dough++;
            semop(ids_semaphores, &(this->lock[1]), 1);
            *(lS + 1) += 1;
            semop(ids_semaphores, &(this->unlock[1]), 1);
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool *&sP, bool status)
        {
            *(sP + 1) = this->status = status;
        }
        
        void run(int *&lS, bool *&sP, int ids_semaphores)
        {
            unsigned int time_for_waiting = 2;
            while(true)
            {
                this->set_status(sP, true);
                while(*(lS + 0) > 0)
                {
                    //Take a leaf from the stack of leaves
                    semop(ids_semaphores, &(this->lock[0]), 1);
                    *(lS + 0) -= 1;
                    semop(ids_semaphores, &(this->unlock[0]), 1);
                    
                    this->set_activity("greasing");
                    std::cout << "I am " << this->get_activity()
                              << " leaves!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    
                    this->set_activity("putting up dough");
                    std::cout << "I am " << this->get_activity()
                              << " to leaves!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    
                    this->increment_count_leaves_with_dough(lS, ids_semaphores);
                }

                this->set_status(sP, false);
                this->set_activity("");
                while(*(lS + 0) <= 0)
                {
                    std::cout << "I am not busy, waiting for leaves. Hurry up!"
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
    
    PutDough lucas = PutDough();
    lucas.run(stacks, status_processes, ids_semaphores);

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