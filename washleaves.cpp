/**
 * @file washleaves.cpp
 * @authors Kevin Márquez V023391555 Lewis Ochoa V026373847
 * @date 10/10/2022
 */

#include "memorykey.h"

// Amount of leaves to generates in the process
const unsigned int INCREMENT_OF_LEAVES = 5;

template<typename T>
void initSharedMemory(key_t ,T *&, int &, size_t);
void initSemaphores(key_t , int &);

class WashLeaves
{
    private:
        unsigned int count_leaves_washed;
        std::string activity;
        bool status, stop;
        struct sembuf lock[2], unlock[2];
    public:
        WashLeaves()
        {
            this->count_leaves_washed = 0;
            this->activity = "";
            this->status = this->stop = false;
            //Lock semaphore one
            this->lock[0].sem_num = 0;
	        this->lock[0].sem_op = -1;
	        this->lock[0].sem_flg = 0;
	        //Unlock semaphore one
	        this->unlock[0].sem_num = 0;
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
                return std::chrono::seconds(6);
            else if(this->get_activity() == "roasting")
                return std::chrono::seconds(4);
            else
                return std::chrono::seconds(2);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(6 * this->get_count_leaves_washed());
        }

        void increment_count_leaves_washed(int *&lS, int ids_semaphores)
        {
            this->count_leaves_washed++;
            semop(ids_semaphores, &(this->lock[0]), 1);
            *(lS + 0) += 1 + rand() % INCREMENT_OF_LEAVES;
            semop(ids_semaphores, &(this->unlock[0]), 1);
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool *&sP, bool status)
        {
            *(sP + 0) = this->status = status;
        }

        void set_stop(int *&lS, int ids_semaphores)
        {
            semop(ids_semaphores, &(this->lock[1]), 1);
            if(*(lS + 4) == 1)
                this->stop = true;
            semop(ids_semaphores, &(this->unlock[1]), 1);
        }
        
        void run(int *&lS, bool *&sP, int ids_semaphores)
        {
            this->set_status(sP, true);
            while(!this->get_stop())
            {
                this->set_activity("washing");
                std::cout << "I am " << this->get_activity() << " leaves!"
                          << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->set_activity("roasting");
                std::cout << "I am " << this->get_activity() << " leaves!"
                          << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->set_activity("cutting up");
                std::cout << "I am " << this->get_activity() << " leaves!"
                          << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->increment_count_leaves_washed(lS, ids_semaphores);
                this->set_stop(lS, ids_semaphores);
            } 
            
            this->set_status(sP, false);
            semop(ids_semaphores, &(this->unlock[0]), 1);
            while(true)
            {            
                std::cout << "I am not washing anymore leaves!" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(10));
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
    
    WashLeaves pepito = WashLeaves();
    pepito.run(stacks, status_processes, ids_semaphores);
    
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