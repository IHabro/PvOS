#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

//Vlakna

void *vlakno(void* param)
{
    char *text = (char*) param;

    cout << text << "\t: pid " << getpid() << ", tid " << gettid() << ", self " << pthread_self() << endl;

    char* result = new char[128];
    sprintf(result, "%s: %d", text, gettid());

    return result;
}

int main_vlakna()
{
    pthread_t vl1, vl2;
    char text[128] = "Karel";

    cout << "main\t: pid " << getpid() << ", tid " << gettid() << ", self " << pthread_self() << endl;

    pthread_create(&vl1, nullptr, vlakno, text);
    strcpy(text, "Vaclav");
    pthread_create(&vl2, nullptr, vlakno, text);

    char *rtn1, *rtn2;

    pthread_join(vl1, (void **) &rtn1);
    pthread_join(vl2, (void **) &rtn2);

    cout << "rtn1 " << rtn1 << ", rtn2 " << rtn2 << endl;

    return 0;
}

//Signaly

void sighandler(int sig)
{
    cout << "Prisel signal " << sig << endl;
}

int main_signaly()
{
    struct sigaction sigs;
    sigs.sa_flags = 0;
    sigs.sa_handler = sighandler;
    sigemptyset(&sigs.sa_mask);

    sigaction(SIGUSR1, &sigs, nullptr);

    while(true)
    {
        cout << "..." << endl;
        sleep(1);
    }

    return 0;
}

//Ukoly

void childCode()
{
    sleep(10);

    exit(1);
}

void ukol1()
{
    int number = 0;

    while(true)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            //Child
            childCode();
        }
        else if (pid > 0)
        {
            //Parent child created
            cout << "Children #" << number++ << endl;
        }
        else
        {
            //Parent child not created

            cout << "No more children can be created" << endl;

            break;
        }
    }

    cout << "Engage sleep activity" << endl;
    sleep(11);
    cout << "Begin waiting for children?" << endl;
    getchar();

    int status = -1;

    while (true)
    {
        pid_t endedPid = waitpid(-1, &status, WNOHANG);

        cout << "End #" << number-- << ", pid " << endedPid << endl;

        if (endedPid == -1)
        {
            cout << "All children ended terminating Parent" << endl;

            break;
        }
    }
    
    cout << "End of 1" << endl;
    getchar();
}

void *vlkn2(void * param)
{
    while ( 1 )
    {
        sleep( 1 );
    }

    return nullptr;
}

void ukol2()
{
    int number = 0, status = 0;

    while(true)
    {
        pthread_t vl;
        status = pthread_create(&vl, nullptr, vlkn2, nullptr);

        cout << "Thread #" << number++ << endl;

        if (status != 0)
            break;
    }

    cout << "End of 2, kill Main thread?" << endl;
    getchar();
}

void *vlkn3(void * param)
{
    return nullptr;
}

void ukol3()
{
    int number = 0, status = 0;

    while(true)
    {
        pthread_t vl;
        status = pthread_create(&vl, nullptr, vlkn3, nullptr);

        cout << "Thread #" << number++ << endl;

        if (status != 0)
            break;
    }

    cout << "End of 3, kill Main thread?" << endl;
    getchar();
}

int signals = 0, eCh = 0, iters = 0, status;

void dayCare(int sig)
{
    signals++;

    while(true)
    {
        pid_t result = waitpid(-1, &status, WNOHANG);

        if(result == 0)
        {
            break;
        }

        if(result > 0)
        {
            eCh++;
        }
    }
}

void child()
{
    srand(time(NULL));

    sleep(rand() % 10);

    //Child Code
    switch (rand() % 5)
    {
        case 0:
        {
            exit(0);
        }break;
        case 1:
        {
            exit(1);
        }break;
        case 2:
        {
            int a = (rand() % 255), b =(rand() % 255);
            int c = a/b;
            exit(2);
        }break;
        case 3:
        {
            int a = (rand() % 255);
            int c = a/0;
            exit(3);
        }break;
        case 4:
        {
            int* c = nullptr;
            *c=5;
            exit(4);
        }break;
        case 5:
        {
            int a = (rand() % 255), b =(rand() % 255);
            int c = a%b;
            exit(5);
        }break;      
    }

    exit(-1);
}

void ukol4()
{
    time_t old_time = 0;
    int numberOfProcesses = 1000, status, T = 10;
    float timeToSleep = T;

    timeToSleep /= 2;
    timeToSleep /= numberOfProcesses;
    timeToSleep *= 1000000;

    std::cout << "N " << numberOfProcesses << endl;
    std::cout << "T " << T << endl;
    std::cout << "TIME TO SLEEP " << timeToSleep << endl;

    //SIGNAL SETUP
    struct sigaction sigs;
    sigs.sa_flags = 0;
    sigs.sa_handler = dayCare;
    sigemptyset(&sigs.sa_mask);

    sigaction(SIGCHLD, &sigs, nullptr);

    cout << "Start the process" << endl;
    getchar();

    while (true)
    {
        if (fork() == 0)
        {
            child();
        }

        //Parent code
        usleep(timeToSleep);

        if(old_time < time(NULL))
        {
            old_time = time(NULL);
            cout << "Signals " << signals << ", Children " << eCh << endl;
        }
    }

    cout << "End of 4, kill Main thread and all Children?" << endl;
    getchar();
}

int main(int argc, char **argv)
{
    // ukol1();
    // ukol2();
    // ukol3();
    ukol4();

    return 0;
}
