/*
 * File name: g2tprop.c
 * Author: Seree Rakwong
 * Date: 07-AUG-2019
 *
 * NOTE:
 *  Date        By      Reason
 *  =========   ====    ================================================
 *
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "g2tprop.h"


#ifdef __cplusplus
extern "C"
{
#endif

struct _SAFEPROP_IMPL_STRUCT
{
    struct _SAFEPROP_STRUCT vtab;
    long tag;
    long nitems;
    safeprop_keyval_t *props;
};
typedef struct _SAFEPROP_IMPL_STRUCT safeprop_impl_t;

/* interfaces */
long safeprop_get_tag(safeprop_t*);
long safeprop_load(safeprop_t*, FILE*, char*, long);
long safeprop_get_string(safeprop_t*, const char* key, char* val, const char* def);
long safeprop_get_int(safeprop_t*, const char* key, long* val, long def);
long safeprop_get_float(safeprop_t*, const char* key, double* val, double def);
unsigned long safeprop_get_version(safeprop_t*, unsigned long* major, unsigned long* minor);
long safeprop_populate(safeprop_t*, long(*)(const char*, const char*, void*), void*);
long safeprop_add_key(safeprop_t*, const char* key, const char* val);
long safeprop_remove_key(safeprop_t*, const char* key);
long safeprop_init_keys(safeprop_t*, const safeprop_keyval_t*, long);
long safeprop_sort_keys(safeprop_t*);
/* helper interfaces */
int _safeprop_sortkey(const void* d1, const void* d2);
int _safeprop_sortkey_desc(const void* d1, const void* d2);
long _safeprop_parse(char* line, char* key, char* val);
long _safeprop_populate(const char* key, const char* val, void* args);

long _TrimRight(char* szStr)
{
    long len = strlen(szStr);
    char* psz = &szStr[len - 1];
    while (len > 0)
    {
        if ((' ' == *psz || '\t' == *psz))
        {
            szStr[len - 1] = 0;
        }
        else
        {
            break;
        }
        --len;
        --psz;
    }
    return strlen(szStr);
}

long _TrimLeft(char* szStr)
{
    char* psz = szStr;
    while (*psz && (' ' == *psz || '\t' == *psz))
    {
        psz++;
    }
    if (psz)
    {
        strcpy(szStr, psz);
    }
    else
    {
        strcpy(szStr, "");
    }
    return strlen(szStr);
}

long _Trim(char* szStr)
{
    _TrimLeft(szStr);
    _TrimRight(szStr);
    return strlen(szStr);
}

int _safeprop_sortkey(const void* d1, const void* d2)
{
    safeprop_keyval_t* kv1 = (safeprop_keyval_t*) d1;
    safeprop_keyval_t* kv2 = (safeprop_keyval_t*) d2;
    return strcmp(kv1->key, kv2->key);
}

int _safeprop_sortkey_desc(const void* d1, const void* d2)
{
    safeprop_keyval_t* kv1 = (safeprop_keyval_t*) d1;
    safeprop_keyval_t* kv2 = (safeprop_keyval_t*) d2;
    return (-1)*strcmp(kv1->key, kv2->key);
}

long _safeprop_parse(char* line, char* key, char* val)
{
    long rc = SPROP_OK;
    char* tok = line;
    long len = 0;

    if (SPROP_COMMENT1 == line[0] || SPROP_COMMENT2 == line[0])
    {
        return SPROP_COMMENTLINE;
    }
    /* empty line */
    len = _Trim(line);
    if (len <= 0)
    {
        return SPROP_EMPTYLINE;
    }
    /* key */
    tok = strtok(line, SPROP_KEYSEPS);
    if (!tok)
    {
        return SPROP_EKEYNOTFOUND;
    }
    _Trim(tok);
    strcpy(key, tok);
    /* value */
    tok = strtok(0, SPROP_VALSEPS);
    if (tok)
    {
        len = _Trim(tok);
        if ((SPROP_QUOTE == tok[0] && SPROP_QUOTE == tok[len - 1]) ||
                (SPROP_SQUOTE == tok[0] && SPROP_SQUOTE == tok[len - 1]))
        {
            memcpy(val, &tok[1], len - 2);
        }
        else
        {
            strcpy(val, tok);
        }
    }
    return rc;
}

unsigned long GetSafePropVersion(unsigned long* major, unsigned long* minor)
{
    *major = SAFEPROP_VERSION_MAJOR;
    *minor = SAFEPROP_VERSION_MINOR;
    return SAFEPROP_VERSION;
}

