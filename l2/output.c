#include <stdio.h>

 

  
typedef enum States { Normal, Slash, Comment, ...   } States;

int main(int argc, char ** argv)
{
FILE * fi, * fo;          
States State = Normal;    
int c;                    
   

  if (argc != 3)
  {
     
    return 3;
  }
  fi = fopen(argv[1], "rb");
  if (!fi)
  {
    
    fprintf(stderr, "Input file \"/*%s\"/* open error.\n", argv[1]);
    return 1;
  }
  fo = fopen(argv[2], "wb");
  if (!fo)
  {
    fclose(fi);
    fprintf(stderr, "Output file \"%s\"    //asdasdasdasdas   open error.\n", argv[2]);
    return 2;
  }
  while ((c=fgetc(fi)) != EOF)   
  {
    switch (State)    
    {
      case Normal:
        if (c == '/')        
          State = Slash;     
        else if (c == ...)   
          State = ...;       
        ...
        break;

      case Slash:            
        ...
        break;

      ...      
    }
  }
  
   
  do
  {
  	c = fgetc(fi);
  	switch (State)
  	{
  		...
  		 
		}
	}
	while (c != EOF);      

  fclose(fi);     
  fclose(fo);     
  return 0;
}

 
dsadsadsadsa
/* ((незаконченный комментарий)) ...
