/*
 * Jason Natale
 * CSC C Project Weeks 3+4
 * Calculator
 * scanner.h
 * 9/30/15
 */

#ifndef SCANNER_H
#define SCANNER_H

typedef enum {
    VAL,       //represents any numerical value, float or int
    ADD, SUBT, // addition and subtraction operators
    MULT, DIV, // multiplication and division operators
    MOD,       // modulo operator
    INC, DEC,  // increment and decrement (++) (--)
    LP, RP,    // left parantheses and right parantheses
    POS, NEG,  // unary operators to represent positive and negative values
    ENDE,      // semicolon for end of expression
    EOFT       // eof token
} token_class;

//each token will have a type and values a numerical value
typedef struct{
    token_class tc;
    char* value;
} token_t;

//single node to make up linked list of tokens
typedef struct ll_node{
    struct ll_node* next;
    token_t tok;
} ll_node;

//all possible states in DFA
typedef enum{
    s0, s1, s2, s3, fin
} state;

//turns file into linked list of tokens
ll_node scan_file(FILE *fp);
//scans single token and creates node
ll_node* scan(FILE *fp);
//utilizes DFA to produce token
token_t* get_token(FILE *fp);
//create a single node with a token
ll_node* new_node(token_t* tokptr);
//make adjustments to initially created linked list
void scan_list(ll_node head);

#endif
