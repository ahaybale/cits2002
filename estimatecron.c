
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



// int64_t milliseconds(void)
// {
//     struct timeval  now;

//     gettimeofday( &now, NULL );       // timezone not required, so we pass NULL
//     return ((int64_t)now.tv_sec * 1000) + now.tv_usec/1000;
// }

// void readfile(char filename[], int buffersize)
// {
//     int     ncalls  = 0;
//     int64_t start   = milliseconds();

//     int fd  = open(filename, O_RDONLY);
//     ++ncalls;

//     if(fd >= 0) {
//         char buffer[buffersize];
//         size_t  got;

//         while((got = read(fd, buffer, sizeof(buffer))) > 0) {
//             ++ncalls;
//         }
//         close(fd);
//         ++ncalls;
//     }

//     printf("%i\t%i\n", buffersize, (int)(milliseconds() - start));
// //    printf("%i\t%i\n", ncalls, (int)(milliseconds() - start));
// }


int main(int argc, char *argv[])
{
   // check arguments
   if (argc != 4)
   {
      fprintf(stderr, "Usage: %s month crontab-file estimates-file\n", argv[0]);
      exit(EXIT_FAILURE);

   }

   // convert month from argv to number
   // check input length
   if (strlen(argv[1]) > 3)
   {
      fprintf(stderr, "Error1: invalid second argument (month); must be either an integer 0..11, or a 3-character name");
      exit(EXIT_FAILURE);

   }
   int month = -1;
   char *months[] = {"jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};

   // if not a number check month array
   if (atoi(argv[1]) == 0)
   {
      for (int n = 0; n < 12; n++)
      {
         if (strcmp(months[n], argv[1]) == 0)
         {
            month = n;
            break;
         }
      }
      // if not found, error
      if (month == -1)
      {
         fprintf(stderr, "Error: invalid second argument (month); 3-character month name not found");
         exit(EXIT_FAILURE);

      }
   }
   // if number given
   else
   {
      // convert num if in range, else error
      if (atoi(argv[1]) < 12 && atoi(argv[1]) >= 0)
      {
         month = atoi(argv[1]);
         exit(EXIT_FAILURE);
}
      else
      {
         fprintf(stderr, "Error: invalid second argument (month); %s out of range must be between 0 and 11", argv[1]);
         exit(EXIT_FAILURE);
}
   }
   printf("Month argument: %d\n", month);

  

   // TODO: STORE PRINTED LINE in array !!!!!!!!!!!!!!!!!!!!!!!, then split into 2d/3d array
   FILE* ptr;
   int line_length=100;
   char str[line_length];
   ptr = fopen(argv[2], "r");
 
   if (NULL == ptr) {
      fprintf(stderr, "Error: file can't be opened \n");
      exit(EXIT_FAILURE);
}

   while (fgets(str, line_length, ptr) != NULL) {
      if (str[0]!='#')
      printf("%s", str);
   }

   fclose(ptr);
   
   return 0;
}
