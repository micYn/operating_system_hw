#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

bool waitforchild=true;
//true from the start.

void redirectin(char *filename){
    	int in = open(filename, O_RDONLY);	
	//now "in" is the file descriptor of fileName, O_RDONLY means that this file will be read-only

    	dup2(in,0);	
	//STDIN is now refering to the same input of filename

    	close(in);	
	//so fileName is no longer in need
}

void redirectout(char *filename){
    	int out = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0600);	
	/*now out is the file descriptor of fileName, 
	  write-only,truncates the contents if the file exists, create one if it doesn't exist.*/

    	dup2(out,1);	
	//STDOUT is now refering to the same output of fileName

    	close(out);
	//fileName is no longer in need
}

void execmd(char **argv){
	pid_t pid;
	if(strcmp(argv[0],"exit")==0)	exit(1);
	else{
		pid = fork();
		
		if(pid<0)	fprintf(stderr,"fork failed");

		else if(pid==0){	//child process
			execvp(argv[0],argv);	
		}
		else{	//parent process
			if(waitforchild){
				waitpid(pid,NULL,0);
			}
			else	waitforchild=false;
		}
		/*important*/
		redirectin("/dev/tty");		//redirect STDIN to terminal 
            	redirectout("/dev/tty");	//redirect STDout to terminal
	}
}

void createpipe(char **argv){

	int fd[2];
	pipe(fd);
	//shell creates a pipe
 
	dup2(fd[1],1);	
	close(fd[1]);
	//let the result of process1 be directed to the input port of the pipe	
	
	execmd(argv);
	//the shell has forked and process1's job is done at this moment,so all the result is in the pipe and shell is resumed.
	//Note at this time, STDIN & STDOUT are both redirected back to normal.( cf. execmd() )
	//So now STDIN is no longer directed to the input port of this pipe.
	//now we starts to direct process2 to read from this pipe's output port:

	dup2(fd[0],0);
	close(fd[0]);
	//let process2's input to be read from the pipe later when we execcvp process2
	//so process2 will be processing the things that is written in the pipe earlier by process1
	//we do this by setting STDIN to be refering to the output port of the pipe
	//also note that children inherit the pipe that is created by their parent
	
	//process2 will later be stored in argv and execute by that execvp() in the shell
	//Again, fork() will be done and shell will wait until process2 finish
	//Due to the work that we did, process2 will read from the pipe but output the result by its normal style
	//after process2 finished and the shell resumes, both STDIN & STDOUT will be redirected back to normal(terminal)
	//At this moment, every thing has been done and recovered perfectly and the shell is ready to read the next command
}

int main(int ac,char **argv){
	char *prompt = "Mshell $ ";	//prompt message
	char *lineptr=NULL;		//the address of the buffer holding user command
	size_t n = 0;			//the allocated size (in bytes)
	ssize_t nchars_read=0;		//how many characters did getline get
	char* readycmd=NULL;		//stores the command that includes ' '
	char* token=NULL;		//temporary container for the parsed commands
	int i,j;			//for for loop
	(void)ac;			//did not used

	while(1){
		/**				      			**
		 **********Prompt and receive the command*****************
		 **				     			**/
		//1.printing prompt message:
		printf("%s", prompt);
		fflush(stdout);
		//2.reading command using getline:
		nchars_read = getline(&lineptr, &n, stdin);

		//return if getline failed or reached the EOF
		if(nchars_read == -1){
			//printf("Exiting...\n");
			return -1;
		}
		

		/**				      			**
		 **********Start parsing commands*************************
		 **				     			**/
		//1.first we need to add ' ' between special characters and store it in a string called readycmd
		//allocate a space for readycmd
		readycmd = (char*)malloc((nchars_read*2)*sizeof(char));

		//start adding
		j=0;
		for (i=0;i<nchars_read-1;i++){	//ignored '\n' by using < instead of <=
			if(lineptr[i] != '>' && lineptr[i] != '<' && lineptr[i] != '|'){
			    readycmd[j++] = lineptr[i];
			}
			else{
			    readycmd[j++] = ' ';
			    readycmd[j++] = lineptr[i];
			    readycmd[j++] = ' ';
			}
		}
		readycmd[j++] = '\0';
		
		//if there is an & at the end of the readycmd -> wait
		if(readycmd[strlen(readycmd)-1]=='&'){
			waitforchild=true;
			readycmd[strlen(readycmd)-1]='\0';	//the & is no long needed
		}

		/*2.finally we can start parsing readycmd into several tokens 
	            and store them into argv using strtok and also branch to the basic functions*/
		argv = (char**)malloc(sizeof(char*) * 60);
		token = strtok(readycmd," ");
		i=0;
		while(token){
			if(*token=='<'){
				redirectin(strtok(NULL," "));
			}
			else if(*token=='>'){
				redirectout(strtok(NULL," "));
			}
			else if(*token=='|'){
				argv[i]=NULL;
				createpipe(argv);
				i=0;
			}
			else{
				argv[i] = token;
				i++;
			}
			token = strtok(NULL," ");
		}
		argv[i]=NULL;



		execmd(argv);

	}	
	free(lineptr);
	free(readycmd);
	for(i=0;i<60;i++)	free(argv[i]);
	free(argv);

	return 0;
}
