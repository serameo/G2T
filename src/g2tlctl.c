
/*-------------------------------------------------------------------
 * File name: g2tlctl.c
 * Author: Seree Rakwong
 * Date: 25-SEP-19
 *-----------------------------------------------------------------*/
#include <windows.h>
#include "g2t.h"

/*-------------------------------------------------------------------
 * LISTCTRL functions
 *-----------------------------------------------------------------*/
struct _TLISTCELLSTRUCT
{
    g2t_int y;
    g2t_int x;
    g2t_int lines;
    g2t_int cols;
    g2t_char caption[G2T_MAX_WNDTEXT + 1];
    g2t_dword textcolor;
    g2t_dword bgcolor;
    g2t_lparam data;
    g2t_dword editstyle;

    struct _TLISTCELLSTRUCT *prev;
    struct _TLISTCELLSTRUCT *next;
};
typedef struct _TLISTCELLSTRUCT tlistcell_t;

struct _THEADERSTRUCT
{
    g2t_int id;
    g2t_char caption[G2T_MAX_WNDTEXT + 1];
    g2t_int cols; /* width */
    g2t_int align; /* left is a default */
    g2t_dword textcolor;
    g2t_dword bgcolor;
    g2t_dword editstyle;
    g2t_int decwidth;

    tlistcell_t* firstcell;
    tlistcell_t* lastcell;

    struct _THEADERSTRUCT *prev;
    struct _THEADERSTRUCT *next;
};
typedef struct _THEADERSTRUCT theader_t;

struct _TLISTCTRLSTRUCT
{
    theader_t* firsthdr;
    theader_t* lasthdr;
    theader_t* firstvisiblehdr;
    theader_t* lastvisiblehdr;

    g2t_int nheaders;
    g2t_int nitems;
    g2t_int hdrids; /* header id */
    g2t_int hdrallwidths;
    /* item control */
    g2t_int curselrow;
    g2t_int curselcol;
    g2t_int firstvisiblerow;

    g2t_wnd editbox;
    g2t_int editingstate;
    g2t_int movingstate;
    g2t_int insertingstate;
    tlistcell_t* editingcell;

    g2t_lparam exparam;

    g2t_long editkey;
    g2t_long insertkey;
    g2t_long delkey;
    g2t_long canceleditkey;

    g2t_int* editcols;
    g2t_int nedtcols;
    g2t_int curedtcol;

    tlistcell_t* firsteditcell;
    tlistcell_t* lasteditcell;
};
typedef struct _TLISTCTRLSTRUCT _TLISTCTRL;
typedef struct _TLISTCTRLSTRUCT *PTLISTCTRL;

theader_t* _TLC_FindHeaderByIndex(PTLISTCTRL lctl, g2t_int col);
tlistcell_t* _TLC_FindCellByIndex(PTLISTCTRL lctl, g2t_int col, g2t_int idx);
tlistcell_t* _TLC_FindCellByHeader(PTLISTCTRL lctl, theader_t* header, g2t_int idx);
g2t_long _TLC_GetCellRect(tlistcell_t* cell, g2t_rect* rect);
g2t_int _TLC_FindHeaderIndex(PTLISTCTRL lctl, theader_t* header);
g2t_int _TLC_GetFirstEditableCol(PTLISTCTRL lctl);
g2t_int _TLC_GetNextEditableCol(PTLISTCTRL lctl, g2t_int prevcol);
g2t_int _TLC_GetPrevEditableCol(PTLISTCTRL lctl, g2t_int prevcol);
g2t_int _TLC_GetLastEditableCol(PTLISTCTRL lctl);
g2t_int _TLC_InsertEmptyItem(g2t_wnd wnd);

g2t_void _TLC_DrawItem(g2t_dc dc, g2t_rect* rccell, g2t_string caption, g2t_dword textcolor, g2t_dword bgcolor, g2t_int align, g2t_int isheader);

g2t_long LISTCTRLPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);
g2t_void _TLC_OnKeyDown(g2t_wnd wnd, g2t_long ch);
g2t_void _TLC_OnChar(g2t_wnd wnd, g2t_long ch);
g2t_void _TLC_OnPaint(g2t_wnd wnd, g2t_dc dc);
g2t_long _TLC_OnAddItem(g2t_wnd wnd, g2t_string text, g2t_int nitems);
g2t_long _TLC_OnInsertFirstItem(g2t_wnd wnd, g2t_string text, g2t_int nitems);
g2t_void _TLC_OnDeleteAllItems(g2t_wnd wnd);
g2t_void _TLC_OnDeleteColumn(g2t_wnd wnd, g2t_int col);
g2t_long _TLC_OnAddColumn(g2t_wnd wnd, THEADERITEM* hdritem);
g2t_void _TLC_OnDestroy(g2t_wnd wnd);
g2t_long _TLC_OnCreate(g2t_wnd wnd);
g2t_long _TLC_OnGetItem(g2t_wnd wnd, g2t_uint32 flags, TSUBITEM* subitem);
g2t_long _TLC_OnSetItem(g2t_wnd wnd, g2t_uint32 flags, TSUBITEM* subitem);
g2t_void _TLC_OnSetFocus(g2t_wnd wnd);
g2t_long _TLC_OnKillFocus(g2t_wnd wnd);
g2t_void _TLC_OnInvalidateItem(g2t_wnd wnd, g2t_uint32 col, g2t_uint32 idx);
g2t_void _TLC_OnSelChanged(g2t_wnd wnd);
g2t_void _TLC_OnBeginMoving(g2t_wnd wnd);
g2t_void _TLC_OnMovingCursor(g2t_wnd wnd, g2t_long ch);
g2t_void _TLC_OnEndMoving(g2t_wnd wnd);
g2t_dword _TLC_OnGetEditStyle(g2t_wnd wnd, g2t_int col);
g2t_void _TLC_OnSetEditStyle(g2t_wnd wnd, g2t_int col, g2t_dword editstyle);
g2t_int _TLC_OnGetCurRow(g2t_wnd wnd);
g2t_void _TLC_OnSetCurRow(g2t_wnd wnd, g2t_int idx);
g2t_void _TLC_OnSetColWidth(g2t_wnd wnd, g2t_int col, g2t_int width);
g2t_wnd _TLC_OnGetEditBox(g2t_wnd wnd);
g2t_void _TLC_OnSetCurPage(g2t_wnd wnd, g2t_int npage);
g2t_int _TLC_OnGetCurPage(g2t_wnd wnd);
g2t_int _TLC_OnGetItemsPerPage(g2t_wnd wnd);
g2t_void _TLC_OnSetEditableCols(g2t_wnd wnd, g2t_int ncols, g2t_int* edtcols);
g2t_void _TLC_OnEndEditRow(g2t_wnd wnd, g2t_int row, g2t_int ok);
g2t_void _TLC_OnBeginEditRow(g2t_wnd wnd, g2t_int row);
g2t_void _TLC_OnBeginInsertRow(g2t_wnd wnd, g2t_int row);
g2t_void _TLC_OnBeginInsertCell(g2t_wnd wnd, g2t_int row);
g2t_void _TLC_OnBeginEdit(g2t_wnd wnd);
g2t_void _TLC_OnEndEdit(g2t_wnd wnd, g2t_int ok);
g2t_void _TLC_OnDeleteItem(g2t_wnd wnd, g2t_int idx);

g2t_int _TLC_FindHeaderIndex(PTLISTCTRL lctl, theader_t* header)
{
    g2t_int col = -1;
    theader_t* ctlhdr = lctl->firsthdr;

    if (header && ctlhdr)
    {
        while (ctlhdr)
        {
            ++col;
            if (header == ctlhdr)
            {
                break;
            }
            ctlhdr = ctlhdr->next;
        }
    }
    return col;
}

