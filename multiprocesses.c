/*
Eric Jeffers
CSC 565
Assignment 2
Multiprocesses
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {	
	int fd, fileSize, i, j, n, status;
	char checkSum;
	int *numbers;
	char *buffer;
	FILE *fp;

	//Determine length of file
	fp = fopen (argv [1], "rb");
	fseek(fp, 0L, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	//Read in number of processes and allocate memory for the individual checksums and the buffer
	int processCount = atoi(argv[2]);
	numbers = malloc (processCount * sizeof (numbers[0]));
	buffer = malloc (fileSize * sizeof (buffer[0]));

	//Set the divided length of the whole file each process must checksum (array[0] will hold the extra modulo)
	for (i = 1; i < (processCount); ++i) {
		numbers[i] = fileSize / processCount;
	}
	numbers[0] = fileSize / processCount + fileSize % processCount; //holds the modulo

	fclose(fp);

	n=0;
	i = 0;
	pid_t pid;

	//Create children processes
	while (i < (processCount - 1)) {
		i++;
		pid = fork();
		if (pid == 0) {
			fp = fopen (argv [1], "rb");
			fd = ((fileSize / processCount * (i)) + (fileSize % processCount));
			fread(buffer, 1, fileSize, fp);
			checkSum = buffer[fd];
			for (j = fd+1; j < fd+(fileSize/processCount); ++j)
				checkSum ^= buffer[j];
			fclose(fp);
			exit(checkSum);
		}
		else {
			++n;
			waitpid(-1, &status, 0);
			numbers[n] = WEXITSTATUS(status);
		}
	}

	//Run the main portion of the CheckSum
	fp = fopen (argv [1], "rb");
	fread(buffer, 1, fileSize, fp);
	checkSum = buffer[0];
	for (i = 1; i < (fileSize/processCount + fileSize%processCount); ++i) {
		checkSum ^= buffer[i];
	}

	fclose(fp);

	//CheckSum all the results
	for (i = 1; i < processCount; ++i)
		checkSum ^= numbers[i];

	//One's complement negative results
	int finalValue;
	if (checkSum < 0)
		finalValue = (int)checkSum + 256;
	else
		finalValue = checkSum;
	printf("The CheckSum is %i!\n", finalValue);

	//Cleanup
	free(numbers);
	free(buffer);
 
	return (0);
}
