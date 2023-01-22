#ifndef _FILES_H_
# define _FILES_H_

#include "structs.h"

FILE *open_file(char *p, const char *name);
void	write_result(FILE *file, double value, long index);
void create_file_name(char *init_file_name, long int num, char file_name[100]);

#endif