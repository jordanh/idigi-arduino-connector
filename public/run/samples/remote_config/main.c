/*
 *  Copyright (c) 1996-2011 Digi International Inc., All Rights Reserved
 *
 *  This software contains proprietary and confidential information of Digi
 *  International Inc.  By accepting transfer of this copy, Recipient agrees
 *  to retain this software in confidence, to prevent disclosure to others,
 *  and to make no use of this software other than that for which it was
 *  delivered.  This is an unpublished copyrighted work of Digi International
 *  Inc.  Except as permitted by federal law, 17 USC 117, copying is strictly
 *  prohibited.
 *
 *  Restricted Rights Legend
 *
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions set forth in sub-paragraph (c)(1)(ii) of The Rights in
 *  Technical Data and Computer Software clause at DFARS 252.227-7031 or
 *  subparagraphs (c)(1) and (2) of the Commercial Computer Software -
 *  Restricted Rights at 48 CFR 52.227-19, as applicable.
 *
 *  Digi International Inc. 11001 Bren Road East, Minnetonka, MN 55343
 *
 * =======================================================================
 *
 */
#include <stdio.h>
#include "idigi_api.h"
#include "remote_config.h"

static char const * const element_type_strings[] = {
    "string",
    "multiline_string",
    "password",
    "int32",
    "uint32",
    "hex32",
    "0xhex",
    "float",
    "enum",
    "on_off",
    "boolean",
    "ipv4",
    "fqdnv4",
    "fqdnv6",
    "datetime"
};

static char * const element_access_strings[] = {
    "read_only",
    "write_only",
    "read_write"
};

void print_name(char const * const label, char const * const name)
{
    unsigned char length = *name;
    char * ptr = (char *)name+1;

    printf("%s \"%.*s\" ", label, length, ptr);
}

int main (void)
{
    size_t i;
    /*
     * Call idigi_init() with a NULL callback, we are only verifying that we 
     * can compile and link the IIK.
     */
    /* (void)idigi_init((idigi_callback_t)0); */

    for (i = 0; i < idigi_group_count; i++)
    {
        idigi_group_t * const group_ptr = &idigi_groups[i];
        size_t j;

        print_name("\n\ngroup", group_ptr->name);

        for (j=0; j < group_ptr->elements.count; j++)
        {
            idigi_group_element_t * const element_ptr = &group_ptr->elements.data[j];
            print_name("\n\telement", element_ptr->name);
            printf("type= \"%s\" ", element_type_strings[element_ptr->type]);
            printf("access = \"%s\" ", element_access_strings[element_ptr->access]);

            if (element_ptr->value_limit != NULL)
            {

                switch (element_ptr->type)
                {
                case idigi_element_type_enum:
                {
                    idigi_element_value_enum_t * const enum_ptr = &element_ptr->value_limit->enum_value;
                    size_t n;

                    for (n=0; n < enum_ptr->count; n++)
                    {
                        print_name("\n\t\tvalue", enum_ptr->value[n]);
                    }
                    break;
                }
                case idigi_element_type_int32:
                {
                    idigi_element_value_signed_integer_t * const value_limit = &element_ptr->value_limit->integer_signed_value;
                    if (value_limit->min_value != 0 || value_limit->max_value != 0)
                    {
                        printf(" min = \"%d\" max = \"%d\" ", value_limit->min_value, value_limit->max_value);
                    }
                    break;
                }
                case idigi_element_type_uint32:
                {
                    idigi_element_value_unsigned_integer_t * const value_limit = &element_ptr->value_limit->integer_unsigned_value;
                    if (value_limit->min_value != 0 || value_limit->max_value != 0)
                    {
                        printf(" min = \"%ul\" max = \"%ul\" ", value_limit->min_value, value_limit->max_value);
                    }
                    break;
                }
                case idigi_element_type_hex32:
                {
                    idigi_element_value_unsigned_integer_t * const value_limit = &element_ptr->value_limit->integer_unsigned_value;
                    if (value_limit->min_value != 0 || value_limit->max_value != 0)
                    {
                        printf(" min = \"%X\" max = \"%X\" ", (unsigned)value_limit->min_value, (unsigned)value_limit->max_value);
                    }
                    break;
                }
                case idigi_element_type_0xhex:
                {
                    idigi_element_value_unsigned_integer_t * const value_limit = &element_ptr->value_limit->integer_unsigned_value;
                    if (value_limit->min_value != 0 &&  value_limit->max_value != 0)
                    {
                        printf(" min = \"0x%X\" max = \"ox%X\" ", (unsigned)value_limit->min_value, (unsigned)value_limit->max_value);
                    }
                    break;
                }
                case idigi_element_type_float:
                {
                    idigi_element_value_float_t * const value_limit = &element_ptr->value_limit->float_value;
                    if (value_limit->min_value != 0 || value_limit->max_value != 0)
                    {
                      printf(" min = \"%f\" max = \"%f\" ", value_limit->min_value, value_limit->max_value);
                    }
                    break;
                }
                case idigi_element_type_string:
                case idigi_element_type_multiline_string:
                {
                    idigi_element_value_string_t * const value_limit = &element_ptr->value_limit->string_value;
                    if (value_limit->min_length_in_bytes != 0 || value_limit->max_length_in_bytes != 0)
                    {
                        printf(" min = \"%d\" max = \"%d\" ", value_limit->min_length_in_bytes, value_limit->max_length_in_bytes);
                    }
                    break;
                }
                default:
                    printf(" invalid min and max on the type");
                    break;
                }
            }
        }

        for (j=0; j < group_ptr->errors.count; j++)
        {
            print_name("\n\terror", group_ptr->errors.description[j]);
        }
        printf("\n");
    }
    return 0;
}
