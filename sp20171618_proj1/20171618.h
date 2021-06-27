#define MAX_COMMAND_SIZE 100
#define MEMORY_SIZE 1048576 //16*65536

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

//shell
typedef struct history_node{
	struct history_node *link;
	int command_count;
	char command[MAX_COMMAND_SIZE];
}history_node;

history_node *history_head;

char* remove_space();
void help();
void dir();
void quit();
void add_history();
void print_history();

//memory
char memory[MEMORY_SIZE];
int last_add=0;

int dump();
int print_dump();
void print_dump_sub();
int edit();
int fill();
void reset();
int valid_address();
int valid_range();
int change_str_to_hexa();


//opcode
typedef struct optable{
	int opcode;
	char operation[10];
	int format;
	struct optable* link;
}optable_node;

optable_node** op_head;

int opcode_mnemonic();
void opcodelist();
int read_opcode_text();
int hash_function();

