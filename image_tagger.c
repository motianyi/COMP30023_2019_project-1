
/*
** http-server.c
*/
#include "sendhttp.h"


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

// // constants
// static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
// Content-Type: text/html\r\n\
// Content-Length: %ld\r\n\r\n";
// static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
// static int const HTTP_400_LENGTH = 47;
// static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
// static int const HTTP_404_LENGTH = 45;

// static char* IMAGE_1 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-1.jpg";
// static char* IMAGE_2 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-2.jpg";
// static char* IMAGE_3 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-3.jpg";
// static char* IMAGE_4 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-4.jpg";

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
static void make_string(char words[][100], int length, char* wordstring){
    
    int i;
    for(i = 0; i < length; i++){
        if(i!=0){
            strcat(wordstring, ", ");
        }
        printf("%s\n",wordstring);
        printf("%s\n",words[i]);
        strcat(wordstring, words[i]);

    }
}

// static char* get_image_name(int turn){
//     if(turn == 1){
//         return IMAGE_1;
//     }else if(turn == 2){
//         return IMAGE_2;
//     }else if(turn == 3){
//         return IMAGE_3;
//     }else{
//         return IMAGE_4;
//     }
// }


// static bool sendhttp(char* filename, int sockfd, char* buff, int* n, int turn){
//     char html[2049];
//     // get the size of the file
//     struct stat st;
//     stat(filename, &st);
    
//     //find the image name of this turn
//     char* img_name = get_image_name(turn);

//     // increase file size to accommodate the username
//     long size = 0;
//     if(strcmp(filename, "6_endgame.html")== 0 || strcmp(filename, "7_gameover.html")==0){
//         size = st.st_size;
//     }else{
//         size = st.st_size + strlen(img_name)-2;
//     }    
//     // long size = st.st_size + strlen(img_name)-2;
//     int a;
//     a = sprintf(buff, HTTP_200_FORMAT, size);
//     // send the header first
//     if (write(sockfd, buff, a) < 0)
//     {
//         perror("write");
//         printf("false1\n");
//         return false;
        
//     }

    
//     // read the content of the HTML file
//     int filefd = open(filename, O_RDONLY);
//     a = read(filefd, buff, 2048);
    
//     if (a < 0)
//     {
//         perror("read");
//         close(filefd);
//         printf("false2\n");
//         return false;
//     }
//     close(filefd);
//     a = sprintf(html, buff, img_name);

//     if (a < 0)
//     {
//         perror("sprintf get error");
//         close(filefd);
//         printf("false3\n");
//         return false;
//     }
//     // printf("%.*s\n",(int)size, html);
//     if (write(sockfd, html, size) < 0)
//     {
//         perror("write");
//         printf("false4\n");
//         return false;
//     }
//     return true;

// }


// static bool sendhttp_2str(char* filename, int sockfd, char* buff, int turn, char* words_string){
//     char html[2049];
//     // get the size of the file
//     struct stat st;
//     stat(filename, &st);
    
//     //find the image name of this turn
//     char* img_name = get_image_name(turn);

//     // increase file size to accommodate the username
//     long size = 0;
//     size = st.st_size + strlen(img_name)+ strlen(words_string)-2-2;
    
    
//     int a;
//     a = sprintf(buff, HTTP_200_FORMAT, size);
//     // send the header first
//     if (write(sockfd, buff, a) < 0)
//     {
//         perror("write");
//         printf("false1\n");
//         return false;
        
//     }

    
//     // read the content of the HTML file
//     int filefd = open(filename, O_RDONLY);
//     a = read(filefd, buff, 2048);
    
//     if (a < 0)
//     {
//         perror("read");
//         close(filefd);
//         printf("false2\n");
//         return false;
//     }
//     close(filefd);
//     a = sprintf(html, buff, img_name, words_string);

//     if (a < 0)
//     {
//         perror("sprintf get error");
//         close(filefd);
//         printf("false3\n");
//         return false;
//     }
//     // printf("%.*s\n",(int)size, html);
//     if (write(sockfd, html, size) < 0)
//     {
//         perror("write");
//         printf("false4\n");
//         return false;
//     }
//     return true;

// }
    