safeprop_t* CreateSafeProp(long tag)
{
    safeprop_impl_t* impl = (safeprop_impl_t*) malloc(sizeof (safeprop_impl_t));
    if (impl)
    {
        memset(impl, 0, sizeof (safeprop_impl_t));

        impl->vtab.GetTag = safeprop_get_tag;
        impl->vtab.LoadFromFile = safeprop_load;
        impl->vtab.GetString = safeprop_get_string;
        impl->vtab.GetInt = safeprop_get_int;
        impl->vtab.GetFloat = safeprop_get_float;
        impl->vtab.GetVersion = safeprop_get_version;
        impl->vtab.Populate = safeprop_populate;
        impl->vtab.AddKey = safeprop_add_key;
        impl->vtab.RemoveKey = safeprop_remove_key;
        impl->vtab.InitKeys = safeprop_init_keys;
        impl->vtab.SortKeys = safeprop_sort_keys;

        /* member variables */
        impl->tag = tag;
    }
    return (safeprop_t*) impl;
}

void DestroySafeProp(safeprop_t* props)
{
    safeprop_impl_t* impl = (safeprop_impl_t*) props;
    if (impl)
    {
        if (impl->props)
        {
            free(impl->props);
        }
        free(impl);
    }
}

long safeprop_load(safeprop_t* props, FILE* fp, char* errmsg, long len)
{
    long rc = SPROP_OK;
    long nitems = 0;
    safeprop_keyval_t* base = 0;
    long size = MAX_PROPS_ITEMS * sizeof (safeprop_keyval_t);
    char line[BUFSIZ];
    char key[MAX_PROP_KEYVAL_LEN + 1];
    char val[MAX_PROP_KEYVAL_LEN + 1];
    long nlines = 0;
    char buf[BUFSIZ];
    long buflen = 0;
    safeprop_impl_t* impl = (safeprop_impl_t*) props;

    if (!fp || feof(fp))
    {
        return SPROP_EFILE;
    }

    base = (safeprop_keyval_t*) malloc(size);
    memset(base, 0, size);

    while (!feof(fp))
    {
        rc = SPROP_OK;
        if (!fgets(line, sizeof (line), fp))
        {
            break;
        }
        ++nlines;

        memset(key, 0, sizeof (key));
        memset(val, 0, sizeof (val));
        rc = _safeprop_parse(line, key, val);
        if (SPROP_EKEYNOTFOUND == rc)
        {
            break;
        }
        else if (SPROP_OK == rc)
        {
            strcpy(base[nitems].key, key);
            strcpy(base[nitems].val, val);
            ++nitems;

            if (nitems >= MAX_PROPS_ITEMS)
            {
                break;
            }
        }
    }
    if (SPROP_EKEYNOTFOUND == rc)
    {
        if (errmsg && len > 0)
        {
            sprintf(buf, "Line %d: Not found key and value format", nlines);
            buflen = strlen(buf);
            memset(errmsg, 0, len);
            if (buflen > len)
            {
                strncpy(errmsg, buf, len - 1);
            }
            else
            {
                strcpy(errmsg, buf);
            }
        }
        free(base);
        return rc;
    }

    /* sorting */
    size = sizeof (safeprop_keyval_t);
    qsort(base, nitems, size, _safeprop_sortkey_desc);
    /* copy */
    if (impl->props)
    {
        free(impl->props);
    }
    impl->props = base;
    impl->nitems = nitems;
    return rc;
}

long safeprop_get_string(safeprop_t* props, const char* key, char* val, const char* def)
{
    long rc = SPROP_OK;
    safeprop_impl_t* impl = (safeprop_impl_t*) props;
    long size = 0;
    safeprop_keyval_t* kv = 0;

    if (!impl->props)
    {
        return SPROP_ERROR;
    }
    size = sizeof (safeprop_keyval_t);
    kv = bsearch(key, impl->props, impl->nitems, size, _safeprop_sortkey_desc);

    if (!kv)
    {
        if (def)
        {
            strcpy(val, def);
        }
        else
        {
            strcpy(val, SPROP_DEFVAL);
        }
        return SPROP_EKEYNOTFOUND;
    }
    strcpy(val, kv->val);
    return rc;
}

long safeprop_get_int(safeprop_t* props, const char* key, long* val, long def)
{
    long rc = SPROP_OK;
    char buf[MAX_PROP_KEYVAL_LEN + 1];

    memset(buf, 0, sizeof (buf));
    rc = safeprop_get_string(props, key, buf, 0);

    *val = def;
    if (SPROP_OK == rc)
    {
        *val = atol(buf);
    }
    return rc;
}