theader_t* _TLC_FindHeaderByIndex(PTLISTCTRL lctl, g2t_int col)
{
    theader_t* header = 0;
    g2t_int i = 0;

    if (col < 0 || col >= lctl->nheaders)
    {
        return 0; /* no header deleted */
    }

    header = lctl->firsthdr;
    while (header)
    {
        if (i == col)
        {
            break;
        }
        header = header->next;
        ++i;
    }
    return header;
}

tlistcell_t* _TLC_FindCellByIndex(PTLISTCTRL lctl, g2t_int col, g2t_int idx)
{
    tlistcell_t* cell = 0;
    theader_t* header = 0;
    g2t_int i = 0;

    if (idx < 0 || idx >= lctl->nitems)
    {
        return 0;
    }
    header = _TLC_FindHeaderByIndex(lctl, col);
    if (header)
    {
        cell = header->firstcell;
        while (cell)
        {
            if (i == idx)
            {
                break;
            }
            cell = cell->next;
            ++i;
        }
    }
    return cell;
}

tlistcell_t* _TLC_FindCellByHeader(PTLISTCTRL lctl, theader_t* header, g2t_int idx)
{
    tlistcell_t* cell = 0;
    g2t_int i = 0;

    if (idx < 0 || idx >= lctl->nitems)
    {
        return 0;
    }

    if (header)
    {
        cell = header->firstcell;
        while (cell)
        {
            if (i == idx)
            {
                break;
            }
            cell = cell->next;
            ++i;
        }
    }
    return cell;
}

g2t_long _TLC_GetCellRect(tlistcell_t* cell, g2t_rect* rect)
{
    rect->y = cell->y;
    rect->x = cell->x;
    rect->lines = cell->lines;
    rect->cols = cell->cols;
    return G2T_OK;
}

g2t_long _TLC_OnCreate(g2t_wnd wnd)
{
    PTLISTCTRL lctl = (PTLISTCTRL) malloc(sizeof (_TLISTCTRL));
    g2t_bool editable = 0;
    if (!lctl)
    {
        return G2T_ENOMEM;
    }
    memset(lctl, 0, sizeof (_TLISTCTRL));
    lctl->hdrids = 1;
    lctl->editbox = g2t_CreateWnd(G2T_EDITBOX,
                                  "",
                                  TWS_CHILD | TES_AUTOHSCROLL,
                                  0,
                                  0, /* y */
                                  0, /* x */
                                  1, /* lines  */
                                  1, /* cols   */
                                  wnd, /* parent */
                                  lctl->hdrids, /* id */
                                  "",
                                  0); /* no parameter */
    if (!lctl->editbox)
    {
        free(lctl);
        return G2T_ENOMEM;
    }
    lctl->curselrow = -1;
    lctl->curselcol = -1;
    lctl->firstvisiblerow = -1;

    lctl->editkey = VK_F2;
    lctl->insertkey = VK_INSERT;
    lctl->delkey = VK_DELETE;
    lctl->canceleditkey = VK_ESCAPE;

    lctl->curedtcol = -1;
    lctl->nedtcols = 256;
    lctl->editcols = (g2t_int*) malloc(lctl->nedtcols * sizeof (TINT));
    if (TLCS_EDITABLE & g2t_GetWndStyle(wnd))
    {
        editable = 1;
    }
    memset(lctl->editcols, editable, lctl->nedtcols * sizeof (TINT));
    lctl->firsteditcell = 0;
    lctl->lasteditcell = 0;
    /* increment child ids */
    ++lctl->hdrids;

    /* save memory */
    g2t_SetWndParam(wnd, (g2t_lparam) lctl);
    g2t_SetTextColor(wnd, G2T_RGB_WHITE);
    g2t_SetBgColor(wnd, G2T_RGB_BLACK);
    return G2T_CONTINUE;
}

g2t_void _TLC_OnSelChanged(g2t_wnd wnd)
{
    g2t_nhdr nmhdr;
    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = TLCN_SELCHANGED;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);
}

g2t_void _TLC_OnSetFocus(g2t_wnd wnd)
{
    g2t_nhdr nmhdr;
    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = TLCN_SETFOCUS;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);
}

g2t_long _TLC_OnKillFocus(g2t_wnd wnd)
{
    g2t_nhdr nmhdr;
    PTLISTCTRL lctl = 0;
    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->movingstate == TLCT_MOVINGCURSOR)
    {
        _TLC_OnEndMoving(wnd);
        lctl->movingstate = TLCT_VIEW;
    }
    else if (lctl->editingstate == TLCT_EDITING)
    {
        _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL);
        lctl->editingstate = TLCT_VIEW;
    }
    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = TLCN_KILLFOCUS;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);

    return G2T_CONTINUE;
}

g2t_void _TLC_OnDestroy(g2t_wnd wnd)
{
    PTLISTCTRL lctl = 0;
    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);

    _TLC_OnDeleteAllItems(wnd);
    free(lctl);
}

g2t_void _TLC_OnSetEditableCols(g2t_wnd wnd, g2t_int ncols, g2t_int* edtcols)
{
    PTLISTCTRL lctl = 0;
    g2t_int i = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (0 == edtcols)
    {
        /* this means re*/
        return;
    }
    if (ncols > lctl->nheaders)
    {
        ncols = lctl->nheaders;
    }
    for (i = 0; i < ncols; ++i)
    {
        lctl->editcols[i] = edtcols[i];
    }
}

g2t_long _TLC_OnAddColumn(g2t_wnd wnd, THEADERITEM* hdritem)
{
    PTLISTCTRL lctl = 0;
    theader_t* header = 0;
    g2t_rect rc;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);

    if (lctl->nitems > 0)
    {
        /* not allowed to add header after there are any items */
        return G2T_ERROR;
    }

    header = (theader_t*) malloc(sizeof (theader_t));
    if (!header)
    {
        return G2T_ENOMEM;
    }
    memset(header, 0, sizeof (theader_t));

    g2t_GetWndRect(g2t_GetParent(wnd), &rc);
    strncpy(header->caption,
            hdritem->caption,
            G2T_MIN(G2T_MAX_WNDTEXT, strlen(hdritem->caption)));
    header->cols = hdritem->cols;
    header->align = hdritem->align;
    header->textcolor = hdritem->textcolor;
    header->bgcolor = hdritem->bgcolor;
    header->editstyle = hdritem->editstyle;
    header->decwidth = hdritem->decwidth;
    header->id = lctl->hdrids;

    /* make link */
    if (lctl->firsthdr)
    {
        header->prev = lctl->lasthdr;
        lctl->lasthdr->next = header;
        lctl->lasthdr = header;
    }
    else
    {
        lctl->firsthdr = lctl->lasthdr = header;
        lctl->firstvisiblehdr = header;
    }
    /* increment child ids */
    ++lctl->hdrids;
    ++lctl->nheaders;
    lctl->hdrallwidths += header->cols;

    return 0;
}

g2t_void _TLC_OnDeleteColumn(g2t_wnd wnd, g2t_int col)
{
    PTLISTCTRL lctl = 0;
    theader_t* next = 0;
    tlistcell_t* cell = 0;
    tlistcell_t* nextcell = 0;
    theader_t* header = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    header = _TLC_FindHeaderByIndex(lctl, col);
    if (!header)
    {
        return;
    }

    /* re-link */
    next = header->next;
    if (next)
    {
        next->prev = header->prev;
    }
    if (header->prev)
    {
        header->prev->next = next;
    }

    if (header == lctl->firsthdr)
    {
        lctl->firsthdr = next;
    }
    else if (header == lctl->lasthdr)
    {
        lctl->lasthdr = header->prev;
    }
    /* delete */
    header->next = header->prev = 0;

    cell = header->firstcell;
    while (cell)
    {
        nextcell = cell->next;
        /*free(cell->caption);*/
        cell->next = cell->prev = 0;
        free(cell);
        cell = nextcell;
    }

    free(header);

    /* done */
    --lctl->nheaders;
}

