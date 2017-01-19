#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include <setjmp.h>
#define MAX_STR_LENGTH 2000
#define SERVERPORT 9933
#define IP 2130706433  /* 127.0.0.1 */
char router_address_A1[] = "192.168.128.1";
char router_address_A2[] = "192.168.128.7";
char router_address_B1[] = "192.168.192.4"; 
char router_address_B2[] = "192.168.192.6";
char router_address_B3[] = "192.168.192.10";
char router_address_C1[] = "192.224.0.5";
char router_address_C2[] = "192.224.0.7";
char router_address_C3[] = "192.224.10.5";
char router_address_C4[] = "192.224.15.6";
char router_address_unknown[] = "168.130.192.01";
char dest[MAX_STR_LENGTH];
char source[MAX_STR_LENGTH];
char stats_file[MAX_STR_LENGTH];
int a2b = 0;
int a2c = 0;
int b2a = 0;
int b2c = 0;
int c2a = 0;
int c2b = 0;
int invalid = 0;
int sum = 0;


void packet_chooser(){
	int random_dest;
	int random_source;
	int random_sub_dest;
	int random_sub_source;
	int coin_flip;
	//choosing the destination and source
	random_dest = (rand()%4);
	random_source =(rand()%3);
	
	// checking for conflicts
	while (random_dest == random_source){
			coin_flip = (rand()%2);
			if (coin_flip == 0){ 
				random_dest = (rand()%4);
			}else{
				random_source = (rand()%3);
			}
	}
	
	//updating the stats
	if (random_dest == 3){
			invalid++;
	} else if((random_source == 0) && (random_dest == 1)){
			a2b++;
	} else if((random_source == 0) && (random_dest == 2)){
			a2c++;
	} else if((random_source == 1) && (random_dest == 0)){
			b2a++;
	} else if((random_source == 1) && (random_dest == 2)){
			b2c++;
	} else if((random_source == 2) && (random_dest == 0)){
			c2a++;
	} else if((random_source == 2) && (random_dest == 1)){
			c2b++;
		
	}
	//getting the specific routing destination
	if (random_dest == 0){
		random_sub_dest = rand()%2;
		if (random_sub_dest == 0){
			strcpy(dest,router_address_A1);
		}else{
			strcpy(dest,router_address_A2);
		}
	} else if (random_dest == 1){
		random_sub_dest = rand()%3;
		if (random_sub_dest == 0){
			strcpy(dest,router_address_B1);
		}else if (random_sub_dest == 1){
			strcpy(dest,router_address_B2);
		}else{
			strcpy(dest,router_address_B3);
		}
	} else if (random_dest == 2){
		random_sub_dest = rand()%4;
		if (random_sub_dest == 0){
			strcpy(dest,router_address_C1);
		}else if (random_sub_dest == 1){
			strcpy(dest,router_address_C2);
		}else if (random_sub_dest == 2){
			strcpy(dest,router_address_C3);
		}else{

			strcpy(dest,router_address_C4);
		}
	} else if (random_dest == 3){
			strcpy(dest,router_address_unknown);
	}
	

	//getting the specific routing source
	if (random_source == 0){
		random_sub_source = rand()%2;
		if (random_sub_source == 0){
			strcpy(source,router_address_A1);
		}else{
			strcpy(source,router_address_A2);
		}
	} else if (random_source == 1){
		random_sub_source = rand()%3;
		if (random_sub_source == 0){
			strcpy(source,router_address_B1);
		}else if (random_sub_source == 1){
			strcpy(source,router_address_B2);
		}else{
			strcpy(source,router_address_B3);
		}
	} else if (random_source == 2){
		random_sub_source = rand()%4;
		if (random_sub_source == 0){
			strcpy(source,router_address_C1);
		}else if (random_sub_source == 1){
			strcpy(source,router_address_C2);
		}else if (random_sub_source == 2){
			strcpy(source,router_address_C3);
		}else{
			strcpy(source,router_address_C4);
		}

	}

	
	//printf("this is the destination %s\n",dest);
	//printf("this is the source %s\n",source);
	
}

//This is where I do my wrting to the stats file
void file_writer(char file_name[]){
	FILE *fp;
	fp = fopen(file_name,"w");
	if(fp == 0){
		fprintf(stderr,"Error while opening the stats file.\n");
		exit(1);
	}
			
	fprintf(fp,"NetA to NetB: %d\n",a2b);
	fprintf(fp,"NetA to NetC: %d\n",a2c);
	fprintf(fp,"NetB to NetA: %d\n",b2a);
	fprintf(fp,"NetB to NetC: %d\n",b2c);
	fprintf(fp,"NetC to NetA: %d\n",c2a);
	fprintf(fp,"NetC to NetB: %d\n",c2b);
	fprintf(fp,"Invalid Destination: %d\n",invalid);
	fclose(fp);
			

}

void handler(){
	file_writer(stats_file);
	exit(1);
}

int main(int argc, char *argv[]){
	//Declaring variables and ensuring the right amount of arguments supplied
	struct sockaddr_in servaddr,cliaddr;
	(void) signal(SIGINT,handler);
	if (argc != 3){
		fprintf(stderr,"You have not enetered the required parameters\n");
		exit(1);
	}
	int  packet_socket;
	char packet[MAX_STR_LENGTH];
	int  port_no;
	char temp_port_no[MAX_STR_LENGTH];
	int  ttl = 0;
	char ttl_string[MAX_STR_LENGTH];
	char payload[] = "Ballin";
	int  id = 0;
	strncpy(stats_file,argv[2],strlen(argv[2]));
	char id_string[MAX_STR_LENGTH];
	
	//printf("%s\n",argv[1]);
	strcpy(temp_port_no,argv[1]);
	port_no = atoi(temp_port_no);
	//printf("this should be the port_no%s\n",argv[1]);
	//printf("This is the port number %d\n",port_no);
	
	//Setting up the socket
	if((packet_socket  = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("Error Creating Socket");
		return 1;
	}


	servaddr.sin_family =  AF_INET;
	servaddr.sin_port = htons(port_no);
	servaddr.sin_addr.s_addr = htonl(IP); 

	int count = 0;
	int big_count = 0;
	int waiting_count = 0;
	while (1){
		//constructing the packet
		id += 1;
		ttl = (rand()%4) + 1;
		packet_chooser();
		sprintf(ttl_string,"%d",ttl);
		sprintf(id_string,"%d",id);
		strcpy(packet,id_string);
		strcat(packet,", ");
		strcat(packet,source);
		strcat(packet,", ");
		strcat(packet,dest);
		strcat(packet,", ");
		strcat(packet,ttl_string);
		strcat(packet,", ");
		strcat(packet,payload);
		//printf("%s\n",packet);
		//printf("\nSending %s to %s:%d\n", packet, inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
		//sending the packet
		sendto(packet_socket,packet, strlen(packet)+1,0,(struct sockaddr *)&servaddr,sizeof(servaddr));
		count++;
		waiting_count++;
		if(waiting_count == 2){
			sleep(1);
			waiting_count = 0;
		}
		if (count == 20){
			
			file_writer(argv[2]);
			
			count = 0;
		}
		//big_count++;
		
		
	}
	//sum = a2b + a2c + b2a+ b2c + c2a + c2b + invalid;
	//printf("\nsum %d\n", sum);
	close(packet_socket);
	return 0;
}
