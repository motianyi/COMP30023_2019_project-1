

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

static void make_string(char words[][100], int length, char* wordstring);
static bool game(char* word, int player, char words_player1[][100], char words_player2[][100], int* length1, int* length2);
static bool handle_http_request(int sockfd);
int main(int argc, char * argv[]);