g2t_void _TLC_OnDeleteAllItems(g2t_wnd wnd)
{
    g2t_int nitems = 0;
    g2t_int i = 0;
    PTLISTCTRL lctl = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    nitems = lctl->nheaders;
    for (i = 0; i < nitems; ++i)
    {
        g2t_SendMsg(wnd, TLCM_DELETEITEM, 0, 0);
    }
}

g2t_void _TLC_OnDeleteItem(g2t_wnd wnd, g2t_int idx)
{
    PTLISTCTRL lctl = 0;
    tlistcell_t* cell = 0;
    tlistcell_t* nextcell = 0;
    theader_t* header = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->nitems <= 0 || idx < 0 || idx >= lctl->nitems)
    {
        return;
    }

    header = lctl->firsthdr;
    while (header)
    {
        cell = _TLC_FindCellByHeader(lctl, header, idx);
        nextcell = cell->next;
        if (nextcell)
        {
            nextcell->prev = cell->prev;
        }
        if (cell->prev)
        {
            cell->prev->next = nextcell;
        }
        if (header->firstcell == cell)
        {
            header->firstcell = cell->next;
        }
        else if (header->lastcell == cell)
        {
            header->lastcell = cell->prev;
        }

        cell->next = cell->prev = 0;
        free(cell);
        /* next header */
        header = header->next;
    }
    --lctl->nitems;
}

g2t_long _TLC_OnInsertFirstItem(g2t_wnd wnd, g2t_string text, g2t_int nitems)
{
    PTLISTCTRL lctl = 0;
    g2t_int i = 0;
    g2t_char* tok;
    theader_t* header = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    tlistcell_t* newcell = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->nheaders > 0)
    {
        /* insert into all listboxes */
        header = lctl->firsthdr;

        strcpy(buf, text);
        tok = strtok(buf, "\t");
        while (/*tok &&*/ i < nitems && i < lctl->nheaders)
        {
            newcell = (tlistcell_t*) malloc(sizeof (tlistcell_t));
            if (!newcell)
            {
                break;
            }
            memset(newcell, 0, sizeof (tlistcell_t));
            if (tok)
            {
                strncpy(newcell->caption, tok, G2T_MIN(G2T_MAX_WNDTEXT, strlen(tok)));
            }
            else
            {
                strcpy(newcell->caption, "");
            }
            newcell->textcolor = g2t_GetSysTextColor(LISTCTRL_TEXTCOLOR);
            newcell->bgcolor = g2t_GetSysBgColor(LISTCTRL_BGCOLOR);

            /* add the new item */
            if (header->firstcell)
            {
                newcell->next = header->firstcell;
                header->firstcell->prev = newcell;
                header->firstcell = newcell;

                lctl->firstvisiblerow = 0;
            }
            else
            {
                header->firstcell = header->lastcell = newcell;

                lctl->firstvisiblerow = 0;
            }
            /* insert next */
            header = header->next;
            tok = strtok(0, "\t");
            ++i;
        }
        /* all items count */
        ++lctl->nitems;

    } /* have header */
    return lctl->nitems;
}

g2t_long _TLC_OnAddItem(g2t_wnd wnd, g2t_string text, g2t_int nitems)
{
    PTLISTCTRL lctl = 0;
    g2t_int i = 0;
    g2t_char* tok;
    theader_t* header = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    tlistcell_t* newcell = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->nheaders > 0)
    {
        /* insert into all listboxes */
        header = lctl->firsthdr;

        strcpy(buf, text);
        tok = strtok(buf, "\t");
        while (/*tok &&*/ i < nitems && i < lctl->nheaders)
        {
            newcell = (tlistcell_t*) malloc(sizeof (tlistcell_t));
            if (!newcell)
            {
                break;
            }
            memset(newcell, 0, sizeof (tlistcell_t));
            if (tok)
            {
                strncpy(newcell->caption, tok, G2T_MIN(G2T_MAX_WNDTEXT, strlen(tok)));
            }
            else
            {
                strcpy(newcell->caption, "");
            }
            newcell->textcolor = g2t_GetSysTextColor(LISTCTRL_TEXTCOLOR);
            newcell->bgcolor = g2t_GetSysBgColor(LISTCTRL_BGCOLOR);

            /* add the new item */
            if (header->firstcell)
            {
                newcell->prev = header->lastcell;
                header->lastcell->next = newcell;
                header->lastcell = newcell;
            }
            else
            {
                header->firstcell = header->lastcell = newcell;

                lctl->firstvisiblerow = 0;
            }
            /* insert next */
            header = header->next;
            tok = strtok(0, "\t");
            ++i;
        }
        /* all items count */
        ++lctl->nitems;

    } /* have header */
    return lctl->nitems;
}

g2t_void _TLC_DrawItem(
                       g2t_dc dc, g2t_rect* rccell, g2t_string caption, g2t_dword textcolor, g2t_dword bgcolor, g2t_int align, g2t_int isheader)
{
    g2t_long len = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    len = g2t_PrintTextAlignment(buf,
                                 caption,
                                 rccell->cols,
                                 align);
    /* is is a header */
    if (isheader)
    {
        buf[0] = '[';
        buf[len - 1] = ']';
    }

    g2t_DrawText(dc,
                 rccell->x,
                 rccell->y,
                 buf,
                 textcolor, bgcolor);
}

