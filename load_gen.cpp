	/*
	Debashish Deka
	173050055
	*/

#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ctime>

using namespace std;
#define ITER 2000
#define N 			2
#define PORT 		8787
#define BUF_SIZE	1024
#define BUF_SIZE_SMALL 256
#define ID_SIZE		10
#define MT_LOGIN	0
#define MT_CONV 	1
#define MT_ENTRY	2
#define MT_WIN		3
#define MT_DRAW		4
#define REJECT_1 	5
#define REJECT_2	6
#define REJECT_3	7
#define ME 			1
#define OP 			0
#define NUM_THREADS 300
#define bytes_to_read_for_turn	1
#define bytes_to_read_for_move	1

struct id_pair {
	int p1;
	int p2;
	int user_number;
	double __time;
};

typedef struct data {
  int socket_fd;
}p_address;

char DELIM = ':';
/*template code for LOG purpose */
template<typename T> 
void LOG(T const& t) { 
	std::cout << t; 
}
template<typename First, typename ... Rest> 
void LOG(First const& first, Rest const& ... rest) {
	std::cout << first<<" "; LOG(rest ...);
}

/*template code for log purpose */

unsigned int client_status = 0; // this will act as flag
void display_table(char (*)[3]);
void clear_table(char (*)[3],bool (*)[3]);
//void update_table(vector<pair<int,int>>,char (*)[3],char,char);
bool is_correct_move(vector<pair<int,int>>,char (*)[3],char);
bool check(char (*)[3],char);
void clear_stdin();

char * server_address;

void update_table(vector<pair<int,int>> &positions,char table[][3],char choice,char marker) {
	table[positions[choice].first][positions[choice].second] = marker;
}

