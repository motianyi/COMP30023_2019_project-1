
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
static char const * const HTTP_200_FORMAT_COOKIES = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\
Set-Cookie: %d\r\n\r\n";
static char const * const HTTP_200_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html\r\n\
Content-Length: %ld\r\n\r\n";
static char const * const HTTP_400 = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_400_LENGTH = 47;
static char const * const HTTP_404 = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
static int const HTTP_404_LENGTH = 45;

char* get_image_name(int turn);
bool sendhttp(char* filename, int sockfd, char* buff, int* n, int turn, int cookie);
bool sendhttp_2str(char* filename, int sockfd, char* buff, int turn, char* words_string, int cookie);
bool sendimage(int sockfd, int cookie);