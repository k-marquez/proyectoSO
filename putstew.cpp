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

// At least 80 grams of stew are placed in each hallaca
const float MIN_STEW_GR = 80.0;
// Amount of grams to put in the extra dough
const unsigned int INCREMENT_OF_STEW = 15;

class PutStew
{
    private:
        float available_stew, min_limit_stew_gr;
        unsigned int count_for_tie_up;
        std::string activity;
        bool status, notified;
        struct sembuf lock[3], unlock[3];
        
        // Generates a random amount of grs to put in dough
        float amount_stew_to_putting(void)
        {
            return MIN_STEW_GR + rand() % INCREMENT_OF_STEW; 
        }
        
        bool stop_washing_leaves(int *&lS, int ids_semaphores)
        {
            if(this->get_available_stew() <= this->get_min_limit_stew_gr())
            {
                std::cout << "Sending notification to stop washing leaves!" << std::endl;
                semop(ids_semaphores, &(this->lock[2]), 1);
                *(lS + 4) = 1;
                semop(ids_semaphores, &(this->unlock[2]), 1);
                return true;
            }
            
            return false;
        }
        
    public:
        PutStew()
        {
            this->available_stew = this->min_limit_stew_gr = 0.;
            this->count_for_tie_up = 0;
            this->activity = "";
            this->status =  this->notified = false;
            //Lock semaphore two
            this->lock[0].sem_num = 1;
            this->lock[0].sem_op = -1;
            this->lock[0].sem_flg = 0;
            //Unlock semaphore two
            this->unlock[0].sem_num = 1;
            this->unlock[0].sem_op = 1;
            this->unlock[0].sem_flg = 0;
            //Lock semaphore three
            this->lock[1].sem_num = 2;
            this->lock[1].sem_op = -1;
            this->lock[1].sem_flg = 0;
            //Unlock semaphore three
            this->unlock[1].sem_num = 2;
            this->unlock[1].sem_op = 1;
            this->unlock[1].sem_flg = 0;
            //Lock semaphore four
            this->lock[2].sem_num = 3;
            this->lock[2].sem_op = -1;
            this->lock[2].sem_flg = 0;
            //Unlock semaphore four
            this->unlock[2].sem_num = 3;
            this->unlock[2].sem_op = 1;
            this->unlock[2].sem_flg = 0;
        }
        
        PutStew(const float as, const unsigned int mlsg) :
                                    available_stew(as * 1000),
                                    min_limit_stew_gr(mlsg),
                                    count_for_tie_up(0),
                                    activity(""),
                                    status(false),
                                    notified(false)
        {
            //Lock semaphore two
            this->lock[0].sem_num = 1;
            this->lock[0].sem_op = -1;
            this->lock[0].sem_flg = 0;
            //Unlock semaphore two
            this->unlock[0].sem_num = 1;
            this->unlock[0].sem_op = 1;
            this->unlock[0].sem_flg = 0;
            //Lock semaphore three
            this->lock[1].sem_num = 2;
            this->lock[1].sem_op = -1;
            this->lock[1].sem_flg = 0;
            //Unlock semaphore three
            this->unlock[1].sem_num = 2;
            this->unlock[1].sem_op = 1;
            this->unlock[1].sem_flg = 0;
            //Lock semaphore four
            this->lock[2].sem_num = 3;
            this->lock[2].sem_op = -1;
            this->lock[2].sem_flg = 0;
            //Unlock semaphore four
            this->unlock[2].sem_num = 3;
            this->unlock[2].sem_op = 1;
            this->unlock[2].sem_flg = 0;
        }

        float get_available_stew(void)
        {
            return this->available_stew;
        }

        float get_min_limit_stew_gr(void)
        {
            return this->min_limit_stew_gr;
        }

        unsigned int get_count_for_tie_up(void)
        {
            return this->count_for_tie_up;
        }

        std::string get_activity(void)
        {
            return this->activity;
        }

        std::string get_status(void)
        {
            if(this->status)
                return "Busy: " + this->get_activity();
            else
                return "Not Busy";
        }
        
        bool get_notified(void)
        {
            return this->notified;
        }
        
        std::chrono::seconds get_busy_time(void)
        {
            if(this->get_activity() == "putting up")
                return std::chrono::seconds(3);
            return std::chrono::seconds(2);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(5 * this->get_count_for_tie_up());
        }

        void set_available_stew(float grs_stew)
        {
            this->available_stew -= grs_stew;
        }
        
        void decrement_leaves_with_dough(int *&lS, int ids_semaphores)
        {
            //Take a hallaca leaf with dough from the stack
            semop(ids_semaphores, &(this->lock[0]), 1);
            *(lS + 1) -= 1;
            semop(ids_semaphores, &(this->unlock[0]), 1);
        }
        
        void increment_count_for_tie_up(int *&lS, int ids_semaphores)
        {
            this->count_for_tie_up++;
            semop(ids_semaphores, &(this->lock[1]), 1);
            *(lS + 2) += 1;
            semop(ids_semaphores, &(this->unlock[1]), 1);
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool *&sP, bool status)
        {
            *(sP + 2) = this->status = status;
        }

        void set_notified(bool notified)
        {
            this->notified = notified;
        }

        void run(int *&lS, bool *&sP, int ids_semaphores)
        {
            float grs_stew = 0.;
            unsigned int time_for_waiting = 2;
            
            while(true)
            {
                if(this->get_available_stew() <= 0.)
                    break;
                    
                this->set_status(sP, true);
                while(*(lS + 1) > 0 && this->get_available_stew() > 0.)
                {
                    this->decrement_leaves_with_dough(lS, ids_semaphores);

                    grs_stew = this->amount_stew_to_putting();
                    //Placing the rest of stew
                    if(this->get_available_stew() < MIN_STEW_GR)
                        grs_stew = this->get_available_stew();
                        
                    this->set_available_stew(grs_stew);

                    this->set_activity("putting up");
                    std::cout << "I am " << this->get_activity() << " "
                              << grs_stew << " grs of stew!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    
                    this->set_activity("closing");
                    std::cout << "I am " << this->get_activity()
                              << " the hallaca!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    
                    this->increment_count_for_tie_up(lS, ids_semaphores);
                    
                    if(!this->get_notified())
                        this->set_notified(this->stop_washing_leaves(lS, ids_semaphores));
                }
                
                this->set_status(sP, false);
                int r_num = 1 + rand() % 3;
                std::this_thread::sleep_for(std::chrono::seconds(r_num));
                this->set_activity("");
                
                while(*(lS + 1) <= 0)
                {
                     std::cout << "I am not busy, waiting for leaves with dough. Hurry up!"
                               << std::endl;
                     std::this_thread::sleep_for
                        (
                            std::chrono::seconds(time_for_waiting++)
                        );
                }
                
                time_for_waiting = 2;
            }
            
            while(true)
            {
                std::cout << "I am not busy, stew sold out!"<< std::endl;
                std::this_thread::sleep_for
                    (
                        std::chrono::seconds(time_for_waiting++)
                    );
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

    PutStew kevin = PutStew(atof(argv[1]), atof(argv[2]));
    std::cout << "Gr of stew: " << kevin.get_available_stew() << std::endl;
    kevin.run(stacks, status_processes, ids_semaphores);

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
