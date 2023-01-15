#include "files.h"
#include <unistd.h>

/**
 * Opens file to read computed replica means
 * @param p Mode of operation: 'r' for read-only, 'w' for read-write
 * @return an array of file pointers with the finite-simulation results
 */
FILE	*open_file(char *p, const char *name)
{
    FILE	*file;
    int		i;

    char *path, *ext, file_name[100];
    // We suppose that the current directory is pmcsn/
    path = "./result/";
    ext = ".csv";

    memset(file_name, '\0', sizeof(file_name));

    strcat(file_name, path);
    strcat(file_name, name);
    strcat(file_name, ext);
    // removes the file if already exists. We have to write in it
    if (strcmp(p, "w") == 0)
        remove(file_name);
    file = fopen(file_name, p);
    if (file == NULL)
    {
        printf("File %s not found. Make sure to run the program from pmcsn/ folder", file_name);
        exit(-1);
    }
    fprintf(file, "%s", "grt,replica\n");
    return (file);
}

void	write_result(FILE *file, double value, double time)
{
    char	value_str[100];
    char	time_str[100];

    sprintf(value_str, "%f", value);
    sprintf(time_str, "%f", time);

    strcat(value_str, ",");
    strcat(value_str, time_str);
    strcat(value_str, "\n");

    fprintf(file, "%s", value_str);
}

void	close_files(FILE **files)
{
    int i;

    for (i = 0; i < BLOCKS; i++)
    {
        if (files[i] != NULL)
        {
            fclose(files[i]);
        }
    }

    free(files);
}