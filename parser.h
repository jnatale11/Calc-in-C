/*
 * Jason Natale
 * CSC173 C Project Weeks 3+4
 * Calculator
 * parser.h
 * 9/30/15
 */

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

//implementation of CFG
void parse_list(ll_node head);
//parse each expression
float parse_A();
//recursively add expressions to be parsed
void parse_list_prime();
//parse extension to A
float parse_A_prime(float num);
//parse value to be added/ subtracted
float parse_B();
//parse extension to b
float parse_B_prime(float num);
//parse value to be multiplied/divided
float parse_C();
int parse_C_prime();
//parse values to be negated
float parse_D();
float parse_D_prime();
//parse values to be prefix increment/decrement
float parse_E();
float parse_E_prime();
//'consume' token and move through global linked list
void consume();
#endif
