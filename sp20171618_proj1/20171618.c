#include "20171618.h" 

int main(void){
	char temp_command[MAX_COMMAND_SIZE];
	char command[MAX_COMMAND_SIZE];
	char copy_command[MAX_COMMAND_SIZE];	//for dumpfunction
	char* command_element;
	read_opcode_text();
	while(1){
		printf("sicsim> ");
		fgets(temp_command, MAX_COMMAND_SIZE, stdin);
		strcpy(command, remove_space(temp_command));
		strcpy(copy_command,command);	
		char* only_command = strtok(command, " \t");
		command_element=strtok(NULL,"\0");
	
		if(only_command){
			if(strcmp("help",only_command) == 0 || strcmp("h",only_command) == 0){
				if(command_element) continue;
				help();
				add_history(copy_command);
			}
			else if(strcmp("dir",only_command) == 0 || strcmp("d",only_command) == 0){
				if(command_element) continue;
				dir();
				add_history(copy_command);
			}
			else if(strcmp("quit",only_command) == 0 || strcmp("q",only_command) == 0){
				if(command_element) continue;
				quit();
				add_history(copy_command);
				break;
			}
			else if(strcmp("history",only_command) == 0 || strcmp("hi",only_command) == 0){
				if(command_element) continue;
				add_history(copy_command);
				print_history();
			}
			else if(strcmp("dump",only_command) == 0 || strcmp("du",only_command) == 0){
				dump(copy_command); //command with out front and back spaces
			} 
			else if(strcmp("edit",only_command) == 0 || strcmp("e", only_command) == 0){
				edit(copy_command);
			}	
			else if(strcmp("fill", only_command) == 0 || strcmp("f", only_command) == 0){
				fill(copy_command);
			}	
			else if(strcmp("reset",only_command)==0){
				if(command_element) continue;
				reset();
				add_history(copy_command);
			}	
			else if(strcmp("opcode",only_command) == 0){
				opcode_mnemonic(copy_command);
			}
			else if(strcmp("opcodelist",only_command) == 0 ){
				if(command_element) continue;
				opcodelist();
				add_history(copy_command);
			}
		}
	}
	return 0;
}

//return refined command
char* remove_space(char* temp_command){
	int i;
	// remove front spaces (count from first)
	for(i=0;temp_command[i] == ' ' || temp_command[i] == '\t' || temp_command[i] == '\n';i++);
	temp_command = temp_command + i;
	
	//remove back spaces (count from last)
	for(i=0 ; temp_command[strlen(temp_command) - i - 1] == ' ' || temp_command[strlen(temp_command) - i - 1] == '\t' || temp_command[strlen(temp_command) - i - 1] == '\n' ; i++ );
	temp_command[strlen(temp_command) - i] = '\0';
	
	
	return temp_command;
}

//print every command
void help(void){
	printf("h[elp]\n");
	printf("d[ir]\n");
	printf("q[uit]\n");
	printf("hi[story]\n");
	printf("du[mp] [start, end]\n");
	printf("e[dit] address, value\n");
	printf("f[ill] start, end, value\n");
	printf("reset\n");
	printf("opcode mnemonic\n");
	printf("opcodelist\n");
}

//print every direectory and file
void dir(void){
	DIR *dp;
	struct stat statbuf;
	struct dirent *dent;
	dp = opendir(".");
	while((dent = readdir(dp)) != NULL){
		printf("\t\t%s",dent->d_name);
		stat(dent->d_name,&statbuf);
		if(S_ISDIR(statbuf.st_mode)){	// "/" if directory
			printf("/");
		}
		else if(statbuf.st_mode & S_IXUSR){	// "*" if execution file
			printf("*");
		}
		printf("\n");	
	}
	closedir(dp);
}

//quit shell
void quit(void){
	int i;
	//free history lists;
	history_node *pmove;
	while(history_head != NULL) {
		pmove = history_head;
		history_head = history_head->link;
		free(pmove);
	}

	//free opcode table;
	optable_node* op_move, *op_temp;
	for(i=0; i<20; i++){
		op_move = op_head[i];
		while(op_move){
			op_temp = op_move;
			op_move = op_move->link;
			free(op_temp);
		}
	}

	printf("quit program success!\n");
}

//add history
void add_history(char* command){
	history_node *pnew = (history_node*)malloc(sizeof(history_node));		//init history list
	strcpy(pnew->command, command);
	pnew->link = NULL;

	if(!history_head){		//first insert to history list
		history_head = pnew;
		pnew->command_count = 1;
		return ;
	}
	
	history_node *pmove;
	for(pmove = history_head ; pmove->link != NULL ; pmove = pmove->link);
	pmove->link = pnew;
	pnew->command_count = pmove->command_count + 1;
}

