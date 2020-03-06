//Program: Small Shell
//Name: Hao (Jeff) Deng

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

//Making global variables to pass around in the functions
char *quitting = "exit";
char *change_directory = "cd";
char *status = "status";
int background_operator;
int file_output_open;
int file_input_open;
int allow_background = 1;

pid_t spawnPid_temp_holder;

//Making a struct sigaction for signal handling
struct sigaction SIGINT_action = {0}, ignore_action = {0}, SIGTSTP_action = {0};


//Making a struct array for holding information about child_PID
struct childPID_info{
	pid_t childPid_array[200];
	int num_child_process;
	int last_exit_status;
	int signal_or_exit;

};


//This built-in function is for changing the directory. Handling command "cd" and "cd whatever"
void changing_directory(char **command_array){

	//If the command only contains cd, then go to the HOME directory
	if(command_array[1] == NULL || strcmp(command_array[1],"&") == 0){
		chdir(getenv("HOME"));
		return;
	}
	
	//Changing the directory and prints error if the directory does not exist
	if(chdir(command_array[1]) != 0)
		perror("");

}


//This function is for exiting the program. One of the built in function, and it kills off the childs
void exit_program(struct childPID_info *child_info){

	int i;

	//Going through the childPid array and kill them off
	for(i=0;i<200;i++){
		if(child_info->childPid_array[i] == -5)
			break;
		kill(child_info->childPid_array[i],SIGKILL);
	}

	exit(0);
}


//This function is to print out the status of the last exit status of a child. A built-in function
void calling_status(struct childPID_info child_info){

	//Signal = 0 and Exit = 1, so the message prints out corresponds to value
	if(child_info.signal_or_exit == 1){	
		printf("exit value %d\n",child_info.last_exit_status);
		fflush(stdout);
	}
	else{
		printf("terminated by signal %d\n", child_info.last_exit_status);
		fflush(stdout);
	}
	
}


//This function is to get the child exit status and store the values inside the struct
void getting_status(struct childPID_info *child_info, int childExitMethod){

	//Receive the status and store inside the child_info object
	if(WIFEXITED(childExitMethod)){
		child_info->last_exit_status = WEXITSTATUS(childExitMethod);
		child_info->signal_or_exit = 1;
	}
	else{
		child_info->last_exit_status = WTERMSIG(childExitMethod);
		child_info->signal_or_exit = 0;
	}

}


//This is the file handler function in which it will get and store the file name if existed when the user enters > or <
int file_handler(char*input_file_name, char*output_file_name){
	int x = 0;
	int input_background = 0;
	int output_background = 0;

	//If the file name is not in the array, indicates the user that the file does not exist
	if(input_file_name[0] != '\0'){
		if((file_input_open = open(input_file_name, O_RDONLY)) < 0){
			printf("cannot open %s for input\n", input_file_name);
			fflush(stdout);
			exit(1);
		}
		dup2(file_input_open,0); //Redirects the stdin
		close(file_input_open);
		input_background += 1;
		x += 1;
	}

	//If the file name is not in the array, indicates the user that the file does not exist
	if(output_file_name[0] != '\0'){
		if((file_output_open = creat(output_file_name, 0644)) < 0){
			printf("cannot open %s for output\n", output_file_name);
			fflush(stdout);
			exit(1);
		}
		dup2(file_output_open,1); //Redirects the stdout
		close(file_output_open);
		x += 1;
		output_background += 1;
	}

	//When it's not redirected and it's in the background, redirect to dev null
	if(input_background == 0 && output_background == 0 && background_operator == 1){
		freopen("/dev/null", "w", stdout);
		freopen("/dev/null", "r", stdin);
		fcloseall();
	}



	return x;
}


//This is the execution handler in which that this function will put the command in to execvp and run
void execution_handler(char **command_array, int c, struct childPID_info *child_info, int childExitMethod){

	char **command_holder = malloc(1*sizeof(char*));
	command_holder[0] = malloc(10*sizeof(char));

	//Check if it's in the background or not and handle the signal action
	if(background_operator == 0){
		SIGINT_action.sa_handler = SIG_DFL;

	}
	else{
		SIGINT_action.sa_handler = SIG_IGN;
	}

	sigaction(SIGINT, &SIGINT_action, NULL);

	
	//If the file name exist, string copy the command and run that command and all the redirections because the execvp will have error if we pass > or < into the execvp
	if(c >= 1){
		strcpy(command_holder[0], command_array[0]);
		if(execvp(command_holder[0], command_holder) == -1){
			perror("");
			exit(2);
		}
	}
	else{	
		if(execvp(command_array[0], command_array) == -1){
			perror("");
			exit(2);
		}
	}
}



