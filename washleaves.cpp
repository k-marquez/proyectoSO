#include <iostream>
#include <chrono>
#include <thread>

class WashLeaves
{
    private:
        std::string activity;
        bool status, stop;
    public:
        WashLeaves()
        {
            this->activity = "";
            this->status = this->stop = false;
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
                return std::chrono::seconds(8);
            else if(this->get_activity() == "roasting")
                return std::chrono::seconds(4);
            else
                return std::chrono::seconds(2);
        }

        std::chrono::seconds get_time_worked(unsigned int leaves)
        {
            return std::chrono::seconds(8 * leaves);
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool status)
        {
            this->status = status;
        }

        void set_stop(bool stop)
        {
            this->stop = stop;
        }
        
        void run()
        {
            this->set_status(true);
            while(!this->get_stop())
            {
                this->set_activity("washing");
                std::cout << "I am " << this->get_activity() << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->set_activity("roasting");
                std::cout << "I am " << this->get_activity() << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(4));
                
                this->set_activity("cutting up");
                std::cout << "I am " << this->get_activity() << " leaves!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
            } 
            this->set_status(false);
        }
};