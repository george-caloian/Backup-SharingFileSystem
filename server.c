#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS	5
#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(1);
}

char *get_cmd (char *cmd_line)
{
	char *fst = strtok(cmd_line," ");
	return fst;
}

int get_new_PID (int a)
{
	int new_pid = getpid()+a;
	return new_pid;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[BUFLEN];
     struct sockaddr_in serv_addr, cli_addr;
     int n, i, j;

     int act_soc[50];
     
     
   

     fd_set read_fds;	//multimea de citire folosita in select()
     fd_set tmp_fds;	//multime folosita temporar 
     int fdmax;		//valoare maxima file descriptor din multimea read_fds

     char act_usr[50][30];

     if (argc < 2) {
         fprintf(stderr,"Usage : %s port\n", argv[0]);
         exit(1);
     }

     int bf[50];

     //golim multimea de descriptori de citire (read_fds) si multimea tmp_fds 
     FD_ZERO(&read_fds);
     FD_ZERO(&tmp_fds);

     FILE *g;
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");

     FILE *f;
     
     portno = atoi(argv[1]);

     memset((char *) &serv_addr, 0, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;

     int aux,aux2,test=0;

     serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     serv_addr.sin_port = htons(portno);

      int usr_nmb;
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd, MAX_CLIENTS);

     //adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
     FD_SET(sockfd, &read_fds);
     fdmax = sockfd;



     char er_1[50]="-1 : Clientul nu e autentificat\n";
     char er_2[50]="-2 : Sesiune deja deschisa\n";
     char er_3[50]="-3 : User/parola gresita\n";
     char er_4[50]="-4 : Fisier inexistent\n";
     char er_5[50]="-5 : Descarcare interzisa\n";
     char er_6[50]="-6 : Fisier deja partajat\n";
     char er_7[50]="-7 : Fisier deja privat\n";
     char er_8[50]="-8 : Brute-force detectat\n";
     char er_9[50]="-9 : Fisier existent pe server\n";
     char er_10[50]="-10 : Fisier in transfer\n";
     char er_11[50]="-11 : Utilizator inexistent\n";

     // main loop
	while (1) {
		tmp_fds = read_fds; 
		if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
			error("ERROR in select");
	
		for(i = 0; i <= fdmax; i++) {
		
			if (FD_ISSET(i, &tmp_fds)) {
			
				if (i == sockfd) {
					// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
					// actiunea serverului: accept()
					clilen = sizeof(cli_addr);
					if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
						error("ERROR in accept");
					} 
					else {
						//adaug noul socket intors de accept() la multimea descriptorilor de citire
						FD_SET(newsockfd, &read_fds);
						if (newsockfd > fdmax) { 
							fdmax = newsockfd;
						}
					}
					printf("Noua conexiune de la %s, port %d, socket_client %d\n ", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
				}
					
				else {
					// am primit date pe unul din socketii cu care vorbesc cu clientii
					//actiunea serverului: recv()
					memset(buffer, 0, BUFLEN);
					if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
						if (n == 0) {
							//conexiunea s-a inchis
							printf("selectserver: socket %d hung up\n", i);
						} else {
							error("ERROR in recv");
						}
						close(i); 
						FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
					} 
					
					else { //recv intoarce >0
						char * instr = get_cmd(buffer);

						char PID[10];
						sprintf (PID,"%d",get_new_PID(i));
						char log_file_name[50]="client-";
						strcat(log_file_name,PID);
						strcat(log_file_name,".log");

						g=fopen(log_file_name,"a");

						if(strcmp(instr,"quit\n")==0)
						{
							fprintf(g,"%s",instr);
							close(i);
							FD_CLR(i,&read_fds);
						}


						else if(strcmp(instr,"login")==0)
						{
							char * usr = get_cmd(NULL);
							char * psw = get_cmd(NULL);
							fprintf(g,"%s %s %s",instr,usr,psw);

							if(act_soc[i]!=0)
							{
								send(i,er_2,250,0);
								fprintf(g,"%s",er_2);
								send(i,act_usr[i],250,0);
								fprintf(g,"%s",act_usr[i]);
							}

							else
							{
								int q=0;
								int valid=-1;
								char ln[50];
								char pr[50];
								char *u_fl;
								char *ps_fl;

								f=fopen(argv[2],"r");
								fscanf(f,"%d",&usr_nmb);

								while ( q<usr_nmb)
								{
									fgets(ln,50,f);
									u_fl=get_cmd(ln);
									ps_fl=get_cmd(NULL);

									if(strcmp(usr,u_fl)==0)
										if(strcmp(psw,ps_fl)==0)
										{
											valid=0;
											strcpy(act_usr[i],usr);
										}
									q++;

								}
								if(valid==-1)
								{
									bf[i]=bf[i]+1;
									send(i,er_3,250,0);
									fprintf(g,"%s",er_3);

									

								}
								else
								{
									char sg[5]=">";
									act_soc[i]=1;
									strcpy(pr,usr);
									strcat(pr,sg);
									strcpy(act_usr[i],pr);

									send(i,pr,250,0);
									fprintf(g,"%s",pr);
								}
								if(bf[i]>2)
								{
									send(i,er_8,250,0);
									fprintf(g,"%s",er_8);
									close(i);
									FD_CLR(i,&read_fds);
								}
								fclose(f);


							}

							
						}

						else if(strcmp(instr,"logout\n")==0)
						{
							if(act_soc[i]==0)
							{
								fprintf(g,"%s",instr);
								fprintf(g,"%s",er_1);
								send(i,er_1,250,0);
							}
							else
							{
								fprintf(g,"%s",instr);
								send(i,"Utilizator delogat\n",250,0);
								strcpy(act_usr[i],"");
								act_soc[i]=0;
							}
						}

				


						fclose(g);					
					}
				} 
			}
			
		}
     }

     close(sockfd);
   
     return 0; 
}


