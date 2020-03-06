#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

//Creates global variables so I can easily access to them
char rooms[7][10];
int start_end[2];
char connection_array[6][10];
char path_to_victory[100][10];
int steps = 0;
int array_size_counter = 0;

void *getting_time(void*);


//Returns a directory pointer that I can later on change to the directory with rooms
char* getting_directory(char *latestDirName){
    int latestDirTime = -1; //modified the timestamp of newest subdir examined
    char targetDirPrefix[32] = "dengh.rooms."; //prefix we are looking for
    memset(latestDirName, '\0', sizeof(latestDirName));

    DIR * directory_check; //Holds the directory we are starting in
    struct dirent *fileInDir; //holds the current subdir
    struct stat dirAtt;//holds info we've gained about subdir

    directory_check = opendir("."); //opens the directory this program running in

    if (directory_check > 0){ // Make sure the current directory could be opened
        while ((fileInDir = readdir(directory_check)) != NULL){
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){ //If entry has prefix
             //   printf("Found the prefex: %s\n", fileInDir->d_name);
                stat(fileInDir->d_name, &dirAtt);

                if((int)dirAtt.st_mtime > latestDirTime){//If this time is bigger
                    latestDirTime = (int)dirAtt.st_mtime;
                    memset(latestDirName, '\0', sizeof(latestDirName));
                    strcpy(latestDirName, fileInDir->d_name);
                  //  printf("Newer subdir: %s, new time: %d\n", fileInDir->d_name, latestDirTime);
                }

            }
        }
    }
    closedir(directory_check); //Close the directory we opened

   // printf("Newest entry found is: %s\n", latestDirName);
    return latestDirName; // return the directory
}


//Checks for the last line of the file for the room type
void check_last_line(char *line, int i){

    char room_start[] = {"START_ROOM"};
    char room_end[] = {"END_ROOM"};
    char temp[20];

    strtok(line,"\n");

    strcpy(temp,line+11);

    //This stores the position of the start room file
    if(strcmp(room_start,temp) == 0){
        
        start_end[0]=i;
        return;
    }

    //stores the end room position
    if(strcmp(room_end,temp) == 0){
        start_end[1]=i;

        return;
    }

 
}


//Looking for the room type from the whole file
void finding_room_type(){
    FILE *file_pointer;
    char line[256];
    char cha;

    //started at -1 because I have 2 new lines at the end of the room type: \n\n
    int i, line_counter = -1, line_counter2 = 0;

    for(i = 0; i < 7; i++){
        file_pointer = fopen(rooms[i],"r");

        //reads the number of lines in a file
        while(!feof(file_pointer)){
            cha = fgetc(file_pointer);
            if(cha == '\n')
                line_counter++;
        }

        fclose(file_pointer);

        //printf("room: %s\n",rooms[i]);
        file_pointer = fopen(rooms[i],"r");
        while (fgets(line, sizeof line, file_pointer) != NULL){
            //reads through the file again, but this time, only grabs the last line for room type
            line_counter2 += 1;
            if( line_counter2 == line_counter ){
                check_last_line(line,i);
                break;
            }

        }

        fclose(file_pointer);
    }
}


//Looking for start room and end room from the files in the directory
void finding_start_end_room(char *latestDirName){


    DIR *dir_pointer;
    struct dirent* in_files;
   

    int counter = 0;

    //change to the rooms directory
    chdir(latestDirName);

    //opens the current directory and get the rooms
    dir_pointer = opendir(".");
    while ((in_files = readdir(dir_pointer)) != NULL){
        if(in_files->d_name[0] != '.' ){
            strcpy(rooms[counter], in_files->d_name);
         counter ++;
        }
    }

    closedir(dir_pointer);

    finding_room_type();

}


//reading the content of the file like the file name and connections
int location_read_file_content(char *line, int counter){

    //I dont want to look for room type
    char temp[256], room_type[]={"ROOM TYPE"};
    int x;

    
    strtok(line,"\n");
  
    //this is use for checking for the Room Type line, and we need to avoid getting that input
    for(x=0;x<9;x++){
        temp[x]=line[x];
    }
    temp[9] = '\0';


    //newline_remove removes the new line character from fgets
    if(strcmp(room_type,temp) != 0){
    	array_size_counter+=1;
        if(counter == 0){
                strcpy(connection_array[counter],line+11);
                return;
        } 
        else{ 
           strcpy(connection_array[counter], line+14);
        }
        return 1;
    }
    else if(strcmp(room_type,temp) == 0){
        return 0;
    }


}