void *load_gen(void *ptr) {	
	
	int total_time = 0;
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0) {
		
		perror("\t\tsocket creation failed in client side : ");
		exit(1);
	
	}else { 

			
	}
	
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;  
	addr.sin_addr.s_addr = inet_addr(server_address);

	addr.sin_port = PORT;

	if(connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {  
		perror("\t\tError connecting to the server-1:\n");  
		exit(1);
	}else { 

	}
		

	char my_id[ID_SIZE],op_id[ID_SIZE];
	
	sprintf(my_id, "%d", ((id_pair*)ptr)->p1);
	sprintf(op_id, "%d", ((id_pair*)ptr)->p2);

	char *buffer = (char *)malloc(BUF_SIZE_SMALL);
	memset(buffer,0,BUF_SIZE_SMALL);
	
	buffer[0] = buffer[0] | (1<< MT_LOGIN);
	
	int idx = 1;
	for(int i=0;i<strlen(my_id);i++) buffer[idx++] = my_id[i];
	buffer[idx++] = DELIM;
	for(int i=0;i<strlen(op_id);i++) buffer[idx++] = op_id[i];
	buffer[idx] = DELIM; 




	clock_t _begin = clock();
	clock_t _end = clock();


	int aa = 0;
	if( (aa = send(sockfd,buffer,strlen(buffer),0) )< 0) {
	
		perror("\t\tError ending data.\n");
		exit(0);
	
	}else {


		//LOG(((id_pair*)ptr)->p1," sent it's ID",clock(),"\n");
	
	}

	


	memset(buffer,0,BUF_SIZE_SMALL);
	


	//LOG(((id_pair*)ptr)->p1," about to call recv for turn form server-1",clock(),"\n");
	
	int res = 0;
	bool op_move_already_recieved = false;
	char op_move;
	//LOG(((id_pair*)ptr)->p1," waiting for turn instance = ",total_time," ",clock(),"\n");
	if( (res = recv(sockfd,buffer,BUF_SIZE_SMALL,0) ) < 0) { // waiting and recieve the ok code for the game to start
		
		perror("\t\tError : recv failed inside rcv_data_from_server function\n");
	
	}else {
		
		// LOG(((id_pair*)ptr)->p1," got turn instance = ",total_time," ",buffer,clock(),"\n");
		// if(res != 1) LOG("1111111111\n");
		// LOG("res = ",res," 000000000000000000000000\n");
		// printf("values of res = %d\n",res);
		// printf("fuck_res = %d\n",res);
		
		if(res >= 2) {
			cout <<" HELLO "<< res <<endl;
			op_move_already_recieved = true;
			op_move = buffer[1];
			// printf("Here res = %d\n",res);
			//LOG("res = ",res," AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
		}

		//LOG(((id_pair*)ptr)->p1," got turn value",clock(),"\n");

	}



	bool turn = (buffer[0] == 1)?true:false;


	while(true) {

			vector<pair<int,int>> positions(10,make_pair(0,0));
			
			positions[1].first = 0; positions[1].second = 0;
			positions[2].first = 0; positions[2].second = 1;
			positions[3].first = 0; positions[3].second = 2;
			positions[4].first = 1; positions[4].second = 0;
			positions[5].first = 1; positions[5].second = 1;
			positions[6].first = 1; positions[6].second = 2;
			positions[7].first = 2; positions[7].second = 0;
			positions[8].first = 2; positions[8].second = 1;
			positions[9].first = 2; positions[9].second = 2;	
			
			char table[3][3];
			
			bool flag[3][3];
			memset(flag,0,sizeof(flag));
			memset(table,0,sizeof(table));
			
			char my_marker;
			char op_marker;
			if(turn) {
				my_marker = 'x';
				op_marker = 'o';
			}else {
				my_marker = 'o';
				op_marker = 'x';
			}

			char choice = 1;
			


			bool game_over = false;
			bool is_full = false;
			bool draw = false;
			bool status_shown = false;
			clear_table(table,flag);
			
			bool score_recieved = false;

			while(!game_over && !is_full) {

				memset(buffer,0,BUF_SIZE_SMALL);
				
				if(turn) {

					//sleep(1);
					//LOG(((id_pair*)ptr)->p1," is going to send()",clock(),"\n");
					update_table(positions,table,choice,my_marker);
					
					if(check(table,my_marker)) {
						game_over = true;
					}
					
					bool ok = false;
					for(int i=0;i<3 && !ok;i++) {
						for(int j=0;j<3 && !ok;j++) {
							if(table[i][j] == ' ') ok = true; 
						}
					}

					if(!ok) is_full = true;
					if(is_full && !game_over) draw = true;
					if(!game_over && !is_full) {
						buffer[0] = buffer[0] | (1<< MT_ENTRY);
						buffer[1] = choice;
					}else if(game_over){
						buffer[0] = buffer[0] | (1<< MT_WIN);
						buffer[1] = choice;
					}else if(draw && !game_over) {

						buffer[0] = buffer[0] | (1<< MT_DRAW);
						buffer[1] = choice;
					}	
					int bb = 0;
					if( (bb = send(sockfd,buffer,strlen(buffer),0) ) < 0) {
						perror("Error ending data.\n");
					}else {
						//if(bb != 2) LOG("2222222222\n");
					//	LOG(((id_pair*)ptr)->p1," did move",clock(),"\n");
					}
					
					turn ^= 1;

				}else {

					//sleep(1);
					//LOG(((id_pair*)ptr)->p1," going to call recv()",clock(),"\n");
					int res2 = 0;
					//LOG(((id_pair*)ptr)->p1," waiting for move instance = ",total_time," ",clock(),"\n");
					if(!op_move_already_recieved) {
						if( ( res2 = recv(sockfd,buffer,BUF_SIZE_SMALL,0) ) < 0) {
							perror("\t\tError : recv failed inside rcv_data_from_server function \n");
							exit(0);
						}else {
							// if(res2 != 1) LOG("3333333333\n");
							// LOG(((id_pair*)ptr)->p1," got move instance = ",total_time," ",buffer,clock(),"\n");
							if(res2 >= 2) {
								//LOG("res2 = ",res2,"\n");
								//LOG("dddddddddd\n");
								score_recieved = true;
								cout << "IMPORTANT !!!!!!!!!!!: "<<res2<< endl;
								//printf("omg_res = %d\n",res2);
								
								//printf("There res = %d %d %d %d\n",res2,buffer[0],buffer[1],buffer[2]);
								//op_move_already_recieved = true;
								//op_move = buffer[1];
								//LOG("res = ",res2," BBBBBBBBBBBBBBBBBBBBB\n");
							}							
						
						}
					}else {

						choice = op_move;
						op_move_already_recieved = false;
					
					}

					//LOG(((id_pair*)ptr)->p1," recieved",buffer[0],clock(),"\n");
					
					choice = buffer[0];
					update_table(positions,table,choice,op_marker);
					if(check(table,op_marker)) {
						game_over = true;

					}
					bool ok = false;
					for(int i=0;i<3 && !ok;i++) {
						for(int j=0;j<3 && !ok;j++) {
							if(table[i][j] == ' ') ok = true; 
						}
					}
					if(!ok) is_full = true;
					if(is_full && !game_over) draw = true;
					if(draw) {

						status_shown = true;
					}

					choice++;
					turn ^= 1;
				}
			}

			memset(buffer,0,BUF_SIZE_SMALL);
			
			char c;
			
			//LOG(((id_pair*)ptr)->p1," done and about to call recv for fetch score.",clock(),"\n");
			
			int res3 = 0;
			//LOG(((id_pair*)ptr)->p1," waiting..instance = ",total_time," ",clock(),"\n");
			


			if(!score_recieved) {

				if( (res3 = recv(sockfd,buffer,BUF_SIZE_SMALL,0)) < 0) {
					
					perror("\t\tError : recv failed inside rcv_data_from_server function \n");
				
				}else {
				//	LOG(((id_pair*)ptr)->p1," got instance = ",total_time," ",buffer,clock(),"\n");
				//	if(res3 != 6) { 
				//		LOG(buffer,"\n");
				//		printf("value of res = %d\n", res);
				//		LOG("4444444444\n");
				//	}
					// LOG("res = ",res3," CCCCCCCCCCCCCCCCCCCCCCC\n");
					// LOG(((id_pair*)ptr)->p1," has recieved",buffer,"\n");
					//cout << "WORLD : "<<res3<<" "<< endl;
					int scores[3],num = 0,idx = 0;
					for(int i=0;i<3;i++) {
						while((c = buffer[idx++]) != DELIM) {
							num = num * 10 + (c - '0');
						}
						scores[i] = num;
						num = 0;
					}



					//printf("Total Matches = %d | Total wins = %d | Total lost =  %d | Total Draw = %d\n",
					//			scores[0],scores[1],scores[2],scores[0] - scores[1] - scores[2]);
				
				}
			}else {
				
				int scores[3],num = 0,idx = 1;
				for(int i=0;i<3;i++) {
					while((c = buffer[idx++]) != DELIM) {
						num = num * 10 + (c - '0');
					}
					scores[i] = num;
					num = 0;
				}

				score_recieved = false;
			
			}




			//LOG(((id_pair*)ptr)->p1," : all done ! exiting !",clock(),"\n");
			
			_end = clock();
			// double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
			// elapsed_secs;

			//cout << elapsed_secs << endl;
			//LOG(((id_pair*)ptr)->p1," := ",elapsed_secs,"\n");
			//free(buffer);
			//close(sockfd);

			total_time++;
			if(total_time >= ITER) break;
			//LOG("------------\n\n");
			//cout << ((id_pair*)ptr)->p1 <<" exiting "<<endl;

		}

	((id_pair*)ptr)->__time += double( _end - _begin) / CLOCKS_PER_SEC;

	pthread_exit(NULL);

}





