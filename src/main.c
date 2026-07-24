#include "http.h"
#include <err.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  int socket_fd, client_fd;
  int opt = 1;
  int capacity = 128;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char *buff = malloc(capacity * sizeof(char) + 1);

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    err(EXIT_FAILURE, "tcp_socket");
  }

  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt failed");
  }

  if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    err(EXIT_FAILURE, "bind");
  }

  if (listen(socket_fd, SOMAXCONN) < 0) {
    err(EXIT_FAILURE, "listen");
  }
  printf("Server listening on port %d...\n", PORT);

  client_fd =
      accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_fd == -1) {
    err(EXIT_FAILURE, "connection failed");
  }

  ssize_t mess_size;
  ssize_t total = 0;
  while ((mess_size = recv(client_fd, buff + total, capacity - total, 0)) > 0) {
    total += mess_size;

    if (total == capacity) {
      capacity *= 2;
      char *tmp = realloc(buff, capacity);

      if (tmp == NULL) {
        free(buff);
        err(EXIT_FAILURE, "realloc failed");
      }

      buff = tmp;
    }

    if (strstr(buff, "\r\n\r\n")) {
      break;
    }
  }

  buff[total] = '\0';
  char *saveptr = buff;

  char *line = strsep(&saveptr, "\n");
  HttpRequestLine type = parse_request_line(line);
  HttpRequestHeader *headers = parse_headers(&saveptr);

  HttpRequestHeader *main_header = headers;

  char *cl_key = "Content-Length";
  char *cl_raw_value = get_header(headers, cl_key);

  if (cl_raw_value == NULL || atoi(cl_raw_value) < 0) {
    err(EXIT_FAILURE, "invalid request content length");
  }

  size_t cl_value = atoi(cl_raw_value);
  printf("%zu\n", cl_value);

  free(main_header);
  free(buff);

  shutdown(socket_fd, SHUT_RDWR);
  close(socket_fd);
  return 0;
}
