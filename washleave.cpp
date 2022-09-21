#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    unsigned int leaves = 0;
    bool cond_stop = false;
    
    while(!cond_stop)
    {
        std::cout << "I am washing leaves!"<< std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "I am roast leaves!"<< std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(4));
        std::cout << "I am cutting up leaves!"<< std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        leaves++;
    }   
    return EXIT_SUCCESS;
}
