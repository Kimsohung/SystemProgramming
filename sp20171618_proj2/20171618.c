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
