#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>





char key[200000], plaintext[200000];

//error message handling
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues


//this function is for file reading
int file_reading(char *argv, int type){
	int x=0;
	char character;

	FILE *f_pointer;
	f_pointer = fopen(argv,"r");

	while(!feof(f_pointer)){

		character = fgetc(f_pointer); //gets each character and checks them if they're alphabets and a space
		if(character == '\n')
			break;
		
		if(isalpha(character) || character == ' '){
			if (type == 2)
				key[x] = character;
			else
				plaintext[x] = character;

			x += 1;
		}
		else{
			fprintf(stderr,"opt_dec error: input contains bad characters\n");
			exit(1);
		}


	}
	fclose(f_pointer);

	return x;
}


//main function, modified from the client.c file
int main(int argc, char *argv[])
{
	int socketFD, portNumber, charWrite, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	int en_num=0, text_num=0;
	char *concat;
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	//localhost
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){ // Connect socket to address
		
			error("CLIENT: ERROR connecting");
		}

	memset(key, '\0', sizeof(key));
	memset(plaintext, '\0', sizeof(plaintext));

	en_num=file_reading(argv[2],2); //gets the number of characters
	text_num=file_reading(argv[1],1);

	if (en_num < text_num){ //check if the key is shorter than the text
		fprintf(stderr,"Error: key '%s' is too short\n",argv[2]);
		exit(1);
	}

	concat = malloc((text_num + en_num + 1)*sizeof(char));

    memset(concat, '\0', sizeof(concat));
    strcpy(concat,"d"); //file type indicator
	strcat(concat,plaintext);
	strcat(concat,"|");
	strcat(concat,key);
	strcat(concat,"#"); //end of file indicator

	// Send message to server


		while(1){
			if(charWrite == strlen(concat))
				break;
			charWrite = send(socketFD, concat, strlen(concat), 0); //sends info to the server
		}
		if (charWrite < 0) error("CLIENT: ERROR writing to socket");


	// Get return message from server
	memset(key, '\0', sizeof(key)); // Clear out the key again for reuse
	memset(plaintext, '\0', sizeof(plaintext));
	int x;

	
		charsRead = recv(socketFD, key, sizeof(key), 0); // Read data from the socket, leaving \0 at end

        while(strstr(key,"#") == NULL){ //checks the end of the file
	       if(strcmp(key,"Invalid") == 0){ //receives the message from the server and exit
             fprintf(stderr, "otp_dec cannot access otp_dec_d\n");
             exit(2);
		    }		
			char buffer[200000];
            memset(buffer, '\0', sizeof(buffer));

			charsRead = recv(socketFD, buffer, sizeof(buffer), 0); //receive info from the server
			if (charsRead < 0) error("CLIENT: ERROR reading from socket");
			strcat(key,buffer);

		}


	key[strlen(key)-1] = '\0';
	printf("%s\n",key);

	close(socketFD); // Close the socket
	return 0;
}
