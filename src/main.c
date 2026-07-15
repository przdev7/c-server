#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define PORT 8080

int main() {
  int tcp_socket, conn;
  struct sockaddr_in addr, conn_addr;
  socklen_t conn_len = sizeof(conn_addr);
  char *buff;

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_socket == -1) {
    err(EXIT_FAILURE, "tcp_socket");
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

  ssize_t mess_size = recv(conn, &buff, sizeof(buff), 0);
  buff = malloc(mess_size);
  buff[mess_size] = '\0';
  printf("%s", buff);

  return 0;
}
