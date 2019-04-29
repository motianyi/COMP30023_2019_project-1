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
static char* IMAGE_1 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-1.jpg";
static char* IMAGE_2 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-2.jpg";
static char* IMAGE_3 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-3.jpg";
static char* IMAGE_4 ="https://swift.rc.nectar.org.au/v1/AUTH_eab314456b624071ac5aecd721b977f0/comp30023-project/image-4.jpg";



// return ther image URL of that turn
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

//send html file with 1 strings input, image URL
bool sendhttp(char* filename, int sockfd, char* buff, int* n, int turn, int cookie){
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
    a = sprintf(buff, HTTP_200_FORMAT_COOKIES, size, cookie);
    // send the header first
    if (write(sockfd, buff, a) < 0)
    {
        perror("write");
        return false;
        
    }

    // read the content of the HTML file
    int filefd = open(filename, O_RDONLY);
    a = read(filefd, buff, 2048);
    
    if (a < 0)
    {
        perror("read");
        close(filefd);
        return false;
    }
    close(filefd);
    a = sprintf(html, buff, img_name);

    if (a < 0)
    {
        perror("sprintf get error");
        close(filefd);
        return false;
    }

    //send html
    if (write(sockfd, html, size) < 0)
    {
        perror("write");
        return false;
    }
    return true;

}

//send html file with 2 strings input, image URL and a string
bool sendhttp_2str(char* filename, int sockfd, char* buff, int turn, char* words_string, int cookie){
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
    a = sprintf(buff, HTTP_200_FORMAT_COOKIES, size, cookie);
    // send the header first
    if (write(sockfd, buff, a) < 0)
    {
        perror("write");
        return false;
        
    }

    
    // read the content of the HTML file
    int filefd = open(filename, O_RDONLY);
    a = read(filefd, buff, 2048);
    
    if (a < 0)
    {
        perror("read");
        close(filefd);
        return false;
    }
    close(filefd);
    a = sprintf(html, buff, img_name, words_string);

    if (a < 0)
    {
        perror("sprintf get error");
        close(filefd);
        return false;
    }

    if (write(sockfd, html, size) < 0)
    {
        perror("write");
        return false;
    }
    return true;

}



//send favicon.ico file to socket
bool sendimage(int sockfd, int cookie){
    char image[2049];
    // get the size of the file
    struct stat st;
    stat("favicon.ico", &st);
    
    // increase file size to accommodate the username
    long size = 0;
    size = st.st_size;
    
    int a;
    a = sprintf(image, HTTP_200_FORMAT_COOKIES, size, cookie);
    // send the image header first
    if (write(sockfd, image, a) < 0)
    {
        perror("write");
        return false;
        
    }

    
    // read the content of the image (favicon.ico) file
    int filefd = open("favicon.ico", O_RDONLY);
    a = read(filefd, image, 2048);
    
    if (a < 0)
    {
        perror("read");
        close(filefd);
        return false;
    }
    close(filefd);
    
    //senf image file
    if (write(sockfd, image, size) < 0)
    {
        perror("write");
        return false;
    }
    return true;

}