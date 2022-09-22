#include <iostream>
#include <chrono>
#include <thread>

class PutDough
{
    private:
        unsigned int leaves_with_dough, leaves;
        std::string activity;
        bool status;
        
    public:
        PutDough()
        {
            this->leaves = this->leaves_with_dough = 0;
            this->activity = "";
            this->status = false;
        }

        PutDough(const unsigned int l, const unsigned int lwd) :
                                    leaves(l),
                                    leaves_with_dough(lwd),
                                    activity(""),
                                    status(false){}
                           
        unsigned int get_leaves_with_dough(void)
        {
            return this->leaves_with_dough;
        }
        
        std::string get_activity(void)
        {
            return this->activity;
        }
        
        std::chrono::seconds get_busy_time(void)
        {
            if(this->get_activity() == "greasing")
                return std::chrono::seconds(6);
            else
                return std::chrono::seconds(4);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(6 * this->leaves_with_dough);
        }
        
        void run(void)
        {
            this->status = true;
            while(this->leaves)
            {
                this->leaves--;
                
                this->activity = "greasing";
                std::cout << (this->status ? "Busy: " : "Not busy.")
                                << "I am " << this->activity << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->activity = "putting up dough";
                std::cout << (this->status ? "Busy: " : "Not busy.")
                                << "I am " << this->activity << " to leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(4));
                this->leaves_with_dough++;
            }      
    } 
};

int main(int argc, char *argv[])
{   
     PutDough carly = PutDough(15, 0);
     std::cout << "Leaves with dough: " << carly.get_leaves_with_dough() << std::endl;
     carly.run();
     std::cout << "Leaves with dough: " << carly.get_leaves_with_dough() << std::endl;
    return EXIT_SUCCESS;
}
