#include <iostream>
#include <chrono>
#include <thread>

class TieUpHallaca
{
    private:
        std::string activity;
        bool status;
    public:
        TieUpHallaca()
        {
            this->activity = "tie up";
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
            return std::chrono::seconds(5);
        }

        std::chrono::seconds get_time_worked(unsigned int finished_hallacas)
        {
            return std::chrono::seconds(5 * finished_hallacas);
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool status)
        {
            this->status = status;
        }
        
        void run()
        {
            this->set_status(true);
            std::cout << "I am "<< this->get_activity() << " Hallacas!"<< std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            this->set_status(false);
        }
};