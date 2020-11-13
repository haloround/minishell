#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int main()
{
    time_t *timep = malloc(sizeof(*timep));
    time(timep);
    char *s = ctime(timep);
    printf("%s",s);
    return 0;  
}
