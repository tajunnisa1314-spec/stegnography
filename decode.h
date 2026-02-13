#include "types.h"
#include<stdio.h>
typedef struct _DecodeInfo
{
  char *stego_fname;
  FILE *fptr_stego;

  char dest_fname[20];
  FILE *fptr_dest;


}DecodeInfo;

OperationType check_operation_type(char *argv[]);
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);
Status skip_bmp_header(FILE *fptr_stego);
Status do_decoding(DecodeInfo *decInfo);
Status decode_magic_string(FILE *fptr_stego,char *user_magic_string);
char lsb_to_byte(char *buffer);
Status decode_extn_size(FILE *fptr_stego, int *extn_size);
Status decode_extn(FILE *fptr_stego,char *extn,int extn_size);
Status decode_sec_file_size(FILE *stego, int *file_size);
Status decode_sec_data(FILE *fptr_stego,FILE *fptr_dest,int file_size);



