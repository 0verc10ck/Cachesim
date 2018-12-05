#include <stdio.h>

#include <unistd.h>

#include <stdlib.h>

#include <memory.h>

#include <sys/time.h>

#define BUF_SIZ 512

#define oops(a) { perror(a); exit(3);}

#define TRASH -842150451





typedef struct memory {

   int mode;

   int address;

   int data;

}memory;



typedef struct cache {

   int valid;

   int tag;

   int *data;

   int address;

}cache;



typedef struct address {

   int address;

   int index;

   int tag;

   int data;

   int byte_offset;

}address;



int makelog(int a)

{

   int cnt = 0;

   while (a != 1)

   {

      a = a / 2;

      cnt++;

   }

   return cnt;

}

int pow2(int a, int b)

{

   int i;

   int result = 1;

   for (i = 0; i < b; i++)

   {

      result *= a;

   }

   return result;

}

int main(int argc, char *argv[])

{

   FILE* read;

   int clock_rate;

   int mem_access_latency;

   int L1_size;

   int L1_line_size;

   char BUF[BUF_SIZ];

   int *mem;

   memory input;

   int i, j;

   char mode;

   cache *L1;

   address address_input;

   int index;

   int word;

   int address;

   int clock_cycle = 0;

   int total_count = 0;

   int miss_count = 0;

   struct timeval start_point,end_point;

   double operating_time;

   if (argc != 3)

   {

      oops("parameter error\n");

   }

   read = fopen(argv[1], "r");

   if (read == NULL)

   {

      oops("cachesim.conf open error\n");

   }

      fscanf(read, "%s %d", BUF, &clock_rate);

      fprintf(stdout, "%s %d\n", BUF, clock_rate);

      fscanf(read, "%s %d", BUF, &mem_access_latency);

      fprintf(stdout, "%s %d\n", BUF, mem_access_latency);

      fscanf(read, "%s %d", BUF, &L1_size);

      fprintf(stdout, "%s %d\n", BUF, L1_size);

      fscanf(read, "%s %d", BUF, &L1_line_size);

      fprintf(stdout, "%s %d\n", BUF, L1_line_size);

   fclose(read);

   mem = (int*)malloc(sizeof(int) * 64 * 1024 * 1024);

   for(i=0;i<64*1024*1024;i++)

   {

      mem[i]=0;

   }

   if(L1_size==0 || L1_line_size==0){

      read = fopen(argv[2], "r");

      while (!feof(read))

      {

         fscanf(read, "%c", &mode);

         if (mode == 'R')

         {

            fscanf(read, "%d", &input.address);

         }

         else

         {

            fscanf(read, "%d %d", &input.address, &input.data);

            mem[input.address / 4] = input.data;

         }



      }



   printf("Avg latency: %.2lf\n", (double)mem_access_latency / (double)clock_rate);

   fclose(read);

   exit(1);

   }

   gettimeofday(&start_point,NULL);

   index = L1_size / L1_line_size;

   L1 = (cache*)malloc(sizeof(cache) * index);

   word = L1_line_size / 4;

   for (i = 0; i < index; i++)

   {

      L1[i].valid = 0;

      L1[i].tag = 0;

      L1[i].data = (int*)malloc(sizeof(int)*word);

      for (j = 0; j < word; j++)

      {

         L1[i].data[j] = 0;

      }



   }

   //start here
   total_count = 0;

   read = fopen(argv[2], "r");

   while (!feof(read))

   {

      fscanf(read, "%c", &mode);

      fscanf(read, "%d", &address_input.address);

      address = address_input.address / 4; //2bit byte offset delete

      address_input.byte_offset = address & (word - 1); // data 써줄때 byte offset에 맞게 써주기

      address = address >> makelog(word); // address에서 byte offset word 만큼 다시 밀기

      address_input.index = address&(index - 1); // index 구하기

      address_input.tag = address / (index); // tag 구하기

      if(word==1)

         address=address_input.address/4;

      else

         address = address << makelog(word); // memory address 시작부분

      total_count=total_count+1;

      if (mode == 'R')

      {

         fscanf(read, "\n");

         if (L1[address_input.index].valid == 0)

         {

            miss_count++;

            clock_cycle += mem_access_latency;

            clock_cycle += 2;

            for (i = 0; i < word; i++)

            {

               L1[address_input.index].data[i] = mem[address + i * 4];

            }

            L1[address_input.index].valid = 1;

            L1[address_input.index].tag = address_input.tag;

            L1[address_input.index].address = address;

         }

         else

         {

            if (L1[address_input.index].tag == address_input.tag)

            {

               clock_cycle += 2;

               L1[address_input.index].valid = 1;

               L1[address_input.index].tag = address_input.tag;

               L1[address_input.index].address = address;

            }

            else {

               miss_count++;

               clock_cycle += 2;

               clock_cycle += mem_access_latency;

               clock_cycle += mem_access_latency;

               for (i = 0; i < word; i++)

               {

                  mem[L1[address_input.index].address + i * 4] = L1[address_input.index].data[i];

               }

               for (i = 0; i < word; i++)

               {

                  L1[address_input.index].data[i] = mem[address + i * 4];

               }

               L1[address_input.index].valid = 1;

               L1[address_input.index].tag = address_input.tag;

               L1[address_input.index].address = address;

            }

         }

      }

      else

      {
         fscanf(read, "%d\n", &address_input.data);

         if (L1[address_input.index].valid == 0)

         {

            if(word!=1)

            {

               clock_cycle += mem_access_latency;

               clock_cycle += 2;

               miss_count++;

               for (i = 0; i < word; i++)

               {

                  L1[address_input.index].data[i] = mem[address + i * 4];

               }

            }

            else

               clock_cycle+=2;

            L1[address_input.index].data[address_input.byte_offset] = address_input.data;

            L1[address_input.index].valid = 1;

            L1[address_input.index].tag = address_input.tag;

            L1[address_input.index].address = address;

         }

         else

         {

            if (L1[address_input.index].tag == address_input.tag)

            {

               clock_cycle += 2;

               L1[address_input.index].data[address_input.byte_offset] = address_input.data;

               L1[address_input.index].valid = 1;

               L1[address_input.index].tag = address_input.tag;

               L1[address_input.index].address = address;



            }

            else

            {

               miss_count++;

               clock_cycle += mem_access_latency;

               clock_cycle += 2;

               for (i = 0; i < word; i++)

               {

                  mem[L1[address_input.index].address + i * 4] = L1[address_input.index].data[i];

               }

               if(word!=1)

               {

                  clock_cycle+=mem_access_latency;

                  for (i = 0; i < word; i++)

                  {

                     L1[address_input.index].data[i] = mem[address + i * 4];

                  }

               }

               L1[address_input.index].data[address_input.byte_offset] = address_input.data;

               L1[address_input.index].valid = 1;

               L1[address_input.index].tag = address_input.tag;

               L1[address_input.index].address = address;

            }



         }



      }



   }


   printf("*L1 Cache Contents\n");

   for (i = 0; i < index; i++)

   {

      printf("%d:", i);

      for (j = 0; j < word; j++)

         printf("%08x ", L1[i].data[j]);

      printf("\n");

   }

   printf("\n");

   gettimeofday(&end_point,NULL);

   operating_time=(double)(end_point.tv_sec)+(double)(end_point.tv_usec)/1000000.0-(double)(start_point.tv_sec)-(double)(start_point.tv_usec)/1000000.0;



   fprintf(stdout, "Total program run time: %lf seconds\n", operating_time);

   fprintf(stdout, "L1 total access: %d\n", total_count);

   fprintf(stdout, "L1 total miss count: %d\n", miss_count);

   fprintf(stdout, "L1 miss rate: %.2lf%%\n", (double)miss_count / (double)total_count*100.0);

   fprintf(stdout, "Average memory access latency: %.2lf ns\n", ((double)clock_cycle / (double)total_count) / (double)clock_rate);



}

