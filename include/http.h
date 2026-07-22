#include <sys/socket.h>

#define PORT 8888

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
  HttpRequestHeader *headers;
} HttpRequest;

HttpRequestLine parse_request_line(char *str);
