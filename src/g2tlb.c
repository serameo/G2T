/*-------------------------------------------------------------------
 * File name: g2tlb.c
 * Author: Seree Rakwong
 * Date: 25-SEP-19
 *-----------------------------------------------------------------*/
#include <windows.h>
#include "g2t.h"

/*-------------------------------------------------------------------
 * LISTBOX functions
 *-----------------------------------------------------------------*/

struct _TLISTBOXITEMSTRUCT
{
    g2t_char itemtext[G2T_MAX_WNDTEXT + 1];
    g2t_lparam data;
    g2t_int checked;
    struct _TLISTBOXITEMSTRUCT *prev;
    struct _TLISTBOXITEMSTRUCT *next;
};
typedef struct _TLISTBOXITEMSTRUCT tlistbox_t;

struct _TUILISTBOXSTRUCT
{
    g2t_int firstvisible;
    g2t_int nitems; /* item counts */
    g2t_int cursel; /* highlight item */

    tlistbox_t* firstitem; /* always be item id = 0 if it is existing */
    tlistbox_t* lastitem;

    tlistbox_t* selitem; /* must be the same cursel item */
    tlistbox_t* firstvisibleitem;

    g2t_int checkeditems; /* count item checked */
    tlistbox_t* lastitemchecked; /* to identify the last item checked */

    g2t_void* exparam;
};
typedef struct _TUILISTBOXSTRUCT _TLISTBOX;
typedef struct _TUILISTBOXSTRUCT *PTLISTBOX;

tlistbox_t* _GLB_FindItemByIndex(g2t_wnd wnd, g2t_int idx);
g2t_void _TLB_OnSelChanged(g2t_wnd wnd);
g2t_int _TLB_OnCountItemCheck(g2t_wnd wnd);
g2t_int _TLB_OnGetItemChecked(g2t_wnd wnd, g2t_int idx);
g2t_int _TLB_OnSetItemChecked(g2t_wnd wnd, g2t_int idx, g2t_int check);
g2t_lparam _TLB_OnGetItemData(g2t_wnd wnd, g2t_int idx);
g2t_void _TLB_OnSetItemData(g2t_wnd wnd, g2t_int idx, g2t_lparam data);
g2t_void _TLB_OnSetCurSel(g2t_wnd wnd, g2t_int idx);
g2t_long _TLB_OnGetItemCount(g2t_wnd wnd);
g2t_void _TLB_OnSetItemText(g2t_wnd wnd, g2t_int idx, g2t_string text);
g2t_long _TLB_OnGetItemText(g2t_wnd wnd, g2t_int idx, g2t_string text);
g2t_int _TLB_OnGetCurSel(g2t_wnd wnd);
g2t_void _TLB_OnDeleteAllItems(g2t_wnd wnd);
g2t_void _TLB_OnDeleteItem(g2t_wnd wnd, g2t_long idx);
g2t_long _TLB_OnAddItem(g2t_wnd wnd, g2t_string text);
g2t_long _TLB_OnInsertFirstItem(g2t_wnd wnd, g2t_string text);
g2t_void _TLB_OnPaint(g2t_wnd wnd, g2t_dc dc);
g2t_void _TLB_OnKeyDown(g2t_wnd wnd, g2t_long ch);
g2t_long _TLB_OnKillFocus(g2t_wnd wnd);
g2t_void _TLB_OnSetFocus(g2t_wnd wnd);
g2t_void _TLB_OnDestroy(g2t_wnd wnd);
g2t_long _TLB_OnCreate(g2t_wnd wnd);
g2t_long G2TLISTBOXPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);

tlistbox_t* _GLB_FindItemByIndex(g2t_wnd wnd, g2t_int idx)
{
    PTLISTBOX lb = 0;
    g2t_int i = 0;
    tlistbox_t* item = 0;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    if (idx < 0 || idx >= lb->nitems)
    {
        return 0;
    }
    item = lb->firstitem;

    for (i = 0; i < lb->nitems && item; ++i, item = item->next)
    {
        if (i == idx)
        {
            return item;
        }
    }
    return 0;
}

