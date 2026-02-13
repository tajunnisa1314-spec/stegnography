#include <stdio.h>
#include "decode.h"
#include "common.h"
#include <string.h>
#include "types.h"
#include <unistd.h>
 Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
 {
  if(argv[2]==NULL)
  {
    printf(".bmp file not passed\n");
    return e_failure;
  }
  if(strstr(argv[2], ".bmp") == NULL)
    {
        printf("Invalid image file name\n");
        return e_failure;
    }
  decInfo->stego_fname = argv[2];

  
  if(argv[3] == NULL)
    {
      strcpy(decInfo->dest_fname, "output");
    }
  else
    {
      strcpy(decInfo->dest_fname, argv[3]);
    }
  
 }
    /*
    1.check argv[2] == .bmp
    yes-> store argv[2] to decInfo

    2.if argv[3] == NULL
    yes->store default name for output file to decInfo

    3. check argv[3] is not having . dot 
    we dont know the type of file while decoding we will find out the type of file
    
    */

Status do_decoding(DecodeInfo *decInfo)
 {
   printf("INFO: ## Decoding Procedure Started ##\n");
   sleep(1);

   printf("INFO: Opening required files\n");
   sleep(1);
   decInfo->fptr_stego = fopen(decInfo->stego_fname, "r");
  if(decInfo->fptr_stego == NULL)
    {
      printf("stego file opening failed");
      return e_failure;
    }
   printf("INFO: Opened stego.bmp\n");
   sleep(1);

  int ret = skip_bmp_header(decInfo->fptr_stego);

  if(ret==e_failure)
  {
    return e_failure;
  }
 
  char magic_string[50];
    printf("INFO: Decoding Magic String Signature\n");
    sleep(1);
    ret = decode_magic_string(decInfo->fptr_stego,magic_string);
    if(ret==e_failure)
    {
      printf("Magic string decode failed\n");
      return e_failure;
    }
    
  char user_magic_string[50];
    printf("Enter magic string:");
    scanf("%s",user_magic_string);
    if(strcmp(user_magic_string,magic_string)!=0)
    {
      printf("invalid magic string\n");
      return e_failure;
    }
    printf("INFO: Done\n");
    sleep(1);
  
  int extn_size;
    printf("INFO: Decoding Secret File Extension\n");
    sleep(1);
    ret = decode_extn_size(decInfo->fptr_stego, &extn_size);
    if(ret==e_failure)
    {
      printf("Decode extn size failed\n");
      return e_failure;
    }
    printf("INFO: Done\n");
    sleep(1);
   
  char extn[10];
    ret= decode_extn(decInfo->fptr_stego,extn,extn_size);
    if(ret==e_failure)
    {
      printf("Decode extn failed\n");
      return e_failure;
    }

    if(strcmp(decInfo->dest_fname,"output")==0)
    strcat(decInfo->dest_fname,extn);
    decInfo->fptr_dest = fopen(decInfo->dest_fname, "w");
    if(decInfo->fptr_dest == NULL)
     {
      printf("dest file opening failed");
      return e_failure;
     }
      printf("INFO: Opened output.txt\n");
      sleep(1);

      printf("INFO: Done. Opened all required files\n");
      sleep(1);

  int file_size;
     printf("INFO: Decoding secret.txt File Size\n");
     sleep(1);
     ret= decode_sec_file_size(decInfo->fptr_stego, &file_size);
    {
      if(ret==e_failure)
      {
        printf("Decode file size failed\n");
        return e_failure;
      }
      printf("INFO: Done\n");
      sleep(1);
    }

     printf("INFO: Decoding secret.txt File Data\n");
     sleep(1);
    ret=decode_sec_data(decInfo->fptr_stego,decInfo->fptr_dest,file_size);
     {
      {
        if(ret==e_failure)
        {
          printf("Decode file data failed\n");
          return e_failure;
        }
       printf("INFO: Done\n");
       sleep(1);
      }
    }
    fclose(decInfo->fptr_stego);
    fclose(decInfo->fptr_dest);
    return e_success;
  }
 
    /*
    1.open stego file
    2.call skip_bmp_header(decInfo->fptr_stego)
    3.char magic_string[50];
    4.decode_magic_string(decInfo->fptr_stego,magic_string) 
    5.read magic string from user 
    6.compare user entered and decoded magic string is same
    no--> err:invalid magic string
    return failure
    7.int extn_size
    8.decode_extn_size(decInfo->fptr_stego, &extn_size)
    9.char extn[10];
    10.  decode_extn(decInfo->fptr_stego,extn,extn_size)
    11.concatinate output file name with decoded extn
    12.open output file
    13.int file_size
    14.decode_sec_file_size(decInfo->stego, &file_size)
    15.decode_sec_data(decInfo->fptr_stego,decInfo->fptr_dest,file_size)
    16.close files
    17.return e_success  
}*/

