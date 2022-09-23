#include <iostream>
#include <chrono>
#include <thread>

//Temporary stack of hallacas for tie up and finished hallacas
unsigned int For_Tie_Up = 2, Hallacas = 0;

class TieUpHallaca
{
    private:
        unsigned int count_hallacas;
        std::string activity;
        bool status;
    public:
        TieUpHallaca()
        {
            this->count_hallacas = 0;
            this->activity = "";
            this->status = false;
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
                return "Busy: "+this->get_activity();
            else
                return "Not Busy";
        }

        std::chrono::seconds get_busy_time(void)
        {
            return std::chrono::seconds(5);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(5 * this->get_count_hallacas());
        }
        
        void increment_count_hallacas(void)
        {
            this->count_hallacas++;
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
            short time_for_waiting = 2;
            while(true)
            {
                this->set_status(true);
                this->set_activity("tie up");
                while(For_Tie_Up > 0)
                {
                    For_Tie_Up--; // Temporary decrement of stack
                    std::cout << "I am "<< this->get_activity() << " Hallacas!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    
                    Hallacas++; // Temporary increment of count hallacas
                    this->increment_count_hallacas();
                }

                this->set_status(false);
                this->set_activity("");
                while(For_Tie_Up <= 0)
                {
                    std::cout << "I am not busy, waiting for hallacas for tie up. Hurry up!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(time_for_waiting++));
                }
                time_for_waiting = 2;
            }
        }
};

int main(int argc, char *argv[])
{
    TieUpHallaca marian = TieUpHallaca();
    marian.run();
}
