/*
 * Jason Natale
 * CSC173 C Project Weeks 3+4
 * Calculator
 * parser.c
 * 9/30/15
 */

#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "scanner.h"

//global variable
ll_node* Ref;

//set global variable
void setRef(ll_node* ref){
    Ref = ref;
}


//parse through linked list with CFG and solve expressions
void parse_list(ll_node head){
    //convert to pointer and set
    setRef(&head);
    //perform recursive descent onto A
    float sol = parse_A();
    //match semicolon at end of expression
    consume();
    //print output to file
    printf("==%f\n\n",sol);
    //recursive descent into list prime
    parse_list_prime();
}

//parse a single expression
float parse_A(){
    //RD to B
    float v = parse_B(0.0);
    //RD to A prime
    float fin = parse_A_prime(v);
    //return expressions solution
    return fin;
}

//parse extra expressions
void parse_list_prime(){
    //either take EOFT or continue RD
    if(Ref->tok.tc == EOFT){
    }
    else{
	//solve another expression and send output
	float sol = parse_A();
	consume();
	printf("==%f\n\n",sol);
	parse_list_prime();	
    }
}

//parse extension to expression
float parse_A_prime(float num){
    float v;
    switch(Ref->tok.tc){
        case ADD:
	    consume();
	    v = parse_B();
	    //perform addition
	    v = v+num;
	    //RD into itself
	    return parse_A_prime(v);
	case SUBT:
	    consume();
	    v = parse_B();
	    //perform subtraction
	    v = num-v;
            return parse_A_prime(v);
	default:
	    //no addition or subtraction next
	    //just return value
	    return num;
    }
}

//values to be added/subtracted later
float parse_B(){
    //RD into C
    float v = parse_C();
    //RD into B prime
    return parse_B_prime(v);
}

//extension to B
float parse_B_prime(float num){
    float v;
    switch(Ref->tok.tc){
	case MULT:
	    //move to next node
	    consume();
	    //RD to C
	    v = parse_C();
	    //multiplication
	    v = v * num;
	    return parse_B_prime(v);
	case DIV:
	    consume();
	    v = parse_C();
	    //operation needed
	    v = num / v;
	    return parse_B_prime(v);
	case MOD:
	    consume();
	    v = parse_C();
	    //convert to ints first
	    int x = (int)v;
	    int y = (int)num;
	    v = y % x;
	    return parse_B_prime(v);
	default:
	    //no matches with continuation return value
	    return num;
    }
}

//parsing values to be multiplied/divided
float parse_C(){
    int v;
    //either there will be a (POS), (NEG), or move down RD immediately
    if(Ref->tok.tc == LP && (Ref->next->tok.tc == POS || Ref->next->tok.tc == NEG)){
	//case with a negative
	if(Ref->next->tok.tc == NEG){
	    //get rid of LP, NEG, RP
	    consume();
	    consume();
	    consume();
	    v = parse_C_prime();
	    switch(v){
		//positive negates to negative
		case 0:
		    v = 1;
		//negative negates to positive
		case 1:
		    v = 0;
	    }
	}
	//case with positive
	else{
	    //get rid of next three tokens
	    consume();
	    consume();
	    consume();
	    v = parse_C_prime();
	}
	//get result from D and perform unary
	float v2 = parse_D();
	if(v == 1)
	    return v2;
	else
	    return v2*(-1);
    }
    else{
	return parse_D();
    }
}

//continuation of C for multiple unary operations
int parse_C_prime(){
    //case with negation
    if(Ref->tok.tc == LP && Ref->next->tok.tc == NEG){
	consume();
	consume();
	consume();
	int v = parse_C_prime();
	//perform negation on value
	if(v == 0)
	    v = 1;
	else
	    v = 0;	    
	return v;
    }
    //case of positive
    else if(Ref->tok.tc == LP && Ref->next->tok.tc == POS){
        consume();
	consume();
	consume();
	int v = parse_C_prime();
	return v;
    }
    //exit recursive loop
    else
	return 1;
}

//values which can be negated in higher functions
float parse_D(){
    //either increment case, decrement case or neither
    if(Ref->tok.tc == LP && Ref->next->tok.tc == INC){
        consume();
	consume();
	consume();
	float v = parse_D_prime();
	float v2 = parse_E();
	//increment
	v+=1;
	return v + v2;
    }
    else if(Ref->tok.tc == LP && Ref->next->tok.tc == DEC){
        consume();
	consume();
	consume();
	float v = parse_D_prime();
	float v2 = parse_E();
	//decrement
	v-=1;
	return v + v2;
    }
    else{
	return parse_E();
    }
}

//continuation of increments and decrements
float parse_D_prime(){
    //case of an increment
    if(Ref->tok.tc == LP && Ref->next->tok.tc == INC){
	consume();
	consume();
	consume();
	float v = parse_D_prime();
	//increment
	return ++v;
    }
    else if(Ref->tok.tc == LP && Ref->next->tok.tc == DEC){
	consume();
	consume();
	consume();
	float v = parse_D_prime();
	//decrement
	return --v;
    }
    //no more increments and decrements
    else
	return 0;
}

//parsing values to be incremented at higher levels
float parse_E(){
    float v,y;
    //expression in closed paren
    if(Ref->tok.tc == LP){
	//get rid of LP
        consume();
	//get value
	v = parse_A();
	//get rid of RP
	consume();
	//handle postfix inc/dec
	y = parse_E_prime();
	v += y;
    }
    else{
	//working with value, turn string into float
	v = atof(Ref->tok.value);
	consume();
	//handle postfix inc/dec
	y = parse_E_prime();
	v += y;
    }
    //return value
    return v;
}

//handling postfix inc and dec
float parse_E_prime(){
    float v;
    if(Ref->tok.tc == LP && (Ref->next->tok.tc == INC || Ref->next->tok.tc == DEC)){
	//get rid of tokens
	consume();
	if(Ref->tok.tc == INC)
	    v = 1;
	else 
	    v = -1;
	consume();
	consume();
	v = v + parse_E_prime();
	//case where next token is RP, post operation is relevant
	if(Ref->tok.tc == RP)
	    return v;
	else
	    return 0;
    }
    return 0;
}

//method for moving through linked list
void consume(){
    //switch statement to output current token to Stdout
    switch(Ref->tok.tc){
	case VAL:
	    //print entire string except for '\n' at end
	    fputs(Ref->tok.value,stdout);
	    break;
	case ADD:
	case POS:
	    printf("+");
	    break;
	case SUBT:
	case NEG:
	    printf("-");
	    break;
	case MULT:
	    printf("*");
	    break;
	case DIV:
	    printf("/");
	    break;
	case MOD:
	    printf("%%");
	    break;
	case INC:
	    printf("++");
	    break;
	case DEC:
	    printf("--");
	    break;
	case LP:
	    printf("(");
	    break;
	case RP:
	    printf(")");
	    break;
	case ENDE:
	    printf(";\n");
	    break;
    }
    //move over one node
    Ref = Ref->next;
}
