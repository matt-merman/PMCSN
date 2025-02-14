#include "files.h"

/**
 * Opens file to read computed replica means
 * @param p Mode of operation: 'r' for read-only, 'w' for read-write
 * @return an array of file pointers with the finite-simulation results
 */
FILE	*open_file(char *p, const char *name)
{
    FILE	*file;

    char *ext, file_name[100];
    ext = ".csv";

    memset(file_name, '\0', sizeof(file_name));
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
    return (file);
}

void	write_result(FILE *file, double value, long index)
{
    char	value_str[100];
    char	time_str[100];

    sprintf(value_str, "%f", value);
    sprintf(time_str, "%ld", index);
    strcat(value_str, ",");
    strcat(value_str, time_str);
    strcat(value_str, "\n");

    fprintf(file, "%s", value_str);
}

void create_file_name(char *init_file_name, long int num, char file_name[100]){

    char num_to_str[100];

    sprintf(num_to_str, "%ld", num);
    strcpy(file_name, "");
    strcat(file_name, init_file_name);
    strcat(file_name, num_to_str);

}