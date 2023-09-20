#include <iostream>
#include <vector>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

void childCode(int T)
{
    srand(time(NULL));

    sleep(rand() % T);

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

void priklad_1(int numP)
{
    int numberOfProcesses = numP;

    srand(time(0));

    vector<pid_t> children = {};
    pid_t currentChild = 0;
    int currentIndex = 0;

    for (size_t i = 0; i < numberOfProcesses; i++)
    {
        currentChild = fork();
        currentIndex = i;
        children.push_back(currentChild);
        //std::cout << children[i] << endl;

        if (currentChild == 0)
            break;
    }

    if (currentChild == 0)
    {
        //Child Code
        childCode(10);
    }
    else
    {
        int tocka = 1;

        vector<int> results = {-5, -5, -5, -5, -5};
        
        //Parent Code
        while (tocka < 15)
        {
            usleep(100000);

            //std::cout << "Round " << tocka << endl;
            //std::cout << "----------------------------------------------" << endl;
            for (size_t i = 0; i < children.size(); i++)
            {
                int status;
                pid_t result = waitpid(children[i], &status, WNOHANG);

                //std::cout << "Child: " << i << " has status: " << status << " and result: " << result << endl;

                if(result == children[i])
                {
                    results[i] = status;
                }
            }
            //std::cout << "----------------------------------------------" << endl;

            tocka++;
        }

        std::cout << "Complete Results" << endl;
        for (size_t i = 0; i < results.size(); i++)
        {
            std::cout << "Child " << i << " ended with " << results[i] << endl;
            std::cout << "Breakdown by macros:" << endl;
            std::cout << "\tTerminated normally?: " << WIFEXITED(results[i]) << endl;
            std::cout << "\tExit status: " << WEXITSTATUS(results[i]) << endl;
            std::cout << "\tTerminated by signal?: " << WIFSIGNALED(results[i]) << endl;
            std::cout << "\tTermination signal number: " << WTERMSIG(results[i]) << endl;
            std::cout << "\tCore dumped?: " << WCOREDUMP(results[i]) << endl;
            std::cout << "\tStopped by signal?: " << WIFSTOPPED(results[i]) << endl;
            std::cout << "\tStop signal number: " << WSTOPSIG(results[i]) << endl;
            std::cout << "\tResumed by signal?: " << WIFCONTINUED(results[i]) << endl;
        }
    }
}

int main(int argc, char **argv)
{
    //input test
    if(argc < 3)
	{
		printf("Not enough arguments.\n");

		return -1;
	}
	else
	{
		printf("Program arguments: ");

		for(int i = 0;i < argc;i++)
		{
			printf("%s, ", argv[i]);
		}

		printf("\n");
	}

    //priklad_1(stoi(argv[1]));

    int numberOfProcesses = stoi(argv[1]), status, T = stoi(argv[2]);
    float timeToSleep = T;
    timeToSleep /= 2;
    timeToSleep /= numberOfProcesses;
    timeToSleep *= 1000000;

    std::cout << "N " << numberOfProcesses << endl;
    std::cout << "T " << T << endl;
    std::cout << "S " << status << endl;
    std::cout << "TIME TO SLEEP " << timeToSleep << endl;

    //Parent Code
    while (true)
    {
        if (fork() == 0)
        {
            childCode(T);
        }

        usleep(timeToSleep);

        while(true)
        {
            pid_t result = waitpid(-1, &status, WNOHANG);

            if(result == 0)
            {
                break;
            }

            if (result == -1)
            {
                std::cout << "ERROR" << endl;
                sleep(20);
            }

            std::cout << "PID: " << result << ", Breakdown by macros:" << endl;
            std::cout << "\tTerminated normally?: " << WIFEXITED(status) << endl;
            std::cout << "\tExit status: " << WEXITSTATUS(status) << endl;
            // std::cout << "\tTerminated by signal?: " << WIFSIGNALED(status) << endl;
            // std::cout << "\tTermination signal number: " << WTERMSIG(status) << endl;
            // std::cout << "\tCore dumped?: " << WCOREDUMP(status) << endl;
            // std::cout << "\tStopped by signal?: " << WIFSTOPPED(status) << endl;
            // std::cout << "\tStop signal number: " << WSTOPSIG(status) << endl;
            // std::cout << "\tResumed by signal?: " << WIFCONTINUED(status) << endl;
        }
    }

    return 0;
}
