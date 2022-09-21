#include <iostream>
#include <chrono>
#include <thread>

class TieUpHallaca
{
    private:
        unsigned int finished_hallacas,
                              for_tie_up;
        std::string activitie;
        bool status;
    public:
        TieUpHallaca()
        {
            this->finished_hallacas = this->for_tie_up = 0;
            this->activitie = "tie up";
            this->status = false;
        }
        
        TieUpHallaca(const unsigned int fh, const unsigned int ftu) :
                                    finished_hallacas(fh),
                                    for_tie_up(ftu),
                                    activitie("tie up"),
                                    status(false) {}
                                    
        unsigned int get_finished_hallacas(void)
        {
            return this->finished_hallacas;
        }
        
        unsigned int get_for_tie_up(void)
        {
            return this->for_tie_up;
        }
        
        std::chrono::minutes get_time(void)
        {
            //Se debe recalcular
            return std::chrono::minutes(15);
        }
        
        void run()
        {
            this->status = true;
            while(for_tie_up > 0)
            {
                std::cout << "I am "<< this->activitie << " Hallacas!"<< std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
                this->finished_hallacas++;
                this->for_tie_up--;
            }
        }
};

int main(int argc, char *argv[])
{    
    TieUpHallaca carlos = TieUpHallaca(0, 4);
    std::cout << "Finished Hallacas: "<< carlos.get_finished_hallacas() << std::endl;
    std::cout << "For tie up: "<< carlos.get_for_tie_up() << std::endl;
    carlos.run();
    std::cout << "Finished Hallacas: "<< carlos.get_finished_hallacas() << std::endl;
    std::cout << "For tie up: "<< carlos.get_for_tie_up() << std::endl;
    
    return EXIT_SUCCESS;
}
