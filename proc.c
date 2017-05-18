#include <string.h>
#include <pwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include "proc.h"

#define MAX_SZ 	1024
#define TEST   	printf( "HERE at line[%d]\n", __LINE__); // macro used for debugging only

char *g_processName; // Global stores process name
char *g_userName; // Global stores user
gint g_processID; // Global store the pid
gfloat g_memory; // Global store the memory
gint g_procTick; // Global store prock tick
gint g_cpuTick; // Global store cpu tick
glong g_cpuPercent;
FILE *input;

gfloat getMemory(char *PID);
char * getProcName(char *filepath) ;
char * getUserName(char *PID) ;
gint getProcTicks(char *PID);
glong getCpuTicks();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Getting process PID */
void getPID(char *procdir){
	struct dirent *direntp;
	DIR *DIRp; // pointer to a directory
	char *cpyofPID;

	DIRp = opendir(procdir);

	while ((direntp = readdir(DIRp)) != NULL) { // go through proc and get all the process directories

		// Get only the numbered processes
		if (isdigit(direntp->d_name[0])) {
			cpyofPID = direntp -> d_name;
			g_processID = atoi(cpyofPID);
			//printf("PID is [%d]\n", g_processID);
			g_processName = getProcName(cpyofPID);
		}
	}

	closedir(DIRp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Extracting the name of the process */
char * getProcName(char *PID) {

	// Make new file path
	char filepath[MAX_SZ];
	strcpy(filepath, "/proc/");
	strcat(filepath, PID);
	strcat(filepath, "/stat");

	// Malloc the string so we can return it
	char *procName = (char *) malloc(sizeof(char)*MAX_SZ);

	// Open the stat file
	FILE *fp;
	char line[MAX_SZ];

	if ((fp = fopen(filepath,"r")) == NULL) {
		//printf("file [%s] couldn't be opened.", filepath);
	}
	else {
		// Find the string located between parens() and store it.
		if (fgets(line, MAX_SZ, fp) != NULL) {
			sscanf(line,"%*d" " %*[(]" "%99[^()']", procName);
		}
		//printf("\tProcess name is [%s]\n", procName);
		fprintf(input, "%s ", procName);
		g_userName = getUserName(PID);
	}

	fclose(fp);
	return procName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Get the username
char * getUserName(char *PID) {

	// Make new file path
	char filepath[MAX_SZ];
	strcpy(filepath, "/proc/");
	strcat(filepath, PID);
	strcat(filepath, "/");
	strcat(filepath, "status");

	// Open the status file
	FILE *fp;
	char line[MAX_SZ];
	struct passwd *user;

	if ((fp = fopen(filepath,"r")) == NULL) {
		//printf("file [%s] couldn't be opened.\n", filepath);
	}
	else {
		// Find the string located after Uid: and store it.
		char *pointerToWordUid;
		int uid;

		while (fgets(line, MAX_SZ, fp) != NULL) {
			pointerToWordUid = strstr(line, "Uid:");
			if ( NULL != pointerToWordUid ) {
				char ignore[MAX_SZ];
				sscanf(line,"%s\t %d",ignore, &uid);
			  user = getpwuid(uid);
				//printf("\t\tUser is [%s]\n", user->pw_name);
				fprintf(input, "%s ", user->pw_name);
				g_memory = getMemory(PID);
			}
		}
	}

	fclose(fp);
	return (user->pw_name);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gfloat getMemory(char *PID) {

	// Make new file path
	char filepath[MAX_SZ];
	strcpy(filepath, "/proc/");
	strcat(filepath, PID);
	strcat(filepath, "/");
	strcat(filepath, "statm");

	// Open the statm file
	FILE *fp;
	char line[MAX_SZ];
	int mem = 0;

	if ((fp = fopen(filepath,"r")) == NULL) {
		//printf("file [%s] couldn't be opened.\n", filepath);
	}
	else {
		// Find the 2 strings located after the first and store it.
		int rssmem = 0; int sharedmem = 0;

		if (fgets(line, MAX_SZ, fp) != NULL) {
			char ignore[MAX_SZ];
			sscanf(line,"%s %d %d", ignore, &rssmem, &sharedmem);
			mem = (rssmem - sharedmem)*4 ;
			//printf("\t\t\t MEMORY is [%d]\n", mem);
			g_procTick = getProcTicks(PID);
		}
	}

	fclose(fp);
	return (mem);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gint getProcTicks(char *PID) {

	// Make new file path
	char filepath[MAX_SZ];
	strcpy(filepath, "/proc/");
	strcat(filepath, PID);
	strcat(filepath, "/stat");

	// Open the statm file
	FILE *fp;
	char line[MAX_SZ];
	int procTicks = 0;

	if ((fp = fopen(filepath,"r")) == NULL) {
		//printf("file [%s] couldn't be opened.\n", filepath);
	}
	else {
		// Find the 2 strings located after the first and store it.
		int num1 = 0; int num2 = 0;

		if (fgets(line, MAX_SZ, fp) != NULL) {
			char ignore[MAX_SZ];
			sscanf(line,"%s %s %s %s %s %s %s %s %s %s %s %s %s %d %d",
			 	ignore, ignore, ignore, ignore, ignore, ignore, ignore,
				ignore, ignore, ignore, ignore, ignore, ignore, &num1, &num2);

			procTicks = num1 + num2 ;
			//printf("\t\t\t\tProcTicks is [%d]\n", procTicks);
			g_procTick = procTicks;
			g_cpuPercent = getCpuTicks();
			//printf("\t\t\t\tg_ProcTicks is [%lu]\n", g_cpuPercent);
		}
	}

	fclose(fp);
	return (procTicks);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

glong getCpuTicks() {

	// Make new file path
	char filepath[MAX_SZ];
	strcpy(filepath, "/proc");
	strcat(filepath, "/stat");

	// Open the statm file
	FILE *fp;
	char line[MAX_SZ];
	int cpuArray[10] = {0};

	if ((fp = fopen(filepath,"r")) == NULL) {
		//printf("file [%s] couldn't be opened.\n", filepath);
	}
	else {
		// Find and store all cpu numbers
		int j = 0;
		char ignore[MAX_SZ];

		for (; j < 1; j++) {
			fgets(line, MAX_SZ, fp);
			sscanf(line,"%s %d %d %d %d %d %d %d %d %d %d",
			ignore, &cpuArray[(j*10)+0], &cpuArray[(j*10)+1], &cpuArray[(j*10)+2],
			&cpuArray[(j*10)+3], &cpuArray[(j*10)+4], &cpuArray[(j*10)+5], &cpuArray[(j*10)+6],
			&cpuArray[(j*10)+7], &cpuArray[(j*10)+8], &cpuArray[(j*10)+9]) ;
			//printf("\t\t\t\tIGNORE is [%s]\n", ignore);
		}
	}
	int i;
	long total = 0;
	for ( i = 0; i < 10; i++) {
		long tmp = cpuArray[i];
		total += tmp;
		//printf("\t\t\t\t\t[%d]\n ", cpuArray[i]);
	}
	 glong cpuPercent = g_procTick/total;
	 fprintf(input, "%lu ", cpuPercent);
	 fprintf(input, "%d         ", g_processID);
	 fprintf(input, "%.1f\n", g_memory);
	fclose(fp);
	return (cpuPercent);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int proc() {

	char filename[] = "data.txt";
	// check the mode to see if you can read the file
    if ((input = fopen(filename, "w")) == NULL) {
        printf("File %s not found\n", filename);
        return 1;
    }
	char procdir[] = "/proc";
  //getCpuTicks();
	getPID(procdir);

	fclose(input);
	return 0;
} // EOproc