g2t_long _TLB_OnCreate(g2t_wnd wnd)
{
    PTLISTBOX lb = 0;
    /* initial memory for static control */
    lb = (PTLISTBOX) g2t_malloc(sizeof (_TLISTBOX));
    if (!lb)
    {
        return G2T_ERROR;
    }
    lb->firstvisible = -1;
    lb->cursel = -1;
    lb->nitems = 0;
    lb->firstitem = 0;
    lb->lastitem = 0;
    lb->selitem = 0;
    lb->firstvisibleitem = 0;
    lb->checkeditems = 0;
    lb->lastitemchecked = 0;

    g2t_SetWndParam(wnd, (g2t_lparam) lb);
    g2t_SetTextColor(wnd, G2T_RGB_WHITE);
    g2t_SetBgColor(wnd, G2T_RGB_BLUE);

    return G2T_CONTINUE;
}

g2t_void _TLB_OnDestroy(g2t_wnd wnd)
{
    PTLISTBOX lb = 0;
    lb = (PTLISTBOX) g2t_GetWndParam(wnd);

    /* release memory of static control */
    _TLB_OnDeleteAllItems(wnd);

    free(lb);
}

g2t_void _TLB_OnSetFocus(g2t_wnd wnd)
{
    PTLISTBOX lb = 0;
    g2t_nhdr nmhdr;
    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    if (lb->cursel >= lb->nitems)
    {
        lb->cursel = 0;
    }
    else if (lb->cursel < 0)
    {
        lb->cursel = 0;
    }
    g2t_InvalidateWnd(wnd);

    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = GLBN_SETFOCUS;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam) & nmhdr);
}

g2t_long _TLB_OnKillFocus(g2t_wnd wnd)
{
    g2t_nhdr nmhdr;
    g2t_long rc = G2T_CONTINUE;

    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = GLBN_KILLFOCUS;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam) & nmhdr);
    return rc;
}

g2t_void _TLB_OnSelChanged(g2t_wnd wnd)
{
    g2t_nhdr nmhdr;
    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = GLBN_SELCHANGED;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam) & nmhdr);
}

g2t_void _TLB_OnKeyDown(g2t_wnd wnd, g2t_long ch)
{
    PTLISTBOX lb = 0;
    g2t_int repaint = 0;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_int lines = 0;
    g2t_rect rc;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    g2t_GetWndRect(wnd, &rc);
    switch (ch)
    {
    case VK_SPACE:
    {
        if ((style & GLBS_CHECKBOX) ||
                (style & GLBS_RADIOBOX))
        {
            /* get current and check it */
            _TLB_OnSetItemChecked(wnd, lb->cursel, GLB_CHECKED);
        }
        ++repaint;
        break;
    }

    case VK_DOWN:
        /*    case KEY_RIGHT:*/
    {
        ++lines;
        ++repaint;
        break;
    }

    case VK_UP:
        /*    case KEY_LEFT:*/
    {
        --lines;
        ++repaint;
        break;
    }

    case VK_PRIOR:
    {
        lines -= rc.lines;
        ++repaint;
        break;
    }

    case VK_NEXT:
    {
        lines += rc.lines;
        ++repaint;
        break;
    }
    case VK_RETURN:
    {
        g2t_nhdr nmhdr;
        /* send notification */
        nmhdr.id = g2t_GetWndID(wnd);
        nmhdr.ctl = wnd;
        nmhdr.code = GLBN_ENTERITEM;
        g2t_PostMsg(g2t_GetFrameWnd(wnd), GWM_NOTIFY, 0, (g2t_lparam) & nmhdr);
    }
    default:
    {
        break;
    }
    }
    do
    {
        if ( !repaint || (lines > 0 && (lb->cursel+1 == lb->nitems)) ||
            (lb->cursel <= 0 && lines < 0))
        {
            break;
        }
        lb->cursel += lines;
        if (lb->cursel >= lb->nitems)
        {
            lb->cursel = lb->nitems - 1;
        }
        else if (lb->cursel < 0)
        {
            lb->cursel = 0;
        }
        /* find the new first visible */
        if (lb->cursel >= lb->firstvisible + rc.lines)
        {
            lb->firstvisible += lines;
            if (lb->firstvisible > lb->nitems - rc.lines)
            {
                lb->firstvisible = lb->nitems - rc.lines;
            }
        }
        else if (lb->firstvisible > lb->cursel)
        {
            lb->firstvisible += lines;
            if (lb->firstvisible < 0)
            {
                lb->firstvisible = 0;
            }
        }

        /* send notification */
        _TLB_OnSelChanged(wnd);
        /* redraw after changing */
        g2t_InvalidateWnd(wnd);
    } while (0);
    lb->selitem = _GLB_FindItemByIndex(wnd, lb->cursel);
    lb->firstvisibleitem = _GLB_FindItemByIndex(wnd, lb->firstvisible);
}

