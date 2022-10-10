/**
 * @file memorykey.h
 * @authors Kevin Márquez V023391555 Lewis Ochoa V026373847
 * @date 10/10/2022
 */

#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sched.h>

#define SHRMKEY 25
#define SHRMBUSYKEY 52
#define SHRMFILE "/bin/ls"

void clear_terminal(void)
{
    pid_t child;
    switch(child = fork())
    {
        case 0:
        {
            execlp("/usr/bin/clear", "clear", NULL);
            exit(1);
        }
        default:
        {
            int stat_val;
            waitpid(child, &stat_val, 0);
        }
    }
}
