#include "string_manipulation.h"

//convert 2d array of char to a single string
void make_string(char words[][100], int length, char* wordstring){
    
    int i;
    for(i = 0; i < length; i++){
        if(i!=0){
            strcat(wordstring, ", ");
        }

        //concat the char to string
        strcat(wordstring, words[i]);

    }
}

//extract the input word from ther buffer
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


//cookie of player from the request header
int get_cookie(char* curr){
    //find the address of string

    char* cookie_string = "0";
    if(strstr(curr, "Cookie: ")!= NULL){
        cookie_string = strstr(curr, "Cookie: ")+8;
    }
    // printf("receiverd_cookie :%s\n",cookie_string);

    //convert to integer
    return atoi(cookie_string);
}
