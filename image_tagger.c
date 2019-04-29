
/*
** imager_tagger.c
*/

#include "image_tagger.h"

#include "sendhttp.h"
#include "game.h"
#include "string_manipulation.h"

// represents the types of method
typedef enum
{
    GET,
    POST,
    UNKNOWN
} METHOD;


const int WORD_LENGTH = 100;  

bool handle_http_request(int sockfd){

    //player1 and player2 input words
    static char words_player1[100][100];
    static char words_player2[100][100];

    //player1 and player2 number of input words
    static int length1 = 0;
    static int length2 = 0;

    //count of quit
    static int  isquit = 0;

    //turn of game (to determine which image should be send)
    static int turn = 1;
    static bool guessed[2] = {true, true};

    //game state can be 1,2,3,5
    //1 means first player is ready
    //2 means second player is ready
    //3 means both players are ready
    //5 means one player guess
    static int gamestate = 0;
    
    //cookeis of players
    static int player_cookies[2] = {0, 0};
    static int num_cookies = 0;

    //stored names of players,
    static char name_list[100][20];


    char buff[2049];
    int n = read(sockfd, buff, 2049);
    if (n <= 0){
        if (n < 0){
            perror("read");
        }else{
            printf("socket %d close the connection\n", sockfd);
        }
        return false;
    }

 
    // terminate the string
    buff[n] = 0;
    char * curr = buff;

    //default cookie is 10
    int this_cookie = 10;
    bool is_new_player = false;

    // if receive a cookie, set this_cookie to recieved one
    // if receive 0, set this_cookie to a new number
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
        return false;
    }
    
    // sanitise the URI, remove . and /
    while (*curr == '.' || *curr == '/'){
        ++curr;
    }

    if (strncmp(curr, "?start=Start", 12) == 0){
        // assume the only valid request URI is "/" but it can be modified to accept more files
        
        if (method == GET){
            
            //assign cookies
            if(player_cookies[0] == 0){
                player_cookies[0] = this_cookie;
            }else if(player_cookies[1] == 0){
                player_cookies[1] = this_cookie;
            }

            //find the state of game
            if(player_cookies[0] != 0 && player_cookies[1] == 0){
                gamestate = 1;
            }else if(player_cookies[1] == 0 && player_cookies[1] != 0){
                gamestate = 2;
            }else if(player_cookies[0] != 0 && player_cookies[1] != 0 && guessed[0] == true && guessed[1] == true){
                gamestate = 3;
                guessed[0] = false;
                guessed[1] = false;
            }

            if(sendhttp("3_first_turn.html", sockfd, buff, &n, turn, this_cookie) == false){
                return false;
            }
           
        }else if (method == POST){
           

            //player want to quit the game
            if (strstr(curr, "quit=Quit") != NULL){
        
                if(player_cookies[0] == this_cookie){
                    //player 1 quit game
                    player_cookies[0] = 0;
                }else if(player_cookies[1] == this_cookie){
                    //player 2 quit game
                    player_cookies[1] = 0;
                }
                isquit += 1;

                if(sendhttp("7_gameover.html", sockfd, buff, &n, turn, this_cookie) == false){
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
                    }

                    //send the gameover page
                    if(sendhttp("7_gameover.html", sockfd, buff, &n, turn, this_cookie) == false){
                        return false;
                    }
                    
                    //increase the isquit
                    isquit +=1;
                   
                    //reset the game,
                    //assign value to all variables
                    turn = 1;
                    guessed[1] = true;
                    guessed[0] = true;
                    player_cookies[1] = 0;
                    player_cookies[0] = 0;
                    gamestate = 0;
                    length1 = 0;
                    length2 = 0;
                    return false;

                //one player guessed, thus another player endgame
                }else if(gamestate == 5 && ((player_cookies[0] == this_cookie && guessed [0] == false) || (player_cookies[1] == this_cookie && guessed [1] == false))){

                    if(sendhttp("6_endgame.html", sockfd, buff, &n, turn, this_cookie) == false){
                        return false;
                    }

                    //reset the player_cookies and guessed               
                    if(player_cookies[0] == this_cookie){
                        player_cookies[0] = 0;
                        guessed[0] = true;
                    }else if(player_cookies[1] == this_cookie){
                        player_cookies[1] = 0;
                        guessed[1] = true;
                    }

                //both player are ready for guessing   
                }else if(gamestate == 3){
                    
                    char word[WORD_LENGTH];

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
                            return false;
                        }

                        //move to next turn(next image)
                        turn +=1;

                        //reset player_cookie of this player
                        if(player_cookies[0] == this_cookie){
                            player_cookies[0] = 0;
                            guessed[0] = true;
                            
                        }else if(player_cookies[1] == this_cookie){
                            player_cookies[1] = 0;
                            guessed[1] = true;
                        }
                    }
                    
                    // make the string of inputted words of player
                    char wordstring[2049] = "";
                    if(player_cookies[0] == this_cookie){
                        make_string(words_player1, length1, wordstring);
                    }else if (player_cookies[1] == this_cookie){
                        make_string(words_player2, length2, wordstring);
                    }

                    //send the html with word string
                    if(sendhttp_2str("4_accepted.html", sockfd, buff, turn, wordstring, this_cookie) == false){
                        return false;
                    }

                }else{
                    //discard the result, send html
                    if(sendhttp("5_discarded.html", sockfd, buff, &n, turn, this_cookie) == false){
                        return false;
                    }
                   
                }
            }
        }

    }else if (strncmp(curr, "favicon.ico", 11) == 0){
        //send the favicon image
        if(sendimage(sockfd, this_cookie) == false){
            return false;
        }
    }else if (*curr == ' '){

        //one player want to quit
        if (method == POST && strstr(curr, "quit=Quit") != NULL){
            if(player_cookies[0] == this_cookie){
                //player 1 quit game
                player_cookies[0] = 0;
            }else if(player_cookies[1] == this_cookie){
                //player 2 quit game
                player_cookies[1] = 0;
            }

            isquit += 1;
            length1 = 0;
            length2 = 0;

            if(sendhttp("7_gameover.html", sockfd, buff, &n, turn, this_cookie) == false){
                return false;
            }
            return false;      

        //simply return the initial html
        }else if (method == GET && is_new_player == true){  
            if(sendhttp("1_intro.html", sockfd, buff, &n, turn, this_cookie) == false){
                return false;
            }
        //player has cookie, not new player
        }else if (method == GET && is_new_player == false){
            if(sendhttp_2str("2_start.html", sockfd, buff, turn, name_list[this_cookie], this_cookie) == false){
                return false;
            }
        }else if (method == POST){ 
            //get the username
            char* username = strstr(curr, "user=")+5;
            strcpy(name_list[this_cookie], username);

            //send the html with username
           if(sendhttp_2str("2_start.html", sockfd, buff, turn, username, this_cookie) == false){
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
    return true;
}