//print history
void print_history(void){

	//if history is empty return
	if(history_head == NULL){
		return ;
	}

	//print history
	history_node *pmove;

	for(pmove = history_head ; pmove != NULL ; pmove = pmove->link){
		printf("\t%d\t%s\n",pmove->command_count,pmove->command);
	};
}

int dump(char* command){
	char copy_command[MAX_COMMAND_SIZE];
	strcpy(copy_command, command);
	char* only_command = strtok(command," \t");
	char *first_command_element = strtok(NULL,"\0");
	char* start_address=NULL;
	char* end_address=NULL;
	int error_check_front,error_check_back;	
	char history_command[MAX_COMMAND_SIZE];
	strcat(history_command, only_command);
	
	if(!first_command_element){	//dump
		if(print_dump(NULL,NULL)==1){		
			add_history(history_command);
		}
		return 0;
	}

	error_check_front = (int)strlen(first_command_element);
	start_address = remove_space(strtok(first_command_element, ","));
	error_check_back = (int)strlen(first_command_element);
	end_address = strtok(NULL,"\0");
	if(!end_address){	//dump start
		if(error_check_front != error_check_back){	// error: dump start,  
			printf("no end error\n");  
			return 0;
		}
		if(print_dump(start_address,NULL)==1){

			//make an informed command
			strcat(history_command, " ");
			strcat(history_command, start_address);
			add_history(history_command);
		}
		return 0;
	}

	end_address = remove_space(end_address);
	// dump start, end
	if(print_dump(start_address, end_address)==1){

		// make an informed command
		strcat(history_command, " ");
		strcat(history_command, start_address);
		strcat(history_command, ", ");
		strcat(history_command, end_address);
		add_history(history_command);
		return 0;
	}
	return 0;
}

//print success: print and return 1, print fail: return 0;
int print_dump(char* start_address, char* end_address){
	int start, end;
	if(start_address == NULL && end_address == NULL){	//dump
		//print
		if(last_add > 0xFFFFF){
			last_add = 0;	//reset last_add to 0
		}
		start = last_add;
		if((start + 159) > 0xFFFFF){
			end = 0xFFFFF;
		}
		else end = start + 0x9F;
		last_add = end + 0x1;	//update last_add;

		print_dump_sub(start, end);

		return 1;
	}
	else if(start_address != NULL && end_address == NULL){	//dump start
		if(valid_address(start_address)==1){
			//print
			start = change_str_to_hexa(start_address);
			if((start + 159) > 0xFFFFF){
				end = 0xFFFFF;
			}
			else end = start + 0x9F;
			
			print_dump_sub(start, end);		
				
			return 1;
		}
		else return 0;
	}
	else if(start_address != NULL && end_address != NULL){	//dump start, end
		if(valid_address(start_address)==1 && valid_address(end_address)==1){
			start = change_str_to_hexa(start_address);
			end = change_str_to_hexa(end_address);
			if(valid_range(start, end)==1){
				//print
				
				print_dump_sub(start, end);

				return 1;
			}
			else return 0;
		}
		else return 0;
	}
	return 0;
}

//sub funtion for printing dump
void print_dump_sub(int start,int end){
	int start_line, end_line, i, j, line_num, address;
	char one_line_ASCII[16];
	start_line = start / 16 * 16;
	end_line = end / 16 * 16;
	line_num = (end_line - start_line) / 16 + 1;

	for(i=0; i< line_num; i++){
		printf("%05X ", start_line + (i*16));
		for(j=0; j<16; j++){
			address = start_line + i*16 + j;
			if(address < start){
				printf("   ");
				one_line_ASCII[j] = '.';
			}
			else if(address > end){
				printf("   ");
				one_line_ASCII[j] = '.';
			}
			else{
				printf("%02hhX ", memory[address]);
				if(memory[address] >= 0x20 && memory[address] <= 0x7E){
					one_line_ASCII[j] = memory[address];
				}
				else one_line_ASCII[j] = '.';
			}
		}
		printf("; ");
		for(j=0; j<16; j++){
			printf("%c",one_line_ASCII[j]);
		}
		printf("\n");
	} 
};

//edit success: return 1, fail: return 0
int edit(char* command){
	char history_command[MAX_COMMAND_SIZE];
	char* only_command = strtok(command, " \t");
	char* first_command_element = strtok(NULL,"\0");
	if(!first_command_element) return 0;	//no address
	char* address = remove_space(strtok(first_command_element, ","));
	char* value = strtok(NULL, "\0");
	int hexa_value;
	int hexa_address;
	if(!value) return 0;	//no value
	value = remove_space(value);

	if(valid_address(address) == 0) return 0;
	if(valid_address(value) == 1){
		hexa_value = change_str_to_hexa(value);
		if(hexa_value > 0xFF) return 0;
		hexa_address = change_str_to_hexa(address);

		memory[hexa_address] = hexa_value;

		//make an infromed command
		strcat(history_command, only_command);
		strcat(history_command, " ");
		strcat(history_command, address);
		strcat(history_command, ", ");
		strcat(history_command, value);
		add_history(history_command);
		return 1;
	}	
	return 0;
}

