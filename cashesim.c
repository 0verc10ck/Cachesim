//include Header
#include <stdio.h>
#include <stdlib.h>

//struct
typedef struct memory
{
    char type;
    int address;
    int data;
}memory;

typedef struct setting
{
    int clock_rate;
    int mem_access_latency;
    int L1_size;
    int L1_line_size;
    int set_associativity;
}settting;

typedef struct cache {

   int valid;

   int tag;

   int *data;

   int address;

}cache;
//define function
void readconf(char **conf, char **argv);
void initmeme();
void readData(char **argv);

//global variable
int *mem;
setting set;
int main(int argc, char **argv)
{
	char **conf = NULL;
	readconf(conf, argv);
        initmeme();
        readData(argv);
}


/*read conf file and print it by stdout*/
void readconf(char **conf, char **argv)
{
	FILE *fi;
	int i = 0;
	int j = 0;
        int tmp[5];
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
                        fscanf(fi, "%s %d", conf[i], &tmp[i]);
			i++;
		}

		printf("input configure data is finished\n");
		for (i = 0; i <5; i++)
                        printf("%s %d\n", conf[i], tmp[i]);
	}

        set.clock_rate = tmp[0];
        set.mem_access_latency = tmp[1];
        set.L1_size = tmp[2];
        set.L1_line_size = tmp[3];
        set.set_associativity = tmp[4];
        fclose(fi);
}
/*malloc initiate 64mb size memory*/
void initmeme()
{
    mem = (int*)malloc(sizeof(int) * 64 * 1024 * 1024);

    for(int i = 0; i < 64 * 1024 * 1024; i++)
        mem[i] = 0;
}
/*read Data from file and save at memory*/
void readData(char **argv)
{
    FILE *fr;
    memory read;
    fr = fopen(argv[2], "r");
    while(!feof(fr))
    {
        fscanf(fr, "%c", &read.type);
        if(read.type == 'R')
        {
            fscanf(fr, "%d", &read.address);
        }

        else if(read.type == 'W')
        {
            fscanf(fr, "%d %d", &read.address, &read.data);

            mem[read.address / 4] = read.data;
        }
    }
    fclose(fr);

    printf("Average memory access latency without cache: %.2lf ns\n", (double)setting[1]/(double)setting[0]);
}
/*make L1*/
void L1()
{
    int line_num = set.L1_size/set.L1_line_size;
}
