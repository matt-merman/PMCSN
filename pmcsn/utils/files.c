#include <unistd.h>
#include "files.h"

/**
 * Opens file to read computed replica means
 * @param p Mode of operation: 'r' for read-only, 'w' for read-write
 * @return an array of file pointers with the finite-simulation results
 */
FILE **open_files(char *p, const char **block_names)
{
	FILE	**files;
	int		i;
    char *path, *ext, file_name[100];

	// We suppose that the current directory is pmcsn/
    path = "./result/";
	ext = ".txt";
	files = (FILE **)malloc(BLOCKS * sizeof(FILE *));
	if (files == NULL)
	{
		printf("Error on file list allocation\n");
		return (NULL);
	}
	for (i = 0; i < BLOCKS; i++)
	{
		memset(file_name, '\0', sizeof(file_name));
		files[i] = (FILE *)malloc(sizeof(FILE));
		if (files[i] == NULL)
		{
			printf("Error on file allocation\n");
			return (NULL);
		}
		strcat(file_name, path);
		strcat(file_name, block_names[i]);
		strcat(file_name, ext);
        // removes the file if already exists. We have to write in it
		if(strcmp(p, "w") == 0)
			remove(file_name);
		
		files[i] = fopen(file_name, p);
        if (files[i] == NULL){
            printf("File %s not found. Make sure to run the program from pmcsn/ folder", file_name);
            exit(-1);
        }
	}
	return (files);
}

void	write_result(FILE *file, double value)
{
	char	value_str[100];

	sprintf(value_str, "%f", value);
	strcat(value_str, "\n");
	fprintf(file, "%s", value_str);
}

void	close_files(FILE **files)
{
	int i;

	for (i = 0; i < BLOCKS; i++){
        if (files[i] != NULL){
            fclose(files[i]);
        }
    }

	free(files);
}