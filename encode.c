#include <stdio.h>
#include "encode.h"
#include "common.h"
#include "types.h"
#include <string.h>
#include <unistd.h>


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");
    sleep(1);
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened beautiful.bmp\n");
    sleep(1);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }
    printf("INFO: Opened secret.txt\n");
    sleep(1);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    printf("INFO: Opened stego.bmp\n");
    sleep(1);
    printf("INFO: Done\n");
    sleep(1);
    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    /*
    1.check arg[2] == NULL
    yes->print  .bmp file not passed 
         return failure
    2.check strstr(arg[2], ".bmp") == NULL
    yes->print invalid image file name
         return failure
    3.encInfo->src_image_fname = argv[2];
    4.check argv[3] == NULL
    yes-> printf .txt file not passed
         return failure
    5.check strstr(argv[3],".txt") == NULL
    yes->print invalid sec file name
         return failure
    6. encInfo->secret_fname = argv[3]
    7.check argv[4] == NULL
    yes--> encInfo->stego_image_fname = "stego.bmp"
    no-> validate .bmp and store to encInfo->stego_image_fname = argv[4]
    8. copy sec file extn to ensInfo->extn_sec_file array
    9. cal sec_file_size --> make sure to set offset back to first byte
    10.encode_secret_file_size(sec_file_size, encInfo);
    11.encode_secret_file_data(encInfo);
    12.copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
    13.close 3 files
    14. return success
     
    */

    if(argv[2] == NULL)
    {
        printf(".bmp file not passed\n");
        return e_failure;
    }

    if(strstr(argv[2], ".bmp")==NULL)
    {
        printf("Invalid image file name\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];

    if(argv[3] == NULL)
    {
        printf(".txt file not passed\n");
        return e_failure;
    }

    if(strstr(argv[3], ".txt") == NULL)
    {
        printf("Invalid secret file name\n");
        return e_failure;
    }
    encInfo->secret_fname = argv[3];

    if (argv[4] == NULL)
    {
        printf("INFO: Output File not mentioned. Creating stego.bmp as default\n");
        sleep(1);
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
        if (strstr(argv[4], ".bmp") == NULL)
        {
            printf("Invalid stego image file name\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }
   
    char *ch = strchr(encInfo->secret_fname, '.');
    strcpy(encInfo->extn_secret_file, ch);
   
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    
   int ret = open_files(encInfo);
    if (ret == e_failure)
    {
        printf("open files failed\n");
        return e_failure;
    }
    printf("INFO: ## Encoding Procedure Started ##\n");
    sleep(1);
    ret = check_capacity(encInfo);
    if (ret == e_failure)
    {
        printf("check capacity failed\n");
        return e_failure;
    }
    printf("Before header file \n");
    printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
      
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
     printf("After header file \n");
     printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));

     ret=encode_magic_string(MAGIC_STRING,encInfo);
    if(ret==e_failure)
    {
        printf("encoding magic string failed\n");
        return e_failure;
    }
    printf("After Magic string\n");
    printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));

   ret = encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);
   if(ret==e_failure)
    {
    printf("file extension size encoded failed\n");
    return e_failure;
    }
    printf("After secret file extn size\n");
    printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));
   
    ret = encode_secret_file_extn((encInfo->extn_secret_file), encInfo);
   if(ret==e_failure)
    {
    printf("file extension encoded failed\n");
    return e_failure;
    }
    printf("After secret file extn\n");
    printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));

 ret=encode_secret_file_size(encInfo->size_secret_file, encInfo);
if(ret==e_failure)
{
    printf("Secret file size encoding failed\n");
    return e_failure;
}
    printf("After secret file size\n");
    printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));

 ret=encode_secret_file_data(encInfo);
if(ret==e_failure)
{
  
    printf("Encoding of secret file data failed\n");
    return e_failure;
}
 printf("After secret file data\n");
 printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));

ret=copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
if(ret==e_failure)
{
  
    printf("Encoding  remaining failed\n");
    return e_failure;
}
 printf("After copying remaining data\n");
 printf("src= %lu stego= %lu\n",ftell(encInfo->fptr_src_image),ftell(encInfo->fptr_stego_image));

 fclose(encInfo->fptr_src_image);
 fclose(encInfo->fptr_secret);
 fclose(encInfo->fptr_stego_image);

    return e_success;
}
/*
  1. open files
      ret = open_files(encInfo)
  2. check ret = failure
     yes->print open file failed
      return failure
  3. ret = check_capacity(encInfo);
  4. check ret == failure
      return failure
  5.copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
  6.encode_magic_string(MAGIC_STRING, encInfo);
  if ret==success
  return
  7.encode_secret_file_extn_size(strlen(file_extn_size), encInfo);
  8.



*/


