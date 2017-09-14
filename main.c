/*
 * Jason Natale
 * CSC173 C Project Weeks 3+4
 * Calculator
 * 9/30/15
 *
*/

#include <stdlib.h>
#include <stdio.h>

#include "scanner.h"
#include "parser.h"

int main (int argc, char *argv[]) {
    FILE *fp;
    
    if (argc < 2) {
        printf("Usage: ./calculator infile.txt\n");
        exit(1);
    }
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error: infile.txt does not exist.\n");
        exit(1);
    }
 
    //scan file into linked list of tokens
    ll_node head = scan_file(fp);

    //run through list, adjusting POS and NEG tokens that need to be altered
    scan_list(head);
    
    //parse and solve equations with CFG
    parse_list(head);

    exit(0);
}
