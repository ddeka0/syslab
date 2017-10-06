/*
Debashish Deka
173050055
*/

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <semaphore.h>

using namespace std;

#define ITER 2000
#define N 2
#define PORT 8787
#define PORT_2 8788
#define BUF_SIZE 1024
#define W_Queue 5000
#define MAX_CLIENT_COUNT 2000
#define BUF_SIZE_SMALL 512

#define ID_SENT		0
#define MT_LOGIN	0
#define MT_CONV 	1
#define MT_ENTRY	2
#define MT_WIN		3
#define MT_DRAW		4
#define REJECT_1 	5
#define REJECT_2	6
#define REJECT_3	7

typedef struct data {
	int socket_fd;
}p_address;
struct client_infos {
	int ID_1;
	int ID_2;
	int sockfd_1;
	int sockfd_2;
	bool turn;
};
int new_sockfd[MAX_CLIENT_COUNT];
map<int, int > ID_SOCK_PAIR;
map<int,int> waiting;
map<int, int> match;
map<int, int> playing;
char DELIM = ':';
char * backend_ip_addr;
int xx = 0;
/*template code for log purpose */
template<typename T> 
void LOG(T const& t) { 
	std::cout << t; 
}
template<typename First, typename ... Rest> 
void LOG(First const& first, Rest const& ... rest) {
	std::cout << first<<" "; LOG(rest ...);
}
/*template code for log purpose */


sem_t pp;