//fill success: return 1, fail: return 0
int fill(char* command){
	int start, end, hexa_value;
	int i;
	char history_command[MAX_COMMAND_SIZE];
	char* only_command = strtok(command, " \t");
	char* first_command_element = strtok(NULL,"\0");
	if(!first_command_element) return 0;
	char* start_address = remove_space(strtok(first_command_element, ","));
	char* end_address = strtok(NULL, ",");
	if(!end_address) return 0;
	end_address = remove_space(end_address);
	char* value = strtok(NULL, "\0");
	if(!value) return 1;
	value = remove_space(value);

	if(valid_address(start_address)==1 && valid_address(end_address)==1){
		start = change_str_to_hexa(start_address);
		end = change_str_to_hexa(end_address);
		if(valid_range(start, end)==1){
			if(valid_address(value)==1){
				hexa_value = change_str_to_hexa(value);
				if(hexa_value > 0xFF) return 0;
				for(i=start; i<=end; i++){
					memory[i] = hexa_value;
				}

				//make an infromed command
				strcat(history_command, only_command);
				strcat(history_command, " ");
				strcat(history_command, start_address);
				strcat(history_command, ", ");
				strcat(history_command, end_address);
				strcat(history_command, ", ");
				strcat(history_command, value);
				add_history(history_command);
				return 1;			
			}
		}
		return 0;
	}
	return 0;
}

void reset(){
	int i;
	for(i=0; i<MEMORY_SIZE; i++){
		memory[i] = 0x00;
	}
}

//valid address: return 1, invalid address: return 0
int valid_address(char* address){
	int i;
	if((int)strlen(address)>5){
		return 0;
	}
	for(i=0; i<(int)strlen(address); i++){
		if( ('0'>address[i]||'9'<address[i]) && ('a'>address[i]||'f'<address[i]) && ('A'>address[i]||'F'<address[i])){
			return 0;
		}
	}
	return 1;
}	

//valid start and end range: return 1, invalid range: return 0
int valid_range(int start, int end){
	if(start > end){
		return 0;
	}
	return 1;
}


int change_str_to_hexa(char* str){
	int hexa;
	sscanf(str, "%x", &hexa);
	return hexa;
}

int read_opcode_text(){
	int i;
	int table_number,opcode;
	char operation[10];
	char format[4];
	FILE* fp = fopen("opcode.txt","r");
	op_head = (optable_node**)malloc(20 * sizeof(optable_node));
	for(i=0; i<20; i++){
		op_head[i] = NULL;
	}

	if(!fp){
		printf("error\n");
		return 0;
	}

	while(fscanf(fp, "%X %s %s\n",&opcode, operation, format) != EOF){
		optable_node* pnew = (optable_node*)malloc(sizeof(optable_node));
		pnew->opcode = opcode;
		strcpy(pnew->operation, operation);
		if(strcmp("1",format)==0){
			pnew->format = 1;
		}
		else if(strcmp("2",format)==0){
			pnew->format = 2;
		}
		else if(strcmp("3/4",format)==0){
			pnew->format = 3;
		}
		else{ 
			pnew->format = 0;
		}
		pnew->link=NULL;
		
		table_number=hash_function(operation);
		if(!op_head[table_number]){
			op_head[table_number] = pnew;
		}
		else{
			optable_node* pmove;
			for(pmove=op_head[table_number]; pmove->link; pmove = pmove->link);
			pmove->link=pnew;
		}
	}
	fclose(fp);
	return 1;
}

//seperate by first letter
int hash_function(char* operation){
	return operation[0]%20;
}

int opcode_mnemonic(char* command){
	char history_command[MAX_COMMAND_SIZE];
	char* only_command = strtok(command," \t");
	char* mnemonic = strtok(NULL,"\0");
	mnemonic = remove_space(mnemonic);
	int i;
	optable_node* pmove;
	for(i=0; i<20; i++){
		for (pmove = op_head[i]; pmove; pmove = pmove->link) {
        		if (strcmp(pmove->operation, mnemonic) == 0) {
        			printf("opcode is %X\n", pmove->opcode);
	
				strcat(history_command, only_command);
				strcat(history_command, " ");
				strcat(history_command, mnemonic);
				add_history(history_command);

            			return 1;
       	 		}
    		}
	}
    	printf("Wrong mnemonic\n");
    	return 0;
}

//print opcodelist
void opcodelist(){
	int i;
	optable_node* pmove;
	for(i=0; i<20; i++){
		printf("%d : ",i);
		pmove=op_head[i];
		if(pmove){
			for(;pmove->link;pmove=pmove->link){
				printf("[%s,%X] -> ", pmove->operation, pmove->opcode);
			}
			printf("[%s,%X]",pmove->operation,pmove->opcode);
		}
		printf("\n");
	}	
}