//This the parent waiting function, this function waits under a certain condition for the child
void parent_wait(pid_t *spawnPid, int childExitMethod, struct childPID_info *child_info){
	
	pid_t parentPid = -5;

	//If there's a background command, this wait statement should run and store the background Pid into the childPid array
	if(background_operator == 1 && allow_background == 1){
		child_info->childPid_array[child_info->num_child_process] = spawnPid_temp_holder;
		child_info->num_child_process++;
		parentPid = waitpid(*spawnPid, &childExitMethod, WNOHANG);	
	}
	else{

		parentPid = waitpid(*spawnPid, &childExitMethod, 0);
	}

	//Retrieving the the status of the child and store into the struct
	getting_status(child_info, childExitMethod);

}


//This is the function where it does all the forking
void forking(char **command_array, int array_size, struct childPID_info *child_info, char *input_file_name, char * output_file_name){

	//Create some variables for the use of forking
	int childExitMethod = -5;
	pid_t spawnPid = -5;
	pid_t temp_spawn = -5;

	spawnPid = fork();
	spawnPid_temp_holder = spawnPid; //This makes a copy of the spawn id as a global variable so I can use it anywhere in the child 

	switch(spawnPid){

		case -1:{
			perror("Fork failed!\n");
			exit(1);
		}
		case 0:{

			int c = file_handler(input_file_name,output_file_name); //When the child is forked, it will go to the file_handler function and returns a c value to indicate whether a file name is existed in the command
			execution_handler(command_array,c, child_info, childExitMethod); //Goes to execution
			exit(2);
			break;
		}
		default:{
			//If the command is being passed to the background, the background message with the Pid will be printed
			if(background_operator == 1 && allow_background == 1){
				printf("background pid is %d\n", spawnPid_temp_holder);
				fflush(stdout);
			}

			//This will call the parent function to wait for the child
			parent_wait(&spawnPid, childExitMethod,child_info);
			break;
		}

	}


	//This condition is for waiting for the BG process to finish and prints out the message indicates that the BG process of ID whatever is done with a status
	if(allow_background == 1){
		while((spawnPid_temp_holder = waitpid(-1, &childExitMethod, WNOHANG)) > 0){
			printf("background pid %d is done: ", spawnPid_temp_holder);
			fflush(stdout);
			getting_status(child_info, childExitMethod);
			calling_status(*child_info);


		}
	}


}


//This is for checking whether if the command line contains a background operator 
void check_for_bg(char**command_array, int array_size){

	//If we enter foreground mode
	if(allow_background == 0)
		return;

	if(strcmp(command_array[array_size-1],"&") == 0) //Setting the background_operator global variable if it exists
		background_operator = 1;
	else
		background_operator = 0;


}


//This is the non-built-in function that handles all the commands that are not cd, exit, or status
void non_built_in_commands(char **command_array, int array_size, struct childPID_info *child_info){

	int stand_in = 0;
	int stand_out = 0;
	char input_file_name[50];
	char output_file_name[50];
	memset(input_file_name,'\0',50*sizeof(char));
	memset(output_file_name,'\0',50*sizeof(char));
	int new_array_size;

	//Checks if the user enters a background operator at the end of the line
	check_for_bg(command_array, array_size);

	//Checks for the global operator if it's 0, then we can check if the last argument if it's < or >, then we can prompt the user that we need a file
	if(background_operator == 0){
		if(strcmp(command_array[array_size-1],">") == 0 || strcmp(command_array[array_size-1],"<") == 0){
			printf("Cannot redirect without a file\n");
			fflush(stdout);
			return;
		}
	}

	//If we detect a background operator in the command line, then we should handle that by setting that argument to null
	if(background_operator == 1 || strcmp(command_array[array_size-1],"&") == 0){
		command_array[array_size-1] = NULL;
		new_array_size = array_size-1;
	}

	else{
		new_array_size = array_size;
	}

	// Checks for the redirections and store the file name into array
	int i;

	for(i=0;i<new_array_size;i++){ //This loop is to go through the whole command line 2d array

		if(strcmp(command_array[new_array_size-1],">") == 0 || strcmp(command_array[new_array_size-1],"<") == 0){			

			command_array[new_array_size-1] = NULL;
			new_array_size = new_array_size-1;
			printf("Redirection error\n");
			fflush(stdout);
			break;
		}

		//check if the file exist after the < or > operator
		if(strcmp(command_array[i],"<") == 0 && stand_in == 0){
			stand_in = 1;
			strcpy(input_file_name,command_array[i+1]);
		}

		else if (strcmp(command_array[i],">") == 0 && stand_out == 0){
			stand_out = 1;
			strcpy(output_file_name,command_array[i+1]);
		}

	}


	//Call the forking function after doing all the command handling
	forking(command_array,new_array_size,child_info,input_file_name,output_file_name);
	



}