void * handle_game(void *ptr) {

	struct timeval tv;
	tv.tv_sec = 30;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors
	//setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));

	client_infos *p = (client_infos *)(ptr);
	char buffer[BUF_SIZE_SMALL];
	memset(buffer,0,BUF_SIZE_SMALL);
	bool _turn = p->turn;
	//printf("Game created\n");
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
	if(sockfd < 0) {
		perror("socket creation failed in client side : ");
		exit(1);
	}else { 
		
		//printf("\t\tsocket created in the client_1 side. \n");
		
	}
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));  
	addr.sin_family = AF_INET;  
	addr.sin_addr.s_addr = inet_addr(backend_ip_addr);
	addr.sin_port = PORT_2;
	if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {  
		perror("\t\tError connecting to the server-2 :\n");  
		exit(1);
	}else { 
		//printf("\t\tConnected to the server-2\n");
	}
	
	setsockopt(p->sockfd_1, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
	setsockopt(p->sockfd_2, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
	
	int times = 0;
	while(true) {

		bool DONE = false; 
		bool all_done = false;
		bool p1 = false,p2 = false;

		while(!DONE) {

			memset(buffer,0,BUF_SIZE_SMALL);
			
			if(!_turn) {
				//LOG("loc-0 : server-1 is about to call recv() for ",((client_infos*)ptr)->ID_1,clock(),"\n");
				
				int res1 = 0;

				if( ( res1 = recv(p->sockfd_1,buffer,BUF_SIZE_SMALL,0 )) < 0) {
					perror("\t\tError :\n");
					//LOG("\t\t","Error in server-1 side. Exiting...","\n");
					exit(0);
				}else {

					cout <<" IMPORTANT ="<<res1<<endl;
					//LOG("loc-1 :recieved from ",((client_infos*)ptr)->ID_1," := ",buffer[1],clock(),"\n");
					if(buffer[0] == (1<<MT_WIN)) {
						DONE = true;
						p1 = true;
					//	printf("\t\t%d wins the game\n",p->ID_1);
					}else if(buffer[0] == (1<<MT_DRAW)) {
						//printf("game draw\n");
						DONE = true;
					//	printf("\t\tGAME draw in between %d and %d \n",p->ID_1,p->ID_2);
					}
				}
				char temp =  buffer[1];
				memset(buffer,0,sizeof(buffer));
				buffer[0] = temp;
				int df = 0;
				if((df = send(p->sockfd_2,buffer,strlen(buffer),0)) < 0) {
					perror("\t\tError ending data.\n");
				}else {
					//if(df != 1) LOG("possible error\n");
					//LOG("loc-2 : ",buffer[0]," sent to ",((client_infos*)ptr)->ID_2,clock(),"\n");
					//printf("\t\tpp data sent to %d : %d\n",p->ID_2,buffer[0]);
				}
				_turn ^= 1;
			}else {
				//LOG("loc-3 : server-1 is about to call recv() for ",((client_infos*)ptr)->ID_2,clock(),"\n");
				int res2 = 0;
				if( ( res2= recv(p->sockfd_2,buffer,BUF_SIZE_SMALL,0) ) < 0) {
					
					perror("\t\tError :\n");
					exit(0);
				
				}else {
					
					//if(res2 != 2) LOG("dhiksauuu\n");
					
					//LOG("loc-4 :recieved from ",((client_infos*)ptr)->ID_2," := ",buffer[1],clock(),"\n");
					if(buffer[0] == (1<<MT_WIN)) {
						DONE = true;
						p2 = true;
					//	printf("\t\t %d wins the game\n",p->ID_2);
					}else if(buffer[0] == (1<<MT_DRAW)) {
						//printf("draw\n");
						DONE = true;
					//	printf("\t\tGAME draw in between %d and %d \n",p->ID_1,p->ID_2);			
					}
				}
				char temp =  buffer[1];
				memset(buffer,0,sizeof(buffer));
				buffer[0] = temp;


				//buffer[0] = buffer[1];
				int rf = 0;
				if((rf = send(p->sockfd_1,buffer,strlen(buffer),0)) < 0) {
					perror("Error ending data.\n");
				}else {
				//	if(rf != 1) LOG("llllllllllllllllll\n");

					//LOG("loc-5 : ",buffer[0]," sent to ",((client_infos*)ptr)->ID_1,clock(),"\n");
					//printf("\t\trr Data sent to %d : %d\n",p->ID_1,buffer[0]);
				}
				_turn ^= 1;
			}
		}
		
		memset(buffer,0,BUF_SIZE_SMALL);
		
		if(p1 == true) {

			sprintf(buffer,"%d:%d:%d:",1,p->ID_1,p->ID_2);

			//buffer[0] = 1;
			//buffer[1] = p->ID_1;
			//buffer[2] = p->ID_2;

		}else if(p2 == true) {

			sprintf(buffer,"%d:%d:%d:",2,p->ID_1,p->ID_2);
			
			//buffer[0] = 2;
			//buffer[1] = p->ID_1;
			//buffer[2] = p->ID_2;

		}else {

			sprintf(buffer,"%d:%d:%d:",3,p->ID_1,p->ID_2);

			//buffer[0] = 3;
			//buffer[1] = p->ID_1;
			//buffer[2] = p->ID_2;
			
		}

		char temp[100];
		

		sprintf(temp,"%s",buffer);
		int gf = 0;
		if( (gf = send(sockfd,buffer,strlen(buffer),0) ) < 0) {
			perror("Error ending data.\n");
		}else {
			cout << "gf = "<<gf<<endl;
			
			//	if(gf != 3) LOG("EEEEEEEEEEEEE\n");
			//	LOG("loc-6 : result sent to server-2 for",((client_infos*)ptr)->ID_1 ,",",((client_infos*)ptr)->ID_2,clock(),"\n");
			//	printf("\t\ttt data sent to %d : %d\n",p->ID_1,buffer[0]);
		
		}



		memset(buffer,0,BUF_SIZE_SMALL);
		//LOG("loc-7 :server-1 about to call recv() from server-2 for ",((client_infos*)ptr)->ID_1,clock(),"\n");
		int res3 = 0;
		if( (res3 = recv(sockfd,buffer,BUF_SIZE_SMALL,0) )< 0) {
			perror("\t\tError :\n");
		}else {

		//	if(res3 != 6) { 
		//		printf("res = %d\n",res3);
		//		LOG(" aaaaaaaaaaaaa\n");
		//	}
			//LOG("res = ",res3," KKKKKKKKKKKKKKKKKKKkk\n");
			//LOG("loc-8 :loc-recieved score from"," server-2 "," of ",((client_infos*)ptr)->ID_1,clock(),"\n");
			if((res3 = send(p->sockfd_1,buffer,strlen(buffer),0)) < 0) {
				perror("\t\tError ending data.\n");
			}else {
		//		if(res3 != 6) LOG("cccccccccccccc\n");
			//	LOG("loc-9 :sent score to ",((client_infos*)ptr)->ID_1,clock(),"\n");
				//printf("\t\tyy Data sent to %d : %s\n",p->ID_1,buffer);
			}
		}
		
		memset(buffer,0,BUF_SIZE_SMALL);
		buffer[0] = 1;
		
		int gh = 0;
		if( (gh = send(sockfd,buffer,strlen(buffer),0)) < 0) {
			perror("\t\tError ending data.\n");
		}else {
		//	if(gh != 1) LOG("awesome !\n");
			//LOG("loc-10 :server-1 sent confirmation to server-2 about sending score to",((client_infos*)ptr)->ID_1,clock(),"\n");
			// nothing
		}


		memset(buffer,0,BUF_SIZE_SMALL);
		//LOG("loc-11 :server-1 about to call recv() from server-2 for ",((client_infos*)ptr)->ID_2,clock(),"\n");
		int res4 = 0;
		if((res4 = recv(sockfd,buffer,BUF_SIZE_SMALL,0) ) < 0) {
			perror("Error :\n");
		}else {
		//	if(res4 != 6) {
		//		printf("res = %d\n",res4);
		//		LOG("ffffffffffffff\n");
		//	}
			//LOG("res = ",res4," WWWWWWWWWWWWWW\n");
			//LOG("loc-12 :recieved score from"," server-2 "," of ",((client_infos*)ptr)->ID_2,clock(),"\n");
			if( (res4 = send(p->sockfd_2,buffer,strlen(buffer),0)) < 0) {
				perror("\t\tError ending data.\n");
			}else {
		//		if(res4 != 6) LOG("qqqqqqqqqqqqqqqq\n");
				//LOG("loc-13 :sent score to ",((client_infos*)ptr)->ID_2,clock(),"\n");
			}
		}


		//printf("\n");
		//playing[p->ID_1] = 1;
		//playing[p->ID_2] = 1;
		//close(p->sockfd_1);
		//close(p->sockfd_2);
		cout << "game over for "<<p->ID_1<<" and "<<p->ID_2<<endl;
		
		times++;
		if(times >= ITER) break;

	
	}


	pthread_exit(NULL);
}


void process_buffer(char *,int *,int *);

sem_t p,q;


int main(int argc,char **argv) {
	//sem_init(&p,0,1);
	//sem_init(&q,0,1);

	srand(time(NULL));
	if(argc < 2) {
		perror("\t\tusage : server_1 < server2_ip_address > : ");
		exit(1);
	}else {
		backend_ip_addr = argv[1];
	}
	int sockfd_listen = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sockfd_listen < 0) {
		perror("\t\tError creating listening socket \n");
		exit(1);
	}else printf("\t\tListening socket created in the server side\n");
	
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = PORT;
	if (bind(sockfd_listen, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		printf("\t\tError binding!\n");
		exit(1);
	}else printf("\t\tBinding done with PORT : %d\n",PORT);

	printf("\t\tWaiting for a connection from client side\n");
	listen(sockfd_listen, W_Queue);
	
	struct sockaddr_in cl_info_addr;
	int len = sizeof(cl_info_addr);
	int client_count = 0;
	bool LISTEN_FLAG_SERVER = true;


	pthread_t thread_pool[10000];
	

	int thread_count = 0;
	char buffer[BUF_SIZE_SMALL];
	memset(buffer,0,BUF_SIZE_SMALL);
	int temp_sock;
								// main thread is polling connections using the listen_socket
	while(LISTEN_FLAG_SERVER) { // check new connections ....
		
		

		new_sockfd[client_count] = accept(sockfd_listen, (struct sockaddr *) &cl_info_addr, (socklen_t *)&len);
		
		if(new_sockfd[client_count] < 0) {
			perror("\t\t sockfd : \n");
			//printf("\t\tError accepting connection!\n");
			exit(1);
		}else {
			char clientAddr[INET_ADDRSTRLEN];
			inet_ntop(AF_INET,&(cl_info_addr.sin_addr),clientAddr,INET_ADDRSTRLEN);
			//printf("\t\tConnection accepted from %s\n", clientAddr); 	
		}
		memset(buffer,0,BUF_SIZE_SMALL);
		xx++;
		//printf("xx = %d\n",xx);
		if(recv(new_sockfd[client_count],buffer,BUF_SIZE_SMALL,0) < 0) {
			perror("\t\tError : recv failed inside rcvmsg function\n");
		}else {
			xx--;
		//	printf("xx = %d\n",xx);
			int p1,p2;
			process_buffer(buffer,&p1,&p2);
			//printf("value of p1 = %d and p2 = %d\n",p1,p2);
			if(ID_SOCK_PAIR[p1] > 0) temp_sock = ID_SOCK_PAIR[p1];
			ID_SOCK_PAIR[p1] = new_sockfd[client_count];
		
			if(match[p2] == p1) {

				//LOG(p1," and ",p2," pair created ",clock(),"\n");
				//printf("%d and %d pair ok\n",p1,p2);
				//printf("%d and %d pair created \n",p1,p2);
				memset(buffer,0,sizeof(BUF_SIZE_SMALL));
				buffer[0] = 1;
				if(send(ID_SOCK_PAIR[p2],buffer,strlen(buffer),0) < 0) { // sending the data
					perror("\t\tError ending data.\n");
				}else {
					//LOG("turn sent to ",p2," ",clock(),"\n");
					//printf("%s\n", );
				}
				memset(buffer,0,sizeof(BUF_SIZE_SMALL));
				buffer[0] = 2;
				if(send(ID_SOCK_PAIR[p1],buffer,strlen(buffer),0) < 0) { // sending the data
					perror("\t\tError ending data.\n");
				} else {
					//LOG("turn sent to ",p1," ",clock(),"\n");
				}
				//sem_wait(&p);
				match[p2] = 0;
				waiting[p1] = 0;
				//sem_wait(&p);
				client_infos *ptr = (client_infos *)malloc(sizeof(client_infos));
				ptr->ID_1 = p2;
				ptr->ID_2 = p1;
				ptr->sockfd_1 = ID_SOCK_PAIR[p2];
				ptr->sockfd_2 = ID_SOCK_PAIR[p1];
				ptr->turn = 0;
				//printf("turn = %d\n",ptr->turn);
				playing[p1] = 1;
				playing[p2] = 1;
				if(pthread_create(&thread_pool[p1], NULL,handle_game, (void *)(ptr)) ) {
					perror("\t\tERROR: pthread failed inside server_1 exiting now !!\n");
					exit(1);
				}else {
					// thread_count++;
				}
				if(pthread_detach(thread_pool[p1])){
					perror("\t\tERROR: pthread_detach failed inside server_1\n");
					exit(1); // try something else...not to completely end this server process
				}
			}

			else {
				//sem_wait(&q);
				//printf("\n\n%d -> %d matched\n\n",p1,p2);
				match[p1] = p2;
				waiting[p2] = p1;
				//printf("\n\n%d -> %d waiting\n\n",p2,p1);
				//sem_post(&q);
			}	
		}
		client_count++;
		//if(client_count == MAX_CLIENT_COUNT) LISTEN_FLAG_SERVER = false;
	}
	close(sockfd_listen);
	exit(1);
}
void process_buffer(char * buffer,int *p1,int *p2) {
	char c; int idx = 1,num = 0;
	switch(buffer[0]) {
		case (1<<MT_LOGIN):
			while((c = buffer[idx++]) != DELIM) {
				num = num*10 + (c - '0');
			}
			*p1 = num;
			num = 0;
			while((c = buffer[idx++]) != DELIM) {
				num = num*10 + (c - '0');
			}
			*p2 = num;
			break;
		default:
			;
	}
}