long safeprop_get_float(safeprop_t* props, const char* key, double* val, double def)
{
    long rc = SPROP_OK;
    char buf[MAX_PROP_KEYVAL_LEN + 1];

    memset(buf, 0, sizeof (buf));
    rc = safeprop_get_string(props, key, buf, 0);

    *val = def;
    if (SPROP_OK == rc)
    {
        *val = atof(buf) + FLOATING_VALUE;
    }
    return rc;
}

unsigned long safeprop_get_version(safeprop_t* props, unsigned long* major, unsigned long* minor)
{
    return GetSafePropVersion(major, minor);
}

long safeprop_populate(safeprop_t* props, long(*fn)(const char*, const char*, void*), void* args)
{
    long rc = SPROP_OK;
    safeprop_impl_t* impl = (safeprop_impl_t*) props;
    long i = 0;

    if (!fn)
    {
        return rc;
    }
    for (i = 0; i < impl->nitems; ++i)
    {
        rc = fn(impl->props[i].key, impl->props[i].val, args);
        if (SPROP_OK != rc)
        {
            break;
        }
    }
    return rc;
}

long safeprop_get_tag(safeprop_t* props)
{
    safeprop_impl_t* impl = (safeprop_impl_t*) props;
    return impl->tag;
}

long safeprop_add_key(safeprop_t* props, const char* key, const char* val)
{
    long rc = SPROP_OK;
    safeprop_impl_t* impl = (safeprop_impl_t*) props;
    long size = MAX_PROPS_ITEMS * sizeof (safeprop_keyval_t);
    safeprop_keyval_t* kv = 0;

    if (impl->nitems >= MAX_PROPS_ITEMS)
    {
        return rc;
    }
    if (0 == impl->nitems)
    {
        impl->props = (safeprop_keyval_t*) malloc(size);
        memset(impl->props, 0, size);
    }
    size = sizeof (safeprop_keyval_t);
    kv = bsearch(key, impl->props, impl->nitems, size, _safeprop_sortkey_desc);

    if (kv)
    {
        /* update */
        strcpy(kv->val, val);
    }
    else
    {
        /* insert */
        strcpy(impl->props[impl->nitems].key, key);
        strcpy(impl->props[impl->nitems].val, val);
        ++impl->nitems;
    }
    /*
    size = sizeof(safeprop_keyval_t);
    qsort(impl->props, impl->nitems, size, _safeprop_sortkey_desc);
     */
    return rc;
}

long safeprop_remove_key(safeprop_t* props, const char* key)
{
    long rc = SPROP_OK;
    safeprop_impl_t* impl = (safeprop_impl_t*) props;
    long size = 0;
    safeprop_keyval_t* kv = 0;

    if (!impl->props)
    {
        return rc;
    }
    size = sizeof (safeprop_keyval_t);
    kv = bsearch(key, impl->props, impl->nitems, size, _safeprop_sortkey_desc);

    if (!kv)
    {
        return SPROP_EKEYNOTFOUND;
    }
    kv->key[0] = 0;
    kv->val[0] = 0;
    --impl->nitems;
    /*
    size = sizeof(safeprop_keyval_t);
    qsort(impl->props, impl->nitems, size, _safeprop_sortkey_desc);
     */
    return rc;
}

long safeprop_init_keys(safeprop_t* props, const safeprop_keyval_t* kv, long nitems)
{
    long rc = SPROP_OK;
    safeprop_impl_t* impl = (safeprop_impl_t*) props;
    safeprop_keyval_t* base = 0;
    long size = MAX_PROPS_ITEMS * sizeof (safeprop_keyval_t);

    if (!kv || nitems <= 0)
    {
        return rc;
    }

    base = (safeprop_keyval_t*) malloc(size);
    memset(base, 0, size);
    if (nitems > MAX_PROPS_ITEMS)
    {
        nitems = MAX_PROPS_ITEMS;
    }
    memcpy(base, kv, nitems * sizeof (safeprop_keyval_t));
    size = sizeof (safeprop_keyval_t);
    qsort(base, nitems, size, _safeprop_sortkey_desc);
    /* copy */
    if (impl->props)
    {
        free(impl->props);
    }
    impl->props = base;
    impl->nitems = nitems;

    return rc;
}

long safeprop_sort_keys(safeprop_t* props)
{
    long rc = SPROP_OK;
    safeprop_impl_t* impl = (safeprop_impl_t*) props;

    if (!impl->props)
    {
        return rc;
    }

    qsort(impl->props, MAX_PROPS_ITEMS, sizeof (safeprop_keyval_t), _safeprop_sortkey_desc);
    return 0;
}

#ifdef __cplusplus
}
#endif

