#include <stdio.h>
#include <string.h>
#include "cJSON.h"

#define setJsonTable(OriginType, Len, ...) \
  struct {                                 \
    const char *na[Len];                   \
    int fl[Len];                           \
    int tp[Len];                           \
    int sz[Len];                           \
  } __attribute__((packed)) JSONTABLE__DONTUSEIT##OriginType =

#define jsonParse(type, str, dest)                 \
  parseTo(str, &dest, &JSONTABLE__DONTUSEIT##type, \
          sizeof(JSONTABLE__DONTUSEIT##type) / 20)

enum JSONType { JSONT_INT, JSONT_DOUBLE, JSONT_STRING };

void *ptr_add(void *base, int inc);

inline void *ptr_add(void *base, int inc) { return (char *)base + inc; }

typedef struct Example {
  int a;
  int b;
  double c;
  char d[40];
} Example;

setJsonTable(Example, 4){{"a", "b", "c", "d"},
                         { 0, 4, 8, 16},
                         {JSONT_INT, JSONT_INT, JSONT_DOUBLE, JSONT_STRING},
                         {0, 0, 0, 40}};

int parseTo(const char *json_string, void *dest, void *json_table,
            const int table_length) {
  cJSON *root = cJSON_Parse(json_string);

  char **names = json_table;
  int *fields = ptr_add(json_table, table_length * sizeof(char *));
  int *types =
      ptr_add(json_table, table_length * (sizeof(char *) + sizeof(int)));
  int *sizes =
      ptr_add(json_table, table_length * (sizeof(char *) + 2 * sizeof(int)));

  for (int i = 0; i < table_length; i++) {
    cJSON *t = cJSON_GetObjectItem(root, names[i]);
    void *base = ptr_add(dest, fields[i]);

    switch (types[i]) {
      case JSONT_INT:
        *(int *)base = t->valueint;
        break;
      case JSONT_DOUBLE:
        *(double *)base = t->valuedouble;
        break;
      case JSONT_STRING:
        strcpy(base, t->valuestring);
        break;
    }
  }

  return 0;
}

int main() {
  struct Example c;

  char str[] = "{\"a\":7777, \"b\": 4396, \"c\": 66.99, \"d\": \"string\"}";
  jsonParse(Example, str, c);

  printf("%d %d %.2lf %s\n", c.a, c.b, c.c, c.d);
}
