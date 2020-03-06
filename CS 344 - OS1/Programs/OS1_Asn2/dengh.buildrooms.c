#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#define room1 "MU"
#define room2 "KEC"
#define room3 "LINC"
#define room4 "McNary"
#define room5 "Library"
#define room6 "Arnold"
#define room7 "ILLC"
#define room8 "SEC"
#define room9 "Tianfu"
#define room10 "Waldo"

//Making global arrays to going back and forth easier between functions
char ROOM_NAME_ARRAY[7][9];
int OUTBOUND_CONNECTION[7][7];


//Forward declaration
void connect_room(int,int);


//Room connection making functions are referencing from Canvas 2.2 section
void room_initializing(char *directory_pid_name){

    FILE *file_pointer;
	//Assigning 10 numbers into an array so I can randomize it
 	int r_array[10] = {0,1,2,3,4,5,6,7,8,9};
 	int x,i,temp;

	char rooms[10][9] = {room1,room2,room3,room4,room5,room6,room7,room8,room9,room10};
	
	//Changing the directory to the rooms, so I can access to the rooms
	chdir(directory_pid_name);


		/*Randomizing an array of 10 numbers to act as a number generator*/
		for(i=0;i<10;i++){
			int ran = rand()%10;
			temp=r_array[i];
			r_array[i]=r_array[ran];
			r_array[ran]=temp;
		}

		//Opening the file and writing the room name into the files
		for(x=0;x<7;x++){
			file_pointer = fopen(rooms[r_array[x]], "w");
			fprintf(file_pointer, "ROOM NAME: %s\n", rooms[r_array[x]]);
			fclose(file_pointer);
			strcpy(ROOM_NAME_ARRAY[x],rooms[r_array[x]]);

		}

}

/*Checks if all the rooms have 3-6 connections*/
int is_graph_full(){


	int i,j;
	int counter = 0;

	for(i=0;i<7;i++){

		//If they have a connection, which is 1, then counter ++
		for(j=0;j<7;j++){
			if(OUTBOUND_CONNECTION[i][j] == 1)
				counter++;
	
		}

		//keep track of whether all of the files have 3-6 connections
		if(counter<3 || counter > 6)
			return 0;
	counter = 0;
	}
	return 1;
}


//This function is for adding random connections to the rooms
void adding_random_connection(){


	int room_a = rand()%7;
	int room_b = rand()%7;

	while(1){
		if (can_add_connection_from(room_a) == 1)
			break;
		else
			room_a = rand()%7;
	}

	while(can_add_connection_from(room_b) == 0 || is_same_room(room_a, room_b) == 1 || connection_already_exists(room_a,room_b) == 1){
		room_b = rand()%7;
	}
	
	//Connecting the rooms together
	connect_room(room_a,room_b);
	connect_room(room_b, room_a);

}


//Checking if the room is able to add connection
int can_add_connection_from(int room_x){

	int i, counter = 0;

	for(i=0;i<7;i++){
		if(OUTBOUND_CONNECTION[room_x][i] == 1)
			counter +=1;
	}
	//if the room has less than 6 connections, it can add more connections
	if (counter < 6)
		return 1;
	else
		return 0;
}


//Checking if the connection is already exist
int connection_already_exists(int room_x, int room_y){
	if(OUTBOUND_CONNECTION[room_x][room_y] == 1)
		return 1;
	else
		return 0;

}


//whether if the two rooms are the same
int is_same_room(int room_x, int room_y){

	if(room_x == room_y)
		return 1;
	else
		return 0;

}


//Connects the room two rooms together
void connect_room(int room_a, int room_b){

	OUTBOUND_CONNECTION[room_a][room_b] = 1;

}

//This function builds the connections
void room_connection_builder(char *directory_pid_name){

	while (is_graph_full() == 0){
		
		adding_random_connection();


	}

}

//Writing the conenctions into the files
void room_writing_file(){

	int i,j,temp,counter = 0;
	FILE *file_pointer;
	//1=start room, 2=end room, 0=mid room
	int random_int[7] = {1,2,0,0,0,0,0};

	//randomize the int array order
	for(i=0;i<7;i++){
		int ran = rand()%7;
		temp=random_int[i];
		random_int[i]=random_int[ran];
		random_int[ran]=temp;
	}

	//writing connection: ******* to the files
	for(i=0;i<7;i++){
		counter = 0;
		for(j=0;j<7;j++){
			if(OUTBOUND_CONNECTION[i][j] == 1){
				counter +=1;
				file_pointer = fopen(ROOM_NAME_ARRAY[i], "a");
				fprintf(file_pointer, "CONNECTION %d: %s\n", counter, ROOM_NAME_ARRAY[j]);
				fclose(file_pointer);
			}

		}
	
		file_pointer = fopen(ROOM_NAME_ARRAY[i], "a");

		//After randomizing the array, randomized the files and just assign the room type
		if(random_int[i] == 1)
			fprintf(file_pointer, "ROOM TYPE: START_ROOM\n");
		else if(random_int[i] == 2)
			fprintf(file_pointer, "ROOM TYPE: END_ROOM\n");
		else
			fprintf(file_pointer, "ROOM TYPE: MID_ROOM\n");

		fclose(file_pointer);
	}

}


int main(){


    int p_id = getpid();
    char* directory_name = "dengh.rooms.";
    char directory_pid_name[19];


    /*Intializes random number*/
    srand((unsigned) time(0));

	//makes a directory with the process id
    snprintf(directory_pid_name,19, "%s%d", directory_name,p_id);
    mkdir (directory_pid_name, 0777);

    room_initializing(directory_pid_name);

	room_connection_builder(directory_pid_name);

	//writes the files
	room_writing_file();

}