g2t_void _TLB_OnPaint(g2t_wnd wnd, g2t_dc dc)
{
    PTLISTBOX lb = 0;
    g2t_int i = 0;
    tlistbox_t* item = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    g2t_char text[G2T_MAX_WNDTEXT + 1];
    g2t_dword attrs = g2t_GetWndTextAttrs(wnd);
    g2t_drawitem di;
    g2t_rect rc;
    g2t_dword style = g2t_GetWndStyle(wnd);
    g2t_int lines = 0;
    g2t_int y, x; /* to move cursor */
    g2t_int ysel, xsel;
    g2t_char bufsel[G2T_MAX_WNDTEXT + 1];
    g2t_dword seltext = g2t_GetSysColor(LISTBOX_HIGHLIGHTED_TEXTCOLOR);
    g2t_dword selbg = g2t_GetSysColor(LISTBOX_HIGHLIGHTED_BGCOLOR);
    g2t_dword textcolor = g2t_GetSysColor(LISTBOX_TEXTCOLOR);
    g2t_dword bgcolor = g2t_GetSysColor(LISTBOX_BGCOLOR);
/*    g2t_dword seltext = g2t_GetReverseColor(textcolor);
    g2t_dword selbg = g2t_GetReverseColor(bgcolor);*/

    if (!g2t_IsWndVisible(wnd))
    {
        return;
    }
    lb = (PTLISTBOX) g2t_GetWndParam(wnd);

    /* draw */
    if (lb->nitems > 0)
    {
        item = lb->firstitem;
        g2t_GetWndRect(wnd, &rc);
        lines = rc.lines;

        for (i = 0; i < lb->nitems && item; ++i, item = item->next)
        {
            if (i < lb->firstvisible)
            {
                /* do nothing */
                continue;
            }
            else if (i - lb->firstvisible < lines)
            {
                if (style & GLBS_OWNERDRAW)
                {
                    g2t_memset(&di, 0, sizeof (g2t_drawitem));
                    di.rcitem.y = rc.y + (i - lb->firstvisible);
                    di.rcitem.x = rc.x;
                    di.rcitem.lines = 1;
                    di.rcitem.cols = rc.cols;
                    di.idx = i;

                    g2t_SendMsg(g2t_GetParent(wnd), GWM_DRAWITEM, (g2t_wparam) i, (g2t_lparam) & di);
                }
                else
                {
                    g2t_strcpy(text, "");
                    if (style & GLBS_CHECKBOX)
                    {
                        if (item->checked)
                        {
                            g2t_strcpy(text, "[X] ");
                        }
                        else
                        {
                            g2t_strcpy(text, "[ ] ");
                        }
                    }
                    else if (style & GLBS_RADIOBOX)
                    {
                        if (item->checked)
                        {
                            g2t_strcpy(text, "(*) ");
                        }
                        else
                        {
                            g2t_strcpy(text, "( ) ");
                        }
                    }

                    /* copy from item text */
                    g2t_strcat(text, item->itemtext);
                    g2t_PrintTextAlignment(buf, text, rc.cols, style);

                    if (i == lb->cursel)
                    {
                        ysel = y = rc.y + (i - lb->firstvisible);
                        xsel = x = rc.x;
                        g2t_strcpy(bufsel, buf);
                        g2t_DrawText(dc,
                                     rc.x,
                                     rc.y + (i - lb->firstvisible),
                                     buf,
                                     seltext, selbg);
                    }
                    else
                    {
                        g2t_DrawText(dc,
                                     rc.x,
                                     rc.y + (i - lb->firstvisible),
                                     buf,
                                     textcolor, bgcolor);
                    }
                }
            }/* not owner draw */
        } /* for each item */
        /*draw the selected item */

        g2t_DrawText(dc,
                     xsel,
                     ysel,
                     bufsel,
                     seltext, selbg);
        g2t_MoveXY(x, y);
    } /* items are valid */
}

