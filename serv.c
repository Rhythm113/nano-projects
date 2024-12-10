/*
 *   Custom HTTP Server
 *   Author : Rhythm113
 *   Date : 23/08/2024
 *   Target OS : Linux
 */

/**
 * refs :
 * https://man7.org/linux/man-pages/man3/perror.3.html
 * https://man7.org/linux/man-pages/man2/socket.2.html
 * https://man7.org/linux/man-pages/man2/bind.2.html
 * https://man7.org/linux/man-pages/man7/ip.7.html
 * https://man7.org/linux/man-pages/man3/htonl.3p.html
 */

// includes
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>

// defs
typedef struct sockaddr_in sock; // ease of use

// Configs global;
#define BUFFER_SIZE 104857600 // 10 MB
#define PORT 4000
    int fd;                                         // server descriptor
    sock addr;                                      // scoket data
    


// --------------------------------------------------------
// Utils
const char *get_ext(const char *file)
{
    const char *pos = strrchr(file, '.');
    if (!pos || pos == file)
    {
        return "";
    }
    return pos + 1;
}

// TO DO : Add more extensions
const char *get_mime_type(const char *file_ext)
{
    if (strcasecmp(file_ext, "html") == 0 || strcasecmp(file_ext, "htm") == 0)
    {
        return "text/html";
    }
    else if (strcasecmp(file_ext, "txt") == 0)
    {
        return "text/plain";
    }
    else if (strcasecmp(file_ext, "jpg") == 0 || strcasecmp(file_ext, "jpeg") == 0)
    {
        return "image/jpeg";
    }
    else if (strcasecmp(file_ext, "png") == 0)
    {
        return "image/png";
    }
    else
    {
        return "application/octet-stream";
    }
}

// cmp skipping case
bool pure_cmp(const char *s1, const char *s2)
{
    while (*s1 && *s2)
    {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
        {
            return false;
        }
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

// umm some ctrl c & ctrl v
char *get_file_case_insensitive(const char *file_name)
{
    DIR *dir = opendir(".");
    if (dir == NULL)
    {
        perror("opendir");
        return NULL;
    }

    struct dirent *entry;
    char *found_file_name = NULL;
    while ((entry = readdir(dir)) != NULL)
    {
        if (pure_cmp(entry->d_name, file_name))
        {
            found_file_name = entry->d_name;
            break;
        }
    }

    closedir(dir);
    return found_file_name;
}

char *url_decode(const char *src)
{
    size_t src_len = strlen(src);
    char *decoded = malloc(src_len + 1);
    size_t decoded_len = 0;

    // decode %2x to hex
    for (size_t i = 0; i < src_len; i++)
    {
        if (src[i] == '%' && i + 2 < src_len)
        {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        }
        else
        {
            decoded[decoded_len++] = src[i];
        }
    }

    // add null terminator
    decoded[decoded_len] = '\0';
    return decoded;
}

//  ------------------------------------------------------------------


//core 

void build_http_response(const char *file_name,
                         const char *file_ext,
                         char *response,
                         size_t *response_len)
{
    // build HTTP header
    const char *mime_type = get_mime_type(file_ext);
    char *header = (char *)malloc(BUFFER_SIZE * sizeof(char));
    snprintf(header, BUFFER_SIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "\r\n",
             mime_type);
    


    // if file not exist, response is 404 Not Found
    int file_fd = open(file_name, O_RDONLY);
    
    
        // handle index.html / 
    if (strcmp(file_name,"index.html") == 0 && file_fd == -1 )
    {
        snprintf(response, BUFFER_SIZE,
               "HTTP/1.1 200 OK\r\n" //no error code
                "Content-Type: text/html\r\n"
                "\r\n"
                 "<!DOCTYPE HTML>\n<html><head>\n<title>Default Page</title>\n</head><body>\n"
                 "<h1>Welcome to the custom built web server.</h1>\n"
                 "<p>Upload your index.html on your server.</p>\n"
                 "<hr>\n<address>Developed by Rhythm113 (Custom HTTP server)</address>\n"
                 "</body></html>\n"
                 );
        *response_len = strlen(response);
        return;
    }
  
    
    
    
    
    
    if (file_fd == -1)
    {
       snprintf(response, BUFFER_SIZE,
               "HTTP/1.1 404 Not Found\r\n" //no error code
                "Content-Type: text/html\r\n"
                "\r\n"
                 "<!DOCTYPE HTML>\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n"
                 "<h1>Not Found</h1>\n"
                 "<p>The requested URL was not found on this server.</p>\n"
                 "<hr>\n<address>Developed by Rhythm113 (Custom HTTP server)</address>\n"
                 "</body></html>\n"
                 );
        *response_len = strlen(response);
        return;
    }

    // get file size for Content-Length
    struct stat file_stat;
    fstat(file_fd, &file_stat);
    off_t file_size = file_stat.st_size;

    // copy header to response buffer
    *response_len = 0;
    memcpy(response, header, strlen(header));
    *response_len += strlen(header);

    // copy file to response buffer
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, response + *response_len, BUFFER_SIZE - *response_len)) > 0)
    {
        *response_len += bytes_read;
    }
    free(header);
    close(file_fd);
}


