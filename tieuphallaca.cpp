#include "memorykey.h"

#define STACK_SIZE (1024 * 1024)

class TieUpHallaca;
struct Arg;

template<typename T>
void initSharedMemory(key_t ,T *&, int &, size_t);
void initSemaphores(key_t , int &);
short consultMenu(void);
int fn_w(void *);
int fn(Arg *);

struct Arg 
{
    TieUpHallaca * p;
    int * lS;
};

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
                return "busy: "+ this->get_activity();
            else
                return "not busy";
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
            //Stack for chil process
            char * stack_child = (char *)malloc(STACK_SIZE);
            
            Arg *arg = new Arg;
            arg->p = this;
            arg->lS = lS;
            
            //Init process to manages querys
            clone(&fn_w, stack_child + STACK_SIZE, CLONE_VM, arg);
            
            short time_for_waiting = 2;
            while(true)
            {
                this->set_status(sP, true);
                this->set_activity("tying up");
                while(*(lS + 2) > 0)
                {
                    this->decrement_leaves_with_stew(lS, ids_semaphores);                
                    std::this_thread::sleep_for(std::chrono::seconds(7));
                            
                    this->increment_count_hallacas(lS, ids_semaphores);
                }

                this->set_status(sP, false);
                this->set_activity("");
                while(*(lS + 2) <= 0)
                {
                    this->set_activity("I'm not busy, waiting for hallacas to tie up!");
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

short consultMenu(void)
{
    short opt;
    std::cout << "1. Consult quantity of hallacas.\n"
                  << "2. Consult quantity of hallacas for tie up.\n"
                  << "3. Static of time.\n"
                  << ":";
    std::cin >> opt;
    return opt;
}

//Function to manage querys
int fn_w(void *arg)
{
    return fn((Arg *)arg);
}

int fn(Arg *arg)
{
    short opt,
          time_to_make_hallacas,
          washleaves_t = 6,
          putdoug_t = 3,
          putstew_t = 5,
          tieup_t = 7;
    
    time_to_make_hallacas = washleaves_t + putdoug_t + putstew_t + tieup_t;
    
    while(true)
    {
        opt = consultMenu();
        if(opt == 1)
            std::cout << "I'm " << arg->p->get_status()
                      << ", the count of hallacas is: "
                      << arg->p->get_count_hallacas()
                      << std::endl;
        else if(opt == 2)
            std::cout << "I'm " << arg->p->get_status()
                      << ", the count for tie up hallacas is: "
                      << *(arg->lS + 2) << std::endl;
        else
            std::cout << "I'm " << arg->p->get_status() << ", time to make "
                      << arg->p->get_count_hallacas() << "is: "
                      << arg->p->get_count_hallacas() * time_to_make_hallacas
                      << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        clear_terminal();
    }
    exit(1);
}
