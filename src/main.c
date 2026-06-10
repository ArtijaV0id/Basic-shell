#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// list of build in commands
static char* types[] = {
  "exit",
  "type",
  "echo"
};

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  // input buffer
  char input[100];

  while(1)
  {
    printf("$ ");
    
    // SAFE ALTERNATIVE TO gets()
    if (fgets(input, sizeof(input), stdin) == NULL) {
      break; // Handle EOF (like pressing Ctrl+D)
    }

    // STRIP THE NEWLINE CHARACTER
    // Converts "exit\n" into "exit\0"
    input[strcspn(input, "\n")] = '\0';

    

    // 1. Handle "exit"
    // Compares input with "exit" if so it just exits the program
    if(strcmp(input, "exit") == 0) // Fixed: explicitly check == 0 for clarity
    {
      exit(0);
    } 
    
    // 2. Handle "echo "
    if(strncmp(input, "echo ", 5) == 0)
    {
      printf("%s\n", input + 5); 
      continue;
    } 

    // 3. Handle "type "
    // compares input with command type and then moves cursor to 5 letters next
    if (strncmp(input, "type ", 5) == 0) 
    {
      // string of command that ignores "type" command
      char *command = input + 5; 
      int found = 0;

      for (int i = 0; i < sizeof(types) / sizeof(char*); i++)
      {
        // compares command with existing buildin commands
        if (strcmp(command, types[i]) == 0) 
        {
          printf("%s is a shell builtin\n", command); 
          found = 1;
          break; 
        }
      }

      if (!found) {
        // gets local enviroment path
        char* path = getenv("PATH");
        // path copy buffer
        char path_copy[1024];
        // copies path enviroment values to buffer 
        strcpy(path_copy, path);

        // removes ":" from path variable
        char* token = strtok(path_copy, ":");

        // loops continues if token isnt 0
        while(token != NULL)
        {
          // full path buffer
          char fullPath[1024];
          // gets full path buffer and formats it to include path to 
          // local executable and saves it
          snprintf(fullPath, sizeof(fullPath), "%s/%s", token, command);
          //printf("%s\n", fullPath);

          // checks if executable in current path have executable previlages
          if(access(fullPath, X_OK) == 0)  // true = exists and executable
          {
            printf("%s is %s\n", command, fullPath);
            found = 1;
            break;
          }

          // tells strok to keep parsing same path string
          token = strtok(NULL, ":");
        }
        
        // if found == false 
        if(!found)
          printf("%s: not found\n", command);
      }
      continue;
    }

    // parse input into args array
    char* args[64];
    int argCount = 0;
    char* token = strtok(input, " ");
    while(token != NULL)
    {
      args[argCount++] = token;
      token = strtok(NULL, " ");
    }
    args[argCount] = NULL;

    // args[0] is the command, search PATH for it
    // YOUR PATH SEARCH HERE (same as type handler)
    // 3. Handle commands system one
    // compares input with command type and then moves cursor to 5 letters next
    if (strcmp(input, args[0]) == 0) 
    {
      // string of command that ignores "type" command
      char *command = input; 
      int found = 0;

      for (int i = 0; i < sizeof(types) / sizeof(char*); i++)
      {
        // compares command with existing buildin commands
        if (strcmp(command, types[i]) == 0) 
        {
          printf("%s is a shell builtin\n", command); 
          found = 1;
          break; 
        }
      }

      if (!found) {
        // gets local enviroment path
        char* path = getenv("PATH");
        // path copy buffer
        char path_copy[1024];
        // copies path enviroment values to buffer 
        strcpy(path_copy, path);

        // removes ":" from path variable
        char* token = strtok(path_copy, ":");

        // loops continues if token isnt 0
        while(token != NULL)
        {
          // full path buffer
          char fullPath[1024];
          // gets full path buffer and formats it to include path to 
          // local executable and saves it
          snprintf(fullPath, sizeof(fullPath), "%s/%s", token, command);
          //printf("%s\n", fullPath);

          // if found: fork + execvp
          // if not found: print "args[0]: not found" 
          // checks if executable in current path have executable previlages
          if(access(fullPath, X_OK) == 0)
          {
            pid_t pid = fork();
            if(pid == 0)
            {
              execvp(fullPath, args);
            }
            else
            {
              wait(NULL);
            }
            found = 1;
            break;
          }

          // tells strok to keep parsing same path string
          token = strtok(NULL, ":");
        }
        
        // if found == false 
        if(!found)
          printf("%s: not found\n", command);
      }
      continue;
    }
    

    // 4. Handle unrecognized commands (so the shell doesn't just silently loop)
    // TODO: Add commands search here as 'else' statement
    if (strlen(input) > 0) {
      printf("%s: command not found\n", input);
    }
  }

  return 0;
}