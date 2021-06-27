#define MAX_COMMAND_SIZE 100
#define MEMORY_SIZE 1048576 //16*65536
#define MAX_ASM_LINE 100
#define MAX_OBJ_LENGTH 80

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>

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
int type_filename();

//memory
unsigned char memory[MEMORY_SIZE];
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


//assembler
typedef struct source_statement{	//global array that contains loc, label, menmonic, operand, object code
	int loc;
	char label[10]; 
	int opcode;
	int format;
	char mnemonic[10];
	char operand[10];
	int obcode;
	int instruction_type;	// type0: comment type1: label type2: no label
	char comment[60];	//for storing comment line
}source_statement;

typedef struct sym_tab{
	int loc;
	char label[10];
}sym_tab;

int asm_line_count;
source_statement instruction[MAX_ASM_LINE] = {0};
sym_tab symtab[MAX_ASM_LINE] = {0};	
int program_length;
int symtab_count;
int BASE;

int assemble();
int pass1();
int label_exist();
char* remove_front_space();
int assign_loc();
int loc_size();
int get_format();
void get_format4_format();
int make_symtab();
void print_symtab();
int pass2();
int get_obcode();
int get_reg_num();
int set_pc();
void get_nix_labeladdr();
int get_TA();
int str_to_ASCII();
int is_comma();
void write_listing_file();
void write_object_file();



//linking loader
typedef struct ES_TAB{
	char csname[7];
	char symname[7];
	int address;
	int length;
}ES_TAB;

typedef struct BP{
	int bp;
	struct BP* link;
}BP_node;

int PROGADDR;
int CSADDR;
int CSLTH;
ES_TAB ESTAB[24] = {0};
int ESTABcnt;
char ref_tab[10][7];
int A, X, L, PC, SW, B, S, T, CC, F;
int EXEC_ADDR;
int EXEC_LEN;
int PROG_START;
int PROG_END;
int continuing;
BP_node* BP_head;
int TEXT[5];
int text_cnt=0;

void set_progaddr();
int linkingloader();
int linkingloader_pass1();
void set_ref_num();
void record_text();
void modify_memory();
int find_symbol_addr();
int* get_reg_ptr(unsigned char reg);
int op_act();
int search_with_opcode();
void print_reg();
int execute_prog();
int set_BP();
int clear_BP();
void print_BP();
int search_BP();
int write_to_memory();

	











