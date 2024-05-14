#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        return 1;
    }
    int pid = fork();
    if (pid < 0)
    {
        printf("fork failed\n");
        return 1;
    }
    else
    {
        if (pid == 0)
        {
            // child process
        	char* argv[] = { (char*)"python", (char*)"hw.py", NULL };
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
            execve("/usr/bin/python", argv, NULL);
			printf("execve failed\n");
            close(pipefd[1]);
        }
        else
        {
            // parent process
            close(pipefd[1]);
            int wstats;
            waitpid(pid, &wstats, 0);
            if (WIFEXITED(wstats))
            {
                printf("child process exited normally\n");
				 char buffer[20];
            	ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));
            	if (bytes_read < 0)
            	{
                	perror("read");
					close(pipefd[0]);
                	return 1;
           		}
				buffer[bytes_read] = '\0';
            	printf("%s\n", buffer);
           		close(pipefd[0]); // Close the read end of the pipe after reading
                return WEXITSTATUS(wstats);
            }
            else
            {
                printf("child process did not exit normally\n");
				close(pipefd[0]);
                return 1;
            }
        }
    }
}