/* $begin shellmain */
#include "csapp.h"
#include<errno.h>
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv, int* argc);
int builtin_command(char **argv); 
void exec_cd();
int fatal(char* s);
void piperec(char**coms[], int*p, int ncmd, int pipe_cnt);
int pipeline(char**coms[], int pipe_cnt);

int main() 
{
    char cmdline[MAXLINE]; /* Command line */

    while (1) {
	/* Read */
	printf("> ");                   
	if(fgets(cmdline, MAXLINE, stdin)); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline);
    } 
}
/* $end shellmain */
  
/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    char filepath[30];
    char* bin = "/bin/";
    int argc;
    int pipe_cnt=0;
    char *command[10][10], **s[10];
    int i=0, j=0, k=0, l=0;

    for(i=0; i<strlen(cmdline); i++){
        if(cmdline[i] == '|') pipe_cnt++;
    }

    strcpy(buf, cmdline);
    bg = parseline(buf, argv, &argc);
    i=0;j=0;k=0; 
    
    if(pipe_cnt){
        while(k<argc){
            while(strcmp(argv[k], "|")!=0){
                //printf("im'in\n");
                char tmp_str[10]={0};
                strcpy(tmp_str, argv[k]);
                command[i][j]=malloc(sizeof(char)*strlen(tmp_str));
                strcpy(command[i][j], tmp_str);
                //command[i][j] = tmp_str;
                j++;
                k++;
                if(k==argc) break;

            }
            command[i][j] = NULL;
            s[i] = command[i];
            k++;
            i++;
            j=0;
            //printf("pipe done\n");
        }
        pipeline(s, pipe_cnt);
        return;
    }
    if (argv[0] == NULL)  
	    return;   /* Ignore empty lines */

    if (!builtin_command(argv)) { //quit -> exit(0), & -> ignore, other -> run
        if(!strcmp(argv[0], "cd")){
            exec_cd(argv, argc);
                return;
        }
        else{    
            strcpy(filepath, bin);
            strcat(filepath, argv[0]);
            if((pid = Fork()) == 0){

                if (execve(filepath, argv, environ) < 0) {	//ex) /bin/ls ls -al &
                    printf("%s: Command not found.\n", argv[0]);
                    exit(0);
                }
            }
        }
	/* Parent waits for foreground job to terminate */
	if (!bg){ 
	    int status;
        if(waitpid(pid, &status, 0) < 0)
            unix_error("waitfg: waitpid error");
	}
	else//when there is backgrount process!
	    printf("%d %s", pid, cmdline);
    }
    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "quit") || !strcmp(argv[0], "exit")) /* quit command */
	exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
	return 1;
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv, int *argc) 
{
    char *delim;         /* Points to first space delimiter */
    //int argc;            /* Number of args */
    int bg;              /* Background job? */

    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	buf++;

    /* Build the argv list */
    *argc = 0;
    while ((delim = strchr(buf, ' '))) {
	argv[(*argc)++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[*argc] = NULL;
    
    if (*argc == 0)  /* Ignore blank line */
	return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[(*argc)-1] == '&')) != 0)
	argv[--(*argc)] = NULL;

    return bg;
}
/* $end parseline */

void exec_cd(char* argv[], int argc){
    char* path;
    if(argc > 1)
        path = argv[1];
    else if((path = (char*)getenv("HOME")) == NULL)
        path = ".";
    
    if(chdir(path) == -1)
        fprintf(stderr, "wrong directory!\n");
}

int fatal(char* s){
    perror(s);
    exit(1);
}

void piperec(char **coms[], int* p, int ncmd, int pipe_cnt){
    int np[2];
    char **com;
    com = coms[pipe_cnt-ncmd];
    if(ncmd==pipe_cnt){
        close(1); // stdout == 1
        dup(p[1]); //  p[1] is writing part
        close(p[1]);
        close(p[0]);
        execvp(com[0],com);
    }
    else{
        if(pipe(np)==-1)
        fatal("pipe call error");
        switch(fork()){
        case -1: fatal("fork error in pipeRec");
        case 0: ncmd++; piperec(coms,np,ncmd,pipe_cnt);
        default : 
            close(0);  // stdin == 0
            dup(np[0]);
            close(np[0]);
            close(np[1]);
            close(1);
            dup(p[1]);
            close(p[1]);
            close(p[0]);
            execvp(com[0],com);
        }
    }
}

int pipeline(char** coms[], int pipe_cnt){
    // each command 
    int i=0,p[2],status;
    char **com1,**com2;

    for(i=0;i<pipe_cnt;i++){
        if(i==0){
        com1 = coms[0];
        }
        com1 = coms[i+1];
    }
  
    // First child
    switch(fork()){
        case -1: fatal("1st fork call in pipeCmd");
        case 0:  break;
        default: 
            // Parent
            wait(&status);
            return status;
    }

    if(pipe(p)==-1)
        fatal("pipe call in join");
    switch(fork()){
        case -1: fatal("2nd fork call in pipeCmd");
        case 0:    // writing process
        // under child 1.
        piperec(coms,p,1,pipe_cnt);
        default :  // Reading process
        // child 1.
        com2 = coms[pipe_cnt];
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        execvp(com2[0],com2);
    }
}
