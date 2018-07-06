#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUFLEN 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    fd_set set;
    fd_set tmp;

    char buffer[BUFLEN];
    if (argc < 3) {
       fprintf(stderr,"Usage %s server_address server_port\n", argv[0]);
       exit(0);
    }  
    
    FD_ZERO(&set);
    FD_ZERO(&tmp);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    FD_SET(sockfd, &set);
    FD_SET(0, &set);
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &serv_addr.sin_addr);
    
    
    if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");    

    
   
    
    int fdmax = sockfd;

    while(1){


    	tmp = set;
    	
        memset(buffer, 0 , BUFLEN); 

        if (select(fdmax + 1, &tmp, NULL, NULL, NULL) == -1) 
			error("ERROR in select");
	


    	if(FD_ISSET(0,&tmp))
        {

		

    		//memset(buffer, 0 , BUFLEN);
    		fgets(buffer, BUFLEN-1, stdin);
    		int n = send(sockfd,buffer, 250, 0);

			
       		
    	}
	
    	
        if(FD_ISSET(sockfd,&tmp))
        {
    		
            

            //memset(buffer, 0, BUFLEN);
    		
            int k = recv(sockfd, buffer, 250, 0);
		
    		printf("%s",buffer);
		  
            
    	}
	
	
        if(strcmp(buffer,"quit\n")==0)
            		break;


	if(strcmp(buffer,"-8 Brute-force detectat\n")==0)
                break;


    }
    return 0;
}