//crash handle 
void handle_sigint(int sig) {
    printf("\nInterrupt signal received. Shutting down the server...\n");
    if (fd != -1) {
        close(fd);
        printf("Socket closed, port released.\n");
    }
    exit(0);
}




void *handle_client(void *arg)
{
    int client_fd = *((int *)arg);
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // receive request data from client and store into buffer
    ssize_t bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0)
    {
        // check if request is GET 
        //TO DO : handle POST PUT etc
        regex_t regex;
        regcomp(&regex, "^GET /([^ ]*) HTTP/1", REG_EXTENDED);
        regmatch_t matches[2];

        if (regexec(&regex, buffer, 2, matches, 0) == 0)
        {
            // extract filename from request and decode URL
            buffer[matches[1].rm_eo] = '\0';
            const char *url_encoded_file_name = buffer + matches[1].rm_so;
            char *file_name = url_decode(url_encoded_file_name);
            

            printf("* request: %s\n",file_name);
            if(file_name[0] == '\0'){
              strcpy(file_name,"index.html");
              }
            /*printf("* HEX : ");
            for(int i = 0; i < sizeof(file_name); i++){
                printf("0x%x " , file_name[i]);
                }
            printf("\n"); */

            // get file extension
            char file_ext[32];
            strcpy(file_ext, get_ext(file_name));

            // build HTTP response
            char *response = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
            size_t response_len;
            build_http_response(file_name, file_ext, response, &response_len);

            // send HTTP response to client
            send(client_fd, response, response_len, 0);

            free(response);
            free(file_name);
        }
        regfree(&regex);
    }
    close(client_fd);
    free(arg);
    free(buffer);
    return NULL;
}

/*

   tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
   udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
   raw_socket = socket(AF_INET, SOCK_RAW, protocol);

*/


int main(int argc, char *argv[]) 
{

    //sig
    signal(SIGINT, handle_sigint);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // IPV4 , TCP
    {
        perror("socket creation failure");
        exit(-1);
    }

    // config
    addr.sin_family = AF_INET;         // IPV4
    addr.sin_addr.s_addr = INADDR_ANY; // any conn
    addr.sin_port = htons(PORT);

    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) // cast needed
    {
        perror("bind failure");
        exit(-1);
    }

    // int listen(int sockfd, int backlog);
    if (listen(fd, 10) < 0) // still don't know why i set the backlog to 10 :)
    {
        perror("listen failure");
        exit(-1);
    }

    printf("init done ! listening on : %d\n", PORT);
        while (1) {
        // client info
        sock client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int *client_fd = malloc(sizeof(int));

        // accept client connection
        if ((*client_fd = accept(fd, 
                                (struct sockaddr *)&client_addr, 
                                &client_addr_len)) < 0) {
            perror("accept failed");
            continue;
        }

        // new thread. TO DO : reduce mem usage
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)client_fd);
        pthread_detach(thread_id);
    }


    close(fd);
    return 0;
}
