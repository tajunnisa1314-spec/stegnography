#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int ret = check_operation_type(argv);

    if(ret == e_unsupported)
    //yes->print ->"err: no arg/invalid"
    {
        printf("Invalid args\n");
        return 0;
    }
    if(ret == e_encode)
    {
        //encoding
        int ret = read_and_validate_encode_args(argv, &encInfo);
        if(ret == e_failure)
        {
            printf("Invalid args\n");
            return 1;
        }
        //start encoding
        ret = do_encoding(&encInfo);
        if (ret == e_failure)
        {
            printf("encoding failed\n");
            return 1;
        }
        else
        {
           printf("INFO: ## Encoding Done Successfully ##\n");
            return 0;
        }
        
    }
   if(ret == e_decode)
    {
        int ret = read_and_validate_decode_args(argv,&decInfo);
        {
            if(ret==e_failure)
            {
             printf("Invalid args\n");
             return 1;
            }
        }
    
        ret=do_decoding(&decInfo);
        if (ret == e_failure)
        {
            printf("decoding failed\n");
            return 1;
        }
        else
        {
               printf("INFO: ## Decoding Done Successfully ##\n");
            return 0;
        }
        /*decoding
        1. call read_and_validate_decode_args(argv,decInfo)
        2.ret=do_decoding(&decInfo)
        if(ret == e_decode)
        */
        
    }    
    
}
OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)
    {
    return e_unsupported;
    }

    if(strcmp(argv[1], "-e") == 0)
    {
    return e_encode;
    }
  
    if(strcmp(argv[1], "-d") == 0)
    {
    return e_decode;
    }

    return e_unsupported;
    /*
    1.check argv[1] == NULL
    yes-> return e_unsupported

    2.(strcmp(arg[1], "-e")==0)
    yes-> return e_encode

    3.((strcmp(arg[1], "-d")==0)
    yes-> return e_decode)

    4.return e_unsupported
    */
}


