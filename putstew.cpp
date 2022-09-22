#include <iostream>
#include <chrono>
#include <thread>

// At least 80 grams of stew are placed in each hallaca
const float min_stew_gr = 80.0;
// Amount of grams to put in the extra dough
const unsigned int increment_of_stew = 15;

class PutStew
{
    private:
        float available_stew, min_limit_stew_gr;
        std::string activity;
        bool status, notified;
        
        // Generates a random amount of grs to put in dough
        float amount_stew_to_putting(void)
        {
            return min_stew_gr + rand() % increment_of_stew; 
        }
        
        bool stop_washing_leaves(void)
        {
            if(this->get_available_stew() <= this->get_min_limit_stew_gr())
            {
                std::cout << "Sending notification to stop washing leaves!" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                return true;
            }
            else
                return false;
        }
        
    public:
        PutStew()
        {
            this->available_stew = this->min_limit_stew_gr = 0.;
            this->activity = "";
            this->status =  this->notified = false;
        }
        
        PutStew(const float as, const unsigned int mlsg) :
                                    available_stew(as*1000),
                                    min_limit_stew_gr(mlsg),
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
        
        bool get_notified(void)
        {
            return this->notified;
        }
        
        std::chrono::seconds get_busy_time(void)
        { 
               return std::chrono::seconds(4);
        }

        std::chrono::seconds get_time_worked(unsigned int leaves_with_dough)
        {
            return std::chrono::seconds(4 * leaves_with_dough);
        }

        void set_available_stew(float grs_stew)
        {
            this->available_stew -= grs_stew;
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
            this->set_status(true);
            while(this->get_available_stew() > min_stew_gr+increment_of_stew)
            {
                grs_stew = this->amount_stew_to_putting();
                this->set_available_stew(grs_stew);

                this->set_activity("putting up");
                std::cout << "I am " << this->get_activity() <<" "<< grs_stew << " grs of stew!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->set_activity("closing");
                std::cout << "I am " << this->get_activity() << " the hallaca!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                if(!this->get_notified())
                    this->set_notified(this->stop_washing_leaves());
            }
            set_status(false);
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
