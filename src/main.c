#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

typedef enum {
  GET,
  POST,
  PUT,
  PATCH,
  DELETE,
  HEAD,
  OPTIONS,
  TRACE,
  CONNECT,
} HttpMethods;

typedef struct {
  HttpMethods method;
  char *request_target; // request path
  char *protocol;       // in most cases HTTP/1.1
} HttpRequestLine;

typedef struct {
  char *key;
  void *value;
} HttpRequestHeader;

typedef struct {
  HttpRequestLine requestLine;
  HttpRequestHeader headers[];
} HttpRequest;

unsigned long parseMethod(char *methodRaw) {
  if (strcmp(methodRaw, "GET") == 0)
    return GET;
  if (strcmp(methodRaw, "POST") == 0)
    return POST;
  if (strcmp(methodRaw, "PUT") == 0)
    return PUT;
  if (strcmp(methodRaw, "PATCH") == 0)
    return PATCH;
  if (strcmp(methodRaw, "DELETE") == 0)
    return DELETE;
  if (strcmp(methodRaw, "HEAD") == 0)
    return HEAD;
  if (strcmp(methodRaw, "OPTIONS") == 0)
    return OPTIONS;
  if (strcmp(methodRaw, "TRACE") == 0)
    return TRACE;
  if (strcmp(methodRaw, "CONNECT") == 0)
    return CONNECT;

  err(EXIT_FAILURE, "unknown method");
}

HttpRequestLine parseRequestLine(char *str) {
  HttpRequestLine type;
  memset(&type, 0, sizeof(type));
  char *saveptr;
  char *strcopy = malloc(strlen(str) + 1);
  strcpy(strcopy, str);
  char *token = strtok_r(strcopy, " ", &saveptr);

  switch (parseMethod(token)) {
  case GET:
  case POST:
  case PUT:
  case PATCH:
  case DELETE:
  case HEAD:
  case OPTIONS:
  case TRACE:
  case CONNECT:
    type.method = parseMethod(token);
    break;
  }

  while (token != NULL) {
    if (token[0] == '/') {
      type.request_target = strdup(token);
    } else {
      type.protocol = strdup(token);
    }

    token = strtok_r(NULL, " ", &saveptr);
  }

  free(strcopy);
  return type;
}

void parseHeaders(char *str) {
  HttpRequestHeader header;
  char *saveptr;
  char *key = strtok_r(str, ":", &saveptr);
  char *value = strtok_r(NULL, ":", &saveptr);
  memset(&header, 0, sizeof(header));

  header.key = key;
  header.value = value;

  printf("%s, %s - header\n", key, value);
}

int main() {
  int tcp_socket, conn;
  int opt = 1;
  struct sockaddr_in addr, conn_addr;
  socklen_t conn_len = sizeof(conn_addr);
  char buff[8192];

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_socket == -1) {
    err(EXIT_FAILURE, "tcp_socket");
  }

  if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt failed");
  }

  if (bind(tcp_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    err(EXIT_FAILURE, "bind");
  }

  if (listen(tcp_socket, SOMAXCONN) == -1) {
    err(EXIT_FAILURE, "listen");
  }
  printf("Server listening on port %d...\n", PORT);

  conn = accept(tcp_socket, (struct sockaddr *)&conn_addr, &conn_len);
  if (conn == -1) {
    err(EXIT_FAILURE, "connection failed");
  }

  char *saveptr;
  ssize_t mess_size = recv(conn, buff, sizeof(buff), 0);
  char *line = strtok_r(buff, "\r\n", &saveptr);

  HttpRequestLine type = parseRequestLine(line);
  /* printf("%d, %s, %s", type.method, type.request_target, type.protocol); */

  while (line != NULL) {
    line = strtok_r(NULL, "\r\n", &saveptr);
    // printf("%s\n", line);
    /* parseHeaders(line); */
  }

  shutdown(tcp_socket, SHUT_RDWR);
  close(tcp_socket);
  return 0;
}
