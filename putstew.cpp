#include <iostream>
#include <chrono>
#include <thread>

#include "memorykey.h"

// At least 80 grams of stew are placed in each hallaca
const float MIN_STEW_GR = 80.0;
// Amount of grams to put in the extra dough
const unsigned int INCREMENT_OF_STEW = 15;

//Temporary stacks of leaves with dough and hallacas pending for tie up
unsigned int Leaves_With_Dough = 10, For_Tie_Up = 0;

class PutStew
{
    private:
        float available_stew, min_limit_stew_gr;
        unsigned int count_for_tie_up;
        std::string activity;
        bool status, notified;
        
        // Generates a random amount of grs to put in dough
        float amount_stew_to_putting(void)
        {
            return MIN_STEW_GR + rand() % INCREMENT_OF_STEW; 
        }
        
        bool stop_washing_leaves(void)
        {
            if(this->get_available_stew() <= this->get_min_limit_stew_gr())
            {
                std::cout << "Sending notification to stop washing leaves!" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
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
        }
        
        PutStew(const float as, const unsigned int mlsg) :
                                    available_stew(as * 1000),
                                    min_limit_stew_gr(mlsg),
                                    count_for_tie_up(0),
                                    activity(""),
                                    status(false),
                                    notified(false){}

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
                return std::chrono::seconds(4);

            return std::chrono::seconds(2);
        }

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(4 * this->get_count_for_tie_up());
        }

        void set_available_stew(float grs_stew)
        {
            this->available_stew -= grs_stew;
        }
        
        void increment_count_for_tie_up(void)
        {
            this->count_for_tie_up++;
        }

        void set_activity(std::string activity)
        {
            this->activity = activity;
        }

        void set_status(bool status)
        {
            this->status = status;
        }

        void set_notified(bool notified)
        {
            this->notified = notified;
        }

        void run()
        {
            float grs_stew = 0.;
            unsigned int time_for_waiting = 2;
            
            while(this->get_available_stew() > 0.)
            {            
                this->set_status(true);
                while(Leaves_With_Dough > 0 and this->get_available_stew() > 0.)
                {
                    Leaves_With_Dough--;
                    
                    grs_stew = this->amount_stew_to_putting();
                    //Placing the rest of stew
                    if(this->get_available_stew() < MIN_STEW_GR)
                        grs_stew = this->get_available_stew();
                        
                    this->set_available_stew(grs_stew);

                    this->set_activity("putting up");
                    std::cout << "I am " << this->get_activity() << " " << grs_stew << " grs of stew!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    
                    this->set_activity("closing");
                    std::cout << "I am " << this->get_activity() << " the hallaca!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    
                    For_Tie_Up++;
                    this->increment_count_for_tie_up();
                    
                    if(!this->get_notified())
                        this->set_notified(this->stop_washing_leaves());
                }
                
                this->set_status(false);
                this->set_activity("");
                while(Leaves_With_Dough <= 0)
                {
                     std::cout << "I am not busy, waiting for leaves with dough. Hurry up!"<< std::endl;
                     std::this_thread::sleep_for(std::chrono::seconds(time_for_waiting++));
                }
                time_for_waiting = 2;
        }
    }       
};

int main(int argc, char *argv[])
{
    PutStew kevin = PutStew(atof(argv[1]), atof(argv[2]));
    std::cout << "Gr of stew: " << kevin.get_available_stew() << std::endl;
    kevin.run();
    std::cout << "Gr of stew: " << kevin.get_available_stew() << std::endl;    
    return EXIT_SUCCESS;
}
