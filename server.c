#include <wiringPi.h>
#include <softPwm.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 1234
#define P_PIN 1

void move_servo(int seconds){
	printf ("Raspberry Pi wiringPi test program\n");
	wiringPiSetup();
	pinMode (P_PIN, PWM_OUTPUT);

	digitalWrite(P_PIN, 0);
	softPwmCreate(P_PIN,0,200);

	printf ("Bajo\n");
	softPwmWrite(P_PIN, 10);
	delay(1000);

	printf ("Termino la bajada, espero\n");
	softPwmWrite(P_PIN, 0);
	delay(seconds*1000);
	printf ("Termino la espera, subo\n");
	softPwmWrite(P_PIN, 16);
	delay(100);
	return;

}

void clean_buffer(char buffer[]){
	int i = 0;
	while((buffer[i] != 0)&&(i<1024)){
		buffer[i] = 0;
		i++;
	}
	printf("-----------------------------\n");
}

int check_fire(char buffer[]){
	int fire = 0;
	char temporal[1024];
	strcpy(temporal, buffer);
	char* token = strtok(temporal, " ");

  	if ((buffer[0]=='f')&&(buffer[1]=='i')&&(buffer[2]=='r')&&(buffer[3]=='e')){
		token = strtok(NULL," ");
		if (token !=NULL){
			fire = atoi(token);
		}
	}
	return fire;
}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
	int seconds;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Forcefully attaching socket to the port 1234
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    // Forcefully attaching socket to the port 1234
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
	while(buffer[0] != 'X'){
		clean_buffer(buffer);
	    valread = read( new_socket , buffer, 1024);
		if (seconds = check_fire(buffer)){
			printf("FIRE IN THE HOLEEEE FOR %d MILISECONDS\n",seconds);
			move_servo(seconds);

		}

		if (buffer[0] == 0){
			return 0;
		}
	}
    return 0;
}