static bool game(char* word, int player, char words_player1[][100], char words_player2[][100], int* length1, int* length2){

    
    // static char words_player1[100][100] = {};
    // static char words_player2[100][100] = {};
    // static int length1 = 0;
    // static int length2 = 0;
    int i;

    for(i = 0; i<*length1; i++){
        printf("%s\n", &words_player1[i][0]);
    }
    printf("\nplayer2\n");
    for(i = 0; i<*length2; i++){
        printf("%s\n", &words_player2[i][0]);
    }
    
    
    if(player == 2){
        
        bool in_array = false;

        //check whether the word is in the array of another player
        for(i = 0; i<*length1; i++){
            if(strcmp(words_player1[i], word)==0){
                in_array = true;
            }
        }
        if(in_array == true){
            //reset the array
            for(i = 0; i<*length1; i++){
                strcpy(words_player1[i],"");
            }
            for(i = 0; i<*length2; i++){
                strcpy(words_player2[i],"");
            }
            //reset the length to 0
            *length1 = 0;
            *length2 = 0;
            
            return true;
        }else{
            //otherwise add the word into the array
            strcpy(words_player2[*length2], word);
            *length2+=1;
            printf("length2 is %d\n",*length2);
        }
        printf("false5\n");
        return false;
    }else if(player == 1){
        
        bool in_array = false;

        //check whether the word is in the array of another player
        for(i = 0; i<*length2; i++){
            if(strcmp(words_player2[i], word)==0){
                in_array = true;
            }
        }
        if(in_array == true){
            //reset the array
            for(i = 0; i<*length2; i++){
                strcmp(words_player2[i],"");
            }
            for(i = 0; i<*length1; i++){
                strcmp(words_player1[i],"");
            }
            //reset the length to 0
            *length1 = 0;
            *length2 = 0;
            
            return true;
        }else{
            //otherwise add the word into the array
            strcpy(words_player1[*length1], word);
            *length1+=1;
            printf("length1 is %d\n",*length1);
        }
        printf("false6\n");
        return false;
    }else{
        printf("ERROR, palyer = 0");
    }
    
}



