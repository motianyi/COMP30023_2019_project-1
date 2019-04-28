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
void make_string(char words[][100], int length, char* wordstring);
bool game(char* word, int player, char words_player1[][100], char words_player2[][100], int* length1, int* length2);
int get_cookie(char* curr);
void get_guessing_word(char* curr, char* word);
bool handle_http_request(int sockfd);