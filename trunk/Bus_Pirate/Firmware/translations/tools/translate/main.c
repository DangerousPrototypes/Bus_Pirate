//
//
//
//
//  Convertion Utility:
//  Purpose:  Convert textfile to assembly format.
//
//  Created by: James L. Santos
//
//
//
//
//
//  Copyright (c) 2010  http://www.dangerousprototypes.com by Ian Lesnet
//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LEN 256     // max dos path
#define MAXCHAR  512

//replace ch1 with ch2 in stringline
char *replace(char *stringline, char ch1, char ch2)
{
   char *buf;
   int i=0,j=0;
   char temp[MAXCHAR];

    while(stringline[i] !='\0'){
          if(stringline[i] == ch1 ){
               temp[j] = ch2;
               i++;
               j++;
          }
           else
             temp[j++] = stringline[i++];
     }
     temp[j] = '\0';
     buf = (char *) malloc(strlen(temp) + 1);
     strcpy(buf,temp);
     return buf;
}
// remove cr-lf and extra space
char *chomp(char *raw_text){
    const char *invalid_characters = "\n\r";
    int len,j=0;
    char stripline[MAXCHAR];
    len=strlen(raw_text);
    char *c=raw_text;
  while (*c)
   {
       if (!strchr(invalid_characters, *c))
       {

               if (*c==0x20)
               {
                   if(stripline[j-1]==0x20) //if previous char is space
                          //don't write it anymore
                          j--;
               }

               stripline[j++]=*c;


       }

       c++;
   }

    //remove extra space at end of line
    if (stripline[j-1]==0x20)
        stripline[j-1]='\0';
           else
        stripline[j]='\0';

    strcpy(raw_text,stripline);

   return raw_text;
    }

char *replace_ext(char *fname,char *ext)
{
        char szPath[LEN];
        strcpy(szPath,fname);
        char *pFile = strrchr(szPath, '/');
        pFile = pFile == NULL ? szPath : pFile+1;
        char *pExt = strrchr(pFile, '.');

        if (pExt != NULL)
            strcpy(pExt, ext);
         else
            strcat(pFile, ext);

         return pFile;
}




int main(int argc, char *argv[])

{

    FILE  *infile, *outsfile, *outhfile;
    char hname[LEN],sname[LEN];    // storage for output filename
    char buffer1[MAXCHAR],buffer2[MAXCHAR],stringdata[MAXCHAR],temp[MAXCHAR];
    char *token;
    char column[3][MAXCHAR];
    int i,k,len;
    printf("\n\n TEXT to Assembly Translator\n");
    printf(" http://www.dangerousprototypes.com\n\n");
    if (argc < 2)  {
         fprintf(stderr, "Usage: %s filename.txt\n", argv[0]);
         exit(-1);
    }


    if ((infile= fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Error Opening Input file \"%s\"\n",argv[1]);
        exit(-1);
    }

     // set up output
    // look for a '.' and replace it with .s and .h



    strcpy(sname,replace_ext(argv[1],".s"));

    if ((outsfile = fopen(sname, "w")) == NULL) {
        fprintf(stderr,"Can't create output file.\n");
        exit(-1);
    }

  strcpy(hname,replace_ext(argv[1],".h"));
    if ((outhfile = fopen(hname, "w")) == NULL) {
        fprintf(stderr,"Can't create output file.\n");
        exit(-1);
    }


// read line by line and parse
    k=0;
     strcpy(buffer2,"#ifdef LANGUAGE_EN_US\n\n");

    fputs(buffer2, outsfile);
    fputs(buffer2, outhfile);
    strcpy(buffer2,".global _bpmessages\n_bpmessages:");
    while (feof(infile) == 0)  {
            fgets(stringdata, sizeof(stringdata), infile);	// Read line by line

           token = strtok(stringdata,";");

           i=0;
           while( token != NULL) {
               strcpy(temp,token);
               strcpy(column[i++],temp);

               token = strtok(NULL,";");
            }
            if (i>2) {  // we got 3 columns
               if (strcmp(column[1],"0")==0){

                    strcpy(buffer1,"#define\t");

                    strcat(buffer1,chomp(column[0]));
                    strcat(buffer1,"\tbpMsg(");
                    itoa(k, temp, 10);
                    strcat(buffer1,temp);
                    strcat(buffer1,", ");

               }
               else {

                     strcpy(buffer1,"#define\t");

                    strcat(buffer1,chomp(column[0]));
                    strcat(buffer1,"\tbpMsgBR(");
                    itoa(k, temp, 10);
                    strcat(buffer1,temp);
                    strcat(buffer1,", ");

               }

                   strcat(buffer2,"\n\t.pascii\t");

                   strcat(buffer2,chomp(column[2]));


                     fputs(buffer2, outsfile);

                   strcpy(temp,chomp(column[2]));

                   strcpy(temp,replace(temp, '\r',' '));
                   strcpy(temp,replace(temp, '\t',' '));
                   strcpy(temp,replace(temp, '\n',' '));
                   strcpy(temp,replace(temp, '\\',' '));
                   strcpy(temp,replace(temp, '\\',' '));

                   strcpy(temp,replace(temp, '\'',' '));
                   len=strlen(temp)-2;
                   k+=len;

                   itoa(len, temp, 10);
                   strcat(buffer1,temp);
                   strcat(buffer1,")\n");

                   fputs(buffer1, outhfile);
                   strcpy(buffer2,"\0");
            }

    }

  strcpy(buffer2,"\r\n\r\n#endif\r\n");
  fprintf(outsfile,buffer2,"%s");
  fprintf(outhfile,buffer2,"%s");

    if (fclose(infile) != 0 || fclose(outsfile)!=0 || fclose(outhfile)!= 0){
         fprintf(stderr,"Error in closing files\n");
     }
     printf("\nConversion Successful. Files created: \n");
     printf("%s   and   %s \n",hname,sname);
    return 0;

}
