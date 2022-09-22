#include <iostream>
#include <chrono>
#include <thread>

class WashLeaves
{
    private:
        unsigned int leaves;
        std::string activity;
        bool status, stop;
    public:
        WashLeaves()
        {
            this->leaves = 0;
            this->activity = "";
            this->status = this->stop = false;
        }
        
        WashLeaves(const unsigned int l) :
                                    leaves(l),
                                    activity(""),
                                    status(false),
                                    stop(false){}                             
        unsigned int get_finished_leaves(void)
        {
            return this->leaves;
        }
        
        std::string get_activity(void)
        {
            return this->activity;
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
            return std::chrono::seconds(8 * this->leaves);
        }
        
        void run()
        {
            this->status = true;
            while(!this->stop)
            {
                this->activity = "washing";
                std::cout << "I am " << this->activity << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->activity = "roasting";
                std::cout << "I am " << this->activity << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(4));
                
                this->activity = "cutting up";
                std::cout << "I am " << this->activity << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                leaves++;
                std::cin >> this->stop; 
            } 
        }
};

int main(int argc, char *argv[])
{    
     WashLeaves lewis = WashLeaves();
     std::cout << "Finished leaves: " << lewis.get_finished_leaves() << std::endl;
     lewis.run();
     std::cout << "Finished leaves: " << lewis.get_finished_leaves() << std::endl;
    return EXIT_SUCCESS;
}