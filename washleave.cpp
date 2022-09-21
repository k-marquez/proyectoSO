#include <iostream>
#include <chrono>
#include <thread>

class WashLeave
{
    private:
        unsigned int leaves;
        std::string activitie;
        bool status, stop;
    public:
        WashLeave()
        {
            this->leaves = 0;
            this->activitie = "";
            this->status = this->stop = false;
        }
        
        WashLeave(const unsigned int l) :
                                    leaves(l),
                                    activitie(""),
                                    status(false),
                                    stop(false){}
                                    
        unsigned int get_finished_leaves(void)
        {
            return this->leaves;
        }
        
        std::string get_activitie(void)
        {
            return this->activitie;
        }
        
        std::chrono::minutes get_busy_time(void)
        {
            //Se debe recalcular
            return std::chrono::minutes(8 * this->leaves);
        }
        
        void run()
        {
            this->status = true;
            while(!this->stop)
            {
                this->activitie = "washing";
                std::cout << "I am " << this->activitie << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->activitie = "roast";
                std::cout << "I am " << this->activitie << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(4));
                
                
                this->activitie = "cutting";
                std::cout << "I am " << this->activitie << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                leaves++;
                std::cin >> this->stop;
            } 
        }
};

int main(int argc, char *argv[])
{    
     WashLeave lewis = WashLeave();
     std::cout << "Finished leaves: " << lewis.get_finished_leaves() << std::endl;
     lewis.run();
     std::cout << "Finished leaves: " << lewis.get_finished_leaves() << std::endl;
    return EXIT_SUCCESS;
}
