#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define INPUT_SIZE 1024

typedef enum {
  BUILTIN_ECHO,
  BUILTIN_EXIT,
  BUILTIN_TYPE,
  BUILTIN_UNKNOWN
} Builtin;

Builtin type_command(const char *cmd);

char *trimwhitespace(char input[INPUT_SIZE]);
void echo_builtin(char input[INPUT_SIZE]);
void type_builtin(char input[INPUT_SIZE]);
int search_path(char *cmd);

int main(void) {
  setbuf(stdout, NULL);

  char input[INPUT_SIZE];
  
  while (1) {  
    printf("$ ");
    fflush(stdout);

    if (fgets(input, sizeof input, stdin) == NULL ) {
      break;
    }
    
    size_t len = strcspn(input, "\r\n");
    input[len] = '\0';

    trimwhitespace(input);

    if (strncmp(input, "exit" , 4) == 0 && (input[4] == ' ' || input[4] == '\0')) {
      break;
    }
    else if (strncmp(input, "echo" , 4) == 0) {
      echo_builtin(input);
    }
    else if (strncmp(input, "type" , 4) == 0) {
      type_builtin(input);
    }
    else {
      if (input[0] != '\0') {
        printf("%s: command not found\n", input);
      }
    }  

  }

  return 0;
}

Builtin type_command(const char *cmd) {
  if (strcmp(cmd, "echo") == 0) return BUILTIN_ECHO;
  if (strcmp(cmd, "exit") == 0) return BUILTIN_EXIT;
  if (strcmp(cmd, "type") == 0) return BUILTIN_TYPE;
  return BUILTIN_UNKNOWN;
}

char *trimwhitespace(char input[INPUT_SIZE]){
  char *ptr_input = input;
  char *end_input;

  //Triming leading and tailing space
  while(isspace((unsigned char)*ptr_input)) ptr_input++;

  memmove(input, ptr_input, strlen(ptr_input) + 1);

  if (*input == '\0') return input;

  end_input = input + strlen(input) - 1;
  while(end_input > input && isspace((unsigned char) *end_input)) end_input--;

  end_input[1] = '\0';

  return input;
}

void echo_builtin(char echo_msg[INPUT_SIZE]) {
  if (echo_msg[4] == '\0') {
    printf("\n");
  } else if (echo_msg[4] == ' ') {
    memmove(echo_msg, &echo_msg[4], strlen(&echo_msg[4]) + 1);
    trimwhitespace(echo_msg);
    printf("%s\n", echo_msg);
  }
}

void type_builtin(char type_cmd[INPUT_SIZE]) {
  if (type_cmd[4] == '\0') {
    printf("usage: type command\n");
  } else if (type_cmd[4] == ' ') {
    memmove(type_cmd, &type_cmd[4], strlen(&type_cmd[4]) + 1);
    trimwhitespace(type_cmd);

    switch (type_command(type_cmd)) {
      case BUILTIN_ECHO:
      case BUILTIN_EXIT:
      case BUILTIN_TYPE:
        printf("%s is a shell builtin\n", type_cmd);
        break;
      case BUILTIN_UNKNOWN:
      default:
        if (search_path(type_cmd) == 1) {
          printf("%s: not found\n", type_cmd);
        }
        break;
    }
  }
}

int search_path(char *cmd) {
  const char *raw_path = getenv("PATH");

  if (raw_path == NULL){
    return 1; //PATH not found
  }

  char *env_path = strdup(raw_path);
  if (env_path == NULL) {
    return 1; //malloc failed
  }

  char *path_ptr = env_path;
  char *dir_ptr;

  while ((dir_ptr = strsep(&path_ptr, ":")) != NULL) {

    if (*dir_ptr == '\0') {
      dir_ptr = ".";
    }

    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir_ptr, cmd);

    struct stat path_stat;
    if (access(full_path, X_OK) == 0 && stat(full_path, &path_stat) == 0) {
      if (S_ISREG(path_stat.st_mode)) {
        printf("%s is %s\n", cmd, full_path);
        free(env_path);
        return 0;
      }
    }
  }

  free(env_path);
  return 1; //command not found
}