#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INPUT_SIZE 1024
#define MAX_ARGS 64

typedef enum {
  BUILTIN_ECHO,
  BUILTIN_EXIT,
  BUILTIN_TYPE,
  BUILTIN_PWD,
  BUILTIN_CD,
  BUILTIN_UNKNOWN
} Builtin;

Builtin type_command(const char *cmd);

char *trimwhitespace(char input[INPUT_SIZE]);
void echo_builtin(int argc, char *argv[]);
void type_builtin(const char *cmd);
void pwd_builtin();
void cd_builtin(const char *path);
char* search_path(const char *full_path);
void print_command_location(const char *cmd);
void run_program(char *argv[]);


int main(void) {
  setbuf(stdout, NULL);
  
  while (1) {  
    char input[INPUT_SIZE];
    char *argv[MAX_ARGS];
    int argc = 0;
    char *arg, *arg_ptr;
    printf("$ ");
    fflush(stdout);

    if (fgets(input, sizeof input, stdin) == NULL ) {
      break;
    }
    
    size_t len = strcspn(input, "\r\n");
    input[len] = '\0';

    trimwhitespace(input);

    arg_ptr = input;
    while ((arg = strsep(&arg_ptr, " ")) != NULL) {
      if (*arg == '\0') {
        continue;
      }

      if (argc < MAX_ARGS - 1) {
        argv[argc++] = arg;
      } else {
        break;
      }
    }

    argv[argc] = NULL;

    if (strncmp(argv[0], "exit" , 4) == 0) {
      break;
    }
    else if (strncmp(argv[0], "echo" , 4) == 0) {
      echo_builtin(argc, argv);
    }
    else if (strncmp(argv[0], "type" , 4) == 0) {
      if (argc == 1) {
        printf("usage: type command\n");
      } else {
        type_builtin(argv[1]);
      }
    }
    else if (strncmp(argv[0], "pwd", 3) == 0) {
      pwd_builtin();
    }
    else if (strncmp(argv[0], "cd", 2) == 0) {
      cd_builtin(argv[1]);
    }
    else {
      if (input[0] != '\0') {
        run_program(argv);
      }
    }

  }

  return 0;
}

Builtin type_command(const char *cmd) {
  if (strncmp(cmd, "echo", 4) == 0) return BUILTIN_ECHO;
  if (strncmp(cmd, "exit", 4) == 0) return BUILTIN_EXIT;
  if (strncmp(cmd, "type", 4) == 0) return BUILTIN_TYPE;
  if (strncmp(cmd, "pwd", 3) == 0) return BUILTIN_PWD;
  if (strncmp(cmd, "cd", 2) == 0) return BUILTIN_CD;
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

void echo_builtin(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++){
    printf("%s", argv[i]);
    if (i < argc - 1) {
      printf(" ");
    }
  }
  printf("\n");
}

void type_builtin(const char *cmd) {
  switch (type_command(cmd)) {
    case BUILTIN_ECHO:
    case BUILTIN_EXIT:
    case BUILTIN_TYPE:
    case BUILTIN_PWD:
    case BUILTIN_CD:
      printf("%s is a shell builtin\n", cmd);
      break;
    case BUILTIN_UNKNOWN:
    default:
      print_command_location(cmd);
      break;
  }
}

void pwd_builtin() {
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  printf("%s\n", cwd);
}

void cd_builtin(const char *path) {
  int status = chdir(path);
  if (status == -1) {
    fprintf(stderr, "cd: %s: No such file or directory\n", path);
    return;
  }
}

char* search_path(const char *cmd) {
  if (cmd == NULL || *cmd == '\0') {
    return NULL;
  }

  const char *raw_path;
  char *env_path, *path_ptr, *dir;
  char *found_path = NULL;
  
  raw_path = getenv("PATH");
  
  if (raw_path == NULL) {
    raw_path = "/usr/bin:/bin";
  }

  env_path = strdup(raw_path);

  if (env_path == NULL) {
    fprintf(stderr, "Memory Allocation Failed\n");
    return NULL;
  }

  path_ptr = env_path;

  while((dir = strsep(&path_ptr, ":")) != NULL) {

    if (*dir == '\0') {
      dir = ".";
    }

    char full_path[PATH_MAX];
    int length_needed = snprintf(full_path, sizeof(full_path), "%s/%s", dir, cmd);

    if (length_needed < 0 || (size_t) length_needed >= sizeof full_path) {
      fprintf(stderr, "Buffer too small (or encoding error)\n");
      continue;
    }

    struct stat path_stat;
    if (access(full_path, X_OK) == 0 && stat(full_path, &path_stat) == 0) {
      if (S_ISREG(path_stat.st_mode)) {
        found_path = strdup(full_path);
        break;
      }
    }
    
  }
  
  free(env_path);
  return found_path;
}

void print_command_location(const char *cmd) {
  char *full_path = search_path(cmd);
  if (full_path == NULL) {
    printf("%s: not found\n", cmd);
  } else {
    printf("%s is %s\n", cmd, full_path);
    free(full_path);
  }
}

void run_program(char *argv[]) {
  char *full_path = search_path(argv[0]);
  if (full_path == NULL) {
    printf("%s: not found\n", argv[0]);
    return;
  }
  
  pid_t pid = fork();
  
  if (pid < 0) {
    perror("fork failed");
    free(full_path);
    return;
  }

  if (pid == 0) {
    execv(full_path, argv);
    perror("execv failed");
    free(full_path);
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, 0);
    free(full_path);
  } 
  
}