g2t_void _TLC_OnPaint(g2t_wnd wnd, g2t_dc dc)
{
    PTLISTCTRL lctl = 0;
    theader_t* header = 0;
    g2t_int width = 0;
    g2t_rect rcitem, rcwnd, rccell;
    g2t_dword textcolor = 0;
    g2t_dword bgcolor = 0;
    g2t_dword htextcolor = 0;
    g2t_dword hbgcolor = 0;
    tlistcell_t* visiblecell = 0;
    g2t_int i = 0;
    g2t_int forcewidth = 0;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_char buf[G2T_BUFSIZ];
    g2t_dword wndbgcolor = g2t_GetBgColor(wnd);
    g2t_int ysel = 0;
    g2t_dword oddtext =  g2t_GetSysColor(LISTCTRL_ODD_TEXTCOLOR);
    g2t_dword oddbg =    g2t_GetSysColor(LISTCTRL_ODD_BGCOLOR);
    g2t_dword eventext = g2t_GetSysColor(LISTCTRL_EVEN_TEXTCOLOR);
    g2t_dword evenbg =   g2t_GetSysColor(LISTCTRL_EVEN_BGCOLOR);


    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);

    /* if no headers or control is hiding */
    if (lctl->nheaders <= 0 || !g2t_IsWndVisible(wnd))
    {
        return;
    }

    /* draw headers */
    g2t_GetWndRect(wnd, &rcwnd);
    rcitem = rcwnd; /* copy some values */
    rcitem.lines = (style & TLCS_NOHEADER ? 0 : 1);
    ysel = rcwnd.y;

    header = lctl->firstvisiblehdr;
    width = header->cols;

    /* first column width must not be over window columns */
    if (width > rcwnd.cols)
    {
        width = rcwnd.cols - 1;
        forcewidth = 1;
    }
    /* fill background */
    textcolor = header->textcolor;
    bgcolor = header->bgcolor;
    htextcolor = header->textcolor;
    hbgcolor = header->bgcolor;

    g2t_memset(buf, ' ', rcwnd.cols);
    buf[rcwnd.cols] = 0;

    while (header && width <= rcwnd.cols)
    {
        rcitem.x = rcwnd.x + width - header->cols;
        rcitem.cols = header->cols;

        if (rcitem.x < 0)
        {
            rcitem.x = rcwnd.x;
            if (forcewidth == 1)
            {
                rcitem.cols = width;
            }
            else
            {
                --rcitem.cols;
            }
        }


        /* fill background */
        g2t_DrawText(dc, rcitem.x, rcitem.y, buf, wndbgcolor, wndbgcolor);
        if (!(style & TLCS_NOHEADER))
        {
            if (style & TLCS_NOBORDER)
            {
                _TLC_DrawItem(dc, &rcitem,
                              header->caption, htextcolor, hbgcolor, header->align, 0); /* 0=no border */
            }
            else
            {
                _TLC_DrawItem(dc, &rcitem,
                              header->caption, htextcolor, hbgcolor, header->align, 1); /* 1=header */
            }
        }

        /* draw cells */
        rccell = rcitem;
        rccell.y += (style & TLCS_NOHEADER ? 0 : 1);
        rccell.lines = 1;
        visiblecell = header->firstcell;
        /* fill background */
        g2t_DrawText(dc, rccell.x, rccell.y, buf, wndbgcolor, wndbgcolor);

        for (i = 0; i < lctl->nitems && visiblecell;
                ++i, visiblecell = visiblecell->next)
        {
            if (i < lctl->firstvisiblerow)
            {
                /* do nothing */
                continue;
            }
            else if (i - lctl->firstvisiblerow <= rcwnd.lines - 2)
            {
                textcolor = visiblecell->textcolor;
                bgcolor = visiblecell->bgcolor;
                if (TLCS_MULTICOLOR & style)
                {
                    if (0 == i%2)
                    {
                        textcolor = eventext;
                        bgcolor = evenbg;
                    }
                    else
                    {
                        textcolor = oddtext;
                        bgcolor = oddbg;
                    }
                }

                if (!(TLCS_NOSELECTION & style) && i == lctl->curselrow)
                {
                    textcolor = g2t_GetReverseColor(textcolor);
                    bgcolor = g2t_GetReverseColor(bgcolor);
                    ysel = rccell.y;
                }
                /* draw th item that it can be seen */
                _TLC_DrawItem(dc, &rccell,
                                  visiblecell->caption, textcolor, bgcolor, header->align, 0);

                /* update cell rect */
                visiblecell->y = rccell.y;
                visiblecell->x = rccell.x;
                visiblecell->lines = rccell.lines;
                visiblecell->cols = rccell.cols;
                /* next cell line */
                ++rccell.y;
            }
            else
            {
                /* no need to draw more items than client lines */
                break;
            }
        } /* for each drawing cell */

        /* draw next header */
        header = header->next;
        if (header)
        {
            width += header->cols;
            if (width > rcwnd.cols)
            {
                break;
            }
        }
    } /* while header */

    if (lctl->firsthdr != lctl->firstvisiblehdr)
    {
        if (!(style & TLCS_NOHEADER))
        {
        }
    }
    /* save the last visible */
    if (header)
    {
        lctl->lastvisiblehdr = header->prev;
        if (!(style & TLCS_NOHEADER))
        {
        }
    }
    else
    {
        lctl->lastvisiblehdr = lctl->lasthdr;
    }

    g2t_MoveXY(rcwnd.x, ysel);
}

g2t_void _TLC_OnBeginMoving(g2t_wnd wnd)
{
    g2t_nhdr nmhdr;
    PTLISTCTRL lctl = 0;
    g2t_rect rccell;
    tlistcell_t* cell = 0;
    theader_t* header = 0;
    g2t_int curselcol = -1;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->movingstate == TLCT_MOVINGCURSOR)
    {
        _TLC_OnEndMoving(wnd);
        lctl->movingstate = TLCT_VIEW;
        return;
    }

    lctl->movingstate = TLCT_MOVINGCURSOR;
    /* move cursor to the current row */
    curselcol = _TLC_FindHeaderIndex(lctl, lctl->firstvisiblehdr);
    /*lctl->curedtcol = _TLC_FindHeaderIndex(lctl, lctl->firstvisiblehdr);*/
    if (curselcol < 0)
    {
        lctl->curselrow = 0;
        g2t_InvalidateWnd(wnd);
    }

    /* draw moving cursor */
    if (0 > lctl->curselcol)
    {
        lctl->curselcol = curselcol;
    }
    if (0 > lctl->curselrow)
    {
        lctl->curselrow = 0;
    }
    header = _TLC_FindHeaderByIndex(lctl, lctl->curselcol);
    /*header = _TLC_FindHeaderByIndex(lctl, lctl->curedtcol);*/
    cell = _TLC_FindCellByHeader(lctl, header, lctl->curselrow);
    _TLC_GetCellRect(cell, &rccell);

    _TLC_DrawItem(g2t_GetDC(), &rccell,
                  cell->caption,
                  cell->textcolor,
                  cell->bgcolor,
                  header->align, 0);

    /* move cursor */
    g2t_MoveXY(rccell.x, rccell.y);

    /* save the editing cell */
    lctl->editingcell = cell;
    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = TLCN_BEGINMOVING;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);
}

g2t_void _TLC_OnMovingCursor(g2t_wnd wnd, g2t_long ch)
{
    PTLISTCTRL lctl = 0;
    g2t_int firstcol = -1, lastcol = -1;
    g2t_int newcol = -1;
    g2t_int newrow = -1;
    g2t_rect rccell;
    g2t_rect rcwnd;
    tlistcell_t* cell = 0;
    theader_t* header = 0;
    g2t_int maxrows = 25;
    g2t_dword style = g2t_GetWndStyle(wnd);

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->movingstate != TLCT_MOVINGCURSOR)
    {
        return;
    }

    g2t_GetWndRect(wnd, &rcwnd);
    firstcol = _TLC_FindHeaderIndex(lctl, lctl->firstvisiblehdr);
    lastcol = _TLC_FindHeaderIndex(lctl, lctl->lastvisiblehdr);
    newcol = lctl->curselcol;
    newrow = lctl->curselrow;

    maxrows = lctl->firstvisiblerow + rcwnd.lines - 2;

    switch (ch)
    {
    case VK_LEFT:
    {
        newcol = _TLC_GetPrevEditableCol(lctl, lctl->curedtcol);
        if (newcol < 0)
        {
            newcol = _TLC_GetFirstEditableCol(lctl);
        }
        /*--newcol;*/
        if (newcol < firstcol)
        {
            newcol = firstcol;
        }
        break;
    }

    case VK_RIGHT:
    {
        newcol = _TLC_GetNextEditableCol(lctl, lctl->curedtcol);
        if (newcol < 0)
        {
            newcol = _TLC_GetLastEditableCol(lctl);
        }
        if (newcol < 0)
        {

        }
        /*++newcol;*/
        if (newcol > lastcol)
        {
            newcol = lastcol;
        }
        break;
    }

    case VK_DOWN:
    {
        if (!(TLCS_LINEEDIT & style))
        {
            /* not fixed line edit style */
            ++newrow;
            if (newrow > maxrows)
            {
                newrow = maxrows;
            }
        }
        break;
    }

    case VK_UP:
    {
        if (!(TLCS_LINEEDIT & style))
        {
            /* not fixed line edit style */
            --newrow;
            if (newrow < lctl->firstvisiblerow)
            {
                newrow = lctl->firstvisiblerow;
            }
        }
        break;
    }
    }

    if (newcol != lctl->curselcol || newrow != lctl->curselrow)
    {
        /* redraw the previous moving cursor */
        header = _TLC_FindHeaderByIndex(lctl, lctl->curselcol);
        cell = _TLC_FindCellByHeader(lctl, header, lctl->curselrow);
        _TLC_GetCellRect(cell, &rccell);
        _TLC_DrawItem(
                      g2t_GetDC(),
                      &rccell,
                      cell->caption,
                      cell->textcolor,
                      cell->bgcolor,
                      header->align,
                      0);

        /* draw moving cursor */
        header = _TLC_FindHeaderByIndex(lctl, newcol);
        cell = _TLC_FindCellByHeader(lctl, header, newrow);
        _TLC_GetCellRect(cell, &rccell);
        _TLC_DrawItem(g2t_GetDC(),
                      &rccell,
                      cell->caption,
                      cell->textcolor,
                      cell->bgcolor,
                      header->align,
                      0);

        g2t_MoveXY(rccell.x, rccell.y);

        /* save the last current selected column */
        lctl->curselcol = newcol;
        lctl->curselrow = newrow;
        /* save the editing cell */
        lctl->curedtcol = newcol;
        lctl->editingcell = cell;
        if (TLCT_EDITING == lctl->editingstate)
        {
        }
    }
}

