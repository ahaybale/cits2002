#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
   char *days[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};

   int month = name2num(sizeof(months) / sizeof(months[0]), months, argv[1], "Error: invalid second argument (month)");
   int dates[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   int days_in_month = dates[month];

   // File reading
   // setup variables
   FILE *ptr;
   int line_length = 100;
   char str[line_length];

   // crontab-file reading
   //  create struct array for storing each line of crontab-file
   struct command_cron
   {
      int min;   // min (0 - 59)
      int hour;  // hour (0 - 23)
      int daym;  // day of month (1 - 31)
      int month; // month (0 - 11)
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

   // estimates-file reading
   //  for comments, see crontab-file
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
         int INT_MAX = 2147483647;
         estimates_data[lines].min = parse_int(array[1], 0, INT_MAX, "Error in mins in estimates-file");
         strcpy(estimates_data[lines].command, strcat(array[0], "\n"));
         lines++;
      }
   }
   fclose(ptr);

   // check that all commands in crontab-data are present in estimates-data
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

   // SIMULATION STATION
   // Setup tracking vars
   int total_sims = 0;
   int max_commands_freq = 0;
   char max_commands[40];

   // loop through all entries
   for (int i = 0; i < (sizeof(crontab_data) / sizeof(crontab_data[0])); i++)
   {
      // frequency counter
      int multiplyer = 0;
      // only select relevant month
      if (crontab_data[i].month == month || crontab_data[i].month == -1)
      {
         // both days wild
         if (crontab_data[i].daym == -1 && crontab_data[i].dayw == -1)
         {
            multiplyer += days_in_month;
         }
         // day m wild and dayw given
         if (crontab_data[i].daym == -1 && crontab_data[i].dayw != -1)
         {
            // calculate days passed in year so far
            int days_so_far = 0;
            for (int n = 0; n <= month; n++)
            {
               days_so_far += dates[n];
            }

            // figure out percentage of the week for the first, last, given day of the month.
            float first_day_of_month = fmod(days_so_far - dates[month], 7.0) / 7.0;
            float last_day_of_month = fmod(days_so_far, 7.0) / 7.0;
            float curr_day_week = (crontab_data[i].dayw + 1) / 7.0;

            // if in the first week of the month add 1
            if (first_day_of_month <= curr_day_week)
            {
               multiplyer += 1;
            }
            // if in the last week of the month add 1
            if (last_day_of_month >= curr_day_week)
            {
               multiplyer += 1;
            }

            // add 3 for the middle weeks
            multiplyer += 3;
         }

         // day m given and dayw wild
         if (crontab_data[i].daym != -1 && crontab_data[i].dayw == -1)
         {
            multiplyer += 1;
         }

         // both days given
         if (crontab_data[i].daym != -1 && crontab_data[i].dayw != -1)
         {
            multiplyer += 1;
         }

         // hours wild
         if (crontab_data[i].hour == -1)
         {
            multiplyer *= 24;
         }

         // mins wild
         if (crontab_data[i].min == -1)
         {
            multiplyer *= 60;
         }

         // add to trackers
         total_sims += multiplyer;
         if (max_commands_freq < multiplyer)
         {
            max_commands_freq = multiplyer;
            strcpy(max_commands, crontab_data[i].command);
         }
      }
   }
   // aby __________________________________

   // calculating minutes
   int sum;
   int remainder;
   int mins;
   for (int n = 0; n < (sizeof(crontab_data) / sizeof(crontab_data[0])); n++)
   {
      for (int d = 0; d < (sizeof(estimates_data) / sizeof(estimates_data[0])); d++)
      {
         if (strcmp(estimates_data[n].command, crontab_data[d].command)==0)
         {
            //{printf("%s\n", estimates_data[n].command);}
            if (estimates_data[d].min > 60)
            { // checks whether the estimated time is greater than 60 minutes
               mins = estimates_data[d].min % 60;
            } // finds the amount of minutes (remainder)
            else
            {
               mins = estimates_data[d].min;
            } // if less than an hour, mins = estimated time
            sum = crontab_data[n].min + mins;
            if (sum >= 0 && sum < 60)
            {
               printf("%d\n", sum);
            }
            else
            {
               remainder = sum - 60;
               printf("%d\n", remainder);
            }
         }
      }
   }

   // calculating hours
   int sumhours;
   int addhours;
   for (int n = 0; n < (sizeof(estimates_data) / sizeof(estimates_data[0])); n++)
   {
      for (int d = 0; d < (sizeof(estimates_data) / sizeof(estimates_data[0])); d++)
      {
         if (strcmp(estimates_data[n].command, crontab_data[d].command)==0)
         {
            if (estimates_data[n].min > 60)
            {
               addhours = estimates_data[n].min / 60;
            } // find the amount of estimated hours command will run for
            else
            {
               addhours = 0;
            }
            if ((sumhours = addhours + crontab_data[d].hour) <= 23)
            { // checks if command will end the following day
               printf("%d\n", sumhours);
            }
            else
            {
               printf("%d\n", sumhours - 24); // provides the time command will end
            }
         }
      }
   }

   // calculating day of week
   for (int n = 0; n < (sizeof(estimates_data) / sizeof(estimates_data[0])); n++)
   {
      for (int c = 0; c < (sizeof(crontab_data) / sizeof(crontab_data[0])); c++)
      {
         if (strcmp(estimates_data[n].command, crontab_data[c].command)==0)
         {
            if (sumhours > 23)
            {
               if ((crontab_data[c].dayw + sumhours % 24) > 6)
               {
                  printf("%d\n", (crontab_data[c].dayw + sumhours % 24) % 6);
               }
               else
               {
                  printf("%d\n", crontab_data[c].dayw + sumhours % 24);
               }
            }
         }
      }
   }

   // calculating day of the month
   for (int n = 0; n < (sizeof(estimates_data) / sizeof(estimates_data[0])); n++)
   {
      for (int c = 0; c < (sizeof(crontab_data) / sizeof(crontab_data[0])); c++)
      {
         if (strcmp(estimates_data[n].command, crontab_data[c].command)==0)
         {
            if (sumhours > 23)
            {
               if ((crontab_data[c].daym + sumhours % 24) >= dates[month])
               {
                  printf("%d\n", dates[month]);
               }
               else
               {
                  printf("%d\n", crontab_data[c].daym + sumhours % 24);
               }
            }
         }
      }
   }

   // debug OUTPUTS ___________________________________________________________________________________________________
   int debug = 0;
   if (debug)
   {
      printf("Crontab entries\n");
      for (int i = 0; i < sizeof(crontab_data) / sizeof(crontab_data[0]); i++)
      {
         printf("%d ", crontab_data[i].min);
         printf("%d ", crontab_data[i].hour);
         printf("%d ", crontab_data[i].daym);
         printf("%d ", crontab_data[i].month);
         printf("%d ", crontab_data[i].dayw);
         printf("%s", crontab_data[i].command);
      }

      printf("\nEstimates entries\n");
      for (int i = 0; i < sizeof(estimates_data) / sizeof(estimates_data[0]); i++)
      {
         printf("%d ", estimates_data[i].min);
         printf("%s", crontab_data[i].command);
      }
      printf("\n");
   }

   // Required outputs
   printf("%s", max_commands);
   printf("%d\n", total_sims);
   printf("%s\n", "MAX FREQ");

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
            return n;
         }
      }
   }
   // number handling
   if (atoi(name) <= size && atoi(name) >= 0)
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
