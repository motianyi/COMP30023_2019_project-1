/*
** http-server.c
*/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// constants
static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\r\n";
static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_400_LENGTH = 47;
static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_404_LENGTH = 45;

static char* IMAGE_1 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-1.jpg";
static char* IMAGE_2 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-2.jpg";
static char* IMAGE_3 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-3.jpg";
static char* IMAGE_4 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-4.jpg";

// represents the types of method
typedef enum
{
    GET,
    POST,
    UNKNOWN
} METHOD;
//1 first player is ready
    //2 second player is ready
    //3 both player are ready

    //5 finish
    
static bool sendhttp(char* filename, int sockfd, char* buff, int n){
    
    // get the size of the file
    struct stat st;
    stat(filename, &st);
    n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
    // send the header first
    if (write(sockfd, buff, n) < 0)
    {
        perror("write");
        return false;
    }
    // send the file
    int filefd = open(filename, O_RDONLY);
    do
    {
        n = sendfile(sockfd, filefd, NULL, 2048);
    }
    while (n > 0);
    if (n < 0)
    {
        perror("sendfile");
        close(filefd);
        return false;
    }
    close(filefd);
}

static bool game(char* word, int player){

    
    static char words_player1[100][100] = {};
    static char words_player2[100][100] = {};
    static int length1 = 0;
    static int length2 = 0;
    int i;

    for(i = 0; i<length1; i++){
        printf("%s\n", &words_player1[i][0]);
    }
    printf("\nplayer2\n");
    for(i = 0; i<length2; i++){
        printf("%s\n", &words_player2[i][0]);
    }
    
    
    if(player == 2){
        
        bool in_array = false;

        //check whether the word is in the array of another player
        for(i = 0; i<length1; i++){
            if(strcmp(words_player1[i], word)==0){
                in_array = true;
            }
        }
        if(in_array == true){
            //reset the array
            for(i = 0; i<length1; i++){
                strcmp(words_player1[i],"");
            }
            for(i = 0; i<length2; i++){
                strcmp(words_player2[i],"");
            }
            //reset the length to 0
            length1 = 0;
            length2 = 0;
            
            return true;
        }else{
            //otherwise add the word into the array
            strcpy(words_player2[length2], word);
            length2+=1;
            printf("length2 is %d\n",length2);
        }
        return false;
    }else if(player == 1){
        
        bool in_array = false;

        //check whether the word is in the array of another player
        for(i = 0; i<length2; i++){
            if(strcmp(words_player2[i], word)==0){
                in_array = true;
            }
        }
        if(in_array == true){
            //reset the array
            for(i = 0; i<length2; i++){
                strcmp(words_player2[i],"");
            }
            for(i = 0; i<length1; i++){
                strcmp(words_player1[i],"");
            }
            //reset the length to 0
            length1 = 0;
            length2 = 0;
            
            return true;
        }else{
            //otherwise add the word into the array
            strcpy(words_player1[length1], word);
            length1+=1;
            printf("length1 is %d\n",length1);
        }
        return false;
    }else{
        printf("ERROR, palyer = 0");
    }
    
}



