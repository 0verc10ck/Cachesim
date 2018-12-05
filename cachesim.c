//include Header
#include <stdio.h>
#include <stdlib.h>

//struct
typedef struct Memory
{
    char type;
    int address;
    int data;
}memory;

typedef struct Setting
{
    int clock_rate;
    int mem_access_latency;
    int L1_size;
    int L1_line_size;
    int set_associativity;
}setting;

typedef struct Cache {

   int valid;

   int tag;

   int *data;

   int address;

}cache;

typedef struct Address {

   int address;

   int index;

   int tag;

   int data;

   int byte_offset;

}address;

//define function
void readconf(char **conf, char **argv);
void initmem();
void readData(char **argv);
void init_cache();
void read_data_cache(char **argv);
void getdata(FILE *fi, memory *tmp);
int getmove(int word);
void Rtype(memory *tmp);
void Wtype(FILE *fi, memory *tmp);
void print_cache();
void print_info();

//global variable
int word;
int index = 0;
int miss =0;
int cycle = 0;
int count = 0;
int *mem;
setting set;
cache *L1_cache;
address add;

int main(int argc, char **argv)
{
	char **conf = NULL;
	readconf(conf, argv);
        initmem();
        printf("fin init mem\n");
        readData(argv);
        printf("fin read data");
        init_cache();
        printf("fin init");
        read_data_cache(argv);
        printf("fin read data");
        print_cache();
        printf("fin print cache");
        print_info();
        printf("fin printinfo");
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
void initmem()
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

    printf("Average memory access latency without cache: %.2lf ns\n", (double)set.mem_access_latency / (double)set.clock_rate);
}
/*allocate and initiate cahce memory*/
void init_cache()
{
    word = set.L1_line_size/ 4;
    index = set.L1_size/set.L1_line_size;
    L1_cache = (cache*)malloc(sizeof(cache) * index);
    for (int i = 0; i < index; i++)
    {

       L1_cache[i].valid = 0;

       L1_cache[i].tag = 0;

       L1_cache[i].data = (int*)malloc(sizeof(int)*word);

       for (int j = 0; j < word; j++)
           L1_cache[i].data[j] = 0;
    }
}
/*read and write data*/
void read_data_cache(char **argv)
{
    FILE *fi;
    memory tmp;

    fi = fopen(argv[2], "r");

    fclose(fi);

    while(!feof(fi))
    {
        getdata(fi, &tmp);
    }

}

void getdata(FILE *fi, memory *tmp)
{
    int move = 0;
    fscanf(fi, "%c", &tmp->type);
    fscanf(fi, "%d", &tmp->address);

    tmp->address /= 4;
    add.byte_offset = tmp->address & (word -1);
    move = getmove(word);
    tmp->address = tmp->address >> move;
    add.index = tmp->address&(index-1);
    add.tag = tmp->address/(index);

    if(word == 1)
        tmp->address = add.address/4;

    else
        tmp->address = tmp->address << getmove(word);

    count++;

    if(tmp->type == 'R')
        Rtype(tmp);
    else
        Wtype(fi, tmp);
}

int getmove(int word)
{

   int cnt = 0;

   while (word != 1)
   {
      word /= 2;
      cnt++;
   }

   return cnt;
}

void Rtype(memory *tmp)
{
    if(L1_cache[add.index].valid == 0)
    {
        miss++;
        cycle = set.mem_access_latency;
        cycle += 2;

        for(int i = 0; i < word; i++)
            L1_cache[add.index].data[i] = mem[tmp->address + i*4];

        L1_cache[add.index].address = tmp->address;
        L1_cache[add.index].tag = add.tag;
        L1_cache[add.index].valid = 1;
    }

    else
    {
        if(L1_cache[add.index].tag == add.tag)
        {
            cycle +=2;
            L1_cache[add.index].address = tmp->address;
            L1_cache[add.index].valid = 1;
            L1_cache[add.index].tag = add.tag;
        }

        else
        {
            miss++;
            cycle += 2;
            cycle += set.mem_access_latency;
            cycle += set.mem_access_latency;

            for(int i = 0; i < word; i++)
                mem[L1_cache[add.index].address + i * 4] = L1_cache[add.index].data[i];

            for(int i = 0; i < word; i++)
                L1_cache[add.index].data[i] = mem[tmp->address + i * 4];

            L1_cache[add.index].address = tmp->address;
            L1_cache[add.index].valid = 1;
            L1_cache[add.index].tag = add.tag;
        }
    }
}

void Wtype(FILE *fi, memory *tmp)
{
    fscanf(fi, "%d", &add.data);

    if(L1_cache[add.index].valid == 0)
    {
        if(word != 1)
        {
            cycle += 2;
            cycle += set.mem_access_latency;
            miss++;
            for(int i = 0; i < word; i++)
                L1_cache[add.index].data[i] = mem[tmp->address + i * 4];
        }

        else
            cycle += 2;

        L1_cache[add.index].address = tmp->address;
        L1_cache[add.index].data[add.byte_offset] = add.data;
        L1_cache[add.index].tag;
        L1_cache[add.index].data[add.byte_offset] = add.data;
    }

    else
    {
        if(L1_cache[add.index].tag == add.tag)
        {
            cycle += 2;
            L1_cache[add.index].address = tmp->address;
            L1_cache[add.index].data[add.byte_offset];
            L1_cache[add.index].valid =1;
            L1_cache[add.index].tag = add.tag;
        }

        else
        {
            miss++;
            cycle += set.mem_access_latency;
            cycle += 2;

            for(int i = 0; i < word; i++)
                mem[L1_cache[add.index].address + i * 4] = L1_cache[add.index].data[i];

            if(word != 1)
            {
                cycle += set.mem_access_latency;

                for(int i = 0; i < word; i++)
                    L1_cache[add.index].data[i] = mem[tmp->address + i * 4];
            }

            L1_cache[add.index].address = tmp->address;
            L1_cache[add.index].data[add.byte_offset] = add.data;
            L1_cache[add.index].valid = 1;
            L1_cache[add.index].tag = add.tag;
        }
    }
}

void print_cache()
{
    printf("*L1 Cache Contents\n");

    for(int i = 0; i < index; i++)
    {
        printf("%d : ", i);

        for(int j = 0; j < word; j++)
            printf("%08x ", L1_cache[i].data[j]);

        printf("\n");
    }
}

void print_info()
{
    //printf("Total program run time :");
    printf("L1 total access : %d\n", count);
    printf("L1 total miss count : %d\n", miss);
    printf("L1 miss rate : %.2lf%%\n", ((double)miss / (double)count) * 100.0);
    printf("Average memory access latency : %.2lf ns\n", (((double)cycle / (double)count)/(double)set.clock_rate));
}
