
/*
** http-server.c
*/
#include "sendhttp.h"
#include "image_tagger.h"





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
void make_string(char words[][100], int length, char* wordstring){
    
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

    

bool game(char* word, int player, char words_player1[][100], char words_player2[][100], int* length1, int* length2){

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
        return false;
        
    }
    
}



bool handle_http_request(int sockfd){

    
    

    static char words_player1[100][100];
    static char words_player2[100][100];
    static int length1 = 0;
    static int length2 = 0;
    static int  isquit = 0;

    static int turn = 1;
    static bool guessed[2] = {true, true};
    // static int player_sockets[2] = {0,0};
    static int gamestate = 0;
    
    static int player_cookies[2] = {0, 0};
    static int num_cookies = 0;
    static char name_list[100][20];


    char buff[2049];
    int n = read(sockfd, buff, 2049);
    if (n <= 0){
        if (n < 0){
            perror("read");
        }else{
            printf("socket %d close the connection\n", sockfd);
        }
        printf("MMMMMMMMMMMMMMMMMMMMMM");
        printf("false10\n");
        return false;
    }

 
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

    

    if (strncmp(curr, "?start=Start", 12) == 0){
        // assume the only valid request URI is "/" but it can be modified to accept more files
        
        if (method == GET){
            
            if(player_cookies[0] == 0){
                player_cookies[0] = this_cookie;
            }else if(player_cookies[1] == 0){
                player_cookies[1] = this_cookie;
            }else{
                printf("WTF happened, is it second round?\n");
            }

            printf("GGGGGGGGGGGGGGgamestate = %d\n",gamestate);

            //find the state
            if(player_cookies[0] != 0 && player_cookies[1] == 0){
                gamestate = 1;
            }else if(player_cookies[1] == 0 && player_cookies[1] != 0){
                gamestate = 2;
            }else if(player_cookies[0] != 0 && player_cookies[1] != 0 && guessed[0] == true && guessed[1] == true){
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
            // printf("first player socket:%d\n",player_sockets[0]);
            // printf("second player socket:%d\n",player_sockets[1]);
    
        }else if (method == POST){
            printf("GGGGGGGGGGGGGGgamestate = %d\n",gamestate);
            printf("isquit = %d\n",isquit);

            //player want to quit the game
            if (strstr(curr, "quit=Quit") != NULL){
        
                if(player_cookies[0] == this_cookie){
                    //player 1 quit game
                    player_cookies[0] = 0;
                }else if(player_cookies[1] == this_cookie){
                    //player 2 quit game
                    player_cookies[1] = 0;
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

            }else if (strstr(curr, "guess=Guess") != NULL){

                //another player has quitted, thus this player also quit the game                
                if(isquit%2 == 1){
                    
                    if(player_cookies[0] == this_cookie){
                        //player 1 quit game
                        player_cookies[0] = 0;
                    }else if(player_cookies[1] == this_cookie){
                        //player 2 quit game
                        player_cookies[1] = 0;
                    }else{
                        printf("error");
                    }

                    //send the gameover page
                    if(sendhttp("7_gameover.html", sockfd, buff, &n, turn, this_cookie) == false){
                        printf("false14\n");
                        return false;
                    }
                    printf("false114\n");

                    //increase the isquit
                    isquit +=1;
                   
                    
                    turn = 1;
                    guessed[1] = true;
                    guessed[0] = true;
                    player_cookies[1] = 0;
                    player_cookies[0] = 0;
                    gamestate = 0;
                    return false;

                //one player guessed, thus another player endgame
                }else if(gamestate == 5 && ((player_cookies[0] == this_cookie && guessed [0] == false) || (player_cookies[1] == this_cookie && guessed [1] == false))){

                    if(sendhttp("6_endgame.html", sockfd, buff, &n, turn, this_cookie) == false){
                        printf("false15\n");
                        return false;
                    }
                                        
                    if(player_cookies[0] == this_cookie){
                        player_cookies[0] = 0;
                        guessed[0] = true;
                    }else if(player_cookies[1] == this_cookie){
                        player_cookies[1] = 0;
                        guessed[1] = true;
                    }else{
                        printf("ERROR\n");
                    }

                //both player are ready for guessing   
                }else if(gamestate == 3){
                    
                    char word[100];

                    //extract the gussing word
                    get_guessing_word(curr, word);

                    int player = 0;
                    if(this_cookie == player_cookies[0]){
                        player = 1;
                    }else if(this_cookie == player_cookies[1]){
                        player = 2;
                    }
                    
                    //one player guessed the word submitted by another player
                    if(game(word, player,words_player1, words_player2,&length1,&length2) == true){
                       
                        //set gamestate to finish game
                        gamestate = 5;
                      
                        
                        if(sendhttp("6_endgame.html", sockfd, buff, &n, turn, this_cookie) == false){
                            printf("false16\n");
                            return false;
                        }

                        //move to next turn(next image)
                        turn +=1;

                        if(player_cookies[0] == this_cookie){
                            player_cookies[0] = 0;
                            guessed[0] = true;
                            
                        }else if(player_cookies[1] == this_cookie){
                            player_cookies[1] = 0;
                            guessed[1] = true;
                        }
                    }
                    
                    // make the string of inputted word
                    char wordstring[2049] = "";
                    if(player_cookies[0] == this_cookie){
                        make_string(words_player1, length1, wordstring);
                        printf("WWW%s\n", wordstring);
                    }else if (player_cookies[1] == this_cookie){
                        make_string(words_player2, length2, wordstring);
                        printf("WWW%s\n", wordstring);
                    }else{
                        printf("ERROR!!!");
                    }

                    //send the html
                    
                    if(sendhttp_2str("4_accepted.html", sockfd, buff, turn, wordstring, this_cookie) == false){
                        printf("false17\n");
                        return false;
                    }

                }else{
                    //discard the result, send html
                    if(sendhttp("5_discarded.html", sockfd, buff, &n, turn, this_cookie) == false){
                        printf("false19\n");
                        return false;
                    }
                   
                }
            }
        }

    }else if (strncmp(curr, "favicon.ico", 11) == 0){
        printf("FAVVVVV");
        bool result;
        result = sendimage(sockfd, this_cookie);
        if(result == false){
            printf("false22\n");
            return false;
        }
    }else if (*curr == ' '){
        if (method == POST && strstr(curr, "quit=Quit") != NULL){
        
            if(player_cookies[0] == this_cookie){
                //player 1 quit game
                player_cookies[0] = 0;
            }else if(player_cookies[1] == this_cookie){
                //player 2 quit game
                player_cookies[1] = 0;
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

        //simply return the initial html
        }else if (method == GET && is_new_player == true){  
            if(sendhttp("1_intro.html", sockfd, buff, &n, turn, this_cookie) == false){
                printf("false20\n");
                return false;
            }
        
        //player has cookie, not new player
        }else if (method == GET && is_new_player == false){
            if(sendhttp_2str("2_start.html", sockfd, buff, turn, name_list[this_cookie], this_cookie) == false){
                printf("false200\n");
                return false;
            }
            
        }else if (method == POST){ 
            //find the username
            char* username = strstr(curr, "user=")+5;
            strcpy(name_list[this_cookie], username);

            //send the html with username
           if(sendhttp_2str("2_start.html", sockfd, buff, turn, username, this_cookie) == false){
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
    return true;
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
    printf("receiverd_cookie :%s\n",cookie_string);

    //convert to integer
    return atoi(cookie_string);
}
