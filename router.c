#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/socket.h>
#include <unistd.h>
#include<signal.h>
#include <setjmp.h>
#define MAX_STR_LENGTH 2000
#define IP 2130706433  /* 127.0.0.1 */
char stats_path[MAX_STR_LENGTH];
char network_address_A[MAX_STR_LENGTH];
char network_address_B[MAX_STR_LENGTH];
char network_address_C[MAX_STR_LENGTH];
void file_writer(char file_name[]);
void handler();
void routing_table(char routing_table[],char incoming_address[]);

/*
char Router1[] = " ";
char Router2[] = " ";
char Router3[] = " ";

int prefix_A;
int prefix_B;
int prefix_C;

char nexthop_A[MAX_STR_LENGTH]; 
char nexthop_B[MAX_STR_LENGTH];
char nexthop_C[MAX_STR_LENGTH];

long addrA;
long addrB;
long addrC;
*/
long addr_dest;

int packetID;
char sourceIP[MAX_STR_LENGTH];
char destIP[MAX_STR_LENGTH];
char payload[MAX_STR_LENGTH];
int ttl;

int expired_packets = 0;
int unroutable_packets = 0;
int delivered_direct_packets = 0;
int delivered_B = 0;
int delivered_C = 0;

/*
void fileReader(char path[]){
	FILE *fp;
	//printf("%s\n", path);
	fp = fopen(path,"r");
	char network_address[MAX_STR_LENGTH];
	char nexthop[MAX_STR_LENGTH];
	int prefix;
	
	if(fp == 0){
		fprintf(stderr,"Error while opening the routing table.\n");
		exit(1);
	}

	while(fscanf(fp, "%s %d %s", network_address, &prefix, nexthop) != EOF){
		//printf("%s\n",network_address);
		if (strncmp(nexthop,"0",strlen("0")) == 0){
			printf("this is the muthafuckin prefix for a %d\n", prefix);
			strncpy(network_address_A,network_address, strlen(network_address));
			prefix_A = prefix;
			if (strncmp(Router1," ",strlen(" ")) == 0){
				strncpy(Router1,"A",strlen("A"));
			}else if((strncmp(Router2," ",strlen(" ")) == 0)&& (strncmp(Router1," ",strlen(" "))!= 0)){
				strncpy(Router2,"A",strlen("A"));
			} else if((strncmp(Router3," ",strlen(" ")) == 0)&&(strncmp(Router2," ",strlen(" ")) != 0)&& (strncmp(Router1," ",strlen(" "))!= 0)){
				strncpy(Router3,"A",strlen("A"));
			}
			
			
		}else if (strncmp(nexthop, "RouterB",strlen("RouterB")) == 0){
			//printf("%s\n", nexthop);
			printf("this is the muthafuckin prefix for b %d\n", prefix);
			strncpy(network_address_B,network_address, strlen(network_address));
			//int *tempB = &prefix;
			prefix_B = prefix;
			if (strncmp(Router1," ",strlen(" ")) == 0){
				strncpy(Router1,"B",strlen("B"));
			}else if((strncmp(Router2," ",strlen(" ")) == 0)&& (strncmp(Router1," ",strlen(" "))!= 0)){
				strncpy(Router2,"B",strlen("B"));
			} else if((strncmp(Router3," ",strlen(" ")) == 0)&&(strncmp(Router2," ",strlen(" ")) != 0)&& (strncmp(Router1," ",strlen(" "))!= 0)){
				strncpy(Router3,"B",strlen("B"));
			}
						

		}else if (strncmp(nexthop,"RouterC",strlen("RouterC")) == 0){
			printf("this is the muthafuckin prefix for c %d\n", prefix);
			strncpy(network_address_C,network_address, strlen(network_address));
			//int *tempC = &prefix;
			prefix_C = prefix;
			if (strncmp(Router1," ",strlen(" ")) == 0){
				strncpy(Router1,"C",strlen("C"));
			}else if((strncmp(Router2," ",strlen(" ")) == 0)&& (strncmp(Router1," ",strlen(" "))!= 0)){
				strncpy(Router2,"C",strlen("C"));
			} else if((strncmp(Router3," ",strlen(" ")) == 0)&&(strncmp(Router2," ",strlen(" ")) != 0)&& (strncmp(Router1," ",strlen(" "))!= 0)){
				strncpy(Router3,"C",strlen("C"));
			}
			
		}
		
	}
	printf("network address a %s\n",network_address_A);
 	printf("network address b %s\n",network_address_B);
	printf("network address c %s\n",network_address_C);
	printf("This is the net prefix for a %d\n", prefix_A);
	printf("This is the net prefix for b %d\n", prefix_B);
	printf("This is the net prefix for c %d\n", prefix_C);
	printf("Router1:%s Router2:%s Router3: %s\n", Router1, Router2, Router3);
	fclose(fp);
	addrA = htonl(inet_addr(network_address_A));
	addrB = htonl(inet_addr(network_address_B));
	addrC = htonl(inet_addr(network_address_C));
	addrA = addrA >> (32 - prefix_A);
	addrB = addrB >> (32 - prefix_B);printf("in main\n");
	addrC = addrC >> (32 - prefix_C);
}
*/