Status check_capacity(EncodeInfo *encInfo)
{
    int src_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    fseek(encInfo->fptr_src_image,0,SEEK_SET);
    fseek(encInfo->fptr_secret,0,SEEK_END);
    printf("INFO: Checking for secret.txt size\n");
    sleep(1);
    encInfo->size_secret_file=ftell(encInfo->fptr_secret);
    printf("INFO: Done. Not Empty\n");
    sleep(1);
    fseek(encInfo->fptr_secret,0,SEEK_SET);

    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    sleep(1);
    int count=((strlen(MAGIC_STRING)+sizeof(int)+strlen(encInfo->extn_secret_file)+
    sizeof(int)+encInfo->size_secret_file)*8)+54;
    if(count<=src_file_size)
    {
        printf("INFO: Done. Found OK\n");
        sleep(1);
        return e_success;
    }
    return e_failure;
/*
  Checking capacity
   1. calculate no. of bytes needed for encoding from src file
     count =  (magic_str_len + 
       extn_size(*int) +
       extn_len +
       file_size(int) +
       file_data_len) * 8 + 54
    2. check count <= src_file_size 
    yes-> return success
    no->return failure

*/
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");
    sleep(1);
    char temp[55];
    fread(temp, 54, 1, fptr_src_image);
    fwrite(temp, 54, 1, fptr_dest_image);

    printf("INFO: Done\n");
    sleep(1);
    return e_success;

    /*
    Copying beautiful.bmp header(src) to stego.bmp(dest)
    char temp[55];
      fread(fptr_src_image, 54, 1, temp);

      fwrite(fptr_dest_image, 54, 1, temp);
    */
}
Status encode_byte_to_lsb(char data, char* image_buffer)
{
    for(int i=0;i<8;i++)
    {
        char mask = 1<<7-i;
        char bit = data & mask;
        image_buffer[i] = image_buffer[i] & 0xfe;
        bit = bit >> 7-i;
        image_buffer[i]=image_buffer[i] | bit;
    }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");
    sleep(1);
    for(int i = 0; i < 2; i++)
    {
        char temp[8];
        fread(temp, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);
        
    }
    printf("INFO: Done\n");
        sleep(1);
    return e_success;
    /*
    Encoding Magic string
    1. for(i=1;i<=2;i++)
    {
    charv temp[8];
    1.read 8 bytes buffer from src file and store to temp array
    2.byte_to_lsb(magic_string[i],temp);
    3.write temp array 8 bytes to stego file
    }
    */
}
   
Status encode_size_lsb(int data, char* image_buffer)
{
    for(int i=0;i<32;i++)
    {
        char mask = 1<<31-i;
        char bit = data & mask;
        image_buffer[i] = image_buffer[i] & 0xfe;
        bit = bit >> 31-i;
        image_buffer[i]=image_buffer[i] | bit;
    }
}

Status encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo)
{
    char temp_buffer[32];
      fread(temp_buffer, 1, 32, encInfo->fptr_src_image);
        encode_size_lsb(file_extn_size, temp_buffer);
        fwrite(temp_buffer, 1, 32, encInfo->fptr_stego_image);
        return e_success;

   /*  
    1.read 32 bytes buffer from src file
    2.call size_to_lsb(file_extn_size,temp_buffer)
    3.write temp_buffer to stego file
    
    */
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Extension\n");
    sleep(1);
     for(int i=0;i<strlen(file_extn);i++)
     {
        char temp[8];
        fread(temp, 1, 8 , encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);
     }
     printf("INFO: Done\n");
        sleep(1);
     return e_success;

    /*
    Encoding secret file extension
    1. run a loop 0 to file_extn_len-1 times
    {
    1.char 8 bytes of buffer from src file
    2. read 8 bytes of buffer from src file
    3. call bytes_to_lsb(file_extn[i],temp);
    4. write 8 bytes temp to stego file 
    }
    */
    }

    uint get_file_size(FILE *fptr)
    {
        fseek(fptr,0,SEEK_END);
        uint size=ftell(fptr);
        fseek(fptr,0,SEEK_SET);
        return size;
    }
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Size\n");
    sleep(1);
    char temp_buffer[32];
    fread(temp_buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_lsb(file_size, temp_buffer);
    fwrite(temp_buffer, 1, 32, encInfo->fptr_stego_image);
    printf("INFO: Done\n");
    sleep(1);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding secret.txt File Data\n");
    sleep(1);
    char ch=0;
    char temp[8];
    while((ch=fgetc(encInfo->fptr_secret) )!=EOF) 
     {
        fread(temp, 1, 8 , encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);
        
     }
     printf("INFO: Done\n");
     sleep(1);
     return e_success;
    }

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");
    sleep(1);
    char ch;
    ch= fgetc(fptr_src);
    while(!feof(fptr_src))
    {
       fputc(ch,fptr_dest);
       ch= fgetc(fptr_src);
      
    }
    printf("INFO: Done\n");
    sleep(1);
    return e_success;
}