int main(int argc,char **argv) {
	
	server_address = argv[1];

	pthread_t thread_pool[NUM_THREADS];
	id_pair * a[NUM_THREADS];
	for(int i=1;i<=NUM_THREADS;i++) {
		a[i-1] = (id_pair*)(malloc(sizeof(id_pair)));
		if(i%2) {
			a[i-1]->p1 = i;
			a[i-1]->p2 = i+1;
		}else {
			a[i-1]->p1 = i;
			a[i-1]->p2 = i-1;
		}
		a[i-1]->user_number = i;
		a[i-1]->__time = 0.0;

		pthread_create(&thread_pool[i-1],NULL,load_gen,(void*)(a[i-1]));
	}

	for(int i=0;i<NUM_THREADS;i++) {
		pthread_join(thread_pool[i],NULL);
	}
	double avg_res = 0.0;
	for(int i=0;i<NUM_THREADS;i++) {
		avg_res += (a[i]->__time)/(ITER);
	}
	
	avg_res = avg_res / NUM_THREADS;
	cout << "Average response time = "<< avg_res << endl;

	for(int i=0;i<NUM_THREADS;i++) {
		free(a[i]);
	}

	exit(1);
}












void display_table(char table [][3]) {
	for(int i=0; i<3; i++) {
		printf(" %c | %c | %c ",table[i][0], table[i][1], table[i][2]);
		if(i<2) printf("\n---|---|---\n");
	}
	printf("\n");
}
void clear_table(char table[][3],bool flag[][3]) {
	for(int i=0;i<3;i++) {
		for(int j=0;j<3;j++) {
			table[i][j] = ' ';
			flag[i][j] = false;			
		}
	}
}
bool is_correct_move(vector<pair<int,int>> &positions,char table[][3],char choice) {
	if( choice >= 1 && choice <= 9 ) {
		if(table[positions[choice].first][positions[choice].second] == ' ') {
			return true;
		}else 
			return false;
	}else {
		return false;
	}
}
bool check(char table[][3],char marker) {
	for(int i=0; i<3; i++)
		if(table[i][0] == marker && table[i][0]==table[i][1] && table[i][0]==table[i][2]) return true;
	for(int i=0; i<3; i++)
		if(table[i][0] == marker && table[0][i]==table[1][i] && table[0][i]==table[2][i]) return true;
	if(table[0][0] == marker && table[0][0]==table[1][1] && table[1][1]==table[2][2]) return true;
	if(table[0][2] == marker && table[0][2]==table[1][1] && table[1][1]==table[2][0]) return true;
	return false;
}
void clear_stdin() {
	char c;
	while (     (c = getchar()) != '\n'   &&      c != EOF    )  { }
}




