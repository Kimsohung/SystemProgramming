#include "20171618.h" 

int main(void){
	char temp_command[MAX_COMMAND_SIZE];
	char command[MAX_COMMAND_SIZE];
	char copy_command[MAX_COMMAND_SIZE];	//for dumpfunction
	char* command_element;
	read_opcode_text();
	PROGADDR = 0;
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
			else if(strcmp("type",only_command) == 0){
				type_filename(copy_command);	//success to find filename and type
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
			else if(strcmp("assemble", only_command) == 0){
				assemble(copy_command);
			}
			else if(strcmp("symbol", only_command) == 0){
				if(command_element) continue;
				print_symtab();
				add_history(copy_command);
			}
			else if(strcmp("progaddr", only_command) == 0){
				set_progaddr(copy_command);
			}
			else if(strcmp("loader", only_command) == 0){
				linkingloader(copy_command);
			}
			else if(strcmp("bp", only_command) == 0){	//bp
				if(!command_element){
					print_BP();
					add_history(only_command);
				}		
				else{
					command_element = remove_space(command_element);
					if(strcmp("clear", command_element) == 0){
						if(clear_BP())
						add_history(copy_command);
					}
					else if(set_BP(command_element))
						add_history(copy_command);
				}
			}
			else if(strcmp("run", only_command) == 0){
				execute_prog();
				add_history(only_command);
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
	printf("assemlble filename\n");
	printf("type filename");
	printf("symbol\n");
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

//type fileaname
int type_filename(char* command){
	char copy_command[MAX_COMMAND_SIZE];
	char history_command[MAX_COMMAND_SIZE];
	strcpy(copy_command, command);
	char *only_command = strtok(command," \t");
	char *filename = strtok(NULL,"\0");
	
	if(!filename){
		printf("you should type filename!\n");
		return 0;	//only "type" command without filename
	}

	filename = remove_space(filename);

	FILE* fp = fopen(filename, "r");	
	if(!fp){
		printf("%s doesn't exist in directory\n", filename);
		return 0;
	}
		
	char c = fgetc(fp);
	while(!feof(fp)){
		printf("%c", c);
		c= fgetc(fp);
	}

	strcat(history_command, only_command);
	strcat(history_command, " ");
	strcat(history_command, filename);
	add_history(history_command);
	return 1;	//success to type filename
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

//assemble success: return 1, fail : return 0
int assemble(char* command){
	char copy_command[MAX_COMMAND_SIZE];
	char history_command[MAX_COMMAND_SIZE];
	strcpy(copy_command, command);
	char *only_command = strtok(command," \t");
	char *filename = strtok(NULL,"\0");
	char only_name[30]={0};
	
	memset(instruction, 0, sizeof(instruction));	//init instruction array

	if(!filename){
		printf("you should type filename!\n");
		return 0;	//only "type" command without filename
	}

	filename = remove_space(filename);
	FILE* fp = fopen(filename, "r");	
	if(!fp){
		printf("%s doesn't exist in directory\n", filename);
		return 0;
	}
	
	char line[60];
	asm_line_count = 0;	//init asm total line count	
	while(!feof(fp)){	//get asm total line count
		fgets(line, sizeof(line), fp);
		if(strcmp("\n", line)!=0){
			asm_line_count++;
		}
	}

	if(pass1(fp)==1){
		if(pass2(filename)==1){
			for(int i=0; i<(int)strlen(filename); i++){
				if(filename[i] != '.') only_name[i] = filename[i];
				else break;
			}
			
			printf("[%s.lst], [%s.obj]\n", only_name, only_name);	//print ilstingfile, objectfile
		}
		else return 0;			
	}
	else return 0;
	
	fclose(fp);
	strcat(history_command, only_command);
	strcat(history_command, " ");
	strcat(history_command, filename);
	add_history(history_command);
	return 1;
}

//assign instruction array (include location), make symbol table
int pass1(FILE* fp){
	int i, j;
	int flag;
	char line[60];
	char copy_line[60];
	
	char* first_element;	
	char* second_element;	
	char* third_element;
	char temp_second[10];
	char temp_third[10];	
	fseek(fp, 0, SEEK_SET);
	for(i=0; i<asm_line_count; i++){
		fgets(line, sizeof(line), fp);
		strcpy(copy_line, line);		//save origin line
		for(j=0; line[j] != '\0'; j++){		//remove '\n'
			if(line[j] == '\n'){
				line[j] = '\0';
			}
		}
		
		first_element = strtok(line, " \t");
	
		if(strncmp(first_element, ".", 1) == 0){	//comment
			instruction[i].instruction_type = 0;
			strncpy(instruction[i].comment, copy_line, strlen(copy_line)-1);
			strcpy(instruction[i].label, "0");
		}
		else{
			flag = label_exist(first_element);
			if(flag == 0){	//no label
				instruction[i].instruction_type = 2;
				second_element = strtok(NULL, "\0");	
				if(!second_element){
					strcpy(instruction[i].mnemonic, first_element);		//no operand, only mnemonic
					strcpy(instruction[i].label, "0");
				//strcpy(instruction[i].operand, "0");
				}
				else{
					strcpy(instruction[i].mnemonic, first_element);
					strcpy(temp_second, remove_space(second_element));		
					strcpy(instruction[i].operand, temp_second);	
					strcpy(instruction[i].label, "0");
				}
			}
			else{	//label exist
				instruction[i].instruction_type = 1;
				second_element = strtok(NULL, " \t");		//mnemonic
				third_element = strtok(NULL, " \t");		//operand
				strcpy(instruction[i].label, first_element);
				strcpy(instruction[i].mnemonic, second_element);
				strcpy(temp_third, remove_front_space(third_element));
				strcpy(instruction[i].operand, temp_third);
			}	
		}
	}

	if(assign_loc() == 1){
		if(make_symtab() == 1){
					
		}
		else return 0;
	}	
	else return 0;
			
	return 1;
}

int label_exist(char* first_element){
	int i;
	char format4_temp[10];	//execption for format4
	optable_node* pmove;
	if(strcmp(first_element, "BASE") == 0) return 0;	// exception for BASE menmonic
	if(strcmp(first_element, "END") == 0) return 0;	//exception for END mnemonic

	for(i=0;i<20;i++){
		for(pmove = op_head[i]; pmove; pmove = pmove->link){
			if(first_element[0] == '+'){
				strcpy(format4_temp, first_element+1);
				if(strcmp(pmove->operation, format4_temp) == 0){	//ignore '+' to search on opcode.txt
					return 0;	
				}
			}
			else{
				if(strcmp(pmove->operation, first_element) == 0){	//no label(first_element is mnemonic)
					return 0;	
				}
			}
		}
	}
	return 1;	//label exist (first element is a label)
}


char* remove_front_space(char* temp_command){
	int i;
	// remove front spaces (count from first)
	for(i=0;temp_command[i] == ' ' || temp_command[i] == '\t' || temp_command[i] == '\n';i++);
	temp_command = temp_command + i;
	return temp_command;
}

int assign_loc(){
	int LOCCTR = 0;
	int i;
	for(i=0; i<asm_line_count; i++){
		if(strcmp(instruction[i].mnemonic, "START") == 0){
			instruction[i].loc = 0;
		}
		else if(strcmp(instruction[i].mnemonic, "END") == 0){
			instruction[i].loc = -1;
		}
		else if(strcmp(instruction[i].mnemonic, "BASE") == 0){
			instruction[i].loc = -1;	//location does not exist (-1)
		}
		else if(instruction[i].instruction_type == 0){	//no location for comment
			instruction[i].loc = -1;
		}
		else{
			instruction[i].loc = LOCCTR;
			LOCCTR += loc_size(i);
		}
	}
	program_length = LOCCTR;
	return 1;
}

int loc_size(int i){
	int n;
	if(strncmp(instruction[i].mnemonic, "+", 1) == 0){	//format 4
		get_format4_format(i);
		return 4;
	}
	else if(strcmp(instruction[i].mnemonic, "WORD") == 0){	
		return 3;
	}
	else if(strcmp(instruction[i].mnemonic, "RESW") == 0){
		n = atoi(instruction[i].operand) * 3;
		return n;		
	}
	else if(strcmp(instruction[i].mnemonic, "RESB") == 0){
		n = atoi(instruction[i].operand);
		return n;
	}
	else if(strcmp(instruction[i].mnemonic, "BYTE") == 0){
		if(instruction[i].operand[0] == 'X') return 1;
		else if(instruction[i].operand[0] == 'C'){
			return (strlen(instruction[i].operand)-3); 	//remove size of C''
		}		
	}
	else{		//format 3, format 2, format 1
		return get_format(i);
	}
	return 0;
}

//get format, opcode
int get_format(int i){
	int format;
	optable_node* pmove;
	int j;

	for(j=0;j<20;j++){
		for(pmove = op_head[j]; pmove; pmove = pmove->link){
			if(strcmp(pmove->operation, instruction[i].mnemonic) == 0){
				format = pmove->format;		//get format from optable
				instruction[i].opcode = pmove->opcode;		//get opcode from optable
				instruction[i].format = pmove->format;
			}
		}
	
	}
	if(format == 1) return 1;
	if(format == 2) return 2;
	if(format == 3) return 3;
	return 0;
}

void get_format4_format(int i){		//case for format4 to save format to array
	int j;
	optable_node* pmove;

	instruction[i].format = 4;

	for(j=0; j<20; j++){
		for(pmove = op_head[j]; pmove; pmove = pmove->link){
			if(strcmp(pmove->operation, (instruction[i].mnemonic)+1) == 0){
				instruction[i].opcode = pmove->opcode;
			}
		}
	}
}

int make_symtab(){
	int i, j, k=0;
	int count = 1;
	
	memset(symtab, 0, sizeof(symtab));	//init symtab array

	for(i=0; i<asm_line_count; i++){
		if((strcmp(instruction[i].label, "0") != 0) && (strcmp(instruction[i].mnemonic, "START") != 0)){
			for(j=0; j<count; j++){
				if(strcmp(instruction[i].label, symtab[j].label) == 0){		//label overlap
					printf("line %d is error!\n", (i+1)*5);		
					return 0;	//label overlap
				}
			}
			strcpy(symtab[k].label, instruction[i].label);
			symtab[k].loc = instruction[i].loc;
			k++;
			count++;	
		}
	}
	symtab_count = count - 1;
	return 1;
}

void print_symtab(){
	int i, j;
	char temp_label[10];
	int temp_loc;	

	for(i=0; i<symtab_count-1; i++){
		for(j=0; j<symtab_count-1-i; j++){
			if(strcmp(symtab[j].label, symtab[j+1].label) > 0){
				strcpy(temp_label, symtab[j].label);
				temp_loc = symtab[j].loc;
				
				strcpy(symtab[j].label, symtab[j+1].label);
				symtab[j].loc = symtab[j+1].loc;

				strcpy(symtab[j+1].label, temp_label);
				symtab[j+1].loc = temp_loc;
			}
		}
	}

	for(i=0; i<symtab_count; i++){
		printf("\t%s\t%04X\n", symtab[i].label, symtab[i].loc);
	}
}

int pass2(char* filename){
	if(get_obcode() == 1){
		write_listing_file(filename);
		write_object_file(filename);
	}		
	else return 0;
	
	return 1;
}

int get_obcode(){
	int i, j;
	int N, I, X, B, P, E;
	int labeladdr;
	int PC, TA;
	char *r1, *r2;
	char temp_operand[10];
		
	for(i=1; i<asm_line_count-1; i++){

		if(strcmp(instruction[i].mnemonic, "BASE") == 0){	//get BASE address
			for(j=0; strcmp(instruction[j].label, instruction[i].operand) != 0; j++);
			BASE = instruction[j].loc;
		}

		if(strncmp(instruction[i].mnemonic, "+", 1) == 0) instruction[i].format = 4;	//case for format4's format setting
		if(strcmp(instruction[i].mnemonic, "BASE") == 0 || strcmp(instruction[i].mnemonic, "RESW") == 0 || strcmp(instruction[i].mnemonic, "RESB") == 0){	//case that no obcode
			instruction[i].obcode = -1;
		}
		else if(strcmp(instruction[i].mnemonic, "BYTE") == 0){
			if(instruction[i].operand[0] == 'X'){		//BYTE X' '
				strncpy(temp_operand, (instruction[i].operand)+2, 2);
				instruction[i].obcode = strtol(temp_operand, NULL, 16);
			}
			else{		//BYTE C' '
				instruction[i].obcode = str_to_ASCII(i);
			}
		}
		else{
			PC=set_pc(i);
			if(instruction[i].format == 4){		//format 4
				E=1;
				get_nix_labeladdr(i, &N, &I, &X, &labeladdr);
				TA = get_TA(i, N, I, X, &B, &P, E, labeladdr, PC);
				instruction[i].obcode = instruction[i].opcode*16*16*16*16*16*16 + TA; 				
			}
			else if(instruction[i].format == 3){		//format 3
				E=0;
				get_nix_labeladdr(i, &N, &I, &X, &labeladdr);
				TA = get_TA(i, N, I, X, &B, &P, E, labeladdr, PC); 
				instruction[i].obcode = instruction[i].opcode*16*16*16*16 + TA;				
			}
			else if(instruction[i].format == 2){	//format 2 - register operation
				
				strcpy(temp_operand, instruction[i].operand);
				r1=remove_space(strtok(temp_operand, ","));
				r2=strtok(NULL,"\0");
				if(!r2){	//mnemonic R1
					instruction[i].obcode = instruction[i].opcode*16*16 + get_reg_num(r1)*16;
				}
				else{	//mnemonic R1, R2	//format 1 - register operation
					r2=remove_space(r2);
					instruction[i].obcode = instruction[i].opcode*16*16 + get_reg_num(r1)*16 + get_reg_num(r2);
				}					
			}
			else if (instruction[i].format == 1){
				instruction[i].obcode = instruction[i].opcode;
			}
		}	
	}
	return 1;
}

int get_reg_num(char* reg){
	int a=0, x=1, l=2, pc=8, sw=9, b=3, s=4, t=5, f=6;
	if(strcmp(reg, "A") == 0)	return a;
	else if(strcmp(reg, "X") == 0)	return x;
	else if(strcmp(reg, "L") == 0)  return l;
	else if(strcmp(reg, "PC") == 0) return pc;
	else if(strcmp(reg, "SW") == 0) return sw;
	else if(strcmp(reg, "B") == 0)  return b;
	else if(strcmp(reg, "S") == 0)  return s;
	else if(strcmp(reg, "T") == 0)  return t;
	else if(strcmp(reg, "F") == 0)  return f;
	return 0;
}

int set_pc(int i){
	int j;
	for(j=i+1; j<asm_line_count; j++){
		if(instruction[j].loc != -1)	return j;
	}
	return 0;
}

//get N, I, X bit and label address
void get_nix_labeladdr(int i, int* N, int* I, int* X, int* labeladdr){
	int j;
	int special_char_flag=0;
	char temp_num[10];
	int comma_flag;	
	int x_flag=0;
	char* temp_x=NULL;	
	char temp_operand[10];
	
	//get N, I bit
	if(strncmp(instruction[i].operand, "@", 1) == 0){		//indirect addressing
		*N=1;
		*I=0;
		special_char_flag=1;
	}
	else if(strncmp(instruction[i].operand, "#", 1) == 0){	//immediate addressing
		*N=0;
		*I=1;
		special_char_flag=2;
	}
	else{		//simple addressing
		*N=1;
		*I=1;
	}
	//get X bit
	comma_flag = is_comma(instruction[i].operand);
	if(comma_flag == 1){	// BUFFER, X	
		strcpy(temp_operand, instruction[i].operand);
		temp_x = strtok(temp_operand, ",");
		*X=1;
		x_flag = 1;
	}
	else *X=0;
	

	//get label address
	if(special_char_flag ==2 && (instruction[i].operand[1] <= '9' && instruction[i].operand[1] >= '0')){
		strcpy(temp_num, (instruction[i].operand)+1);
		*labeladdr = atoi(temp_num);
	}
	for(j=0; j<symtab_count; j++){
		if(special_char_flag == 1 || special_char_flag == 2){		//case for #, @
			if(strcmp(symtab[j].label, (instruction[i].operand)+1) == 0){
				*labeladdr = symtab[j].loc;	
			}
		}
		else{	
			if(x_flag==1){
				if(strcmp(symtab[j].label, temp_x) == 0) *labeladdr = symtab[j].loc;
			}
			if(strcmp(symtab[j].label, instruction[i].operand) == 0){
				*labeladdr = symtab[j].loc;
			}
		}
	}
}

//get TA for format3 and format4
int get_TA(int i, int N, int I, int X, int* B, int* P, int E, int labeladdr, int PC){
	int TA;
	int NIXBPE_TA;
	char temp_operand[10];

	if(instruction[i].format == 4){
		TA = labeladdr;
		*B=0;
		*P=0;
		NIXBPE_TA = N*(int)pow((double)2, (double)25) + I*(int)pow((double)2, (double)24) + X*(int)pow((double)2, (double)23) + (*B)*(int)pow((double)2, (double)22) + (*P)*(int)pow((double)2, (double)21) + E*(int)pow((double)2, (double)20) + TA;
	}

	else if(strcmp(instruction[i].operand, "\0") == 0){	//case for no operand only mnemonic, ex) RSUB
		TA = 0;
		*B=0;
		*P=0; 
		NIXBPE_TA = N*(int)pow((double)2, (double)17) + I*(int)pow((double)2, (double)16) + X*(int)pow((double)2, (double)15) + (*B)*(int)pow((double)2, (double)14) + (*P)*(int)pow((double)2, (double)13) + E*(int)pow((double)2, (double)12) + TA;
	} 
	
	else{
		if(strncmp(instruction[i].operand, "#", 1) == 0 && (instruction[i].operand[1] <= '9' && instruction[i].operand[1] >= '0')){	//#3
			strcpy(temp_operand, (instruction[i].operand)+1);
			TA = atoi(temp_operand);
			*B=0;
			*P=0;
		}
		else if(((labeladdr - instruction[PC].loc) <= 2047) && ((labeladdr - instruction[PC].loc) >= -2048)){	//PC addressing mode
			if((labeladdr - instruction[PC].loc)<0){	//for negative number
				TA = labeladdr - instruction[PC].loc + 16*16*16;
			}
			else TA = labeladdr- instruction[PC].loc;
			*B=0;
			*P=1;
		}	
		else{		//BASE addressing mode
			TA = labeladdr - BASE;
			*B=1;
			*P=0;
		}
		NIXBPE_TA = N*(int)pow((double)2, (double)17) + I*(int)pow((double)2, (double)16) + X*(int)pow((double)2, (double)15) + (*B)*(int)pow((double)2, (double)14) + (*P)*(int)pow((double)2, (double)13) + E*(int)pow((double)2, (double)12) + TA;
	}
		
	return NIXBPE_TA;	
}

int str_to_ASCII(int i){
	int a, b, c;
	a = instruction[i].operand[2];
	b = instruction[i].operand[3];
	c = instruction[i].operand[4];
	return a*16*16*16*16 + b*16*16 +c;
}

int is_comma(char* input){
	if(!input) return 0;
	for(int i=0;i<(int)strlen(input);i++){
		if(input[i] == ',') return 1;
	}
	return 0;
}

void write_listing_file(char* filename){
	int i;
	FILE* listing;
	char name[30]={0};
	char temp[10];

	
	
	for(i=0; i<(int)strlen(filename); i++){
		if(filename[i] != '.') name[i] = filename[i];
		else break;
	}

	strcat(name, ".lst");
	listing = fopen(name, "w");

	for(i=0; strcmp(instruction[i].mnemonic, "END")!=0; i++){	
		if(strcmp(instruction[i].label, "0") == 0){
			strcpy(temp, "");
		}
		else strcpy(temp, instruction[i].label);

		if(instruction[i].comment[0] == '.'){		//comment line
			fprintf(listing, "%d\t%s\n", (i+1)*5, instruction[i].comment);
		}
		else if(instruction[i].loc == -1){	//no location
			if(instruction[i].format == 4){
				fprintf(listing, "%d\t\t\t%s\t%s\t%s\t\t%08X\n", (i+1)*5, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else if(instruction[i].format == 3){
				fprintf(listing, "%d\t\t\t%s\t%s\t%s\t\t%06X\n", (i+1)*5, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else if(instruction[i].format == 2){
				fprintf(listing, "%d\t\t\t%s\t%s\t%s\t\t%04X\n", (i+1)*5, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else if(instruction[i].format == 1){
				fprintf(listing, "%d\t\t\t%s\t%s\t%s\t\t%02X\n", (i+1)*5, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else{	
				if(instruction[i].obcode == 0 || instruction[i].obcode == -1) {		//no object code
					fprintf(listing, "%d\t\t%s\t%s\t%s\n", (i+1)*5, temp, instruction[i].mnemonic, instruction[i].operand);
				}
				else if(instruction[i].operand[0] == 'X'){
					fprintf(listing, "%d\t\t\t%s\t%s\t%s\t\t%02X\n", (i+1)*5, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
				}
				else if(instruction[i].operand[0] == 'C'){
					fprintf(listing, "%d\t\t\t%s\t%s\t%s\t\t%06X\n", (i+1)*5, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
				}
			}	
		}
		else{
			if(instruction[i].format == 4){
				fprintf(listing, "%d\t%04X\t%s\t%s\t%s\t\t%08X\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else if(instruction[i].format == 3){
				if((int)strlen(instruction[i].operand) < 8)
					fprintf(listing, "%d\t%04X\t%s\t%s\t%s\t\t%06X\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
				else
					fprintf(listing, "%d\t%04X\t%s\t%s\t%s\t%06X\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else if(instruction[i].format == 2){
				fprintf(listing, "%d\t%04X\t%s\t%s\t%s\t\t%04X\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else if(instruction[i].format == 1){
				fprintf(listing, "%d\t%04X\t%s\t%s\t%s\t\t%02X\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
			}
			else{	
				if(instruction[i].obcode == 0 || instruction[i].obcode == -1) {		//no object code
					fprintf(listing, "%d\t%04X\t%s\t%s\t%s\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand);
				}
				else if(instruction[i].operand[0] == 'X'){
					fprintf(listing, "%d\t%04X\t%s\t%s\t%s\t\t%02X\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
				}
				else if(instruction[i].operand[0] == 'C'){
					fprintf(listing, "%d\t%04X\t%s\t%s\t%s\t\t%06X\n", (i+1)*5, instruction[i].loc, temp, instruction[i].mnemonic, instruction[i].operand, instruction[i].obcode);
				}
			}	

		}
	}
	fprintf(listing, "%d\t\t\t%s\t%s\n", (i+1)*5, instruction[i].mnemonic, instruction[i].operand);
	fclose(listing);
}

void write_object_file(char* filename){
	int i, j, k, l;
	FILE* object;
	char name[30]={0};
	int M[MAX_ASM_LINE];
	int m_count = 0;
	int prefer_line_count=0;
	char line[60];	
	int escape_flag;
	char temp_obcode[10];	
	int i_count;

	for(i=0; i<(int)strlen(filename); i++){
		if(filename[i] != '.') name[i] = filename[i];
		else break;
	}

	strcat(name, ".obj");
	object = fopen(name, "w");
	
	fprintf(object, "H%-6s000000%06X\n", instruction[0].label, program_length);

	i = 1;
       	while(strcmp(instruction[i].mnemonic, "END") != 0){
		j = 0;
		strcpy(line, "\0");
		escape_flag = 0;
		i_count = 0;
		k = 0;
		while(1){	//line by line
			if(strcmp(instruction[i].mnemonic, "RESW") == 0 || strcmp(instruction[i].mnemonic, "RESB") == 0){	//object code is const or character
				escape_flag=1;	
				k=i;
				l=0;
				while(strcmp(instruction[k].mnemonic, "RESW")==0 || strcmp(instruction[k].mnemonic, "RESB")==0){
					k++;
					l++;
				}
				i_count+=l;
				i+=l;
			}
			else if(instruction[i].obcode == 0 || instruction[i].obcode == -1){	//no object code
				i_count++;
			 	i++;
			}
			else{
				sprintf(temp_obcode, "%X", instruction[i].obcode);
				if(   ((int)strlen(temp_obcode)) % 2 == 1){	//need to add 0 infront of
					strcpy(temp_obcode+1, temp_obcode);
					temp_obcode[0] = '0';
					j+=(int)strlen(temp_obcode);
					if(j>60){	//line finished
						escape_flag = 1;
					
					}
				
					else{
						strcpy(line+j-(int)strlen(temp_obcode), temp_obcode);	//update on line

						if(instruction[i].format == 4){		//get M code info
							M[m_count] = prefer_line_count + (j - 6)/2;
							m_count++;
						}
						
						i_count++;	
						i++;					
					}
				}
				else{
					j+=(int)strlen(temp_obcode);
					if(j>60){	//line finished
						escape_flag = 1;
					}
					else{
						sprintf(line+j-(int)strlen(temp_obcode), "%X", instruction[i].obcode); //update on line

						if(instruction[i].format == 4){		//get M code info
							M[m_count] = prefer_line_count + (j - 6)/2;
							m_count++;
						}
	
						i_count++;	
						i++;
					}
				}
				if(strcmp(instruction[i].mnemonic, "END") == 0) break;
			}
			if(strcmp(instruction[i].mnemonic, "END") == 0) break;
			if(escape_flag) break;
			strcpy(temp_obcode, "\0");
		}
		if(instruction[i - i_count].loc == -1){
			while(instruction[i - i_count].loc == -1){
				i_count--;
			}
		}
		fprintf(object, "T%06X%02X%s \n", instruction[i - i_count].loc, (int)strlen(line)/2, line);
		prefer_line_count += (int)strlen(line)/2;
	}

	for(i=0;i<m_count-1;i++){	//write modification code
		fprintf(object, "M%06X05\n", M[i]);
	}
	
	fprintf(object, "E000000\n");

	fclose(object);
}

void set_progaddr(char* command){
	char history_command[MAX_COMMAND_SIZE];
	char* only_command = strtok(command," \t");
	char* strprogaddr = strtok(NULL, "\0");
	
	strprogaddr = remove_space(strprogaddr);
	PROGADDR = change_str_to_hexa(strprogaddr);
	EXEC_ADDR = PROG_START = PROGADDR;
	strcat(history_command, only_command);
	strcat(history_command, " ");
	strcat(history_command, strprogaddr);
	add_history(history_command);

}

int linkingloader(char* command){
	
	if(linkingloader_pass1(command)==1){
		return 1;
	}
	return 1;
}

int linkingloader_pass1(char* command){
	
	//pass1
	char* only_command = strtok(command, " \t");
	char* objfiles = strtok(NULL, "\0");
	objfiles = remove_space(objfiles);
	char** objfilelist = (char**)malloc(3*sizeof(char*));
	FILE** objfp = (FILE**)malloc(3 * sizeof(FILE*));
	int objcnt = 0;
	int i, j, k;
	char temp_str[7];
	char temp_str_2[7];	
	int symbol_num;

	objfilelist[objcnt] = strtok(objfiles, " \t");	//put first obj file name to objfilelist
	objcnt++;
	do{
		objfilelist[objcnt] = strtok(NULL, " \t");		//put obj file name to objfilelist
	}while(objfilelist[objcnt++]);
	objcnt--;


	for(i=0; i<objcnt; i++){
		objfp[i] = fopen(objfilelist[i], "r");
		if(!objfp[i]){
			printf("filename doesn't match\n");
			return 0;
		}
	}
	CSADDR = PROGADDR;
	ESTABcnt = 0;
	char line[MAX_OBJ_LENGTH];
	for(i=0; i<objcnt; i++){

		while(1){
			memset(line, '\0', sizeof(line));
			fgets(line, MAX_OBJ_LENGTH, objfp[i]);

			if(line[0]!='E'){
				line[strlen(line)-1] = '\0';
			}

			if(line[0] == 'H'){
				strncpy(temp_str, line + 1, 6);
				strcpy(temp_str, remove_space(temp_str));
				strcpy(ESTAB[ESTABcnt].csname, temp_str);	//put control section name into ESTAB
			
				ESTAB[ESTABcnt].address = CSADDR;
				strncpy(temp_str, line + 13, 6);
				ESTAB[ESTABcnt].length = CSLTH = change_str_to_hexa(temp_str);		//put control section's length into ESTAB
				ESTABcnt++;

			}
			else if(line[0] == 'D'){
				k=1;
				symbol_num = (strlen(line) - 1) / 12;
				for(j=0; j<symbol_num; j++){
					strncpy(temp_str, line+k, 6);
					strcpy(temp_str, remove_space(temp_str));
					strcpy(ESTAB[ESTABcnt].symname, temp_str);	//put symbol name into ESTAB
					strncpy(temp_str, line+k+6, 6);
					ESTAB[ESTABcnt].address = change_str_to_hexa(temp_str) + CSADDR;	//put symbol name's address into ESTAB
					k+=12;
					ESTABcnt++;
				}
				CSADDR+=CSLTH;
			}
			else break;
		}
	}

	//pass2	
	CSADDR = PROGADDR;
	for(i=0; i<objcnt; i++){	//by each object file
		fseek(objfp[i], 0, SEEK_SET);
		while(1){	//during end of each file
			memset(line, '\0', sizeof(line));
			fgets(line, MAX_OBJ_LENGTH, objfp[i]);
			
			if(line[0]!='E'){
				line[strlen(line)-1] = '\0';
			}
			
			if(line[0] == 'H'){
				strncpy(temp_str, line+13, 6);
				CSLTH = change_str_to_hexa(temp_str);		//set CSLTH		
				
				strncpy(temp_str_2, line+1 ,6);
				strcpy(temp_str, remove_space(temp_str_2));
				strcpy(ref_tab[1], temp_str);		//store control section's reference number 01 in ref_tab 		
			}
			else if(line[0] == 'D'){
				//skip
			}
			else if(line[0] == 'R'){	
				set_ref_num(line);
			}
			else if(line[0] == '.'){
				//skip
			}
			else if(line[0] == 'T'){
				record_text(line);
			}
			else if(line[0] == 'M'){
				modify_memory(line);
			}
			else if(line[0] == 'E'){
				if(line[1] != 0){	//end of main file
					strncpy(temp_str, line + 1, 6);
					break;
				}
				else break;	//end of file
			}	
			else{
				printf("No 'E' on object file\n");
				return 0;	//no E error
			}
		}
		//init ref_tab for next control section
		for(j=0; j<10; j++){
			for(k=0; k<7; k++){
				ref_tab[j][k] = '\0';
			}
		}
		CSADDR += CSLTH;	//update CSADDR
	}

	//print load map (ESTAB)
	int total_len=0;
	printf("control\tsymbol\taddress\tlength\n");
	printf("section\tname\n");
	printf("--------------------------------\n");
	for(i=0; i<ESTABcnt; i++){
		if(ESTAB[i].csname[0] != 0){
			printf("%s\t%s\t%04X\t%04X\n", ESTAB[i].csname, ESTAB[i].symname, ESTAB[i].address, ESTAB[i].length);		//print control section name
			total_len += ESTAB[i].length;
		}
		else{
			printf("%s\t%s\t%04X\n", ESTAB[i].csname, ESTAB[i].symname, ESTAB[i].address);		//print symbol name (no length)
		}
	}
	printf("--------------------------------\n");
	printf("\ttotal length\t%04X\n", total_len);

	add_history(only_command);

	//init global variable
	for(i=0;i<10;i++){
		memset(ref_tab[i], '\0', sizeof(ref_tab[i]));
	}

	//set for run
	EXEC_ADDR = PROG_START = ESTAB[0].address;
	EXEC_LEN = ESTAB[objcnt-1].address + ESTAB[objcnt-1].length - EXEC_ADDR;
	PROG_END = PROG_START + EXEC_LEN;
	continuing = 0;

	return 1;
}

//store reference number and symbol name in ref_tab;
void set_ref_num(char* line){
	int temp_ref_num;
	//char temp_str[7];
	//char temp_str_2[7];
	int i,j;
		
	j=1;
	for(i=0; i < (((int)strlen(line)+1) / 8); i++){
		char temp_str[7] = {0};
		char temp_str_2[7] = {0};
		strncpy(temp_str, line + j, 2);
		temp_ref_num = change_str_to_hexa(temp_str);
		strncpy(temp_str_2, line + j + 2, 6);
		strcpy(temp_str, remove_space(temp_str_2));
		strcpy(ref_tab[temp_ref_num], temp_str);
		j+=8;
	}

}

//load T object codes on memory
void record_text(char* line){
	int start_addr;
	int t_len;
	int one_digit_objcode;
	char temp_str[7];
	char temp_len[2];
	char* temp_char_1=(char*)malloc(1*sizeof(char));
	char* temp_char_2=(char*)malloc(1*sizeof(char));
	int temp_int1;
	int temp_int2;
	int i, j;
	
	strncpy(temp_str, line+1, 6);
	start_addr = CSADDR + change_str_to_hexa(temp_str);	//set starting address for T line (relocation)

	TEXT[text_cnt] = start_addr;
	text_cnt++;	

	strncpy(temp_len, line+7, 2);
	t_len = change_str_to_hexa(temp_len);		//set length of T line
	j=9;
	for(i=0; i<t_len; i++){
		
		memset(temp_char_1, '\0', strlen(temp_char_1)*sizeof(char));
		memset(temp_char_2, '\0', strlen(temp_char_2)*sizeof(char));
		temp_char_1[0] = line[j]; 
		temp_char_2[0] = line[j + 1];
		
		temp_int1=change_str_to_hexa(temp_char_1);
		temp_int2=change_str_to_hexa(temp_char_2);
		one_digit_objcode = temp_int1*16 + temp_int2;	//combine two character to one byte
		memory[start_addr + i] = one_digit_objcode;		//load object code on memory by 1 byte
		j+=2;
	}
	free(temp_char_1);
	free(temp_char_2);	
}

//modify M object code in memory
void modify_memory(char* line){
	int m_addr;
	int m_len;
	unsigned int temp_mem_num=0;
	int referencing_num;
	int referencing_addr;
	char temp_str[7];
	char temp_len[2];	

	char* temp_ref_num=(char*)malloc(2*sizeof(char));
	int quotient;
	int i, j;

	strncpy(temp_str, line+1, 6);
	m_addr = change_str_to_hexa(temp_str);		//set modifying starting address

	for(i=0; i<(int)strlen(temp_str); i++) temp_str[i] = '\0';	//init temp_str
	strncpy(temp_len, line+7, 2);
	m_len = change_str_to_hexa(temp_len);		//set modifying length
		j = (m_len+1)/2;
		for(i=0; i<(m_len+1)/2; i++){	//
			temp_mem_num += (int)memory[CSADDR + m_addr + i] << ((j-1)*8);	//get original object code from memory	
			j--;
		}
		strncpy(temp_ref_num, line+10, 2);
		referencing_num = change_str_to_hexa(temp_ref_num);
		referencing_addr = find_symbol_addr(ref_tab[referencing_num]);
		if(line[9] == '+'){	//add symbol
			temp_mem_num += referencing_addr;
		}
		else temp_mem_num -= referencing_addr;		//subtract symbol
		
		//store modified object code on memory
		j = (m_len+1)/2;
		for(i=0; i<(m_len+1)/2; i++){
			quotient = temp_mem_num / (1 << (j-1)*8);
			memory[CSADDR + m_addr + i] = quotient;
			temp_mem_num -= ((1 << (j-1)*8) * quotient);
			j--; 		
		} 	
}

//find symbol's address on ESTAB
int find_symbol_addr(char* symbol){
	int i;
	
	for(i=0; i<ESTABcnt; i++){
		if(strcmp(symbol, ESTAB[i].csname) == 0 || strcmp(symbol, ESTAB[i].symname) == 0){
			return ESTAB[i].address;
		}
	}
	printf("symbol error\n");
	return -1;
}

int execute_prog(){
	int i;
	if(EXEC_LEN == -1){
		printf("no program loaded\n");
	}
	
	PC = EXEC_ADDR;
	int opcode;
	unsigned char flags, reg, ni;
	int format;
	int target;

	int cnt = 0;
	int MAX_CNT = (PROG_END - PROG_START) * 1000;
	int endFlag=0;
	
	if(EXEC_LEN + EXEC_ADDR >= 0x100000){
		printf("too long\n");
		return 0;
	}

	if(!continuing) L=PROG_END;
	while(PC >= PROG_START && PC < PROG_END){
		opcode = memory[PC];
		ni = opcode & (unsigned char)0x03;
		opcode &= (unsigned char)0xFC;

		format = search_with_opcode(opcode);
		int byte_size;

		if(format == 0){	//WORD or BYTE
			PC+=3;
		}

		else if(format == 3 && ni == 0){	//SIC MACHINE (ni)
			PC++;
			flags = memory[PC++];
			target = (flags & (unsigned char)0x7F) << 8;
			flags = flags >> 7 << 3;
			target += memory[PC++];
		
			if(!op_act(opcode, format, target, flags)) return 0;
				
			for(i=0;i<5;i++){
				byte_size = TEXT[i] - PC;
				if(byte_size < 1 || byte_size>2) byte_size = 0; 	//might be different			
			}
			PC += byte_size;
		}	
		else{
			//for oppcode
			PC++;
			switch(format){
				case 1:
					op_act(opcode, format, 0, 0);
					break;
				case 2:
					reg = memory[PC++];
					op_act(opcode, format, reg, 0);
					break;
				case 3:
					flags = memory[PC++];
					target = (flags & (unsigned char)0x0F) << 8;
					flags = (flags >> 4) + (ni << 4);
					
					if(flags%2){	//extened set
						format = 4;
						target = (target << 8);
					}					
					for(i = (format == 3) ? 0 : 1; i >= 0; i--){
						target += (memory[PC++]) << (i*8);
					}
					
					if(!op_act(opcode, format, target, flags)) return 0;
					break;
			}
		}

		if(++cnt == MAX_CNT){
			printf("program stopped\n");
			return 0;
		}
		if(BP_head){
			if(search_BP(PC)){
				print_reg();
				endFlag = 1;
				printf("     stop at checkpoint [%X]\n", PC);
				EXEC_LEN -= (PC - EXEC_ADDR);	
				EXEC_ADDR = PC;
				continuing = 1;
			 	break;
			}
		}
	}
	if(endFlag == 0){
		print_reg();
		printf("     End Prgogram\n");
		EXEC_ADDR = PROG_START;
		EXEC_LEN = PROG_END - PROG_START;
		continuing = 0;
	}
	return 1;
}

int search_with_opcode(int opcode){
	if(!op_head) return 0;
	optable_node* pmove;
	for(int i=0; i<20; i++){
		for(pmove=op_head[i]; pmove; pmove = pmove->link)
			if(pmove->opcode == opcode) return pmove->format;
	}
	return 0;
}

void print_reg(){
	printf("A : %06X X : %06X\n", A, X);
	printf("L : %06X PC : %06X\n", L, PC);
	printf("B : %06X S : %06X\n", B, S);
	printf("T : %06X\n", T);
}

int set_BP(char* addr){
	int bp_addr = change_str_to_hexa(addr);
	
	BP_node* pnew = (BP_node*)malloc(sizeof(BP_node));
	pnew->bp = bp_addr;
	pnew->link = NULL;
	
	printf("     [ok] create breakpoint %X\n", bp_addr);

	if(!BP_head){
		BP_head = pnew;
		return 1;
	}
	if(bp_addr < BP_head->bp){
		pnew->link = BP_head;
		BP_head = pnew;
	}
	else if(bp_addr == BP_head->bp){
		printf("     bp already exists\n");
		return 0;
	}
	else{
		pnew->link = BP_head->link;
		BP_head->link = pnew;

		BP_node* pmove1 = BP_head;
		BP_node* pmove2 = pnew->link;
		while(pmove2){
			if(pmove2->bp == bp_addr){
				printf("     bp already exists\n");
				return 0;
			}
			else if(pmove2->bp < bp_addr){
				pmove1->link = pmove2;
				pnew->link = pmove2->link;
				pmove2->link = pnew;
				pmove1 = pmove1->link;
				pmove2 = pnew->link;
			}
			else break;
		}
	}
	return 1;
}

int clear_BP(){
	if(!BP_head){
		printf("BP is empty\n");
		return 0;
	}
	BP_node* pfree;
	while(BP_head){
		pfree = BP_head;
		BP_head = BP_head->link;
		free(pfree);
	}
	BP_head = NULL;
	printf("      [ok] clear all breakpoints\n");
	return 1;
}

void print_BP(){
	if(!BP_head){
		printf("      no BP\n");
		return ;
	}
	printf("     breakpoint\n");
	printf("     ----------\n");
	BP_node* pmove = BP_head;
	for( ; pmove; pmove=pmove->link)
		printf("     %X\n", pmove->bp);
}

int search_BP(int PC){
	BP_node* pmove = BP_head;
	while(pmove){
		if(pmove->bp == PC) return 1;	//PC and break point match
		else pmove = pmove->link;	
	}
	return 0;
}

int* get_reg_ptr(unsigned char reg) {
    switch (reg) {
        case 0x00:
            return &A;
        case 0x01:
            return &X;
        case 0x02:
            return &L;
        case 0x03:
            return &B;
        case 0x04:
            return &S;
        case 0x05:
            return &T;
        case 0x06:
            return &F;
        case 0x08:
            return &PC;
        case 0x09:
            return &SW;
        default:
            return NULL;
    }
}

int op_act(int opcode, int format, int target, int flags){
	if(format == 1){
		switch(opcode){
			case 0xC4:
				A=(int)F;	
				break;
			case 0xC0:
				F=(float)A;
				break;
		}
	}
	else if(format == 2){
		unsigned char reg1 = target >> 4;
		unsigned char reg2 = target & (unsigned char)0x0F;

		int* r1 = get_reg_ptr(reg1);
		int* r2 = get_reg_ptr(reg2);
		switch(opcode){
			case 0x90:	//ADDR
				if(!r1) return 0;
				*r2 += *r1;
				break;				
			case 0xB4:	//CLEAR
				if(!r1) return 0;
				*r1=0;
				break;
			case 0xA0:	//COMPR
				if(*r1 == *r2) CC=0;
				else if(*r1 > *r2) CC=1;
				else CC = -1;
				break;
			case 0x9C:	//DIVR
				if(!r1 || !r2) return 0;
				if(*r1 == 0){
					 printf("divide error\n");
					return 0;
				}
				*r2 /= *r1;
				break;

			case 0x98:	//MULR
				if(!r1 || !r2) return 0;
				*r2 *= *r1;	
				break;
			case 0xAC:	//RMO
				if(!r1 || !r2) return 0;
				*r2 = *r1;
				break;
			case 0xA4:	//SHIFTL
				if(!r1) return 0;
				*r1 = (*r1 << (reg2+1));
				break;
			case 0xA8:	//SHIFTR
				if(!r1) return 0;
				*r1 = (*r1 >> (reg2+1));
				break;
			case 0x94:	//SUBR
				if(!r1 || !r2) return 0;
				*r2 -= *r1;
				break;
			case 0xB8:	//TIXR
				X += 1;
				if(!r1) return 0;
				if(X == *r1) CC = 0;
				else if(X > *r1) CC = 1;
				else CC = -1;
				break; 
		}	
	}
	else{	//format 3 and format 4
		unsigned char ni, x, b, p;
		ni = flags >> 4;
		x = (flags >> 3) % 2;
		b = (flags >> 2) % 2;
		p = (flags >> 1) % 2;

		if(p) target = (signed char)target + PC;
		else if(b) target += B;
		if(x) target += X;
		
		int mem_val = 0;
		int LOC = target;
		switch(ni){
			case 1:		//immediate
				mem_val = target;
				break;
			case 2:		//indirect
				for(int j=0; j<2; j++){		
					mem_val = 0;
					for(int i=2; i>=0; i--){
						mem_val += (memory[LOC++] << (i*8));
					}
					if(j==0){
						LOC = mem_val;
						target = mem_val;
					}
				}
				break;
			case 3:		//simple
				for(int i=2; i>=0; i--){
					mem_val += (memory[LOC++] << (i*8));
				}
				break;
		}
		
		switch(opcode){
			case 0x18:        // ADD m
               			 A += mem_val;
               			 break;
           
			case 0x40:        // AND m
  		        	A &= mem_val;
             		  	break;
  		        case 0x28:        // COMP m
             			if (A == mem_val) CC = 0;
  		                else if (A > mem_val) CC = 1;
             		        else CC = -1;
   		                break;
      		        case 0x24:        // DIV m
             		        if (mem_val == 0) {
                  			printf("Divided by 0\n");
               		        	return 0;
              		        }
    		                A /= mem_val;
            		        break;
      		      	case 0x3C:        // J m
          		        PC = target;
          		        break;
       		        case 0x30:        // JEQ m
         		        if (CC == 0) PC = target;
      		                break;
   		        case 0x34:        // JGT m
      		                if (CC == 1) PC = target;
    		                break;
     		        case 0x38:        // JLT m
               			if (CC == -1) PC = target;
           		        break;
   	 	       case 0x48:        // JSUB m
				L = PC;
                                PC = target;
                                break;
            	       case 0x00:        // LDA m
                       		A = mem_val;
              			break;
           	       case 0x68:        // LDB m
             			B = mem_val;
                		break;
            	       case 0x50:        // LDCH m
               			mem_val = (unsigned)(mem_val >> 16);// | (unsigned char)0xFFFF00;
                		A = (A >> 8) << 8;
                		A += mem_val;
                		break;
            	       case 0x08:        // LDL m
                		L = mem_val;
                		break;
            	       case 0x6C:        // LDS m
                		S = mem_val;
                		break;
                       case 0x74:        // LDT m
                		T = mem_val;
                                break;
            	       case 0x04:        // LDX m
                		X = mem_val;
               			break;
                       case 0x20:        // MUL m
                		A *= mem_val;
                		break;
                       case 0x44:        // OR m
                		A |= mem_val;
                		break;
                       case 0xD8:        // RD m
                		CC = 1;
                		break;
                       case 0x4C:        // RSUB
                		PC = L;
                		break;
                       case 0x0C:        // STA m
                		LOC -= 3;
                		if ( !write_to_memory(LOC, A) )
                    			return 0;
                		break;
                       case 0x78:        // STB m
                		LOC -= 3;
                       		if ( !write_to_memory(LOC, B) )
                    		return 0;
                		break;
            	       case 0x54:        // STCH m
                		LOC -= 3;
                		mem_val = A & (unsigned char)0x0FF;
                		memcpy(memory + LOC, &mem_val, 1);
                		break;
            	       case 0x14:        // STL m
                		LOC -= 3;
                		if ( !write_to_memory(LOC, L) )
                    		return 0;
                		break;
            	       case 0x7C:        // STS m
                		LOC -= 3;
                		if ( !write_to_memory(LOC, S) )
                    		return 0;
                		break;
                       case 0xE8:        // STSW m
                		LOC -= 3;
                		if ( !write_to_memory(LOC, SW) )
                    		return 0;
                		break;
                       case 0x84:        // STT m
                		LOC -= 3;
                		if ( !write_to_memory(LOC, T) )
                    		return 0;
                		break;
                       case 0x10:        // STX m
              			LOC -= 3;
             			if ( !write_to_memory(LOC, X) )
                 		return 0;
                		break;
                       case 0x1C:        // SUB m
                		A -= mem_val;
                		break;
                       case 0xE0:        // TD m
                		CC = 1;
                		break;
                       case 0x2C:        // TIX m
                		X += 1;
                		if (X == mem_val) CC = 0;
                		else if (X > mem_val) CC = 1;
                		else CC = -1;
                		break;
		}
	}
	return 1;
}

int write_to_memory(int LOC, int mem_val) {
    unsigned char b[3];
    b[0] = mem_val >> 16;
    b[1] = (mem_val >> 8) & (unsigned char)0x00FF;
    b[2] = mem_val & (unsigned char)0x00FF;
    
    for (int i = 0; i < 3; i++) {
        b[i] &= (unsigned char)0xFF;
        memcpy(memory + LOC++, &(b[i]), 1);
    }

    return 1;
}