//This function prints out the room information in a format
void print_room_info(){


	int x;
	printf("CURRENT LOCATION: %s\n", connection_array[0]);

	printf("POSSIBLE CONNECTIONS: ");
	for(x=1;x<array_size_counter;x++){
		if(x == array_size_counter-1)
			printf("%s.\n", connection_array[x]);
		else
			printf("%s, ", connection_array[x]);
	}
}


//This function is the room processing function that gets the user's input to go to another file from connection
char *initial_location(char *choose_room, int grab_room, char *latestDirName){


    FILE *file_pointer;
    char line[256];
    int counter = 0; 
    array_size_counter = 0;
    int condition;
    char checking_time[10] = {"time"};

    //Starts at 0 because that's the start room
    if(steps == 0){
        file_pointer = fopen(rooms[start_end[0]], "r");
    }
    else{
        file_pointer = fopen(rooms[grab_room], "r");
    }

    //just getting the connections from the file and store them into an array
    while(fgets(line, sizeof line, file_pointer) != NULL){
        condition = location_read_file_content(line,counter);
        counter++;
        if(condition == 0)
            break;
    }

    fclose(file_pointer);

    //Prints the room info to the user, takes in the counter and int position of room
    print_room_info();

    printf("WHERE TO? >");
    scanf("%s",choose_room);
    printf("\n");

    //Checks if the user inputs "time" and if so, calls the getting time function and calls back the initial function
    if(strcmp(choose_room,checking_time) == 0){
        //Creating threads for the time calling 
        pthread_mutex_t variable = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock (&variable);
        pthread_t th_id;
        pthread_create( &th_id, NULL, getting_time, (void *) &variable);
        pthread_mutex_unlock (&variable);
        pthread_join(th_id,NULL);
        
        chdir(latestDirName);
        
        
    }

    return choose_room;
}

//Got the name from the connections, compare to the room array to see which one matches
int find_connected_room(char *connection_array){
    int i;

    for(i=0;i<7;i++){
        if(strcmp(connection_array,rooms[i]) == 0)
            return i;
    }

}



//Getting the local time, reference from STACK OVERFLOW
void* getting_time( void*variable){
    FILE *file_pointer;

    //Locks the thread
    pthread_mutex_lock((pthread_mutex_t *) variable);
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    char out[200];

    //reformats the local time and print them out like the requirement
    strftime(out, 200 ,"%l:%M %p, %A, %B %d, %Y",timeinfo);

    printf(" %s\n\n",out);

    chdir("..");
    //takes into account when the directory has been changed

    
    //writing the time to the file
    file_pointer = fopen("currentTime.txt","w");
    fprintf(file_pointer," %s\n",out);
    fclose(file_pointer);

    //Unlocks the thread
    pthread_mutex_unlock((pthread_mutex_t *) variable);
}


//This is where the game starts and the game will loop as long as the user plays it
void gameplay(char *latestDirName){


	char* where_to_go, choose_room[256];    
    int x, if_room_exist = 0, grab_room;
    char checking_time[10] = {"time"};


    //This is where the game runs using a do while so I can call start room first
    do{


        //where to go is the user's input for which room does he/she wants to go
        where_to_go = initial_location(choose_room, grab_room,latestDirName);

        if(strcmp(checking_time,where_to_go) == 0)
            where_to_go = initial_location(choose_room, grab_room,latestDirName);

        else{
            strcpy(path_to_victory[steps],where_to_go);
            for(x=1;x<array_size_counter;x++){
                //if the connection matches user's input, store the position of the file location and indicates that the room exist
                if(strcmp(where_to_go,connection_array[x]) == 0){
                    grab_room = find_connected_room(connection_array[x]);
                    if_room_exist += 1;
                    steps += 1;
                    break;
                }
            }
            if (if_room_exist == 0){
                printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
            }

        }
    }while(grab_room != start_end[1]);//this is when the file location is not the same as end file


    //Prints out the victory path
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n",steps);

    int i;
    for(i=0;i<steps;i++){
        printf("%s\n",path_to_victory[i]);
    }

}


// Plays the adventure game after building the rooms
int main(){


    char latestDirName[256]; //creates a char array for directory name 
    getting_directory(latestDirName);

    finding_start_end_room(latestDirName);

    gameplay(latestDirName);

}