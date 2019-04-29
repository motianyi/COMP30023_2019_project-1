#include "game.h"

bool game(char* word, int player, char words_player1[][100], char words_player2[][100], int* length1, int* length2){
    int i;
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