g2t_void _TLC_OnEndMoving(g2t_wnd wnd)
{
    g2t_nhdr nmhdr;
    PTLISTCTRL lctl = 0;
    tlistcell_t* cell = 0;
    theader_t* header = 0;
    g2t_rect rccell;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->movingstate != TLCT_MOVINGCURSOR)
    {
        return;
    }
    lctl->movingstate = TLCT_ENDMOVING;

    /* redraw the previous moving cursor */
    header = _TLC_FindHeaderByIndex(lctl, lctl->curselcol);
    /*header = _TLC_FindHeaderByIndex(lctl, lctl->curedtcol);*/
    cell = _TLC_FindCellByHeader(lctl, header, lctl->curselrow);
    _TLC_GetCellRect(cell, &rccell);

    _TLC_DrawItem(
                  g2t_GetDC(),
                  &rccell,
                  cell->caption,
                  cell->textcolor,
                  cell->bgcolor,
                  header->align,
                  0);

    /* update state */
    lctl->movingstate = TLCT_VIEW;
    /*lctl->editingstate  = TLCT_VIEW;*/

    g2t_InvalidateWnd(wnd);
    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = TLCN_ENDMOVING;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);
}

g2t_void _TLC_OnBeginInsertRow(g2t_wnd wnd, g2t_int row)
{
    PTLISTCTRL lctl = 0;
    TNMHDRITEMSTRUCT nmhdri;

    if (row < 0)
    {
        row = 0;
    }
    /* save all cells on the row */
    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    lctl->insertingstate = TLCT_INSERTING;

    /* send notification */
    nmhdri.hdr.id = g2t_GetWndID(wnd);
    nmhdri.hdr.ctl = wnd;
    nmhdri.hdr.code = TLCN_BEGININSERTROW;
    nmhdri.row = lctl->curselrow;
    nmhdri.col = lctl->curselcol;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdri);
}

g2t_void _TLC_OnEndInsertRow(g2t_wnd wnd, g2t_int row, g2t_int ok)
{
    TNMHDRITEMSTRUCT nmhdri;
    PTLISTCTRL lctl = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    lctl->insertingstate = TLCT_ENDINSERTING;

    if (TLC_ENDEDITCANCEL == ok)
    {
        /* rollback */
        _TLC_OnDeleteItem(wnd, row);
    }

    /* send notification */
    nmhdri.hdr.id = g2t_GetWndID(wnd);
    nmhdri.hdr.ctl = wnd;
    nmhdri.hdr.code = (TLC_ENDEDITOK == ok ?
            TLCN_ENDINSERTROWOK : TLCN_ENDINSERTROWCANCEL);
    nmhdri.row = lctl->curselrow;
    nmhdri.col = lctl->curselcol;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdri);
}

g2t_void _TLC_OnBeginEditRow(g2t_wnd wnd, g2t_int row)
{
    PTLISTCTRL lctl = 0;
    tlistcell_t* cell = 0;
    tlistcell_t* newcell = 0;
    g2t_int i = 0;
    TNMHDRITEMSTRUCT nmhdri;

    if (row < 0)
    {
        row = 0;
    }
    /* save all cells on the row */
    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);

    lctl->firsteditcell = lctl->lasteditcell = 0;

    for (i = 0; i < lctl->nheaders; ++i)
    {
        cell = _TLC_FindCellByIndex(lctl, i, row);
        newcell = (tlistcell_t*) malloc(sizeof (tlistcell_t));
        memcpy(newcell, cell, sizeof (tlistcell_t));
        newcell->next = newcell->prev = 0;

        if (0 == lctl->lasteditcell)
        {
            lctl->firsteditcell = lctl->lasteditcell = newcell;
        }
        else
        {
            newcell->prev = lctl->lasteditcell;
            lctl->lasteditcell->next = newcell;
            lctl->lasteditcell = newcell;
        }
    }
    /* send notification */
    nmhdri.hdr.id = g2t_GetWndID(wnd);
    nmhdri.hdr.ctl = wnd;
    nmhdri.hdr.code = TLCN_BEGINEDITROW;
    nmhdri.row = lctl->curselrow;
    nmhdri.col = lctl->curselcol;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdri);
}

g2t_void _TLC_OnEndEditRow(g2t_wnd wnd, g2t_int row, g2t_int ok)
{
    TNMHDRITEMSTRUCT nmhdri;
    PTLISTCTRL lctl = 0;
    tlistcell_t* cell = 0;
    tlistcell_t* nextcell = 0;
    g2t_int i = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (TLC_ENDEDITCANCEL == ok)
    {
        /* rollback */
        nextcell = lctl->firsteditcell;
        for (i = 0; i < lctl->nheaders && nextcell != 0; ++i)
        {
            cell = _TLC_FindCellByIndex(lctl, i, row);
            strcpy(cell->caption, nextcell->caption);
            nextcell = nextcell->next;
        }
    }

    /* release backed-up cells */
    nextcell = lctl->firsteditcell;
    while (nextcell)
    {
        cell = nextcell;
        nextcell = nextcell->next;

        free(cell);
    }
    lctl->firsteditcell = lctl->lasteditcell = 0;

    /* send notification */
    nmhdri.hdr.id = g2t_GetWndID(wnd);
    nmhdri.hdr.ctl = wnd;
    nmhdri.hdr.code = (TLC_ENDEDITOK == ok ?
            TLCN_ENDEDITROWOK : TLCN_ENDEDITROWCANCEL);
    nmhdri.row = lctl->curselrow;
    nmhdri.col = lctl->curselcol;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdri);
}

