#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>


//This is the error function that prints out the error
void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues


//This is the encryption function that encrypts any text
char *encryption(char *buffer_old){

	char key[200000],message[200000];
	char *result = malloc(200000*sizeof(char));
	char array[27]="ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	int i,x,checker=0;
	int temp, key_holder, message_holder ;
	char buffer[200000];
	memset(buffer, '\0', sizeof(buffer)); //memset all the array to make sure everything is a null
	memset(result, '\0', sizeof(result));
	memset(key, '\0', sizeof(key));
	memset(message, '\0', sizeof(message));
	strcpy(buffer,buffer_old+1); //Take out the file identifier

	x=0;
	for(i=0;i<strlen(buffer);i++){//loop through the message and store the info
		
		if(buffer[i] == '|'){
			checker = 1;
			continue;
		}
		else if(buffer[i] == '#')
			continue;

		if(checker == 1){
				key[x] = buffer[i];
				x +=1;
		}
		else
				message[i] = buffer[i];
		
	}


	for(i=0;i<strlen(message);i++){ //going through and encodes the message with the key

		int x;
		for(x=0;x<27;x++){
			if(message[i] == array[x])
				message_holder = x;
			if(key[i] == array[x])
				key_holder = x;
		}

		temp = (message_holder + key_holder)%27;
		result[i] = array[temp];
	}


	return result;


}


//main function, modified the server.c file
int main(int argc, char *argv[])
{
	pid_t spawnPid = -5, wpid;
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char temp_holder[200000];
	struct sockaddr_in serverAddress, clientAddress;
	char *en_message;
	int status;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

	// Accept a connection, blocking if one is not available until one connects
	int x;
	while(1){ //constantly waiting for connections
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect

		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept

		spawnPid = fork();

		switch(spawnPid){

			case -1:{
				perror("Fork failed\n");
				close(establishedConnectionFD);
				close(listenSocketFD);
				exit(1);
			}
			case 0:{
		// 		// Get the message from the client and display it
				
				memset(temp_holder, '\0', 200000);	
				
			
				charsRead = recv(establishedConnectionFD, temp_holder, sizeof(temp_holder), 0); // Read the client's message from the socket
				if(temp_holder[0] != 'e') //if the client is not from encrypting client side, it will reject.
					charsRead = send(establishedConnectionFD, "Invalid", 8, 0);

				else{
					
					while(strstr(temp_holder,"#") == NULL){//if it's not end of the file

						char buffer[200000];
						memset(buffer, '\0', sizeof(buffer));
						charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0);
						
						strcat(temp_holder,buffer);
		
					}
					if (charsRead < 0) error("ERROR reading from socket");
				
					en_message = encryption(temp_holder); //calls the encryption function

					strcat(en_message,"#"); //adds the end of file indicator

					// Send a Success message back to the client
					do{
						charsRead = send(establishedConnectionFD, en_message, strlen(en_message), 0);
						if (charsRead < 0) error("ERROR writing to socket");
					}while(charsRead < strlen(en_message));
				
				}

					close(establishedConnectionFD); // Close the existing socket which is connected to the client

				
			}
			default:{


			}


		}
	
	}
	close(listenSocketFD); // Close the listening socket

	return 0; 
}
