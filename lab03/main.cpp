#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

// #include <assert.h>
// #include <stdbool.h>
// #include <stdint.h>
// #include <errno.h>


using namespace std;

void oli()
{
    int i = 0;

    while (true)
    {
        // Pro omezeni bufferovani zmenit nastveni / newline
        // printf("%d\n", i++);
        fprintf(stdout, "%d\n", i++);
        
        usleep(100);
    }
}

void wakeUpCall(int sig)
{
    printf("Vstavej!\n");

    alarm(2);
}

void oli(int cnst)
{
    struct sigaction signal;
    signal.sa_handler = wakeUpCall;
    signal.sa_flags = 0;
    sigemptyset(&signal.sa_mask);

    //Muzu volat vicekrat pro vicero signalu a resim si je vramci 1 funkce -> wakeUpCall
    sigaction(SIGALRM, &signal, nullptr);

    alarm(5);

    char buf[128];

    printf("Zadej heslo:\n");
    int ret = read(0, buf, sizeof(buf));
    printf("Read result: %d %s\n", ret, strerror(errno));

    // while(true)
    // {
    //     printf("text to print\n");
    //     sleep(1);
    // }
}

//Kelvin stuff

char str[] = "-\\|/";
int i = 0;
int t = 0;

void vrtule_1(int prt)
{
    while (t < 1000)
    {
        i = (i+1)%4;

        switch (prt)
        {
            case 0:
            {
                printf("%c\r", str[i]);
                fflush(stdout);
            }break;
            case 1:
            {
                setvbuf(stdout, nullptr, _IONBF, 0);
                printf("%c\r", str[i]);
            }break;
        }
        
        usleep(10000);
        t++;
    }

    t=0;
}

void sigVert(int sig)
{
    i = (i+1)%4;

    printf("%c\r", str[i]);
    fflush(stdout);
}

void vrtule_2(int TimePeriod)
{
    struct sigaction signal;
    signal.sa_handler = sigVert;
    signal.sa_flags = 0;

    //Muzu volat vicekrat pro vicero signalu a resim si je vramci 1 funkce -> wakeUpCall
    sigaction(SIGALRM, &signal, nullptr);

    struct itimerval alarm = { { 0, TimePeriod * 1000 }, { 0, TimePeriod * 1000 } };
    setitimer( ITIMER_REAL, &alarm, NULL );

    printf("%c\r", str[i]);
    fflush(stdout);
    
    while (true)
    {
        getchar();
    }
}

void childCode(int trm)
{
    char terminal[11];
    sprintf(terminal, "/dev/pts/%d", trm);
    cout << "Child " << getpid() << ", trm " << trm << ", terminal " << terminal << endl;

    FILE *fd = fopen(terminal, "r+");

    while (true)
    {
        i = (i+1)%4;

        fprintf(fd, "%c\r", str[i]);
        fflush(fd);

        usleep(100000);
    }
}

void vrtule_3(int count)
{
    vector<pid_t> children = vector<pid_t>();
    vector<pid_t> stopped = vector<pid_t>();

    for (size_t i = 0; i < count; i++)
    {
        pid_t child = fork();

        if (child == 0)
        {
            //Child code
            childCode(i);
        }
        else
        {
            //Parent code
            children.push_back(child);
        }
    }

    while (true)
    {
        //Do Signals
        
        int tmp;

        for (int i = 0; i < count; i++)
        {
            tmp = kill(children[i], SIGSTOP);
            printf("%d: stopped   with result %d\r", i, tmp);
            fflush(stdout);

            sleep(1);
        }

        for (int i = 0; i < count; i++)
        {
            tmp = kill(children[i], SIGCONT);
            printf("%d: continued with result %d\r", i, tmp);
            fflush(stdout);

            sleep(1);
        }
    }
}

int main(int argc, char **argv)
{
    // oli(0);
    // cout << "Vrtule 1 - 1" << endl;
    // vrtule_1(1);

    cout << "Vrtule 2 - 100" << endl;
    vrtule_2(100);

    // cout << "Vrtule 3 - 6" << endl;
    // vrtule_3(6);

    return 0;
}