g2t_long _TLB_OnInsertFirstItem(g2t_wnd wnd, g2t_string text)
{
    PTLISTBOX lb = 0;
    g2t_long len = 0;
    tlistbox_t* item = 0;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    item = (tlistbox_t*) malloc(sizeof (tlistbox_t));
    if (item)
    {
        memset(item, 0, sizeof (tlistbox_t));
        len = strlen(text);
        if (len > G2T_MAX_WNDTEXT)
        {
            len = G2T_MAX_WNDTEXT;
        }
        strncpy(item->itemtext, text, len);

        if (lb->firstitem)
        {
            item->next = lb->firstitem;
            lb->firstitem->prev = item;
            lb->firstitem = item;
        }
        else
        {
            lb->firstitem = lb->lastitem = item;
            lb->firstvisible = 0;
        }
        ++lb->nitems;
        return lb->nitems;
    }
    return G2T_ENOMEM;
}

g2t_long _TLB_OnAddItem(g2t_wnd wnd, g2t_string text)
{
    PTLISTBOX lb = 0;
    g2t_long len = 0;
    tlistbox_t* item = 0;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    item = (tlistbox_t*) malloc(sizeof (tlistbox_t));
    if (item)
    {
        memset(item, 0, sizeof (tlistbox_t));
        len = strlen(text);
        if (len > G2T_MAX_WNDTEXT)
        {
            len = G2T_MAX_WNDTEXT;
        }
        strncpy(item->itemtext, text, len);

        if (lb->firstitem)
        {
            item->prev = lb->lastitem;
            lb->lastitem->next = item;
            lb->lastitem = item;
        }
        else
        {
            lb->firstitem = lb->lastitem = item;
            lb->firstvisible = 0;
        }
        ++lb->nitems;
        return lb->nitems;
    }
    return G2T_ENOMEM;
}

g2t_void _TLB_OnDeleteItem(g2t_wnd wnd, g2t_long idx)
{
    PTLISTBOX lb = 0;
    tlistbox_t* item = 0;
    tlistbox_t* nextitem = 0;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    item = _GLB_FindItemByIndex(wnd, idx);

    if (item)
    {
        /* remove the links */
        nextitem = item->next;
        if (nextitem)
        {
            nextitem->prev = item->prev;
        }
        if (item->prev)
        {
            item->prev->next = nextitem;
        }
        /* check if removed item is the first item or the last item */
        if (item == lb->firstitem)
        {
            lb->firstitem = nextitem;
        }
        else if (item == lb->lastitem)
        {
            lb->lastitem = item->prev;
        }

        /* free the memory */
        item->next = item->prev = 0;
        free(item);

        /* decrement items */
        --lb->nitems;
        if (lb->cursel >= lb->nitems)
        {
            lb->cursel = lb->nitems - 1;
        }
    }
}

