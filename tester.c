#include <stdio.h>
#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>


    typedef unsigned long long Address_t;

    struct cmdLine_parameters
    {
        unsigned h, v, s, E, b;
        char *file;
    };
    struct cmdLine_parameters cmd;


    typedef struct Linee{
        Address_t tag;
        unsigned age;/*each time the set to which
        this lines belongs is accessed, if this line
        doesn't have the  tag, we increment the count.
        The line with largest count in a set is the
        least recelt used*/
        char valid; // 1 is valid; 0 is invalid
         
    } Line;




    typedef Line* Set_t;

    Set_t* cache[1];



int main(int argc, char const *argv[])
{
    Line a,b;
    a.age = 4;
    b.age = 5;

    Set_t sett[2] = {&a, &b};

    cache[0] = sett;

    printf("%d\n", sett[1]->age); 
    printf("%d\n", cache[0][1]->age);
    return 0;
}