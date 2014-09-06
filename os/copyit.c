#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

int copy(char *filename, char *targetname)
{
    char buffer[5000] = {};
    int fd = 0;
    int fdw = 0;
    int counter = 0;

    fd = open(filename, O_RDONLY, 0);
    if (fd < 0)
    {
        printf("unable to open %s: %s\n", filename, strerror(errno));
	exit(1);
    }

    fdw = open(targetname, O_CREAT | O_WRONLY, 0);
    if (fdw < 0)
    {
        printf("unable to open %s: %s\n", targetname, strerror(errno));
	exit(1);
    }

    while (1)
    {
        int result = read(fd, buffer, 4096);
	if (result == 0)
	    break;
        else if (result < 0)
        {
	    printf("read error!\n");
            exit(1);
        }

	int ret_w = write(fdw, buffer, 4096);
        if (ret_w < 0)
	{
	    printf("unable to write %s: %s\n", targetname, strerror(errno));
	    exit(1);
	}

        counter += ret_w;
    }
    
    close(fd);
    close(fdw);
    printf("copit: Copied %d bytes from file %s to %s\n", counter, filename, targetname);
    printf("copy success!\n");
}

void display_message(int s)
{
    printf("copyit: still copying...\n");
    alarm(1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("bad arguments\n");
	printf("usage: copyit <sourcefile> <targetfile>\n");
	exit(1);
    }

    signal(SIGALRM, display_message);
    alarm(1);	
    copy(argv[1], argv[2]);
}