g2t_void _TLB_OnDeleteAllItems(g2t_wnd wnd)
{
    PTLISTBOX lb = 0;
    g2t_int nitems = 0;
    g2t_int i = 0;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    nitems = lb->nitems;
    for (i = 0; i < nitems; ++i)
    {
        g2t_PostMsg(wnd, GLBM_DELETEITEM, 0, 0);
    }
}

g2t_int _TLB_OnGetCurSel(g2t_wnd wnd)
{
    PTLISTBOX lb = 0;
    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    return lb->cursel;
}

g2t_long _TLB_OnGetItemText(g2t_wnd wnd, g2t_int idx, g2t_string text)
{
    tlistbox_t* item = 0;

    item = _GLB_FindItemByIndex(wnd, idx);

    if (item)
    {
        strcpy(text, item->itemtext);
    }
    return strlen(text);
}

g2t_void _TLB_OnSetItemText(g2t_wnd wnd, g2t_int idx, g2t_string text)
{
    tlistbox_t* item = 0;

    item = _GLB_FindItemByIndex(wnd, idx);
    if (item)
    {
        strcpy(item->itemtext, text);
    }
}

g2t_long _TLB_OnGetItemCount(g2t_wnd wnd)
{
    PTLISTBOX lb = 0;
    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    return lb->nitems;
}

g2t_void _TLB_OnSetCurSel(g2t_wnd wnd, g2t_int idx)
{
    PTLISTBOX lb = 0;
    g2t_rect rc;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);

    if (idx < 0 || idx >= lb->nitems)
    {
        idx = -1;
    }
    lb->cursel = idx;

    if (lb->cursel >= 0)
    {
        lb->firstvisible = lb->cursel;
        g2t_GetWndRect(wnd, &rc);
        if (lb->firstvisible + rc.lines > lb->nitems)
        {
            lb->firstvisible = lb->nitems - rc.lines;
        }
    }
    else
    {
        lb->firstvisible = 0;
    }

    /* send notification */
    _TLB_OnSelChanged(wnd);
    /* redraw after changing */
    g2t_InvalidateWnd(wnd);

    lb->selitem = _GLB_FindItemByIndex(wnd, lb->cursel);
    lb->firstvisibleitem = _GLB_FindItemByIndex(wnd, lb->firstvisible);
}

g2t_void _TLB_OnSetItemData(g2t_wnd wnd, g2t_int idx, g2t_lparam data)
{
    tlistbox_t* item = 0;

    item = _GLB_FindItemByIndex(wnd, idx);
    if (item)
    {
        item->data = data;
    }
}

g2t_lparam _TLB_OnGetItemData(g2t_wnd wnd, g2t_int idx)
{
    tlistbox_t* item = 0;

    item = _GLB_FindItemByIndex(wnd, idx);
    if (item)
    {
        return item->data;
    }
    return 0;
}

g2t_int _TLB_OnSetItemChecked(g2t_wnd wnd, g2t_int idx, g2t_int check)
{
    tlistbox_t* item = 0;
    PTLISTBOX lb = 0;
    g2t_dword style = g2t_GetWndStyle(wnd);

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    item = _GLB_FindItemByIndex(wnd, idx);
    /* found and do the check item state */
    if (item)
    {
        if (check == GLB_UNCHECKED)
        {
            --lb->checkeditems;
            if (lb->checkeditems < 0)
            {
                lb->checkeditems = 0;
                lb->lastitemchecked = 0;
            }
            item->checked = GLB_UNCHECKED;
        }
        else
        {
            /* unselected the last if it is radio style */
            if (style & GLBS_RADIOBOX)
            {
                if (lb->lastitemchecked)
                {
                    lb->lastitemchecked->checked = GLB_UNCHECKED;
                }
            }
            else if (style & GLBS_CHECKBOX)
            {
                if (item->checked == GLB_CHECKED)
                {
                    --lb->checkeditems;
                    if (lb->checkeditems < 0)
                    {
                        lb->checkeditems = 0;
                        lb->lastitemchecked = 0;
                    }
                    item->checked = GLB_UNCHECKED;
                    return GLB_OK;
                }
            }
            /* count checked item */
            ++lb->checkeditems;
            if (lb->checkeditems > lb->nitems)
            {
                lb->checkeditems = lb->nitems;
            }
            /* checked and save the last checked item */
            item->checked = GLB_CHECKED;
            lb->lastitemchecked = item;
        }
        return GLB_OK;
    }
    return GLB_ERROR;
}

