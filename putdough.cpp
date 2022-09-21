#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    unsigned int leaves_with_dough = 0, leaves = 5;
    bool status = true;
    
    while(leaves)
    {
        leaves--;
        std::cout << (status ? "Busy: " : "Not busy.") << "I am greasing leaves!"<< std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << (status ? "Busy: " : "Not busy.") << "I am putting up dough to leaves!"<< std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(4));
        leaves_with_dough++;
    }   
    return EXIT_SUCCESS;
}
