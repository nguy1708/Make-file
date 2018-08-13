#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

void show_error_message(char * ExecName) {
  fprintf(stderr, "Usage: %s [options] [target] : only single target is allowed.\n", ExecName);
  fprintf(stderr, "-f FILE\t\tRead FILE as a makefile.\n");
  fprintf(stderr, "-h\t\tPrint this message and exit.\n");
  exit(0);
}

int main(int argc, char *argv[]) {
  target_t targets[MAX_NODES];
  int nTargetCount = 0;

  /* Variables you'll want to use */
  char Makefile[64] = "Makefile";
  char TargetName[64];

  /* Declarations for getopt */
  extern int optind;
  extern char * optarg;
  int ch;
  char * format = "f:h";

  while((ch = getopt(argc, argv, format)) != -1) {
    switch(ch) {
      case 'f':
        strcpy(Makefile, strdup(optarg));
        break;
      case 'h':
      default:
        show_error_message(argv[0]);
        exit(1);
    }
  }

  argc -= optind;
  if(argc > 1) {
    show_error_message(argv[0]);
    return -1;
  }

  /* Init Targets */
  memset(targets, 0, sizeof(targets));

  /* Parse graph file or die */
  if((nTargetCount = parse(Makefile, targets)) == -1) {
    return -1;
  }

  /* Comment out the following line before submission */
  show_targets(targets, nTargetCount);

  /*
   * Set Targetname
   * If target is not set, set it to default (first target from makefile)
   */
  if(argc == 1) {
    strcpy(TargetName, argv[optind]);
  } else {
    strcpy(TargetName, targets[0].TargetName);
  }

  /*
   * Now, the file has been parsed and the targets have been named.
   * You'll now want to check all dependencies (whether they are
   * available targets or files) and then execute the target that
   * was specified on the command line, along with their dependencies,
   * etc. Else if no target is mentioned then build the first target
   * found in Makefile.
   */

  /*
   * INSERT YOUR CODE HERE
   */

   // TO DO: Dealing with arg Input
   // CASE 1: No target is mentioned
   // If there are no valid input file(s) or target(s),
   // the program will print an error message and be terminated.

   // CASE 2: If no target is specified on the command-line, build the FIRST target

   // OTHER CASE:
   make_progess(targets, 0);
}

  /*
   * For unchecked targets,
   * if it has no dependency, execute it [BASE CASE]
   * else, make it wait [RECURSIVE CASES]
   */
  void make_progess(target_t targets[], int t_index){
    if (status_map[t_index] == 0) {
      status_map[t_index] = 1; // set it to be checked
      if (targets[t_index].DependencyCount == 0) { // Detect CHILD
        track_back(targets[], targets[index].TargetName);
        execvp(*build_argv(targets[t_index].Command), build_argv(targets[t_index].Command));
        // fork() here?
      }
      else{
        child_exec_wait(target_t targets[], t_index)
        make_progess(targets[], t_index++);
      }
    }
  }

   void child_exec_wait(target_t targets[], int t_index){
     int pid;
     int status = 0;
     pid = fork();
     if (pid > 0){
       pid = waitpid(pid, &status, 0);
     }
     else if (pid == 0){
       return;
     }
     else{
       perror ("fork problem");
       exit (-1);
     }
   }

   /*
    * Depth-first-search
    */
    static bool tag[10];
    memset(tag, false, sizeof(tag));

   bool dfs(target_t targets[], int index){
     bool should_update = false;
     for (int i = 0; i < targets[index].DependencyCount; i++){
       if (dfs(targets[], get_dependency(targets, targets[index])[i])){
         should_update = true;
       }
     }
     if (should_update){
       tag[index] = true;
     }
     return tag[index];
   }

   /*
    * Helper function for DFS
    * Get the other targets which depends on the current target_t,
    * Return a list of target_t
    */
   target_t[] get_dependency(target_t targets[], int curr_index){
     target_t dep[10];
     target_t curr_target = targets[curr_index];
     for (int i = 0; i < curr_target.DependencyCount; i++){
       for (int j = 0; j < sizeof(targets); j++){
         if (curr_target.DependencyNames[i] == targets[j].TargetName){
           dep[i] = targets[j];
         }
       }
     }
   }

   /*
    * A boolean array to store the status of each target
    * in order to track the making-process
    * true: the target has already been compiled
    * false: not visited yet
    */
   static bool status[10];
   memset(status, false, sizeof(status));
