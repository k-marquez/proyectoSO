#include <iostream>
#include <chrono>
#include <thread>

// Entries min and increment_of_minimun are in gr, output are in gr
float amount_stew_to_putting(int min_gr, int max_gr)
{
    return min_gr + rand() % max_gr; 
}

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

int main(int argc, char *argv[])
{
    unsigned int leaves_with_dough = 15,
                           for_tie_up = 0;
    float available_stew = atof(argv[1]),
              min_limit_stew_gr = to_kg(atof(argv[2])),
              min_stew_kg = to_kg(min_limit_stew_gr),
              min_stew_gr = 80,
              increment_of_stew = 15,
              grs_stew = 0;
    bool status = true;
    
    while(leaves_with_dough and available_stew > 0.)
    {
        leaves_with_dough--;
        
        grs_stew = amount_stew_to_putting(min_stew_gr,increment_of_stew);
        if(to_kg(grs_stew) > available_stew)
            grs_stew = to_gr(available_stew);
            
        available_stew -= to_kg(grs_stew);
        std::cout << (status ? "Busy: " : "Not busy.")
                        << "I am putting up " << grs_stew
                        << "grs of stew!"<< std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        std::cout << (status ? "Busy: " : "Not busy.") << "I am closing the hallaca!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(4));
        
        if (available_stew <= min_limit_stew_gr)
        {
             std::cout << (status ? "Busy: " : "Not busy.")
                             << "Sending notification to stop washing leaves!"
                             << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        
        for_tie_up++;
        std::cout << "Kg of stew: " << available_stew << std::endl;
    }  
    return EXIT_SUCCESS;
}
