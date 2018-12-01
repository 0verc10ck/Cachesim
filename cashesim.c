#include <stdio.h>
#include <stdlib.h>

void readconf(char **conf, char **argv);

int setting[5]; //clock_rate, mem_access_latency, L1_size, L1_line_size, int set_associativity

int main(int argc, char **argv)
{
	char **conf = NULL;
	readconf(conf, argv);
}


void readconf(char **conf, char **argv)
{
	FILE *fi;
	int i = 0;
	int j = 0;
	fi = fopen(argv[1], "r");

	if (fi == NULL)
	{
		printf("Cannot open %s\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	else
	{
		conf = (char**)malloc(sizeof(char*) * 5);
		for (i = 0; i < 5; i++)
			conf[i] = (char*)malloc(sizeof(char) * 50);

		i = 0;
		int j;
		while (!feof(fi))
		{
			fscanf(fi, "%s %d", conf[i], &setting[i]);
			i++;
		}

		printf("input configure data is finished\n");

		for (i = 0; i <5; i++)
			printf("%s %d\n", conf[i], setting[i]);
	}
}
