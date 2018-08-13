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
  //show_targets(targets, nTargetCount);

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
   *
   *
   *
   */
   int t_status = check_progress(TargetName, targets, nTargetCount);


  return 0;
}

void fork_to_run_command(target_t targets[], int t_index){
	pid_t pid = fork();                       //We want the process to fork
	if (pid > 0) {       //Make the parent process wait for child process.
    int wstatus;
    wait(&wstatus);
    if(WEXITSTATUS(wstatus) != 0){
      printf("Child exited with error code=%d\n", WEXITSTATUS(wstatus));
      exit(-1);
    }
	}
	else if (pid == 0) {     //Otherwise, run the command that is in the makefile
		printf("Command: %s", targets[t_index].Command);
    char **arg = build_argv(targets[t_index].Command);
		execvp(*arg, arg);
		perror("Exec failed");	    //If item could not run process, then returns error
		exit(-1);
	}
	else {
		perror("Fork failed");     //If pid is less than 0, fork failed.
		exit(-1);
	}
}

int check_progress(char* TargetName, target_t targets[], int nTargetCount){
/*This will check the timestamp of the files and call make_children to create
 *the new processes for the dependencies. We then call this function in main.
 *This function will call itself recursively.
 */

   //status is a local status of whether the command needs to be run or not.
   // 0 means run command, 1 means don't run command.
  int status = 0;

  //find index of target
  int t_index = find_target(TargetName, targets, nTargetCount);

  //if TargetName is in targets array...
  if (t_index != -1){
    //targets[t_index].Status = 0;
    if (targets[t_index].DependencyCount == 0 && targets[t_index].Status == 0) {
  	   status = 0;
    }
    else if(targets[t_index].DependencyCount == 0){
      status = 1;
    }
    
    int i;
    for (i = 0; i < targets[t_index].DependencyCount; i++) {
    	if (check_progress(targets[t_index].DependencyNames[i], targets, nTargetCount) != 1 ||
          compare_modification_time(targets[t_index].TargetName, targets[t_index].DependencyNames[i]) == -1 ||
          compare_modification_time(targets[t_index].TargetName, targets[t_index].DependencyNames[i]) == 2) {
            if (targets[find_target(targets[t_index].DependencyNames[i], targets, nTargetCount)].Status == 1){
      		    status = 0;
    	      }
            else {
              status = 1;
            }
      //   else{
      //     status = 1;
      //   }
      }
      // else {
      //   status = 1;
      // }
      // else {
      //   targets[t_index].Status = 1;
      // }
    }

    if (status == 0) {
      //int j;
      // for(j = 0; j < targets[t_index].DependencyCount; j++){
      //   printf("Recursive call for %s dependency\n", targets[t_index].DependencyNames[i]);
      //   check_progress(targets[t_index].DependencyNames[i], targets, nTargetCount);
      //   fork_to_run_command(targets, t_index);
      // }
    	fork_to_run_command(targets, t_index);
      //check_progress(targets[t_index].DependencyNames[i], targets, nTargetCount);
      targets[t_index].Status = 1;
      status = 1;
    }

    // if (status == 1) {
    //   targets[t_index].Status = 1;
    // }
  }
  //if TargetName is not in targets array...
  else {
    if (does_file_exist(TargetName) == -1) {
      printf("The file %s doesn't exist\n", TargetName);
      exit(-1);
    }
    return status;
  }

  //This is as far as I got [Hoang 2/14]

  return status;
}
