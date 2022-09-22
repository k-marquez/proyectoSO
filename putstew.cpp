#include <iostream>
#include <chrono>
#include <thread>

// Convert amount grs to Kgs
float to_kg(float grs)
{
    return grs / 1000;
}
// Convert amount of Kgs to grs
float to_gr(float kgs)
{
    return kgs * 1000;
}

class PutStew
{
    private:
        unsigned int leaves_with_dough,
                              for_tie_up,
                              min_stew_gr,
                              increment_of_stew,
                              min_limit_stew_gr;
        float available_stew;
        std::string activitie;
        bool status, notified;
        
        // Generates a random amount of grs to put in dough
        float amount_stew_to_putting(void)
        {
            return this-> min_stew_gr + rand() % this->increment_of_stew; 
        }
        
        bool stop_washing_leaves(void)
        {
            //Aquí se debe enviar la notificación al proceso de lavado de hojas
            std::cout << (this->status ? "Busy: " : "Not busy.")
                             << "Sending notification to stop washing leaves!"
                             << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return true;
        }
        
    public:
        PutStew()
        {
            this->leaves_with_dough = this->for_tie_up = 0;
            this->available_stew = this->min_limit_stew_gr = 0.;
            this->min_stew_gr = this->increment_of_stew = 0.;
            this->activitie = "";
            this->status =  this->notified = false;
        }
        
        PutStew(const unsigned int lwd, const unsigned int ftu, const float as,
                        const unsigned int mlsg, const unsigned int msg,
                        const unsigned int ios) :
                                    leaves_with_dough(lwd),
                                    for_tie_up(ftu),
                                    available_stew(as),
                                    min_limit_stew_gr(mlsg),
                                    min_stew_gr(msg),
                                    increment_of_stew(ios),
                                    activitie(""),
                                    status(false),
                                    notified(false){}
                                    
        unsigned int get_for_tie_up(void)
        {
            return this->for_tie_up;
        }
        
        std::string get_activitie(void)
        {
            return this->activitie;
        }
        
        float get_available_stew(void)
        {
            return this->available_stew;
        }
        
        std::chrono::minutes get_busy_time(void)
        {
            //Se debe recalcular
            return std::chrono::minutes(4 * this->for_tie_up);
        }
        
        void run()
        {
            float grs_stew = 0.;
            this->status = true;
            while(this->leaves_with_dough and this->available_stew > 0.)
            {
                this->leaves_with_dough--;
                
                grs_stew = amount_stew_to_putting();
                if(to_kg(grs_stew) > this->available_stew)
                    grs_stew = to_gr(this->available_stew);
                    
                this->available_stew -= to_kg(grs_stew);
                this->activitie = "putting up";
                std::cout << (status ? "Busy: " : "Not busy.")
                                << "I am  "<< this->activitie << " " << grs_stew
                                << " grs of stew!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                this->activitie = "closing";
                std::cout << (status ? "Busy: " : "Not busy.") << "I am "
                                << this->activitie << " the hallaca!" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                
                if (this->available_stew <= to_kg(this->min_limit_stew_gr) and not notified)
                {
                    this->notified = true;                
                     this->stop_washing_leaves();
                }
                
                this->for_tie_up++;
            }
        }  
};

int main(int argc, char *argv[])
{
    PutStew kevin = PutStew(13, 0, atof(argv[1]), atof(argv[2]), 80, 15);
    std::cout << "Hallacas for tie up: " << kevin.get_for_tie_up() << std::endl;
    std::cout << "Kg of stew: " << kevin.get_available_stew() << std::endl;
    kevin.run();
    std::cout << "Hallacas for tie up: " << kevin.get_for_tie_up() << std::endl;
    std::cout << "Kg of stew: " << kevin.get_available_stew() << std::endl;    
    return EXIT_SUCCESS;
}
