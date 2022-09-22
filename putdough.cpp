#include <iostream>
#include <chrono>
#include <thread>

//Temporary stack of leaves with dough and leaves washed
unsigned int Leaves_With_Dough = 0, Leaves_Washed = 10;

class PutDough
{
    private:
        unsigned int count_leaves_whith_dough;
        std::string activity;
        bool status;
        
    public:
        PutDough()
        {
            this->count_leaves_whith_dough = 0;
            this->activity = "";
            this->status = false;
        }

        unsigned int get_count_leaves_with_dough(void)
        {
            return this->count_leaves_whith_dough;
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

        std::chrono::seconds get_time_worked(void)
        {
            return std::chrono::seconds(6 * this->get_count_leaves_with_dough());
        }
        
        void increment_count_leaves_with_dough(void)
        {
            this->count_leaves_whith_dough++;
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
            unsigned int time_for_waiting = 2;
            while(true)
            {
                this->set_status(true);
                while(Leaves_Washed > 0)
                {
                    Leaves_Washed--;
                    
                    this->set_activity("greasing");
                    std::cout << "I am " << this->get_activity() << " leaves!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    
                    this->set_activity("putting up dough");
                    std::cout << "I am " << this->get_activity() << " to leaves!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(4));
                    
                    Leaves_With_Dough++;
                    this->increment_count_leaves_with_dough();
                }

                this->set_status(false);
                this->set_activity("");
                while(Leaves_Washed <= 0)
                {
                    std::cout << "I am not busy, waiting for leaves. Hurry up!"<< std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(time_for_waiting++));
                }
                time_for_waiting = 2;
            }
                
        } 
};

int main(int argc, char *argv[])
{
    PutDough lucas = PutDough();
    lucas.run();
}
