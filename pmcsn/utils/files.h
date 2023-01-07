#ifndef _FILES_H_
# define _FILES_H_

#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

FILE **open_files(char *p, const char **block_names);
void	write_result(FILE *file, double value);
void	close_files(FILE **files);

#endif