//This is where I write to file all of the router statistics.
void file_writer(char file_name[]){
	FILE *fp;
	fp = fopen(file_name,"w");
	if(fp == 0){
		fprintf(stderr,"Error while opening the stats file.\n");
		exit(1);
	}
			
	fprintf(fp,"expired packets: %d\n",expired_packets);
	fprintf(fp,"unroutable_packets: %d\n",unroutable_packets);
	fprintf(fp,"delivered direct: %d\n",delivered_direct_packets);
	fprintf(fp,"router B: %d\n",delivered_B);
	fprintf(fp,"router C: %d\n",delivered_C);
	fclose(fp);
}

//This function reads the routing table for each packet and determines whether
//there is a match in the routing table or not and "delivers the packet properly"
void routing_table(char routing_table[],char incoming_address[]){
	//printf("in routing table function\n");
	FILE *fp;
	//printf("%s\n", path);
	char network_address[MAX_STR_LENGTH];
	char nexthop[MAX_STR_LENGTH];
	int prefix;
	int match = 0;
	long curr_addr;
	long temp_packet_addr;
	long manipulated_packet_addr;
	//printf("\n Thisd is the incoming IP  dest address %s\n",incoming_address);
	temp_packet_addr = htonl(inet_addr(incoming_address));
	fp = fopen(routing_table,"r");
	if(fp == 0){
		fprintf(stderr,"Error while opening the routing table.\n");
		exit(1);
	}
	while(fscanf(fp, "%s %d %s", network_address, &prefix, nexthop) != EOF){
		//printf("%s\n", network_address);
		
		curr_addr = htonl(inet_addr(network_address));
		

		manipulated_packet_addr = temp_packet_addr >> (32 - prefix);
		//printf("incoming packet address after shift %ld\n", manipulated_packet_addr);
		curr_addr = curr_addr >> (32 - prefix);
		//printf("routing table address %ld\n",curr_addr);

		if(curr_addr == manipulated_packet_addr){
			match = 1;
			if(strncmp(nexthop,"0",strlen("0"))==0){
				delivered_direct_packets++;
				printf("packet ID=%d, dest=%s,\n",packetID,destIP);
				
			}else if(strncmp(nexthop,"RouterB",strlen("RouterB"))==0){
				delivered_B++;
				
			}else if(strncmp(nexthop,"RouterC",strlen("RouterC"))==0){
				delivered_C++;
			}
			break;	
		}

	}
	if (match == 0){
		unroutable_packets++;
		
	}
	match = 0;	
	fclose(fp);
}

void handler(){
	file_writer(stats_path);
	exit(1);
}


int main(int argc, char *argv[]){
	struct sockaddr_in servaddr,cliaddr;
	(void) signal(SIGINT,handler);
	if (argc != 4){
		fprintf(stderr,"You have not enetered the required parameters\n");
		exit(1);
	}
	//int file_update_count = 0;
	int packet_socket, packet_sock_length = sizeof(cliaddr);
	int port_no;
	int count = 0;
	char packet[MAX_STR_LENGTH];
	char *splicer;
	//char temp_port_no[MAX_STR_LENGTH];
	//strcpy(temp_port_no,argv[1]);
	char routing_table_path[MAX_STR_LENGTH];
	strncpy(routing_table_path,argv[2],strlen(argv[2]));
	//printf("in main\n");
	//printf("%s\n",argv[2]);
	strncpy(stats_path,argv[3],strlen(argv[3]));
	
	port_no = atoi(argv[1]);
	//printf("this should be the port_no %d\n",port_no);
	if ( ( packet_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) == -1 ){
		printf("Error in creating socket");
		return 1;
	}
	memset((char *) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port_no);
	servaddr.sin_addr.s_addr = htonl(IP);
	
	if( bind(packet_socket, (const struct sockaddr *) &servaddr, sizeof(servaddr)) == -1 ){
		printf("Error in binding socket");
		return 2;
	}
	//printf("\n\nServer listening to %s:%d\n\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
	char destination_IP[MAX_STR_LENGTH];
	char temp_packet[MAX_STR_LENGTH];
	while(1){
		//recieving the packet and splicing the data
		if (recvfrom(packet_socket, packet, MAX_STR_LENGTH, 0, (struct sockaddr *) &cliaddr,(socklen_t *) &packet_sock_length) != -1){
			//printf("ok\n");
			//printf("\nReceived packet from %s:%d  Data: %s\n\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), packet);
			//printf("packet is : %s", packet);
			//strcpy(temp_packet,packet);
			//sscanf(packet,"%d, %s, %s, %d, %s", &packetID, sourceIP, destination_IP, &ttl, payload);
			splicer = strtok(packet,", ");
			packetID = atoi(splicer);
			splicer = strtok(NULL,", ");
			strcpy(sourceIP,splicer);
			splicer =strtok(NULL,", ");
			strcpy(destIP,splicer);
			splicer = strtok(NULL,", ");
			ttl = atoi(splicer);
			splicer = strtok(NULL,", ");
			strcpy(payload, splicer);
			
			///printf("\n is it here %d %s\n",ttl, destIP);
			
			if ((ttl - 1) == 0){
				expired_packets++;
			}else{
				//printf("here\n");
				
				routing_table(argv[2],destIP);
			}

			count++;
			//writing to the file after 20 packets
			if(count == 20){
				file_writer(argv[3]);
				count = 0;
			}
			
	
			
		}


	}
	close(packet_socket);
	return 0;

}
