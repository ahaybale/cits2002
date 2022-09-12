#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
         fprintf(stderr, "Error1: invalid second argument (month); must be either an integer 0..11, or a 3-character name");
         exit(EXIT_FAILURE);
      }

// Month Selection
   int month = -1;

   // if name given: Validate and convert month names to num
   if (atoi(argv[1]) == 0 && strcmp(argv[1], "0") != 0)
   {
      // go through each month and if the arg is that, then set the num
      char *months[] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};
      for (int n = 0; n < 12; n++)
      {
         if (strcmp(months[n], argv[1]) == 0)
         {
            month = n + 1;
         }
      }
      //if month not found: error
      if (month == -1)
      {
         fprintf(stderr, "Error: invalid second argument (month); 3-character month name not found");
         exit(EXIT_FAILURE);
      }
   }
   // else (num given): Validate and set month num
   else
   {
      //if month in range, set it
      if (atoi(argv[1]) <= 12 && atoi(argv[1]) >= 1)
      {
         month = atoi(argv[1]);
      }
      //else error
      else
      {
         fprintf(stderr, "Error: invalid second argument (month); %s out of range must be between 0 and 11", argv[1]);
         exit(EXIT_FAILURE);
      }
   }

// Crontab-file reading
   // Setup variables for reading file
   FILE *ptr;
   int line_length = 100;
   char str[line_length];

   // Open file and handle errors
   ptr = fopen(argv[2], "r");
   if (NULL == ptr)
   {
      fprintf(stderr, "Error: file can't be opened \n");
      exit(EXIT_FAILURE);
   }

   // count non-comment lines
   int lines = 0;
   while (fgets(str, line_length, ptr) != NULL)
   {
      if (str[0] != '#')
      {
         lines++;
      }
   }

   // MAKE THIS BETTER!!!
   fclose(ptr);
   ptr = fopen(argv[2], "r");

   // create struct array for storing each line of crontab-file
   struct command_cron
   {
      int min;   // min (0 - 59)
      int hour;  // hour (0 - 23)
      int daym;  // day of month (1 - 31)
      int month; // month (1 - 12)
      int dayw;  // day of week (0 - 6)
      char command[40];
   } crontab_data[lines];

   lines = 0;
   while (fgets(str, line_length, ptr) != NULL)
   {
      if (str[0] != '#')
      {
         // split each command line at each space and populate crontab_data
         char *array[6];
         int i = 0;
         char *p = strtok(str, " ");

         while (p != NULL)
         {
            array[i++] = p;
            p = strtok(NULL, " ");
         }

         crontab_data[lines].min = atoi(array[0]);
         crontab_data[lines].hour = atoi(array[1]);
         crontab_data[lines].daym = atoi(array[2]);
         crontab_data[lines].month = atoi(array[3]);
         crontab_data[lines].dayw = atoi(array[4]);
         strcpy(crontab_data[lines].command, array[5]);
         lines++;
      }
   }
   fclose(ptr);

// print out everything that we need
   for (int i = 0; i < lines; i++)
   {
      printf("%d ", crontab_data[i].min);
      printf("%d ", crontab_data[i].hour);
      printf("%d ", crontab_data[i].daym);
      printf("%d ", crontab_data[i].month);
      printf("%d ", crontab_data[i].dayw);
      printf("%s", crontab_data[i].command);
   }

   printf("\nMonth: %d\n", month);
   printf("commands: %lu\n", (sizeof(crontab_data) / sizeof(crontab_data[0])));

   return 0;
}
