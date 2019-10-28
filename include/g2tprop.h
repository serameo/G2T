/*
* File name: g2tprop.h
* Author: Seree Rakwong
* Date: 07-AUG-2019
* Purpose:
*  Read/Write properties from/to a property file
*
* NOTE:
*  Date        By      Reason
*  =========   ====    ================================================
*
*/
#ifndef __M_SAFEPROP_H__
#define __M_SAFEPROP_H__

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SAFEPROP_VERSION_MINOR  0x0001
#define SAFEPROP_VERSION_MAJOR  0x0100
#define SAFEPROP_VERSION        (SAFEPROP_VERSION_MAJOR | SAFEPROP_VERSION_MINOR)

#define FLOATING_VALUE          (0.0000001)
    enum  _SAFEPROP_ENUM
    {
        SPROP_EMPTYLINE = 2,
        SPROP_COMMENTLINE = 1,
        SPROP_OK = 0,
        SPROP_ERROR = -1,
        SPROP_EFILE = -2,
        SPROP_EKEYNOTFOUND = -3
    };

#define SPROP_COMMENT1          '!'
#define SPROP_COMMENT2          '#'
#define SPROP_KEYSEPS           "="
#define SPROP_VALSEPS           "\r\n"
#define SPROP_QUOTE             '\"'
#define SPROP_SQUOTE             '\''
#define SPROP_DEFVAL            ""

#define MAX_PROP_KEYVAL_LEN           64
#define MAX_PROPS_ITEMS               256

    struct _SAFEPROP_KEYVAL_STRUCT
    {
        char key[MAX_PROP_KEYVAL_LEN + 1];
        char val[MAX_PROP_KEYVAL_LEN + 1];
    };
    typedef struct _SAFEPROP_KEYVAL_STRUCT safeprop_keyval_t;

    struct _SAFEPROP_STRUCT;
    typedef struct _SAFEPROP_STRUCT safeprop_t;

    safeprop_t*  CreateSafeProp(long tag);
    void         DestroySafeProp(safeprop_t*);
    unsigned long   GetSafePropVersion(unsigned long* major, unsigned long* minor);


    /* safeprop_t object functions */
    typedef long(*fn_safeprop_get_tag)(safeprop_t*);
    typedef long(*fn_safeprop_load)(safeprop_t*, FILE*, char*, long);
    typedef long(*fn_safeprop_get_string)(safeprop_t*, const char* key, char* val, const char* def);
    typedef long(*fn_safeprop_get_int)(safeprop_t*, const char* key, long* val, long def);
    typedef long(*fn_safeprop_get_float)(safeprop_t*, const char* key, double* val, double def);
    typedef unsigned long(*fn_safeprop_get_version)(safeprop_t*, unsigned long* major, unsigned long* minor);
    typedef long(*fn_safeprop_populate)(safeprop_t*, long(*)(const char*, const char*, void*), void*);
    /* manipulate functions */
    typedef long(*fn_safeprop_add_key)(safeprop_t*, const char* key, const char* val);
    typedef long(*fn_safeprop_remove_key)(safeprop_t*, const char* key);
    typedef long(*fn_safeprop_init_keys)(safeprop_t*, const safeprop_keyval_t*, long nitems);
    typedef long(*fn_safeprop_sort_keys)(safeprop_t*);

    struct _SAFEPROP_STRUCT
    {
        /* interface methods */
        fn_safeprop_get_tag          GetTag;             /* get user tag */
        fn_safeprop_load             LoadFromFile;
        fn_safeprop_get_string       GetString;
        fn_safeprop_get_int          GetInt;
        fn_safeprop_get_float        GetFloat;
        fn_safeprop_get_version      GetVersion;
        fn_safeprop_populate         Populate;
        fn_safeprop_add_key          AddKey;
        fn_safeprop_remove_key       RemoveKey;
        fn_safeprop_init_keys        InitKeys;
        fn_safeprop_sort_keys        SortKeys;
    };

#ifdef __cplusplus
}
#endif

#endif /* __M_SAFEPROP_H__ */