Status skip_bmp_header(FILE *fptr_stego)
  {
    fseek(fptr_stego,54,SEEK_SET);
    return e_success; 
  }

Status decode_magic_string(FILE *fptr_stego,char *magic_string)
  {
    unsigned char temp[8];
    for(int i = 0; i < 2; i++)
     {
        fread(temp, 1, 8,fptr_stego);
        magic_string[i] = lsb_to_byte((char*)temp);
     }
    magic_string[2]='\0';
    return e_success;
  }

    /*
    Decoding magic string
    for(int i=0;i<2;i++)
    1. read 8 bytes buffer from stego file
    char temp[8];
    2. call magic_string[i] = lsb_to_byte(temp)
    */
    
char lsb_to_byte(char *buffer)//8 bytes
{
  char data=0;
   for(int i=0;i<8;i++)
   {
    char lsb=buffer[i]&1;
    data = data | (lsb<<(7-i));
   }
   return data;
}
  /*
      run a loop for 8 times
      {
        get lsb of each index of buffer and store to ith position of data variable
      }
      retrun data
  */

int lsb_to_size(char *buffer)//32 bytes
 {
   char size=0;
   for(int i=0;i<32;i++)
   {
    char lsb=buffer[i]&1;
    size = size | (lsb<<(31-i));
   }
   return size;
}


Status decode_extn_size(FILE *fptr_stego, int *extn_size)
 {
   char temp[32];
   fread(temp, 1, 32 ,fptr_stego);
   *extn_size = lsb_to_size(temp);
   return e_success;
 }
/*
  1. read 32 bytes buffer
  2. *extn_size = lsb_to_size(temp)
  3. return success
*/

Status decode_extn(FILE *fptr_stego,char *extn,int extn_size)
{
  char temp[8];
  for(int i=0;i<extn_size;i++)
  {
    fread(temp, 1, 8 ,fptr_stego);
    extn[i]=lsb_to_byte(temp); 
  }
  extn[extn_size]='\0';
  return e_success;
}
    /*
    run a loop for 0 to extn_size-1
      1.read 8 byte temp buffer
      2. extn[i]=lsb_to_byte(temp)
    
    */
Status decode_sec_file_size(FILE *fptr_stego, int *file_size)
{
   char temp[32];
   fread(temp, 1, 32 ,fptr_stego);
   *file_size = lsb_to_size(temp);
   return e_success;
}
 /*
 1.read 32 bytes buffer
 2.*file_size = lsb_to_size(buffer)
 */
Status decode_sec_data(FILE *fptr_stego,FILE *fptr_dest,int file_size)
{
 char ch=0;
 char temp[8];
 for(int i=0;i<file_size;i++)
  {
    fread(temp, 1, 8 , fptr_stego);
    char ch = lsb_to_byte(temp);
    fputc(ch,fptr_dest);      
  }
return e_success;
}
    /*   
    run a loop for file_size_times
    1.read 8 bytes buffer
    2. char ch = lsb_to_byte(temp)
    3.write ch to dest file
    */
    
    





