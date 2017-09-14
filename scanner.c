/*
 * Jason Natale
 * CSC173 C Project Weeks 3+4
 * Calculator
 * scanner.c
 * 9/30/15
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "scanner.h"

//create a linked list of tokens for the entire file
ll_node scan_file(FILE *fp){
    //create 'head' node for linked list
    ll_node* head;
    //now use scan function to create token and node
    head = scan(fp);

    //know that >1 tokens exist so >1 nodes
    //therefore, can set head node as new and begin search for more
    ll_node* new = head;
    //need 'old' node to set when moving through list
    ll_node* old;
    //loop through until a node is reached with a token of EOF
    while(new->tok.tc != EOFT){
	//set new node to old
	old = new;
	//scan for new node
	new = scan(fp);
        //link the nodes
        old->next = new;
    }
    //return the starting node
    return *head;
}

//reads the next token and creates node
ll_node* scan(FILE *fp){
    //find next token
    token_t* newtok = get_token(fp);
    //create node struct
    ll_node* ret = new_node(newtok);
    //return node
    return ret;
}

//Performs analysis on input file using DFA to produce a single token
token_t* get_token(FILE *fp){
    //allocate memory for token struct
    token_t* tok;
    tok = (token_t*)malloc(sizeof(token_t));
    //set blank string to be filled
    char *string = malloc(sizeof(char*));
    int length = 0;
    //begin utilizing DFA to determine token_class and value
    //start state s0
    state currState = s0;
    //loop through reading characters until final state reached
    while(currState != fin){
	//read next character
	char* next = (char*)malloc(sizeof(char*));
	//fgets is NULL when eof read
	if(fgets(next,2,fp)==NULL){
	    tok->tc = EOFT;
	    break;}
	//account for each state
	switch(currState){
	    case s0:
		switch(next[0]){
		    case ')':
			tok->tc = RP;
			currState = fin;
			break;
		    case '(':
			tok->tc = LP;
			currState = fin;
			break;
		    case ';':
			tok->tc = ENDE;
			currState = fin;
			break;
		    case '+':
			tok->tc = POS;
			currState = fin;
			break;
		    case '-':
			tok->tc = NEG;
			currState = fin;
			break;
		    case '*':
			tok->tc = MULT;
			currState = fin;
			break;
		    case '/':
			tok->tc = DIV;
			currState = fin;
			break;
		    case '%':
			tok->tc = MOD;
			currState = fin;
			break;
		    case ' ':
		    case '\t':
		    case '\n':
			//continue looping when whitespace reached
			break;
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
		    case '0':
			tok->tc = VAL;
			string[0] = next[0];
			length+=1;
			currState = s1;
			break;
		    case '.':
			tok->tc = VAL;
			string[0] = next[0];
			length+=1;
			currState = s2;
			break;
		    default:
			printf("weird input ");
		}
		break;
	    case s1:
		switch(next[0]){
		    case '.':
			string[length] = next[0];
			length+=1;
			currState = s2;
			break;
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
		    case '0':
			string[length] = next[0];
			length+=1;
			break;
		    default:
			currState = fin;
			//place char back on buffer
			ungetc(next[0],fp);
			break;
		}
		break;
	    case s2:
		string[length] = next[0];
                length+=1;
		currState = s3;
		break;
 	    case s3:
		switch(next[0]){
		    case '1':
		    case '2':
		    case '3':
		    case '4':
		    case '5':
		    case '6':
		    case '7':
		    case '8':
		    case '9':
		    case '0':
			string[length] = next[0];
			length+=1;
			break;
		    default:
			currState = fin;
			//place char back on buffer
			ungetc(next[0],fp);
			break;
		}
	}
	//deallocate memory from string
        free(next);
    }
    //allocate memory for token value and set
    tok->value = (char*)malloc(sizeof(char*));
    tok->value = strdup(string);
    //deallocate memory for string
    free(string);
    //finally return the token which has been scanned
    return tok;
}

//allocate memory for single node
ll_node* new_node(token_t* tokptr){
    //memory allocation
    ll_node* new = (ll_node*)malloc(sizeof(ll_node*));
    //set tok variable
    new->tok = *tokptr;
    return new;
}

//adjusts linked list to account for adjacent positive and negative tokens
//also determine whether a sign is positive/addition or negative/subtraction
//and condense values of white space tokens 
void scan_list(ll_node head){
    //take reference node
    ll_node* ref = &head;
    //loop through all nodes
    while(ref->next){
	//case where two POS tokens in a row 
	//switch to INC and skip over second
	if(ref->tok.tc == POS && ref->next->tok.tc == POS){
	    ref->tok.tc = INC;
	    ref->next = ref->next->next;
	}
	//case with two NEG's now
	if(ref->tok.tc == NEG && ref->next->tok.tc == NEG){
	    ref->tok.tc = DEC;
	    ref->next = ref->next->next;
	}
	//move along list
	ref = ref->next;	
    }
    //set reference back to beginning
    ref = &head;
    
    //again loop through all nodes
    while(ref->next){
	//if a POS or NEG is encountered, check surroundings
	if(ref->next->tok.tc == POS || ref->next->tok.tc == NEG){
            switch(ref->tok.tc){
		//cases to make it either ADD or SUBT
		case VAL:
		case RP:
		    if(ref->next->tok.tc == POS)
			ref->next->tok.tc = ADD;
		    else
			ref->next->tok.tc = SUBT;
	    }
        }
        ref = ref->next;
    }
    //loop one more time to take care of negative values and positively marked
    //values
    ref = &head;
    while(ref->next){
	if(ref->tok.tc == NEG && ref->next->tok.tc == VAL){
	    //convert to one negative value token
	    ref->tok.tc = VAL;
	    //allocate memory and create new string of value
	    char* str = (char*)malloc(sizeof(ref->next->tok.value)+sizeof(char));
	    str[0] = '-';
	    int strlen = 1;
	    int valptr = 0;
	    while(ref->next->tok.value[valptr]){
		str[strlen] = ref->next->tok.value[valptr];
	        valptr+=1;
		strlen+=1;
	    }
	    ref->tok.value = str;
	    //lastly, skip over old value
	    ref->next = ref->next->next;	
	}
	//condensing a positive value
	if(ref->tok.tc == POS && ref->next->tok.tc == VAL){
	    ref->tok.tc = VAL;
	    ref->tok.value = ref->next->tok.value;
	    ref->next = ref->next->next;
	}
	ref = ref->next;
    }
}