g2t_void _TLC_OnBeginEdit(g2t_wnd wnd)
{
    PTLISTCTRL lctl = 0;
    g2t_rect rccell;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_dword editstyle = 0;
    TNMHDRITEMSTRUCT nmhdri;

    if (!(g2t_GetWndStyle(wnd) & TLCS_EDITABLE))
    {
        /* if it has no EDIT style */
        return;
    }
    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);

    lctl->editingstate = TLCT_BEGINEDITING;
    /* show edit box at the current row and column */
    _TLC_GetCellRect(lctl->editingcell, &rccell);
    g2t_MoveWnd(lctl->editbox, rccell.y, rccell.x, rccell.lines, rccell.cols);

    editstyle = _TLC_OnGetEditStyle(wnd, lctl->curselcol);

    g2t_SetWndStyle(lctl->editbox, editstyle);
    if (editstyle & TES_DECIMAL)
    {
        theader_t* header = _TLC_FindHeaderByIndex(lctl, lctl->curselcol);
        TEDT_SetDecimalWidth(lctl->editbox, header->decwidth);
    }

    /* update text before showing */
    strcpy(buf, lctl->editingcell->caption);
    g2t_SetWndText(lctl->editbox, buf);
    g2t_ShowWnd(lctl->editbox, TW_SHOW);

    g2t_SetFocus(lctl->editbox);

    /* update state */
    lctl->editingstate = TLCT_EDITING;

    /* send notification */
    nmhdri.hdr.id = g2t_GetWndID(wnd);
    nmhdri.hdr.ctl = wnd;
    nmhdri.hdr.code = TLCN_BEGINEDIT;
    nmhdri.row = lctl->curselrow;
    nmhdri.col = lctl->curselcol;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdri);

    g2t_InvalidateWnd(wnd);
}

g2t_void _TLC_OnEndEdit(g2t_wnd wnd, g2t_int ok)
{
    TNMHDRITEMSTRUCT nmhdri;
    PTLISTCTRL lctl = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_int rc = G2T_CONTINUE;

    if (!(g2t_GetWndStyle(wnd) & TLCS_EDITABLE))
    {
        /* if it has no EDIT style */
        return;
    }
    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->editingstate != TLCT_EDITING)
    {
        return;
    }
    lctl->editingstate = TLCT_ENDEDITING;

    /* hide edit box */
    rc = g2t_SendMsg(lctl->editbox, TWM_KILLFOCUS, 0, 0);
    g2t_ShowWnd(lctl->editbox, TW_HIDE);
    if (ok)
    {
        if (G2T_CONTINUE == rc)
        {
            /* update */
            g2t_GetWndText(lctl->editbox, buf, G2T_MAX_WNDTEXT);
            strcpy(lctl->editingcell->caption, buf);
        }
    }

    /* send notification */
    nmhdri.hdr.id = g2t_GetWndID(wnd);
    nmhdri.hdr.ctl = wnd;
    nmhdri.hdr.code = (TLC_ENDEDITOK == ok ?
            TLCN_ENDEDITOK : TLCN_ENDEDITCANCEL);
    nmhdri.row = lctl->curselrow;
    nmhdri.col = lctl->curselcol;

    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdri);

    g2t_InvalidateWnd(wnd);
}

g2t_void _TLC_OnChar(g2t_wnd wnd, g2t_long ch)
{
    PTLISTCTRL lctl = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (lctl->editingstate == TLCT_EDITING)
    {
        /* forward key entered to edit */
        g2t_SendMsg(lctl->editbox, TWM_CHAR, (TWPARAM) ch, 0);
    }
}

g2t_int _TLC_GetFirstEditableCol(PTLISTCTRL lctl)
{
    return _TLC_GetNextEditableCol(lctl, -1);
}

g2t_int _TLC_GetNextEditableCol(PTLISTCTRL lctl, g2t_int prevcol)
{
    g2t_int col = -1;
    g2t_int i = 0;
    for (i = prevcol + 1; i < lctl->nheaders; ++i)
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            if (lctl->editcols[i] != 0)
            {
                col = i;
                break;
            }
        }
        else
        {
            col = i;
            break;
        }
    }
    return col;
}

g2t_int _TLC_InsertEmptyItem(g2t_wnd wnd)
{
    g2t_char sz[G2T_BUFSIZ];
    PTLISTCTRL lctl = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    memset(sz, '\t', lctl->nheaders);
    sz[lctl->nheaders] = 0;
    return _TLC_OnAddItem(wnd, sz, lctl->nheaders);
}

g2t_int _TLC_GetLastEditableCol(PTLISTCTRL lctl)
{
    return _TLC_GetPrevEditableCol(lctl, lctl->nheaders);
}

g2t_int _TLC_GetPrevEditableCol(PTLISTCTRL lctl, g2t_int prevcol)
{
    g2t_int col = -1;
    g2t_int i = 0;
    for (i = prevcol - 1; i >= 0; --i)
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            if (lctl->editcols[i] != 0)
            {
                col = i;
                break;
            }
        }
        else
        {
            col = i;
            break;
        }
    }
    return col;
}