static bool handle_http_request(int sockfd){

    static int cookies[100] = {};
    static int num_cookies = 0;

    static char words_player1[100][100] = {};
    static char words_player2[100][100] = {};
    static int length1 = 0;
    static int length2 = 0;
    static int  isquit = 0;

    static int turn = 1;
    static bool guessed[2] = {true, true};
    static int player_sockets[2] = {0,0};
    static int gamestate = 0;
    
    static int player_cookies[2] = {0, 0};
    static char name_list[100][20] = {};

    // int m;
    
    // for(m =0; m<100; m++){
    //     printf("%s\n",name_list[m]);
    // }
    

    char buff[2049];
    int n = read(sockfd, buff, 2049);
    if (n <= 0){
        if (n < 0){
            perror("read");
        }else{
            
            printf("socket %d close the connection\n", sockfd);
            // if(player_sockets[0]==sockfd){
            //     player_sockets[0] = 0;
            // }else if(player_sockets[1]==sockfd){
            //     player_sockets[1] = 0;
            // }
        }
        printf("MMMMMMMMMMMMMMMMMMMMMM");
        printf("false10\n");
        return false;
    }

    printf("first player socket:%d\n",player_sockets[0]);
    printf("second player socket:%d\n",player_sockets[1]);

    // terminate the string
    buff[n] = 0;

    char * curr = buff;


    //default cookie
    int this_cookie = 10;

    bool is_new_player = false;
    if(get_cookie(curr)==0){
        if(num_cookies < 90){
            this_cookie = num_cookies + 10;
            num_cookies ++;
            is_new_player = true;
            
        }
    }else{
        this_cookie = get_cookie(curr);
    }
    

   
    
    
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
        printf("false11\n");
        return false;
    }
    
    // sanitise the URI
    while (*curr == '.' || *curr == '/'){
        ++curr;
    }

    // printf("%s\n",curr);

    if (strstr(curr, "quit=Quit") != NULL){
        
        if(player_sockets[0] == sockfd){
            //player 1 quit game
            player_sockets[0] = 0;
        }else if(player_sockets[1] == sockfd){
            //player 2 quit game
            player_sockets[1] = 0;
        }else{
            printf("error");
        }
        isquit += 1;

        bool result;
        result = sendhttp("7_gameover.html", sockfd, buff, &n, turn, this_cookie);
        if(result == false){
            printf("false12\n");
            return false;
        }

        return false;
        // printf("first player socket:%d\n",player_sockets[0]);
        // printf("second player socket:%d\n",player_sockets[1]);

    }else if (strncmp(curr, "?start=Start", 12) == 0){
        // assume the only valid request URI is "/" but it can be modified to accept more files
        
        if (method == GET){
            //identify the socket
            if(player_sockets[0] == 0){
                player_sockets[0] = sockfd;
            }else if(player_sockets[1] == 0){
                player_sockets[1] = sockfd;
            }else{
                printf("both sockets are allocated, is it second round?\n");
            }

            printf("GGGGGGGGGGGGGGgamestate = %d\n",gamestate);

            //find the state
            if(player_sockets[0] != 0 && player_sockets[1] == 0){
                gamestate = 1;
            }else if(player_sockets[1] == 0 && player_sockets[1] != 0){
                gamestate = 2;
            }else if(player_sockets[0] != 0 && player_sockets[1] != 0 && guessed[0] == true && guessed[1] == true){
                gamestate = 3;
                guessed[0] = false;
                guessed[1] = false;
            }else{
                printf("ERROR, start");
            }
            printf("After:GGGGGGGGGGGGGGgamestate = %d\n",gamestate);

            bool result;
            result = sendhttp("3_first_turn.html", sockfd, buff, &n, turn, this_cookie);
            if(result == false){
                printf("false13\n");
                return false;
            }
            printf("first player socket:%d\n",player_sockets[0]);
            printf("second player socket:%d\n",player_sockets[1]);
    
        }else if (method == POST){
            printf("GGGGGGGGGGGGGGgamestate = %d\n",gamestate);
            printf("isquit = %d\n",isquit);
            
            if (strstr(curr, "guess=Guess") != NULL){

                //another player has quitted, thus this player also quit the game                
                if(isquit%2 == 1){
                    if(player_sockets[0] == sockfd){
                        //player 1 quit game
                        player_sockets[0] = 0;
                    }else if(player_sockets[1] == sockfd){
                        //player 2 quit game
                        player_sockets[1] = 0;
                    }else{
                        printf("error");
                    }

                    //send the gameover page
                    bool result;
                    result = sendhttp("7_gameover.html", sockfd, buff, &n, turn, this_cookie);
                    if(result == false){
                        printf("false14\n");
                        return false;
                    }
                    printf("false114\n");

                    // static char words_player1[100][100] = {};
                    // static char words_player2[100][100] = {};
                    // static int length1 = 0;
                    // static int length2 = 0;
                    // static int  isquit = 0;

                    // static int turn = 1;
                    // static bool guessed[2] = {true, true};
                    // static int player_sockets[2] = {0,0};
                    // static int gamestate = 0;
                                    

                    //increase the isquit
                    isquit +=1;
                   
                    
                    turn = 1;
                    guessed[1] = true;
                    guessed[0] = true;
                    player_sockets[1] = 0;
                    player_sockets[0] = 0;
                    gamestate = 0;
                    return false;

                //one player guessed, thus another player endgame
                }else if(gamestate == 5 && ((player_sockets[0] == sockfd && guessed [0] == false) || (player_sockets[1] == sockfd && guessed [1] == false))){
                    bool result;
                    result = sendhttp("6_endgame.html", sockfd, buff, &n, turn, this_cookie);
                    if(result == false){
                        printf("false15\n");
                        return false;
                    }
                                        
                    if(player_sockets[0] == sockfd){
                        player_sockets[0] = 0;
                        guessed[0] = true;
                    }else if(player_sockets[1] == sockfd){
                        player_sockets[1] = 0;
                        guessed[1] = true;
                    }else{
                        printf("ERROR\n");
                    }

                //both player are ready for guessing   
                }else if(gamestate == 3){
                    if(player_sockets[0] == 0 && player_sockets[1] != 0 && player_sockets[1]!= sockfd){
                        player_sockets[0] = sockfd;
                    }else if(player_sockets[1] == 0 && player_sockets[0] != 0 && player_sockets[0]!= sockfd){
                        player_sockets[1] = sockfd;
                    }else{
                        printf("both have sockets");
                    }
                    
                    char word[100] = {};

                    //extract the gussing word
                    get_guessing_word(curr, word);


                    int player = 0;
                    if(sockfd == player_sockets[0]){
                        player = 1;
                    }else if(sockfd == player_sockets[1]){
                        player = 2;
                    }
                    
                    //one player guessed the word submitted by another player
                    if(game(word, player,words_player1, words_player2,&length1,&length2) == true){
                       
                        //set gamestate to finish game
                        gamestate = 5;
                      
                        bool result;
                        result = sendhttp("6_endgame.html", sockfd, buff, &n, turn, this_cookie);
                        if(result == false){
                            printf("false16\n");
                            return false;
                        }

                        //move to next turn(next image)
                        turn +=1;

                        if(player_sockets[0] == sockfd){
                            player_sockets[0] = 0;
                            guessed[0] = true;
                            
                        }else if(player_sockets[1] == sockfd){
                            player_sockets[1] = 0;
                            guessed[1] = true;
                            
                        }else{
                            printf("ERROR\n");
                            
                        }
                        // printf("after end this game:\n");
                        // printf("first player socket:%d\n",player_sockets[0]);
                        // printf("second player socket:%d\n",player_sockets[1]);
                    }else{
                        printf("NNNN");
                    }
                    // printf("\n%s\n\n",curr);
                    
                    //both player are ready  

                    char wordstring[2049] = "";
                    if(player_sockets[0] == sockfd){
                        make_string(words_player1, length1, wordstring);
                        printf("WWW%s\n", wordstring);
                    }else if (player_sockets[1] == sockfd){
                        make_string(words_player2, length2, wordstring);
                        printf("WWW%s\n", wordstring);
                    }else{
                        printf("ERROR!!!");
                    }
                
                    bool result;
                    result = sendhttp_2str("4_accepted.html", sockfd, buff, turn, wordstring, this_cookie);
                    if(result == false){
                        printf("false17\n");
                        return false;
                    }

                }else{
                    printf("discard:\n");
                    printf("first player socket:%d\n",player_sockets[0]);
                    printf("second player socket:%d\n",player_sockets[1]);

                    bool result;
                    result = sendhttp("5_discarded.html", sockfd, buff, &n, turn, this_cookie);
                    if(result == false){
                        printf("false19\n");
                        return false;
                    }
                   
                }
            }
        }
        // printf("GAMESTATE = %d\n",gamestate);
        // printf("first player socket:%d\n",player_sockets[0]);
        // printf("second player socket:%d\n",player_sockets[1]);
    }else if (strncmp(curr, "favicon.ico", 11) == 0){
        printf("FAVVVVV");
        bool result;
        result = sendimage(sockfd);
        if(result == false){
            printf("false22\n");
            return false;
        }
    }else if (*curr == ' '){

        //simply return the initial html
        if (method == GET && is_new_player == true){   
            bool result;
            result = sendhttp("1_intro.html", sockfd, buff, &n, turn, this_cookie);
            if(result == false){
                printf("false20\n");
                return false;
            }
            
        }else if (method == GET && is_new_player == false){   
            bool result;
            result = sendhttp_2str("2_start.html", sockfd, buff, turn, name_list[this_cookie], this_cookie);
            if(result == false){
                printf("false200\n");
                return false;
            }
            
        }else if (method == POST){ 

            //finf the username
            char* username = strstr(curr, "user=")+5;
            int username_length = strlen(username);
            strcpy(name_list[this_cookie], username);
            
            printf("USERNAME: %s\n",username);
            
            
            bool result;
            result = sendhttp_2str("2_start.html", sockfd, buff, turn, username, this_cookie);
            
            // printf("GAMESTATE = %d\n",gamestate);
            // printf("first player socket:%d\n",player_sockets[0]);
            // printf("second player socket:%d\n",player_sockets[1]);
            if(result == false){
                printf("false22\n");
                return false;
            }
        }
        else
            // never used, just for completeness
            fprintf(stderr, "no other methods supported");
    // send 404
    }else if (write(sockfd, HTTP_404, HTTP_404_LENGTH) < 0){
        perror("write");
        printf("false23\n");
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
                else if (!handle_http_request(i)){   
                    printf("CLOSE");
                    close(i);
                    FD_CLR(i, &masterfds);
                }
            }
    }

    return 0;
}


void get_guessing_word(char* curr, char* word){
    //find the address of string
    char* guess = strstr(curr, "guess=Guess");
    char* keyword = strstr(curr, "keyword=");
    
    //skip the string "keyword=",
    char* x = keyword+8;
    int i = 0;
    
    //iterate through, store the word
    while(x != guess){
        word[i] = *x;
        i++;
        x++;
    }
    word[i-1] = '\0';
}

int get_cookie(char* curr){
    //find the address of string

    char* cookie_string = "0";
    if(strstr(curr, "Cookie: ")!= NULL){
        cookie_string = strstr(curr, "Cookie: ")+8;
    }
    int cookie_string_length = strlen(cookie_string);
    printf("receiverd_cookie :%s\n",cookie_string);

    return atoi(cookie_string);
}
