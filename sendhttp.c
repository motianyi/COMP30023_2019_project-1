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


// constants

char* get_image_name(int turn){
    if(turn == 1){
        return IMAGE_1;
    }else if(turn == 2){
        return IMAGE_2;
    }else if(turn == 3){
        return IMAGE_3;
    }else{
        return IMAGE_4;
    }
}


bool sendhttp(char* filename, int sockfd, char* buff, int* n, int turn){
    char html[2049];
    // get the size of the file
    struct stat st;
    stat(filename, &st);
    
    //find the image name of this turn
    char* img_name = get_image_name(turn);

    // increase file size to accommodate the username
    long size = 0;
    if(strcmp(filename, "6_endgame.html")== 0 || strcmp(filename, "7_gameover.html")==0){
        size = st.st_size;
    }else{
        size = st.st_size + strlen(img_name)-2;
    }    
    // long size = st.st_size + strlen(img_name)-2;
    int a;
    a = sprintf(buff, HTTP_200_FORMAT, size);
    // send the header first
    if (write(sockfd, buff, a) < 0)
    {
        perror("write");
        printf("false1\n");
        return false;
        
    }

    
    // read the content of the HTML file
    int filefd = open(filename, O_RDONLY);
    a = read(filefd, buff, 2048);
    
    if (a < 0)
    {
        perror("read");
        close(filefd);
        printf("false2\n");
        return false;
    }
    close(filefd);
    a = sprintf(html, buff, img_name);

    if (a < 0)
    {
        perror("sprintf get error");
        close(filefd);
        printf("false3\n");
        return false;
    }
    // printf("%.*s\n",(int)size, html);
    if (write(sockfd, html, size) < 0)
    {
        perror("write");
        printf("false4\n");
        return false;
    }
    return true;

}


bool sendhttp_2str(char* filename, int sockfd, char* buff, int turn, char* words_string){
    char html[2049];
    // get the size of the file
    struct stat st;
    stat(filename, &st);
    
    //find the image name of this turn
    char* img_name = get_image_name(turn);

    // increase file size to accommodate the username
    long size = 0;
    size = st.st_size + strlen(img_name)+ strlen(words_string)-2-2;
    
    
    int a;
    a = sprintf(buff, HTTP_200_FORMAT, size);
    // send the header first
    if (write(sockfd, buff, a) < 0)
    {
        perror("write");
        printf("false1\n");
        return false;
        
    }

    
    // read the content of the HTML file
    int filefd = open(filename, O_RDONLY);
    a = read(filefd, buff, 2048);
    
    if (a < 0)
    {
        perror("read");
        close(filefd);
        printf("false2\n");
        return false;
    }
    close(filefd);
    a = sprintf(html, buff, img_name, words_string);

    if (a < 0)
    {
        perror("sprintf get error");
        close(filefd);
        printf("false3\n");
        return false;
    }
    // printf("%.*s\n",(int)size, html);
    if (write(sockfd, html, size) < 0)
    {
        perror("write");
        printf("false4\n");
        return false;
    }
    return true;

}