//This is the initialization function for the struct
void intializing_struct(struct childPID_info *child_info){

	struct childPID_info child_info_copy;

	child_info_copy = *child_info;

	int pid_array_size;
	//Assigning a negative value into the childPid array as default
	for(pid_array_size=0;pid_array_size<200;pid_array_size++)
		child_info_copy.childPid_array[pid_array_size] = -5;

	child_info_copy.num_child_process = 0;
	child_info_copy.last_exit_status = 0;
	child_info_copy.signal_or_exit = 1;

	*child_info = child_info_copy; 

}


//This is for handling the $$ argument in the command line to print out the Pid
void shell_id(char **command_array, int array_size){
	int i,j;
	char holder[2048];
	for(i=0;i<array_size;i++){
		for(j=0;j<strlen(command_array[i]);j++){
			if(command_array[i][j] == '$' && command_array[i][j+1] == '$'){	//Handling the $$ by assigning them to null and attach that will the actual Pid
				command_array[i][j] = '\0';
				command_array[i][j+1] = '\0';
				snprintf(holder, 2048, "%s%d", command_array[i], getpid());
				strcpy(command_array[i],holder);
			}
		}
	}

}


//This is the entering foreground mode function, which triggers by when the user does control z or send SIGTSTP signal
void entering_foreground_mode(int signo){

	//If we are in the foreground, we turn set the allow background to 0 to indicates only foreground processes allowed
	if(allow_background == 1){
		char *message = "Entering foreground-only mode (& is now ignored)\n";
		allow_background = 0;
		write(STDOUT_FILENO, message, 49);
		fflush(stdout);
	}
	else{
		char *message = "Exiting foreground-only mode\n";

		allow_background = 1;
		write(STDOUT_FILENO, message, 29);
		fflush(stdout);
	}




}


//This is the main function for first handling the signals, getting the user command inputs, and executing the command appropriately
int main(){

	char *input = malloc(2048*sizeof(char)); //Making dynamic arrays for better memory usage
	char **command_array = malloc(512*sizeof(char*));
	int command_size;

	//signal handling struct declaration reference from the Block 3 slides
	SIGINT_action.sa_handler = SIG_IGN;
	sigfillset(&SIGINT_action.sa_mask);
	sigaction(SIGINT, &SIGINT_action, NULL);


	SIGTSTP_action.sa_handler = entering_foreground_mode; //If SIGTSTP is sent, it triggers the foreground mode function
	SIGTSTP_action.sa_flags = SA_RESTART; // it will reset before interruption
	sigfillset(&SIGTSTP_action.sa_mask);
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	for(command_size=0;command_size<512;command_size++)
		command_array[command_size] = malloc(50*sizeof(char));


	struct childPID_info child_info; //Making a struct PID array object

	intializing_struct(&child_info); //pass struct object by reference


	//these variables are for getline function call and chopping them into a 2d array
	char *saveptr;
	char *spaces = " ";
	int num_chars_entered;
	size_t bufferSize = 0;

	//Infinite while loop for keep prompting the user for input until the user enters "exit"
	while(1){

		int x;
		while(1){	//This loop is for prompting the user input with getline
			printf(": ");
			fflush(stdout);
			num_chars_entered = getline(&input, &bufferSize, stdin);

			if(num_chars_entered == -1)
				clearerr(stdin);
			else
				break;
		}

		//Newline character will be assigned with a null
		input[strlen(input)-1] = '\0';


		//If newline or comment character is detected, we continue with the loop
		if(input[0] == '#' || input[0] == '\0')			
			continue;
		

		else{ //This section is for cutting up the user input into pieces and store them into the 2d array

			int cutting_count = 0;
			command_array[0] = strtok_r(input, spaces, &saveptr);

			while(1){
				cutting_count += 1;
				command_array[cutting_count] = strtok_r(NULL, spaces, &saveptr);
				if(command_array[cutting_count] == NULL)
					break;
			}


			//This checks if the user's input contains the $$ for printing Pid
			shell_id(command_array,cutting_count);

			
			if(strcmp(command_array[0],change_directory) == 0){ //Comparing the string to see if the user entered "cd"
				
				changing_directory(command_array);
			}

			else if(strcmp(command_array[0],quitting) == 0){//Comparing the string to see if the user entered "exit"

				exit_program(&child_info);
			}

			else if(strcmp(command_array[0],status) == 0){//Comparing the string to see if the user entered "status"

				calling_status(child_info);
			}
			else{
				
				non_built_in_commands(command_array,cutting_count, &child_info); //Comparing the string to see if the user entered any other non built-in commands 

			}

			
		}


	}


	return 0;

}

