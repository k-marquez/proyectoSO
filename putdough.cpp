#include <iostream>
#include <chrono>
#include <thread>

class PutDough
{
    private:
        std::string activity;
        bool status;
        
    public:
        PutDough()
        {
            this->activity = "";
            this->status = false;
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
                return std::chrono::seconds(6);
            else
                return std::chrono::seconds(4);
        }

        std::chrono::seconds get_time_worked(unsigned int leaves_with_dough)
        {
            return std::chrono::seconds(6 * leaves_with_dough);
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool status)
        {
            this->status = status;
        }
        
        void run(void)
        {
            this->set_status(true);
            this->set_activity("greasing");
            std::cout << "I am " << this->get_activity() << " leaves!"<< std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            
            this->set_activity("putting up dough");
            std::cout << "I am " << this->get_activity() << " to leaves!"<< std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(4));
            this->set_status(false);
        } 
};