g2t_void _TLC_OnKeyDown(g2t_wnd wnd, g2t_long ch)
{
    PTLISTCTRL lctl = 0;
    g2t_int repaint = 0;
    g2t_rect rc;
    g2t_int lines = 0;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_int row = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    g2t_GetWndRect(wnd, &rc);

    if (lctl->nheaders <= 0)
    {
        return;
    }

    row = lctl->curselrow;
    if (row < 0)
    {
        row = 0;
    }
    if (ch == lctl->editkey && (style & TLCS_EDITABLE))
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITOK); /* edit ok */
            _TLC_OnEndMoving(wnd);
            if (TLCT_INSERTING == lctl->insertingstate)
            {
                _TLC_OnEndInsertRow(wnd, row, TLC_ENDEDITOK);
            }
            else
            {
                _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITOK);
            }
            return;
        }
        else
        {
            if (lctl->nitems <= 0)
            {
                return;
            }
            lctl->curedtcol = _TLC_GetFirstEditableCol(lctl);
            lctl->curselcol = lctl->curedtcol;
            _TLC_OnBeginMoving(wnd);
            _TLC_OnBeginEditRow(wnd, row);
            _TLC_OnBeginEdit(wnd); /* begin editing */
        }
    }
    else if (ch == lctl->canceleditkey && (style & TLCS_EDITABLE))
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            _TLC_OnEndMoving(wnd);
            if (TLCT_INSERTING == lctl->insertingstate)
            {
                _TLC_OnEndInsertRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            else
            {
                _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            _TLC_OnSetCurRow(wnd, row - 1);
        }
        return;
    }
    else if (ch == lctl->insertkey && (style & TLCS_EDITABLE))
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            _TLC_OnEndMoving(wnd);
            if (TLCT_INSERTING == lctl->insertingstate)
            {
                _TLC_OnEndInsertRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            else
            {
                _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            _TLC_OnSetCurRow(wnd, row - 1);
        }
        else
        {
            row = _TLC_InsertEmptyItem(wnd);
            if (row <= 0)
            {
                return;
            }
            --row; /* move to the last inserted row */
            /* find the properly first visible row */
            if (row >= lctl->firstvisiblerow + rc.lines - 1)
            {
                lctl->firstvisiblerow += 1;
                if (lctl->firstvisiblerow - 1 > lctl->nitems - rc.lines)
                {
                    lctl->firstvisiblerow = lctl->nitems - rc.lines;
                }
            }
            else if (lctl->firstvisiblerow > row)
            {
                lctl->firstvisiblerow += 1;
                if (lctl->firstvisiblerow < 0)
                {
                    lctl->firstvisiblerow = 0;
                }
            }
            _TLC_OnSetCurRow(wnd, row);

            lctl->curedtcol = _TLC_GetFirstEditableCol(lctl);
            lctl->curselcol = lctl->curedtcol;
            _TLC_OnBeginMoving(wnd);
            _TLC_OnBeginInsertRow(wnd, row);
            _TLC_OnBeginEdit(wnd); /* begin editing */
        }
        return;
    }
    else if (ch == lctl->delkey)
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            _TLC_OnEndMoving(wnd);
        }
        else if (TLCT_INSERTING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            _TLC_OnEndMoving(wnd);
        }
        _TLC_OnDeleteItem(wnd, row);
        if (row >= lctl->nitems)
        {
            --row;
        }
        if (row < 0)
        {
            g2t_InvalidateWnd(wnd);
        }
        else
        {
            _TLC_OnSetCurRow(wnd, row);
        }
        return;
    }

    switch (ch)
    {
    case VK_RIGHT:
    {
        if (lctl->editingstate == TLCT_EDITING)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITOK);
            _TLC_OnMovingCursor(wnd, ch);
            _TLC_OnBeginEdit(wnd);
        }
        else if (lctl->lastvisiblehdr != lctl->lasthdr)
        {
            lctl->firstvisiblehdr = lctl->firstvisiblehdr->next;
            ++repaint;
        }
        break;
    }

    case VK_LEFT:
    {
        if (lctl->editingstate == TLCT_EDITING)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITOK);
            _TLC_OnMovingCursor(wnd, ch);
            _TLC_OnBeginEdit(wnd);
        }
        else if (lctl->firstvisiblehdr != lctl->firsthdr)
        {
            lctl->firstvisiblehdr = lctl->firstvisiblehdr->prev;
            ++repaint;
        }
        break;
    }

    case VK_DOWN:
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            /*_TLC_OnMovingCursor(wnd, ch);*/
            _TLC_OnEndMoving(wnd);
            if (TLCT_INSERTING == lctl->insertingstate)
            {
                _TLC_OnEndInsertRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            else
            {
                _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            }
        }
        else
        {
            ++lines;
            ++repaint;
        }
        break;
    }


    case VK_UP:
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            /*_TLC_OnMovingCursor(wnd, ch);*/
            _TLC_OnEndMoving(wnd);
            if (TLCT_INSERTING == lctl->insertingstate)
            {
                _TLC_OnEndInsertRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            else
            {
                _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            }
        }
        else
        {
            --lines;
            ++repaint;
        }
        break;
    }

    case VK_PRIOR:
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            /*_TLC_OnMovingCursor(wnd, ch);*/
            _TLC_OnEndMoving(wnd);
            if (TLCT_INSERTING == lctl->insertingstate)
            {
                _TLC_OnEndInsertRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            else
            {
                _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            }
        }
        else
        {
            lines -= rc.lines - 1;
            ++repaint;
        }
        break;
    }

    case VK_NEXT:
    {
        if (TLCT_EDITING == lctl->editingstate)
        {
            _TLC_OnEndEdit(wnd, TLC_ENDEDITCANCEL); /* edit cancel */
            /*_TLC_OnMovingCursor(wnd, ch);*/
            _TLC_OnEndMoving(wnd);
            if (TLCT_INSERTING == lctl->insertingstate)
            {
                _TLC_OnEndInsertRow(wnd, row, TLC_ENDEDITCANCEL);
            }
            else
            {
                _TLC_OnEndEditRow(wnd, row, TLC_ENDEDITCANCEL);
            }
        }
        else
        {
            lines += rc.lines - 1;
            ++repaint;
        }
        break;
    }

    default:
    {
        break;
    }
    }
    /* repaint */
    do
    {
        if (!repaint || (lines > 0 && (lctl->curselrow + 1 == lctl->nitems)) ||
                (lctl->curselrow <= 0 && lines < 0))
        {
            break;
        }
        lctl->curselrow += lines;
        if (lctl->curselrow >= lctl->nitems)
        {
            lctl->curselrow = lctl->nitems - 1;
        }
        else if (lctl->curselrow < 0)
        {
            lctl->curselrow = 0;
        }

        if (lctl->curselrow >= lctl->firstvisiblerow + rc.lines - 1)
        {
            lctl->firstvisiblerow += lines;
            if (lctl->firstvisiblerow - 1 > lctl->nitems - rc.lines)
            {
                lctl->firstvisiblerow = lctl->nitems - rc.lines;
            }
        }
        else if (lctl->firstvisiblerow > lctl->curselrow)
        {
            lctl->firstvisiblerow += lines;
            if (lctl->firstvisiblerow < 0)
            {
                lctl->firstvisiblerow = 0;
            }
        }
        /* send notification */
        _TLC_OnSelChanged(wnd);

        g2t_InvalidateWnd(wnd);
    }
    while (0);
}

g2t_long _TLC_OnGetItemCount(g2t_wnd wnd)
{
    PTLISTCTRL lctl = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    return lctl->nitems;
}

g2t_long _TLC_OnSetItem(g2t_wnd wnd, g2t_uint32 flags, TSUBITEM* subitem)
{
    PTLISTCTRL lctl = 0;
    tlistcell_t* cell = 0;
    g2t_long rc = G2T_ERROR;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    cell = _TLC_FindCellByIndex(lctl, subitem->col, subitem->idx);
    if (cell)
    {
        if (flags & TLCFM_TEXT)
        {
            strcpy(cell->caption, subitem->text);
        }
        if (flags & TLCFM_DATA)
        {
            cell->data = subitem->data;
        }
        if (flags & TLCFM_ATTRS)
        {
            cell->textcolor = subitem->textcolor;
            cell->bgcolor = subitem->bgcolor;
        }
        rc = G2T_OK;
    }
    return rc;
}

g2t_long _TLC_OnGetItem(g2t_wnd wnd, g2t_uint32 flags, TSUBITEM* subitem)
{
    PTLISTCTRL lctl = 0;
    tlistcell_t* cell = 0;
    g2t_long rc = G2T_ERROR;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    cell = _TLC_FindCellByIndex(lctl, subitem->col, subitem->idx);
    if (cell)
    {
        if (flags & TLCFM_TEXT)
        {
            subitem->text = cell->caption;
        }
        if (flags & TLCFM_DATA)
        {
            subitem->data = cell->data;
        }
        if (flags & TLCFM_ATTRS)
        {
            subitem->textcolor = cell->textcolor;
            subitem->bgcolor = cell->bgcolor;
        }
        rc = G2T_OK;
    }
    return rc;
}

g2t_void _TLC_OnInvalidateItem(g2t_wnd wnd, g2t_uint32 col, g2t_uint32 idx)
{
    PTLISTCTRL lctl = 0;
    tlistcell_t* cell = 0;
    theader_t* header = 0;
    g2t_rect rcwnd;
    g2t_rect rccell;
    g2t_dword textcolor = 0;
    g2t_dword bgcolor = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    cell = _TLC_FindCellByIndex(lctl, col, idx);
    if (cell)
    {
        /* check if the cell is available on screen */
        g2t_GetWndRect(wnd, &rcwnd);
        if ((g2t_int) idx >= lctl->firstvisiblerow &&
                (g2t_int) idx < lctl->firstvisiblerow + rcwnd.lines)
        {
            if ((g2t_int) col >= _TLC_FindHeaderIndex(lctl, lctl->firstvisiblehdr) &&
                    (g2t_int) col < _TLC_FindHeaderIndex(lctl, lctl->lastvisiblehdr))
            {
                header = _TLC_FindHeaderByIndex(lctl, col);
                _TLC_GetCellRect(cell, &rccell);


                /*attrs = cell->attrs;*/
                textcolor = cell->textcolor;
                bgcolor = cell->bgcolor;
                if ((g2t_int) idx == lctl->curselrow)
                {
                    /*attrs = TuiReverseColor(CYAN_BLACK);*/
                    textcolor = g2t_GetReverseColor(textcolor);
                    bgcolor = g2t_GetReverseColor(bgcolor);
                }
                /* draw th item that it can be seen */
                _TLC_DrawItem(g2t_GetDC(), &rccell,
                              cell->caption, textcolor, bgcolor, header->align, 0);
            }
        }
    }
}

