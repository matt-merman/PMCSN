#include "files.h"

// returns a .txt file list to fill with the simulation's results
FILE	**open_files(char *p)
{
	FILE	**files;
	int		i;
     char *path, *ext, file_name[100];
	
     char *names[] = {"Primi", "Secondi e Contorni", "Frutta_Dessert", "Casse_Fast", "Casse_standard", "Locale_Mensa"};
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
		strcat(file_name, names[i]);
		strcat(file_name, ext);
		
		if(strcmp(p, "w") == 0)
			remove(file_name);
		
		files[i] = fopen(file_name, p);
	}
	return (files);
}

void	write_result(FILE *file, double value)
{
	char	value_str[100];

	sprintf(value_str, "%f", value);
	strcat(value_str, "\n");
	fprintf(file, "%s", value_str);
	return ;
}

void	close_files(FILE **files)
{
	int i;

	for (i = 0; i < BLOCKS; i++)
		fclose(files[i]);

	free(files);
}