#include <iostream>
#include <chrono>
#include <thread>

#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid_wl;
    int status;
    std::cout << "Running washleaves:" << std::endl;
    pid_wl = fork();
    switch (pid_wl) {
      case -1:
      {    
          perror("fork");
          break;
      }
      case 0:
      {    
          std::string cmd = "qterminal -e sh -c ./washleaves.out";
          system(cmd.c_str());
          //execl("./washleaves.out", (char *) 0);
          perror("exec");
          break;
      }
      default:
      {
          int times_to_kill = 3;
          while(true)
          {
              printf("Washleaves pid: %i\n", pid_wl);
              std::this_thread::sleep_for(
                  std::chrono::seconds(times_to_kill--));
              if(times_to_kill <= 0)
              {            
                  std::cout << "Killing process washleaves" << std::endl;
                  kill(pid_wl,SIGSTOP);
                  break;
              }
                  
          }
        break;
      }
    }
    return EXIT_SUCCESS;
}
