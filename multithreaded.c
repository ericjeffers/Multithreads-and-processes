/*
Eric Jeffers
CSC 565
Assignment 2
Multithreaded
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <pthread.h>

int *numbers;
char *buffer;
int length;

//Function for threads
void *CheckSum(void *i) {
	int tid, start, j;
	char checkSum;

	tid = *(int *)i;
	start = numbers[tid];
	checkSum = buffer[start];

	for (j = (start + 1); j < (start + length); ++j) {
		checkSum ^= buffer[j];
		}

	numbers[tid] = checkSum;
	return NULL;
}

//Main function
int main(int argc, char* argv[]) {
	int fd, fileSize, i, rc;
	char checkSum;
	FILE *fp;
	pthread_t *threads;

	//Determine length of file
	fp = fopen (argv [1], "rb");
	fseek(fp, 0L, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	//create arrays thorough memory allocation
	int threadCount = atoi(argv[2]);
	numbers = malloc (threadCount * sizeof (numbers[0]));
	buffer = malloc (fileSize * sizeof (buffer[0]));
	threads = malloc (threadCount * sizeof (threads[0]));

	length = fileSize / threadCount;
	for (i = 1; i < (threadCount); ++i) {
		numbers[i] = ((i * length) + fileSize % threadCount);
	}

	fread(buffer, 1, fileSize, fp);
	//Create threads equal to threadCount
	for (i = 1; i < threadCount; ++i) {
		pthread_create(&threads[i], NULL, CheckSum, &i);
		pthread_join(threads[i], NULL);
	}

	//run the Main portion of the CheckSum
	checkSum = buffer[0];
	for (i = 1; i < (length + fileSize % threadCount); ++i) {
		checkSum ^= buffer[i];
		}

	//CheckSum all the results
	for (i = 1; i < threadCount; ++i)
		checkSum ^= numbers[i];

	//One's complement negative results
	int finalValue;
	if (checkSum < 0)
		finalValue = (int)checkSum + 256;
	else
		finalValue = checkSum;

	printf("The Checksum is %d!\n", finalValue);

	//Cleanup
	fclose(fp);
	free(numbers);
	free(buffer);
	free(threads);
 
	return (0);
}