g2t_int _TLB_OnGetItemChecked(g2t_wnd wnd, g2t_int idx)
{
    tlistbox_t* item = 0;

    item = _GLB_FindItemByIndex(wnd, idx);
    if (item)
    {
        return item->checked;
    }
    return GLB_ERROR;
}

g2t_int _TLB_OnCountItemCheck(g2t_wnd wnd)
{
    PTLISTBOX lb = 0;

    lb = (PTLISTBOX) g2t_GetWndParam(wnd);
    return lb->checkeditems;
}

g2t_long G2TLISTBOXPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    switch (msg)
    {
    case GLBM_COUNTITEMCHECKED:
    {
        return _TLB_OnCountItemCheck(wnd);
    }
    case GLBM_GETITEMCHECKED:
    {
        return _TLB_OnGetItemChecked(wnd, (g2t_int) wparam);
    }
    case GLBM_SETITEMCHECKED:
    {
        return _TLB_OnSetItemChecked(wnd, (g2t_int) wparam, (g2t_int) lparam);
    }
    case GWM_CREATE:
    {
        return _TLB_OnCreate(wnd);
    }
    case GWM_DESTROY:
    {
        _TLB_OnDestroy(wnd);
        return 0;
    }
    case GWM_SETFOCUS:
    {
        _TLB_OnSetFocus(wnd);
        break;
    }
    case GWM_KILLFOCUS:
    {
        return _TLB_OnKillFocus(wnd);
    }
    case GWM_KEYDOWN:
    {
        _TLB_OnKeyDown(wnd, (g2t_long) wparam);
        break;
    }
    case GWM_PAINT:
    {
        _TLB_OnPaint(wnd, g2t_GetMemDC());
        return 0;
    }
    case GLBM_INSERTFIRSTITEM:
    {
        return _TLB_OnInsertFirstItem(wnd, (g2t_string) lparam);
    }
    case GLBM_ADDITEM:
    {
        return _TLB_OnAddItem(wnd, (g2t_string) lparam);
    }
    case GLBM_DELETEITEM:
    {
        _TLB_OnDeleteItem(wnd, (g2t_long) wparam);
        return 0;
    }
    case GLBM_GETCURSEL:
    {
        return _TLB_OnGetCurSel(wnd);
    }
    case GLBM_SETCURSEL:
    {
        _TLB_OnSetCurSel(wnd, (g2t_int) wparam);
        return 0;
    }
    case GLBM_DELETEALLITEMS:
    {
        _TLB_OnDeleteAllItems(wnd);
        return 0;
    }
    case GLBM_GETITEMCOUNT:
    {
        return _TLB_OnGetItemCount(wnd);
    }
    case GLBM_SETITEMDATA:
    {
        _TLB_OnSetItemData(wnd, (g2t_int) wparam, (g2t_lparam) lparam);
        return 0;
    }
    case GLBM_GETITEMDATA:
    {
        return (g2t_long) _TLB_OnGetItemData(wnd, (g2t_int) wparam);
    }
    case GLBM_SETITEMTEXT:
    {
        _TLB_OnSetItemText(wnd, (g2t_int) wparam, (g2t_string) lparam);
        return 0;
    }
    case GLBM_GETITEMTEXT:
    {
        return _TLB_OnGetItemText(wnd, (g2t_int) wparam, (g2t_string) lparam);
    }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}

