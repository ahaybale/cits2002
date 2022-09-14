#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int name2num(int size, char *names[], char *name, char *error);
int count_command_lines(char *filename);
int parse_int(char *text, int min, int max, char *error);

int main(int argc, char *argv[])
{
// Input validation
   // validate number of args
   if (argc != 4)
   {
      fprintf(stderr, "Usage: %s month crontab-file estimates-file\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   // validate month arg length
   if (strlen(argv[1]) > 3)
   {
      fprintf(stderr, "Error1: invalid second argument (month); must be either an integer 1..12, or a 3-character name");
      exit(EXIT_FAILURE);
   }

// Month Selection
   char *months[] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
   char *days[] = {"mon", "tue", "wed", "thu", "fri", "sat", "sun"};

   int month = name2num(sizeof(months) / sizeof(months[0]), months, argv[1], "Error: invalid second argument (month)");

// File reading
   //setup variables
   FILE *ptr;
   int line_length = 100;
   char str[line_length];

   //crontab-file reading
   // create struct array for storing each line of crontab-file
   struct command_cron
   {
      int min;   // min (0 - 59)
      int hour;  // hour (0 - 23)
      int daym;  // day of month (1 - 31)
      int month; // month (1 - 12)
      int dayw;  // day of week (0 - 6)
      char command[40];
   } crontab_data[count_command_lines(argv[2])];

   // Open file and handle errors
   ptr = fopen(argv[2], "r");
   if (NULL == ptr)
   {
      fprintf(stderr, "Error: file can't be opened \n");
      exit(EXIT_FAILURE);
   }

   // go through each command line
   int lines = 0;
   while (fgets(str, line_length, ptr) != NULL)
   {
      if (str[0] != '#')
      {
         // split each command line at each space and add to array
         char *array[6];
         int i = 0;
         char *p = strtok(str, " ");
         while (p != NULL)
         {
            array[i++] = p;
            p = strtok(NULL, " ");
         }

         // add data from array to struct
         crontab_data[lines].min = parse_int(array[0], 0, 59, "Error in mins in crontab-file");
         crontab_data[lines].hour = parse_int(array[1], 0, 23, "Error in hours in crontab-file");
         crontab_data[lines].daym = parse_int(array[2], 1, 31, "Error in day of month in crontab-file");
         crontab_data[lines].month = name2num(sizeof(months) / sizeof(months[0]), months, array[3], "Error in months in crontab-file");
         crontab_data[lines].dayw = name2num(sizeof(days) / sizeof(days[0]), days, array[4], "Error in day of week in crontab-file");
         strcpy(crontab_data[lines].command, array[5]);
         lines++;
      }
   }
   fclose(ptr);

   //estimates-file reading
   // for comments, see crontab-file
   struct command_estimates
   {
      int min; // min (0 - 59)
      char command[40];
   } estimates_data[count_command_lines(argv[3])];

   ptr = fopen(argv[3], "r");
   if (NULL == ptr)
   {
      fprintf(stderr, "Error: file can't be opened \n");
      exit(EXIT_FAILURE);
   }

   lines = 0;
   while (fgets(str, line_length, ptr) != NULL)
   {
      if (str[0] != '#')
      {
         char *array[2];
         int i = 0;
         char *p = strtok(str, " ");
         while (p != NULL)
         {
            array[i++] = p;
            p = strtok(NULL, " ");
         }
         int INT_MAX =2147483647;
         estimates_data[lines].min = parse_int(array[1], 0, INT_MAX, "Error in mins in estimates-file");
         strcpy(estimates_data[lines].command, strcat(array[0], "\n"));
         lines++;
      }
   }
   fclose(ptr);

   //check that all commands in crontab-data are present in estimates-data
   char *commands[(sizeof(estimates_data) / sizeof(estimates_data[0]))];
   for (int i = 0; i < (sizeof(estimates_data) / sizeof(estimates_data[0])); i++)
   {
      commands[i] = estimates_data[i].command;
   }

   for (int i = 0; i < (sizeof(crontab_data) / sizeof(crontab_data[0])); i++)
   {
      int found = 0;
      for (int n = 0; n < (sizeof(commands) / sizeof(commands[0])); n++)
      {
         if (strcmp(crontab_data[i].command, commands[n]) == 0)
         {
            found = 1;
         }
      }
      if (!found)
      {
         fprintf(stderr, "%s", "Commands from crontab-file are missing from estimates-file");
         exit(EXIT_FAILURE);
      }
   }

   // print out everything that we have
   printf("Crontab-file data\n");
   for (int i = 0; i < (sizeof(crontab_data) / sizeof(crontab_data[0])); i++)
   {
      printf("%d ", crontab_data[i].min);
      printf("%d ", crontab_data[i].hour);
      printf("%d ", crontab_data[i].daym);
      printf("%d ", crontab_data[i].month);
      printf("%d ", crontab_data[i].dayw);
      printf("%s", crontab_data[i].command);
   }
   printf("\nEstimates-file data\n");
   for (int i = 0; i < (sizeof(estimates_data) / sizeof(estimates_data[0])); i++)
   {
      printf("%d ", estimates_data[i].min);
      printf("%s", estimates_data[i].command);
   }

   printf("\nMonth: %d\n", month);

   // the name of the most frequently executed command (a single word),
   //  the total number of commands invoked (a non-negative integer),
   //  and the maximum number of commands running at any time (a non-negative integer).

   return 0;
}

int name2num(int size, char *names[], char *name, char *error)
{
   // wildcard handling
   if (strcmp("*", name) == 0)
   {
      return -1;
   }
   // name handling
   if (atoi(name) == 0)
   {
      for (int n = 0; n < size; n++)
      {
         if (strcmp(name, names[n]) == 0)
         {
            return n + 1;
         }
      }
   }
   // number handling
   if (atoi(name) <= size && atoi(name) >= 1)
   {
      return atoi(name);
   }

   fprintf(stderr, "%s", error);
   exit(EXIT_FAILURE);
}

int count_command_lines(char *filename)
{
   FILE *ptr = fopen(filename, "r");
   int line_length = 100;
   char str[line_length];

   if (NULL == ptr)
   {
      fprintf(stderr, "Error: file can't be opened \n");
      exit(EXIT_FAILURE);
   }

   int lines = 0;
   while (fgets(str, line_length, ptr) != NULL)
   {
      if (str[0] != '#')
      {
         lines++;
      }
   }
   fclose(ptr);
   return lines;
}

int parse_int(char *text, int min, int max, char *error)
{
   if (strcmp("*", text) == 0)
   {
      return -1;
   }

   if (strcmp(text, "0") == 0)
   {
      return 0;
   }

   if (atoi(text) != 0)
   {
      if (atoi(text) <= max && atoi(text) >= min)
      {
         return atoi(text);
      }
   }

   fprintf(stderr, "%s", error);
   exit(EXIT_FAILURE);
}
