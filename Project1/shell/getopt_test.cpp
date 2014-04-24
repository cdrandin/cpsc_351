#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int c;
char *filename;
extern char *optarg;
extern int optind, optopt, opterr;

int smain(int argc, char *argv[]) 
{
    // is "z:" then "-z <arg>" optarg = <arg>
	while ((c = getopt(argc, argv, ":abf:z:")) != -1) 
    {
        switch(c) 
        {
            // "-a"
        case 'a':
            printf("a is set\n");
            break;

            // "-b"
        case 'b':
            printf("b is set\n");
            break;
        case 'z':
            printf("Print: %s\n", optarg);
            break;

            // "-f <file_name>"
        case 'f':
            filename = optarg;
            printf("filename is %s\n", filename);
            break;

            // Missing argument after "-f "
        case ':':
            printf("-%c without filename\n", optopt);
            break;

            // No valid argument 
        case '?':
            printf("unknown arg %c\n", optopt);
            break;
        }
    }

    return 0;
}