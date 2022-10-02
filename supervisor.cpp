#include <iostream>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t processes_id[4], child;
    std::string processes[] = 
        {
            "./washleaves.out",
            "./putdough.out",
            "./putstew.out",
            "./tieuphallaca.out"
        };
    
    size_t pos = 0;
    short i;
    
    std::cout << "Invoke processes:" << std::endl;
    for (i = 0; i < 4; i++)
    {
        pos = processes[i].find("./");
        std::cout << "\t" << processes[i].substr(pos) << std::endl;
        switch (child = fork())
        {
            case -1:
            {
                return -1;
            }
            case 0:
            {
                execlp("/bin/qterminal",
                       "/bin/qterminal",
                       "-e",
                       processes[i].c_str(),
                       NULL);
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
            default:
            {
                processes_id[i] = child;
            }
        }
    }
    
    int times_to_kill = 5;
    bool running = true;
    while(running)
    {
        printf("Waiting times to kill processes: %i\n", times_to_kill);
        std::this_thread::sleep_for(std::chrono::seconds(times_to_kill--));
        if(times_to_kill <= 0)
        {
            for(i = 0; i < 4; i++)
            {
                pos = processes[i].find("./");
                std::cout << "Killing process "
                          << processes[i].substr(pos)
                          << std::endl;
                kill(processes_id[i],SIGTERM);
            }
            running = false;
        }
    }
    return EXIT_SUCCESS;
}