g2t_void _TLC_OnSetEditStyle(g2t_wnd wnd, g2t_int col, g2t_dword editstyle)
{
    PTLISTCTRL lctl = 0;
    theader_t* header = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    header = _TLC_FindHeaderByIndex(lctl, col);
    if (header)
    {
        header->editstyle = TWS_CHILD | editstyle;
    }
}

g2t_dword _TLC_OnGetEditStyle(g2t_wnd wnd, g2t_int col)
{
    PTLISTCTRL lctl = 0;
    theader_t* header = 0;
    g2t_dword editstyle = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    header = _TLC_FindHeaderByIndex(lctl, col);
    if (header)
    {
        editstyle = header->editstyle;
    }
    return editstyle;
}

g2t_void _TLC_OnSetCurRow(g2t_wnd wnd, g2t_int idx)
{
    PTLISTCTRL lctl = 0;
    g2t_rect rcwnd;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    if (idx < 0 || idx >= lctl->nitems)
    {
        if (lctl->nitems <= 0)
        {
            lctl->curselrow = -1;
        }
        else
        {
            lctl->curselrow = lctl->nitems - 1;
        }
        g2t_InvalidateWnd(wnd);
        return;
    }

    /* find the new appropriate first visible item */
    g2t_GetWndRect(wnd, &rcwnd);
    if (idx >= lctl->firstvisiblerow &&
            idx < lctl->firstvisiblerow + rcwnd.lines)
    {
        /* no moved first visible item */
    }
    else
    {
        if (idx > lctl->nitems - rcwnd.lines)
        {
            lctl->firstvisiblerow = lctl->nitems - rcwnd.lines;
        }
        else
        {
            lctl->firstvisiblerow = idx;
        }
    }
    /* set current selected row */
    lctl->curselrow = idx;

    /* re-fresh window */
    g2t_InvalidateWnd(wnd);
}

g2t_int _TLC_OnGetCurRow(g2t_wnd wnd)
{
    PTLISTCTRL lctl = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);

    return lctl->curselrow;
}

g2t_void _TLC_OnSetColWidth(g2t_wnd wnd, g2t_int col, g2t_int width)
{
    PTLISTCTRL lctl = 0;
    theader_t* header = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    header = _TLC_FindHeaderByIndex(lctl, col);
    if (header)
    {
        header->cols = width;
    }
}

g2t_wnd _TLC_OnGetEditBox(g2t_wnd wnd)
{
    PTLISTCTRL lctl = 0;

    lctl = (PTLISTCTRL) g2t_GetWndParam(wnd);
    return lctl->editbox;
}

g2t_void _TLC_OnSetCurPage(g2t_wnd wnd, g2t_int npage)
{
    g2t_int idx = 0;
    g2t_int nitemspp = _TLC_OnGetItemsPerPage(wnd);

    idx = (npage * nitemspp);
    g2t_SendMsg(wnd, TLCM_SETCURROW, (TWPARAM) idx, 0);
}

g2t_int _TLC_OnGetCurPage(g2t_wnd wnd)
{
    g2t_int curselrow = _TLC_OnGetCurRow(wnd);
    g2t_int nitems = _TLC_OnGetItemCount(wnd);
    g2t_int nitemspp = _TLC_OnGetItemsPerPage(wnd);
    g2t_int npage = -1;

    /* items is greater than zero */
    if (nitems > 0 && nitemspp > 0)
    {
        /* has selected row yet? */
        if (curselrow < 0)
        {
            curselrow = 0;
        }
        /* find the current page */
        npage = (curselrow / nitemspp);
    }
    return npage;
}

g2t_int _TLC_OnGetItemsPerPage(g2t_wnd wnd)
{
    g2t_rect rcwnd;
    g2t_GetWndRect(wnd, &rcwnd);
    return (rcwnd.lines > 1 ? rcwnd.lines - 1 : 0);
}

g2t_long G2TLISTCTRLPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    switch (msg)
    {
    case TWM_CREATE:
    {
        /* initial memory for static control */
        return _TLC_OnCreate(wnd);
    }
    case TWM_DESTROY:
    {
        /* release memory of static control */
        _TLC_OnDestroy(wnd);
        return 0;
    }

    case TWM_SETFOCUS:
    {
        _TLC_OnSetFocus(wnd);
        break;
    }
    case TWM_KILLFOCUS:
    {
        return _TLC_OnKillFocus(wnd);
    }

    case TWM_KEYDOWN:
    {
        _TLC_OnKeyDown(wnd, (TLONG) wparam);
        break;
    }

    case TWM_CHAR:
    {
        _TLC_OnChar(wnd, (TLONG) wparam);
        break;
    }

    case TWM_PAINT:
    {
        _TLC_OnPaint(wnd, g2t_GetMemDC());
        return 0;
    }

    case TLCM_ADDCOLUMN:
    {
        return _TLC_OnAddColumn(wnd, (THEADERITEM*) lparam);
    }
    case TLCM_DELETECOLUMN:
    {
        _TLC_OnDeleteColumn(wnd, (TINT) wparam);
        return 0;
    }
    case TLCM_DELETEALLCOLUMNS:
    {
        _TLC_OnDeleteAllItems(wnd);
        return 0;
    }
    case TLCM_INSERTFIRSTITEM:
    {
        return _TLC_OnInsertFirstItem(wnd, (g2t_string) lparam, (TINT) wparam);
    }
    case TLCM_ADDITEM:
    {
        return _TLC_OnAddItem(wnd, (g2t_string) lparam, (TINT) wparam);
    }
    case TLCM_DELETEITEM:
    {
        _TLC_OnDeleteItem(wnd, (TINT) lparam);
        return 0;
    }
    case TLCM_DELETEALLITEMS:
    {
        _TLC_OnDeleteAllItems(wnd);
        return 0;
    }
    case TLCM_SETITEM:
    {
        return _TLC_OnSetItem(wnd, (TUINT) wparam, (TSUBITEM*) lparam);
    }
    case TLCM_GETITEM:
    {
        return _TLC_OnGetItem(wnd, (TUINT) wparam, (TSUBITEM*) lparam);
    }
    case TLCM_GETITEMCOUNT:
    {
        return _TLC_OnGetItemCount(wnd);
    }
    case TLCM_INVALIDATEITEM:
    {
        _TLC_OnInvalidateItem(wnd, (TUINT) wparam, (TUINT) lparam);
        return 0;
    }
    case TLCM_SETEDITSTYLE:
    {
        _TLC_OnSetEditStyle(wnd, (TINT) wparam, (TDWORD) lparam);
        return 0;
    }
    case TLCM_GETEDITSTYLE:
    {
        return _TLC_OnGetEditStyle(wnd, (TINT) wparam);
    }
    case TLCM_GETCURROW:
    {
        return _TLC_OnGetCurRow(wnd);
    }
    case TLCM_SETCURROW:
    {
        _TLC_OnSetCurRow(wnd, (TINT) lparam);
        return 0;
    }
    case TLCM_SETCOLWIDTH:
    {
        _TLC_OnSetColWidth(wnd, (TINT) wparam, (TINT) lparam);
        return 0;
    }
    case TLCM_GETEDITBOX:
    {
        return (TLONG) _TLC_OnGetEditBox(wnd);
    }
    case TLCM_SETCURPAGE:
    {
        _TLC_OnSetCurPage(wnd, (TINT) wparam);
        return 0;
    }
    case TLCM_GETCURPAGE:
    {
        return _TLC_OnGetCurPage(wnd);
    }
    case TLCM_GETITEMSPERPAGE:
    {
        return _TLC_OnGetItemsPerPage(wnd);
    }
    case TLCM_SETEDITABLECOLS:
    {
        _TLC_OnSetEditableCols(wnd, (TINT) wparam, (g2t_int*) lparam);
        break;
    }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}