static bool handle_http_request(int sockfd){
    static int turn = 1;
    static int player_sockets[2] = {0,0};
    static int gamestate = 0;
    // try to read the request
    char buff[2049];
    int n = read(sockfd, buff, 2049);
    if (n <= 0)
    {
        if (n < 0)
            perror("read");
        else
            printf("socket %d close the connection\n", sockfd);
            if(player_sockets[0]==sockfd){
                player_sockets[0] = 0;
            }else if(player_sockets[1]==sockfd){
                player_sockets[1] = 0;
            }
        return false;
    }

    printf("first player socket:%d\n",player_sockets[0]);
    printf("second player socket:%d\n",player_sockets[1]);

    // terminate the string
    buff[n] = 0;

    char * curr = buff;
    
    // printf("%s\n",curr);
    // parse the method
    METHOD method = UNKNOWN;
    if (strncmp(curr, "GET ", 4) == 0)
    {
        curr += 4;
        method = GET;
    }
    else if (strncmp(curr, "POST ", 5) == 0)
    {
        curr += 5;
        method = POST;
    }
    else if (write(sockfd, HTTP_400, HTTP_400_LENGTH) < 0)
    {
        perror("write");
        return false;
    }
    
    // sanitise the URI
    while (*curr == '.' || *curr == '/'){
        ++curr;
    }
    if (strstr(curr, "quit=Quit") != NULL){
        
        if(player_sockets[0] == sockfd){
            //player 1 quit game
            gamestate = -1;
            player_sockets[0] = 0;
        }else if(player_sockets[1] == sockfd){
            //player 2 quit game
            gamestate = -2;
            player_sockets[1] = 0;
        }else{
            printf("error");
        }

        bool result;
        result = sendhttp("7_gameover.html", sockfd, buff, n);
        if(result == false){
            return false;
        }
        // // get the size of the file
        // struct stat st;
        // stat("7_gameover.html", &st);
        // n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
        // // send the header first
        // if (write(sockfd, buff, n) < 0)
        // {
        //     perror("write");
        //     return false;
        // }
        // // send the file
        // int filefd = open("7_gameover.html", O_RDONLY);
        // do
        // {
        //     n = sendfile(sockfd, filefd, NULL, 2048);
        // }
        // while (n > 0);
        // if (n < 0)
        // {
        //     perror("sendfile");
        //     close(filefd);
        //     return false;
        // }
        // close(filefd);
        // printf("GAMESTATE = %d\n",gamestate);
        // printf("first player socket:%d\n",player_sockets[0]);
        // printf("second player socket:%d\n",player_sockets[1]);
    }
    
    // assume the only valid request URI is "/" but it can be modified to accept more files
    if (strncmp(curr, "?start=Start", 12) == 0){
        
        printf("START = START%d\n",sockfd);
        if (method == GET){
            if(player_sockets[0] == 0){
                player_sockets[0] = sockfd;
            }else if(player_sockets[1] == 0){
                player_sockets[1] = sockfd;
            }else{
                printf("Already 2 players");
            }


            if(player_sockets[0] != 0 && player_sockets[1] == 0){
                gamestate = 1;
            }else if(player_sockets[1] == 0 && player_sockets[1] != 0){
                gamestate = 2;
            }else if(player_sockets[0] != 0 && player_sockets[1] != 0){
                gamestate = 3;
            }else{
                printf("ERROR, start");
            }

            bool result;
            result = sendhttp("3_first_turn.html", sockfd, buff, n);
            if(result == false){
                return false;
            }
    
            // // get the size of the file
            // struct stat st;
            // stat("3_first_turn.html", &st);
            // n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
            // // send the header first
            // if (write(sockfd, buff, n) < 0)
            // {
            //     perror("write");
            //     return false;
            // }
            // // send the file
            // int filefd = open("3_first_turn.html", O_RDONLY);
            // do
            // {
            //     n = sendfile(sockfd, filefd, NULL, 2048);
            // }
            // while (n > 0);
            // if (n < 0)
            // {
            //     perror("sendfile");
            //     close(filefd);
            //     return false;
            // }
            // close(filefd);
            
        }else if (method == POST){
            if(player_sockets[0] == 0 && player_sockets[1] != 0 && player_sockets[1]!= sockfd){
                player_sockets[0] = sockfd;
            }else if(player_sockets[1] == 0 && player_sockets[0] != 0 && player_sockets[0]!= sockfd){
                player_sockets[1] = sockfd;
            }else{
                printf("Already 2 players");
            }
            if (strstr(curr, "guess=Guess") != NULL){
                printf("GUESS_SOCKET=%d\n",sockfd);
                
                
                if(gamestate < 0){
                    if(player_sockets[0] == sockfd){
                        //player 1 quit game
                        gamestate = -1;
                        player_sockets[0] = 0;
                    }else if(player_sockets[1] == sockfd){
                        //player 2 quit game
                        gamestate = -2;
                        player_sockets[1] = 0;
                    }else{
                        printf("error");
                    }
                    bool result;
                    result = sendhttp("7_gameover.html", sockfd, buff, n);
                    if(result == false){
                        return false;
                    }
                    // // get the size of the file
                    // struct stat st;
                    // stat("7_gameover.html", &st);
                    // n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
                    // // send the header first
                    // if (write(sockfd, buff, n) < 0)
                    // {
                    //     perror("write");
                    //     return false;
                    // }
                    // // send the file
                    // int filefd = open("7_gameover.html", O_RDONLY);
                    // do
                    // {
                    //     n = sendfile(sockfd, filefd, NULL, 2048);
                    // }
                    // while (n > 0);
                    // if (n < 0)
                    // {
                    //     perror("sendfile");
                    //     close(filefd);
                    //     return false;
                    // }
                    // close(filefd);
                }else if(gamestate == 5){


                    bool result;
                    result = sendhttp("6_endgame.html", sockfd, buff, n);
                    if(result == false){
                        return false;
                    }
                    
                    // // get the size of the file
                    // struct stat st;
                    // stat("6_endgame.html", &st);
                    // n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
                    // // send the header first
                    // if (write(sockfd, buff, n) < 0)
                    // {
                    //     perror("write");
                    //     return false;
                    // }
                    // // send the file
                    // int filefd = open("6_endgame.html", O_RDONLY);
                    // do
                    // {
                    //     n = sendfile(sockfd, filefd, NULL, 2048);
                    // }
                    // while (n > 0);
                    // if (n < 0)
                    // {
                    //     perror("sendfile");
                    //     close(filefd);
                    //     return false;
                    // }
                    // close(filefd);
                    
                    if(player_sockets[0] == sockfd){
                        player_sockets[0] = 0;
                    }else if(player_sockets[1] == sockfd){
                        player_sockets[1] = 0;
                    }else{
                        printf("ERROR\n");
                    }        
                }else if(gamestate == 3){
                    char* guess = strstr(curr, "guess=Guess");
                    char* keyword = strstr(curr, "keyword=");
                    // printf("%p", guess);
                    // printf("%p", keyword);

                    //skip the string "keyword="
                    char* x = keyword+8;
                    int i = 0;
                    
                    char word[100] = {};
                    // for(i = 0; i<10; i++){
                    while(x != guess){
                        
                        // printf("%p\n",x);
                        // printf("%c\n",*x);
                        word[i] = *x;
                        i++;
                        x++;
                    }
                    word[i-1] = '\0';
                    printf("%s\n",word);

                    int player = 0;
                    if(sockfd == player_sockets[0]){
                        player = 1;
                    }else if(sockfd == player_sockets[1]){
                        player = 2;
                    }
                    if(game(word, player) == true){
                        //set gamestate to finish game
                        gamestate = 5;
                        printf("YYY");
                        // // get the size of the file
                        // struct stat st;
                        // stat("6_endgame.html", &st);
                        // n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
                        // // send the header first
                        // if (write(sockfd, buff, n) < 0)
                        // {
                        //     perror("write");
                        //     return false;
                        // }
                        // // send the file
                        // int filefd = open("6_endgame.html", O_RDONLY);
                        // do
                        // {
                        //     n = sendfile(sockfd, filefd, NULL, 2048);
                        // }
                        // while (n > 0);
                        // if (n < 0)
                        // {
                        //     perror("sendfile");
                        //     close(filefd);
                        //     return false;
                        // }
                        // close(filefd);

                        bool result;
                        result = sendhttp("6_endgame.html", sockfd, buff, n);
                        if(result == false){
                            return false;
                        }
                            

                        if(player_sockets[0] == sockfd){
                            player_sockets[0] = 0;
                        }else if(player_sockets[1] == sockfd){
                            player_sockets[1] = 0;
                        }else{
                            printf("ERROR\n");
                        }
                    }else{
                        printf("NNNN");
                    }
                    // printf("\n%s\n\n",curr);
                    
                    //both player are ready  
                    // get the size of the file
                    struct stat st;
                    stat("4_accepted.html", &st);
                    n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
                    // send the header first
                    if (write(sockfd, buff, n) < 0)
                    {
                        perror("write");
                        return false;
                    }
                    // send the file
                    int filefd = open("4_accepted.html", O_RDONLY);
                    do
                    {
                        n = sendfile(sockfd, filefd, NULL, 2048);
                    }
                    while (n > 0);
                    if (n < 0)
                    {
                        perror("sendfile");
                        close(filefd);
                        return false;
                    }
                    close(filefd);
                }else{

                    bool result;
                    result = sendhttp("5_discarded.html", sockfd, buff, n);
                    if(result == false){
                        return false;
                    }
                    // // get the size of the file
                    // struct stat st;
                    // stat("5_discarded.html", &st);
                    // n = sprintf(buff, HTTP_200_FORMAT, st.st_size);
                    // // send the header first
                    // if (write(sockfd, buff, n) < 0)
                    // {
                    //     perror("write");
                    //     return false;
                    // }
                    // // send the file
                    // int filefd = open("5_discarded.html", O_RDONLY);
                    // do
                    // {
                    //     n = sendfile(sockfd, filefd, NULL, 2048);
                    // }
                    // while (n > 0);
                    // if (n < 0)
                    // {
                    //     perror("sendfile");
                    //     close(filefd);
                    //     return false;
                    // }
                    // close(filefd);
                }
            }
        }
        printf("GAMESTATE = %d\n",gamestate);
        printf("first player socket:%d\n",player_sockets[0]);
        printf("second player socket:%d\n",player_sockets[1]);
    }
    if (*curr == ' '){
        if (method == GET)
        {   
            bool result;
            result = sendhttp("1_intro.html", sockfd, buff, n);
            if(result == false){
                return false;
            }
            
        }
        else if (method == POST)
        {   
            bool result;
            result = sendhttp("2_start.html", sockfd, buff, n);
            
            printf("GAMESTATE = %d\n",gamestate);
            printf("first player socket:%d\n",player_sockets[0]);
            printf("second player socket:%d\n",player_sockets[1]);
            if(result == false){
                return false;
            }
        }
        else
            // never used, just for completeness
            fprintf(stderr, "no other methods supported");
    // send 404
    }else if (write(sockfd, HTTP_404, HTTP_404_LENGTH) < 0){
        perror("write");
        return false;
    }
    printf("return true\n");
    return true;
}

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "usage: %s ip port\n", argv[0]);
        return 0;
    }

    // create TCP socket which only accept IPv4
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // reuse the socket if possible
    int const reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // create and initialise address we will listen on
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // if ip parameter is not specified
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // bind address to socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // listen on the socket
    listen(sockfd, 5);

    // initialise an active file descriptors set
    fd_set masterfds;
    FD_ZERO(&masterfds);
    FD_SET(sockfd, &masterfds);
    // record the maximum socket number
    int maxfd = sockfd;

    while (1)
    {   
        // monitor file descriptors
        int total = 0;
        fd_set readfds = masterfds;
        for (int i = 0; i <= maxfd; ++i){
            if (FD_ISSET(i, &readfds)){
                total++;
            }
        }
        // printf("total number of socket %d\n",total);

        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // loop all possible descriptor
        for (int i = 0; i <= maxfd; ++i)
            // determine if the current file descriptor is active
            if (FD_ISSET(i, &readfds))
            
            {   
                printf("%d is readey to read\n", i);
                // create new socket if there is new incoming connection request
                if (i == sockfd)
                {
                    struct sockaddr_in cliaddr;
                    socklen_t clilen = sizeof(cliaddr);
                    int newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
                    if (newsockfd < 0)
                        perror("accept");
                    else
                    {
                        // add the socket to the set
                        FD_SET(newsockfd, &masterfds);
                        // update the maximum tracker
                        if (newsockfd > maxfd)
                            maxfd = newsockfd;
                        // print out the IP and the socket number
                        char ip[INET_ADDRSTRLEN];
                        printf(
                            "new connection from %s on socket %d\n",
                            // convert to human readable string
                            inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, ip, INET_ADDRSTRLEN),
                            newsockfd
                        );
                    }
                }
                // a request is sent from the client
                else if (!handle_http_request(i))
                {
                    close(i);
                    FD_CLR(i, &masterfds);
                }
            }
    }

    return 0;
}



