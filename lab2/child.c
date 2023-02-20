#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
    pid_t pid = getpid();
    printf("i'm %s, my PID=%d \n\n", argv[0], pid);

    FILE *fp;
    fp = fopen(argv[1], "r");
    
    char* key = (char*) malloc(255*sizeof(char));
    while(!feof(fp))
    {
        fscanf(fp, "%s", key);
        printf("%s = %s\n", key, getenv(key));
    }

    fclose(fp);

    return 0;
}