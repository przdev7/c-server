#include "http.h"
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

unsigned long parse_method(char *methodRaw) {
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

HttpRequestLine parse_request_line(char *str) {
  HttpRequestLine type;
  memset(&type, 0, sizeof(type));
  char *saveptr;
  char *strcopy = malloc(strlen(str) + 1);
  strcpy(strcopy, str);
  char *token = strtok_r(strcopy, " ", &saveptr);

  switch (parse_method(token)) {
  case GET:
  case POST:
  case PUT:
  case PATCH:
  case DELETE:
  case HEAD:
  case OPTIONS:
  case TRACE:
  case CONNECT:
    type.method = parse_method(token);
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

HttpRequestHeader *parse_headers(char **saveptr) {
  int16_t count = 0;
  char *token, *saveptr_header;
  HttpRequestHeader *headers =
      malloc(MAX_HEADERS_COUNT * sizeof(HttpRequestHeader));

  while ((token = strsep(saveptr, "\n")) != NULL) {
    if (count > MAX_HEADERS_COUNT) {
      err(EXIT_FAILURE, "max headers count is 30");
    }

    if (token[0] == '\r' || token[0] == '\0') {
      break;
    }

    HttpRequestHeader header;
    memset(&header, 0, sizeof(header));

    header.key = strtok_r(token, ":", &saveptr_header);
    header.value = strtok_r(NULL, ":", &saveptr_header);
    headers[count] = header;

    count++;
  }

  headers = realloc(headers, count * sizeof(HttpRequestHeader));

  if (headers == NULL) {
    err(EXIT_FAILURE, "headers realloc failed");
  }

  return headers;
}
