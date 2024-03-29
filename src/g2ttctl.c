/*-------------------------------------------------------------------
 * File name: tuitctl.c
 * Author: Seree Rakwong
 * Date: 25-FEB-19
 *-----------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "g2t.h"
#include "tui_tree.h"
#include "tui_queue.h"
#include "tui_list.h"
#include "tui_stack.h"

/*-------------------------------------------------------------------
 * TREECTRL functions
 *-----------------------------------------------------------------*/
struct _TTREECTRLSTRUCT
{
    ttree_t* tree; /* see also tui_tree.h */
    LPTREEFINDITEMPROC findproc; /* to compare item */
    TTREEITEM* firstvisibleitem; /* the first item is visible in the tree control */
    TTREEITEM* lastvisibleitem; /* the first item is visible in the tree control */
    TTREEITEM* curselitem; /* the current selected item */
    tlist_t* visibleitems;
    TINT indent;
    TINT shifted_right;
    TINT shifted;
};
typedef struct _TTREECTRLSTRUCT _TTREECTRLSTRUCT;
typedef struct _TTREECTRLSTRUCT TTREECTRLSTRUCT;
typedef struct _TTREECTRLSTRUCT *TPTREECTRLSTRUCT;

TVOID _TTC_OnPaint(TWND wnd, TDC dc);
TLONG _TTC_OnCreate(TWND wnd);
TLONG G2TTREECTRLPROC(TWND wnd, TUINT msg, TWPARAM wparam, TLPARAM lparam);
TTREEITEM* _TTC_OnInsertItem(TWND wnd, TTREEITEM* parentitem, TTREEITEMDATA* insertitem);
TLONG _TTC_OnDeleteItem(TWND wnd, TTREEITEM* item);
TLONG _TTC_OnSetItem(TWND wnd, TTREEITEM* item, TTREEITEMDATA* setitem);
TLONG _TTC_OnGetItem(TWND wnd, TTREEITEM* item, TTREEITEMDATA* getitem);
TTREEITEM* _TTC_OnFindItem(TWND wnd, TTREEITEMDATA* finditem);
TTREEITEM* _TTC_OnFindNextItem(TWND wnd, TTREEITEM* previtem, TTREEITEMDATA* finditem);
TLONG _TTC_OnExpandItem(TWND wnd, TTREEITEM* item);
TLONG _TTC_OnCollapseItem(TWND wnd, TTREEITEM* item);
TVOID _TTC_OnKeyDown(TWND wnd, TLONG ch);
TVOID _TTC_OnDestroy(TWND wnd);
TVOID _TTC_OnSetFocus(TWND wnd);
TLONG _TTC_OnKillFocus(TWND wnd);
LPTREEFINDITEMPROC _TTC_OnSetFindItemProc(TWND wnd, LPTREEFINDITEMPROC findproc);
TLONG _TTC_OnSetSelItem(TWND wnd, TTREEITEM* setitem);
TTREEITEM* _TTC_OnGetSelItem(TWND wnd);
TLONG _TTC_OnExportToFile(TWND wnd, FILE* fp, LPTREEEXPORTPROC prnproc);
TLONG _TTC_OnImportFromFile(TWND wnd, FILE* fp, LPTREEIMPORTPROC impproc);
TVOID _TTC_OnExpandAllItems(TWND wnd);
TVOID _TTC_OnCollapseAllItems(TWND wnd);
TINT _TTC_OnGetIndentText(TWND wnd);
TVOID _TTC_OnSetIndentText(TWND wnd, TINT indent);
TINT _TTC_OnGetShiftedText(TWND wnd);
TVOID _TTC_OnSetShiftedText(TWND wnd, TINT shifted);


TVOID _TTC_FreshView(TWND wnd);
/*typedef tui_i32         (*fn_tree_compare_proc)(const tui_void*, const tui_void*);*/
tui_i32 _TTC_DefFindItemProc(const tui_void* datap, const tui_void* itemp);
TVOID _TTC_GetDisplayedText(
                            TWND wnd,
                            TUI_CHAR* outtext, TINT* startx, TINT* endx,
                            TTREEITEM* item,
                            TINT maxlen,
                            TBOOL file);

TVOID _TTC_AdjustVisibleItems(TWND wnd);
TTREEITEM* _TTC_MoveNext(TWND wnd, TLONG times);
TTREEITEM* _TTC_MovePrev(TWND wnd, TLONG times);

struct _TTREEVIEWITEM
{
    TTREEITEM* item;
};
typedef struct _TTREEVIEWITEM TTREEVIEWITEM;
/*typedef tui_i32         (*fn_tree_traverse_proc)(tui_void*, tree_iter_t, const tui_void*, tui_ui32);*/
tui_i32 _TTC_PreorderTraverseProc(tui_void* args, tree_iter_t item, const tui_void* node, tui_ui32 size);
tui_i32 _TTC_ExpandAllItemsProc(tui_void* args, tree_iter_t item, const tui_void* node, tui_ui32 size);
tui_i32 _TTC_CollapseAllItemsProc(tui_void* args, tree_iter_t item, const tui_void* node, tui_ui32 size);

/* helper functions */
tui_i32 _TTC_DefFindItemProc(const tui_void* datap, const tui_void* itemp)
{
    /* this function compare the item text */
    TTREEITEMDATA* dataitem = (TTREEITEMDATA*) datap;
    TTREEITEMDATA* treeitem = (TTREEITEMDATA*) itemp;
    return memcmp(dataitem->itemtext, treeitem->itemtext, TUI_MAX_WNDTEXT);
}

TLONG _TTC_OnImportFromFile(TWND wnd, FILE* fp, LPTREEIMPORTPROC proc)
{
    TUI_CHAR buf[BUFSIZ + 1];
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEMDATA data;
    TTREEVIEWITEM view;
    tstack_t* stack = 0;
    TTREEITEM* parent = 0;
    TINT tabs = 0;
    TUI_CHAR* psz = 0;
    TINT items = 0;
    TTREEITEM* newitem = 0;
    TINT len = 0;

    if (!fp)
    {
        /* nothing to do */
        return -1;
    }
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    parent = tc->tree->GetRootItem(tc->tree);

    /**/
    stack = Stack_Create(512, 0);
    view.item = parent;
    stack->Push(stack, &view, sizeof (view));

    while (!feof(fp))
    {
        if (!fgets(buf, BUFSIZ, fp))
        {
            break;
        }
        /* comment */
        if (buf[0] == 0 || buf[0] == '#' || buf[0] == '\n')
        {
            continue;
        }
        /* look up the parent item */
        tabs = 0;
        psz = buf;
        while (psz)
        {
            if (*psz == '\t')
            {
                ++tabs;
                ++psz;
            }
            else
            {
                break;
            }
        }
        /* tabs are required */
        if (0 == tabs)
        {
            continue;
        }
        len = strlen(psz);
        if (len > 0 && psz[len - 1] == '\n')
        {
            psz[len - 1] = 0;
        }
        /* insert the new item */
        items = stack->Count(stack);
        if (tabs > items)
        {
            /* insert to the next parent */
            if (tabs > items + 1)
            {
                /* invalid item */
                continue;
            }
            stack->Top(stack, &view, sizeof (view));
            parent = view.item;
        }
        else if (tabs < items)
        {
            /* pop the stack */
            while (items >= tabs)
            {
                stack->Pop(stack);
                items = stack->Count(stack);
            }
            if (stack->IsEmpty(stack))
            {
                parent = 0;
            }
            else
            {
                stack->Top(stack, &view, sizeof (view));
                parent = view.item;
            }
        }
        else
        {
            /* same level */
            stack->Pop(stack);
            if (stack->IsEmpty(stack))
            {
                parent = 0;
            }
            else
            {
                stack->Top(stack, &view, sizeof (view));
                parent = view.item;
            }
        }
        /* format text or data */
        memset(&data, 0, sizeof (data));
        if (proc)
        {
            if (proc(&data, psz) != 0)
            {
                /* sometimes input may not required by user */
                continue;
            }
        }
        else
        {
            /* no procedure introduced */
            strcpy(data.itemtext, psz);
        }

        newitem = TTC_InsertItem(wnd, parent, &data);

        view.item = newitem;
        stack->Push(stack, &view, sizeof (view));

    }
    Stack_Destroy(stack);
    return 0;
}

TLONG _TTC_OnExportToFile(TWND wnd, FILE* fp, LPTREEEXPORTPROC prnproc)
{
    tqueue_t* queue = 0;
    TUI_CHAR buf[BUFSIZ + 1];
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEMDATA data;
    TTREEVIEWITEM view;
    TTREEITEM* root = 0;
    TINT startx = 0, endx = 0;

    if (!fp)
    {
        /* nothing to do */
        return -1;
    }
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    /* visible window & there is a visible item */
    if (!tc->firstvisibleitem)
    {
        return 0;
    }
    /* populate all items */
    queue = Queue_Create(0);
    root = tc->tree->GetRootItem(tc->tree);
    tc->tree->Populate(tc->tree,
                       tc->tree->GetFirstChild(root),
                       (tui_void*) queue, _TTC_PreorderTraverseProc, TPO_PRE);

    /* print all items to file */
    while (!queue->IsEmpty(queue))
    {
        queue->First(queue, &view, sizeof (view));
        queue->Dequeue(queue);

        tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));

        memset(buf, 0, sizeof (buf));
        _TTC_GetDisplayedText(wnd, buf, &startx, &endx, view.item, BUFSIZ - 1, TUI_TRUE);

        if (prnproc)
        {
            if (prnproc(fp, &data) != 0)
            {
                return 1;
            }
        }
        else
        {
            fprintf(fp, "%s\n", buf);
        }
    }
    /* release memory */
    Queue_Destroy(queue);
    return 0;
}

TVOID _TTC_AdjustVisibleItems(TWND wnd)
{
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEMDATA data;
    TTREEVIEWITEM view;
    TTREEITEM* root = 0;
    TTREEITEM* next_item = 0;
    tqueue_t* queue;
    tqueue_t* subqueue;
    tlist_t* items = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    /* visible window & there is a visible item */
    if (!tc->firstvisibleitem)
    {
        return;
    }

    items = List_Create(0);

    queue = Queue_Create(0);
    root = tc->tree->GetRootItem(tc->tree);
    tc->tree->Populate(tc->tree,
                       tc->tree->GetFirstChild(root),
                       queue, _TTC_PreorderTraverseProc, TPO_PRE);

    while (!queue->IsEmpty(queue))
    {
        queue->First(queue, &view, sizeof (view));
        queue->Dequeue(queue);

        items->InsertLast(items, &view, sizeof (view));

        /* selected item */
        tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));

        /* has data but not expanded */
        if (data.children > 0 && !data.expanded)
        {
            next_item = tc->tree->GetNextItem(view.item);

            /* skip its children */
            subqueue = Queue_Create(0);
            tc->tree->Populate(tc->tree,
                               view.item,
                               subqueue, _TTC_PreorderTraverseProc, TPO_PRE);

            subqueue->Dequeue(subqueue);
            while (!subqueue->IsEmpty(subqueue))
            {
                subqueue->First(subqueue, &view, sizeof (view));
                subqueue->Dequeue(subqueue);
                if (view.item == next_item)
                {
                    break;
                }
                queue->Dequeue(queue);
            }
            Queue_Destroy(subqueue);
        }
    }
    Queue_Destroy(queue);

    /* assign to the visible items */
    if (items->Count(items) > 0)
    {
        if (tc->visibleitems)
        {
            List_Destroy(tc->visibleitems);
        }
        tc->visibleitems = items;
    }
    else
    {
        List_Destroy(items);
    }
}

TVOID _TTC_FreshView(TWND wnd)
{
    /* refresh update if the new item or deleting item is in the visible bound */
    g2t_InvalidateWnd(wnd);
}

TTREEITEM* _TTC_MoveNext(TWND wnd, TLONG move_times)
{
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEMDATA data;
    list_iter_t iter = 0;
    TTREEVIEWITEM view;
    TLONG i = 0;
    TINT items = 0;
    TINT firstindex = 0;
    TINT curindex = 0;
    list_iter_t first_iter = 0;
    list_iter_t cur_iter = 0;
    TRECT rc;
    TINT first_moves = 0;
    TINT cur_moves = 0;
    TBOOL movefirst = TUI_TRUE;

    g2t_GetWndRect(wnd, &rc);
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    items = tc->visibleitems->Count(tc->visibleitems);

    /* change selection item */
    iter = tc->visibleitems->Begin(tc->visibleitems);
    /* find the first index */
    while (iter != tc->visibleitems->End(tc->visibleitems))
    {
        tc->visibleitems->GetItemData(iter, &view, sizeof (view));
        tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));
        if (view.item == tc->firstvisibleitem)
        {
            first_iter = iter;
            break;
        }
        iter = tc->visibleitems->GetNext(iter);
        ++firstindex;
    }
    /* find the current selection index */
    curindex = firstindex;
    while (iter != tc->visibleitems->End(tc->visibleitems))
    {
        tc->visibleitems->GetItemData(iter, &view, sizeof (view));
        tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));
        if (view.item == tc->curselitem)
        {
            cur_iter = iter;
            break;
        }
        iter = tc->visibleitems->GetNext(iter);
        ++curindex;
    }

    if (curindex == items - 1)
    {
        /* no move required the last item */
        return tc->curselitem;
    }

    /* un-highlighting the old item */
    tc->tree->GetItemData(tc->curselitem, &data, sizeof (data));
    data.selected = 0;
    tc->tree->SetItemData(tc->curselitem, &data, sizeof (data));

    /* highlighting the new item */
    iter = cur_iter;
    cur_moves = TUI_MAX(0, TUI_MIN(move_times, items - curindex - 1));
    for (i = 0; i < cur_moves && iter != tc->visibleitems->End(tc->visibleitems); ++i)
    {
        iter = tc->visibleitems->GetNext(iter);
    }
    tc->visibleitems->GetItemData(iter, &view, sizeof (view));
    tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));
    tc->curselitem = view.item;
    tc->tree->GetItemData(tc->curselitem, &data, sizeof (data));
    data.selected = 1;
    tc->tree->SetItemData(tc->curselitem, &data, sizeof (data));

    /* is the new index out-of-bound visible items? */
    if ((curindex + cur_moves) < (firstindex + rc.lines))
    {
        /* no move first */
        movefirst = TUI_FALSE;
    }
    if ((curindex + cur_moves) >= items)
    {
        movefirst = TUI_FALSE;
    }

    if (movefirst)
    {
        /* how actual many movements */
        first_moves = TUI_MIN(cur_moves, items - move_times);
        iter = first_iter;
        for (i = 0; i < first_moves && iter != tc->visibleitems->End(tc->visibleitems); ++i)
        {
            iter = tc->visibleitems->GetNext(iter);
        }
        tc->visibleitems->GetItemData(iter, &view, sizeof (view));
        tc->firstvisibleitem = view.item;
    }
    return tc->curselitem;
}

TTREEITEM* _TTC_MovePrev(TWND wnd, TLONG move_times)
{
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEMDATA data;
    list_iter_t iter = 0;
    TTREEVIEWITEM view;
    TLONG i = 0;
    TINT firstvisibleindex = 0;
    TINT curindex = 0;
    list_iter_t first_iter = 0;
    list_iter_t cur_iter = 0;
    TRECT rc;
    TINT first_moves = 0;
    TINT cur_moves = 0;
    TBOOL movefirst = TUI_FALSE;

    g2t_GetWndRect(wnd, &rc);
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    /* change selection item */
    /* find the current selection index */
    firstvisibleindex = 0;
    iter = tc->visibleitems->Begin(tc->visibleitems);
    while (iter != tc->visibleitems->End(tc->visibleitems))
    {
        tc->visibleitems->GetItemData(iter, &view, sizeof (view));
        tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));
        if (view.item == tc->firstvisibleitem)
        {
            first_iter = iter;
            break;
        }
        iter = tc->visibleitems->GetNext(iter);
        ++firstvisibleindex;
    }
    if (0 == first_iter)
    {
        return tc->curselitem;
    }

    /* find the current index */
    cur_iter = first_iter;
    curindex = firstvisibleindex;
    while (iter != tc->visibleitems->End(tc->visibleitems))
    {
        tc->visibleitems->GetItemData(iter, &view, sizeof (view));
        tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));
        if (view.item == tc->curselitem)
        {
            cur_iter = iter;
            break;
        }
        iter = tc->visibleitems->GetNext(iter);
        ++curindex;
    }

    /* un-highlighting the old item */
    tc->tree->GetItemData(tc->curselitem, &data, sizeof (data));
    data.selected = TUI_FALSE;
    tc->tree->SetItemData(tc->curselitem, &data, sizeof (data));

    /* highlighting the new item */
    iter = cur_iter;
    cur_moves = 0;
    for (i = 0; i < move_times && iter != 0; ++i, ++cur_moves)
    {
        iter = tc->visibleitems->GetPrev(iter);
    }

    if (firstvisibleindex > 0 && curindex - cur_moves < firstvisibleindex)
    {
        movefirst = TUI_TRUE;
    }
    else if (0 == firstvisibleindex && cur_moves >= curindex)
    {
        iter = tc->visibleitems->Begin(tc->visibleitems);
    }

    tc->visibleitems->GetItemData(iter, &view, sizeof (view));
    tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));
    tc->curselitem = view.item;
    tc->tree->GetItemData(tc->curselitem, &data, sizeof (data));
    data.selected = TUI_TRUE;
    tc->tree->SetItemData(tc->curselitem, &data, sizeof (data));

    if (movefirst)
    {
        /* how actual many movements */
        first_moves = TUI_MIN(cur_moves, move_times);
        iter = first_iter;
        for (i = 0; i < first_moves && iter != tc->visibleitems->Begin(tc->visibleitems); ++i)
        {
            iter = tc->visibleitems->GetPrev(iter);
        }
        tc->visibleitems->GetItemData(iter, &view, sizeof (view));
        tc->firstvisibleitem = view.item;
    }
    return tc->curselitem;
}

TVOID _TTC_OnKeyDown(TWND wnd, TLONG ch)
{
    TTREECTRLSTRUCT* tc = 0;
    TRECT rc;
    TTREEITEMDATA data;
    TTREEITEM* selitem = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    if (0 == tc->visibleitems)
    {
        /* no item */
        return;
    }
    g2t_GetWndRect(wnd, &rc);

    switch (ch)
    {
    case VK_SPACE:
    {
        /* expand or collapse the current selection item */
        tc->tree->GetItemData(tc->curselitem, &data, sizeof (data));
        if (data.children > 0)
        {
            /* toggle item */
            if (data.expanded)
            {
                _TTC_OnCollapseItem(wnd, tc->curselitem);
            }
            else
            {
                _TTC_OnExpandItem(wnd, tc->curselitem);
            }
        }
        break;
    }

    case VK_UP:
    {
        /* check if the last visible item */
        selitem = _TTC_MovePrev(wnd, 1);
        /* notify select item */
        _TTC_OnSetSelItem(wnd, selitem);
        break;
    }

    case VK_DOWN:
    {
        selitem = _TTC_MoveNext(wnd, 1);
        _TTC_OnSetSelItem(wnd, selitem);
        break;
    }

    case VK_NEXT:
    {
        selitem = _TTC_MoveNext(wnd, rc.lines);
        _TTC_OnSetSelItem(wnd, selitem);
        break;
    }

        /* move up 1 page */
    case VK_PRIOR:
    {
        selitem = _TTC_MovePrev(wnd, rc.lines);
        _TTC_OnSetSelItem(wnd, selitem);
        break;
    }

    case VK_RIGHT:
    {
        tc->shifted_right = TUI_MIN(rc.cols, tc->shifted_right + tc->shifted);
        /* update item on the screen */
        _TTC_FreshView(wnd);
        break;
    }

    case VK_LEFT:
    {
        tc->shifted_right = TUI_MAX(0, tc->shifted_right - tc->shifted);
        /* update item on the screen */
        _TTC_FreshView(wnd);
        break;
    }

    default:
    {
        break;
    }
    }
}

LPTREEFINDITEMPROC _TTC_OnSetFindItemProc(TWND wnd, LPTREEFINDITEMPROC findproc)
{
    LPTREEFINDITEMPROC oldproc = 0;
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    oldproc = tc->findproc;
    tc->findproc = findproc;
    return oldproc;
}

TVOID _TTC_OnDestroy(TWND wnd)
{
    TTREECTRLSTRUCT* tc = 0;
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    List_Destroy(tc->visibleitems);
    Tree_Destroy(tc->tree);
    free(tc);
}

TVOID _TTC_OnSetFocus(TWND wnd)
{
    TNMHDR nmhdr;

    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = TTCN_SETFOCUS;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);
}

TLONG _TTC_OnKillFocus(TWND wnd)
{
    TLONG rc = TUI_CONTINUE;
    TNMHDR nmhdr;

    /* send notification */
    nmhdr.id = g2t_GetWndID(wnd);
    nmhdr.ctl = wnd;
    nmhdr.code = TTCN_KILLFOCUS;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);

    return rc;
}

TTREEITEM* _TTC_OnInsertItem(TWND wnd, TTREEITEM* parentitem, TTREEITEMDATA* insertitem)
{
    TTREECTRLSTRUCT* tc = 0;
    TLONG children = 0;
    TTREEITEM* newitem = 0;
    TTREEITEMDATA parentdata;

    if (0 == insertitem)
    {
        return 0;
    }

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    /* get a number of children before inserting the new one */
    children = tc->tree->CountItem(tc->tree);
    if (0 == children)
    {
        insertitem->selected = 1;
    }
    /* insert the new item */
    newitem = (TTREEITEM*) tc->tree->InsertItem(
                                                tc->tree,
                                                parentitem,
                                                insertitem,
                                                sizeof (TTREEITEMDATA));
    if (newitem)
    {
        /* set the first visible item if need */
        if (0 == children)
        {
            tc->firstvisibleitem = tc->curselitem = (TTREEITEM*) newitem;
        }
        /* update parent */
        if (parentitem)
        {
            tc->tree->GetItemData(parentitem, &parentdata, sizeof (parentdata));
            ++parentdata.children;
            tc->tree->SetItemData(parentitem, &parentdata, sizeof (parentdata));
        }
        /* update item on the screen */
        _TTC_FreshView(wnd);
    }

    return (TTREEITEM*) newitem;
}

TLONG _TTC_OnDeleteItem(TWND wnd, TTREEITEM* item)
{
    TTREECTRLSTRUCT* tc = 0;
    TLONG children = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    tc->tree->DeleteItem(tc->tree, item);
    /* update item on the screen */
    _TTC_FreshView(wnd);

    children = tc->tree->CountItem(tc->tree);
    /* set non-visible if need */
    if (0 == children)
    {
        tc->firstvisibleitem = tc->lastvisibleitem = tc->curselitem = 0;
    }

    return 0;
}

TLONG _TTC_OnExpandItem(TWND wnd, TTREEITEM* item)
{
    TNMHDRTREEITEMCTRL nmhdr;
    TTREEITEMDATA data;
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    nmhdr.hdr.id = g2t_GetWndID(wnd);
    nmhdr.hdr.ctl = wnd;
    nmhdr.hdr.code = TTCN_ITEMEXPANDING;
    nmhdr.item = item;
    g2t_SendMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);

    /* mark expanded */
    tc->tree->GetItemData(item, &data, sizeof (TTREEITEMDATA));
    if (data.children > 0)
    {
        data.expanded = TUI_TRUE;
    }
    tc->tree->SetItemData(item, &data, sizeof (TTREEITEMDATA));


    nmhdr.hdr.code = TTCN_ITEMEXPANDED;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);

    /* update item on the screen */
    _TTC_FreshView(wnd);
    return 0;
}

TINT _TTC_OnGetIndentText(TWND wnd)
{
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    return tc->indent;
}

TVOID _TTC_OnSetIndentText(TWND wnd, TINT indent)
{
    TTREECTRLSTRUCT* tc = 0;
    TRECT rc;

    g2t_GetWndRect(wnd, &rc);
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    if (indent < 2)
    {
        indent = 2;
    }
    else if (indent >= rc.cols)
    {
        indent = rc.cols - 1;
    }
    tc->indent = indent;
}

TINT _TTC_OnGetShiftedText(TWND wnd)
{
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    return tc->shifted;
}

TVOID _TTC_OnSetShiftedText(TWND wnd, TINT shifted)
{
    TTREECTRLSTRUCT* tc = 0;
    TRECT rc;

    g2t_GetWndRect(wnd, &rc);
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    if (shifted < 1)
    {
        shifted = 1;
    }
    else if (shifted >= rc.cols)
    {
        shifted = rc.cols - 1;
    }
    tc->shifted = shifted;
}

TVOID _TTC_OnExpandAllItems(TWND wnd)
{
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEM* root = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    root = tc->tree->GetRootItem(tc->tree);
    tc->tree->Populate(tc->tree, tc->tree->GetFirstChild(root),
                       wnd, _TTC_ExpandAllItemsProc, TPO_PRE);

    _TTC_OnExpandItem(wnd, root);
}

TVOID _TTC_OnCollapseAllItems(TWND wnd)
{
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEM* root = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    root = tc->tree->GetRootItem(tc->tree);
    tc->tree->Populate(tc->tree, tc->tree->GetFirstChild(root),
                       wnd, _TTC_CollapseAllItemsProc, TPO_PRE);

    _TTC_OnCollapseItem(wnd, root);
}

TLONG _TTC_OnCollapseItem(TWND wnd, TTREEITEM* item)
{
    TNMHDRTREEITEMCTRL nmhdr;
    TTREEITEMDATA data;
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    nmhdr.hdr.id = g2t_GetWndID(wnd);
    nmhdr.hdr.ctl = wnd;
    nmhdr.hdr.code = TTCN_ITEMCOLLAPSING;
    nmhdr.item = item;
    g2t_SendMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);

    /* mark collapse */
    tc->tree->GetItemData(item, &data, sizeof (TTREEITEMDATA));
    if (data.children > 0)
    {
        data.expanded = TUI_FALSE;
    }
    tc->tree->SetItemData(item, &data, sizeof (TTREEITEMDATA));

    nmhdr.hdr.code = TTCN_ITEMCOLLAPSED;
    g2t_PostMsg(g2t_GetFrameWnd(wnd), TWM_NOTIFY, 0, (TLPARAM) & nmhdr);

    /* update item on the screen */
    _TTC_FreshView(wnd);
    return 0;
}

TLONG _TTC_OnSetSelItem(TWND wnd, TTREEITEM* setitem)
{
    TTREECTRLSTRUCT* tc = 0;
    TTREEITEMDATA data;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    /* remove the highlighting */
    if (tc->curselitem)
    {
        tc->tree->GetItemData(tc->curselitem, &data, sizeof (data));
        data.selected = TUI_FALSE;
        tc->tree->SetItemData(tc->curselitem, &data, sizeof (data));
    }

    /* new selected item */
    tc->curselitem = setitem;

    /* set the highlighting */
    if (tc->curselitem)
    {
        tc->tree->GetItemData(tc->curselitem, &data, sizeof (data));
        data.selected = TUI_TRUE;
        tc->tree->SetItemData(tc->curselitem, &data, sizeof (data));
    }
    /* update item on the screen */
    _TTC_FreshView(wnd);
    return 0;
}

TTREEITEM* _TTC_OnGetSelItem(TWND wnd)
{
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    return tc->curselitem;
}

TLONG _TTC_OnSetItem(TWND wnd, TTREEITEM* item, TTREEITEMDATA* setitem)
{
    TTREECTRLSTRUCT* tc = 0;

    if (!item)
    {
        return -1;
    }
    if (!setitem)
    {
        return -2;
    }

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    tc->tree->SetItemData(item, setitem, sizeof (TTREEITEMDATA));
    return 0;
}

TLONG _TTC_OnGetItem(TWND wnd, TTREEITEM* item, TTREEITEMDATA* getitem)
{
    TTREECTRLSTRUCT* tc = 0;
    if (!item)
    {
        return -1;
    }
    if (!getitem)
    {
        return -2;
    }
    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    tc->tree->GetItemData(item, getitem, sizeof (TTREEITEMDATA));
    return 0;
}

TTREEITEM* _TTC_OnFindItem(TWND wnd, TTREEITEMDATA* finditem)
{
    TTREECTRLSTRUCT* tc = 0;
    tree_iter_t founditem = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    founditem = tc->tree->FindItem(tc->tree,
                                   finditem,
                                   (tc->findproc ? tc->findproc : _TTC_DefFindItemProc));

    return (TTREEITEM*) founditem;
}

TTREEITEM* _TTC_OnFindNextItem(TWND wnd, TTREEITEM* previtem, TTREEITEMDATA* finditem)
{
    TTREECTRLSTRUCT* tc = 0;
    tree_iter_t founditem = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    founditem = tc->tree->FindNextItem(tc->tree,
                                       previtem,
                                       finditem,
                                       (tc->findproc ? tc->findproc : _TTC_DefFindItemProc));

    return (TTREEITEM*) founditem;
}

TLONG _TTC_OnCreate(TWND wnd)
{
    TTREECTRLSTRUCT* tc = (TTREECTRLSTRUCT*) malloc(sizeof (_TTREECTRLSTRUCT));
    if (!tc)
    {
        return TUI_MEM;
    }
    memset(tc, 0, sizeof (_TTREECTRLSTRUCT));
    tc->indent = 2;
    tc->shifted_right = 0;
    tc->shifted = 4;

    tc->tree = Tree_Create(0);
    if (!tc->tree)
    {
        free(tc);
        return TUI_MEM;
    }
    /* set tree */
    g2t_SetWndParam(wnd, (g2t_lparam)tc);
    g2t_SetTextColor(wnd, G2T_RGB_WHITE);
    g2t_SetBgColor(wnd, G2T_RGB_BLUE);

    return TUI_CONTINUE;
}

TVOID _TTC_GetDisplayedText(
                            TWND wnd,
                            TUI_CHAR* outtext, TINT* startx, TINT* endx,
                            TTREEITEM* item,
                            TINT maxlen,
                            TBOOL file)
{
    TINT xpos = 0;
    TTREEITEMDATA data;
    TTREECTRLSTRUCT* tc = 0;
    TDWORD children = 0;
    TUI_CHAR buf[BUFSIZ + 1];
    TINT shifted_right = 0;
    TINT textlen = 0;
    TDWORD style = g2t_GetWndStyle(wnd);
    TLONG len = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    tc->tree->GetItemData(item, &data, sizeof (data));

    memset(buf, 0, sizeof (buf));
    /*buf[BUFSIZ]  = 0;*/
    xpos = tc->tree->GetLevel(item);
    /* fill indents */
    if (xpos > 0)
    {
        if (file)
        {
            memset(buf, '\t', xpos);
        }
        else
        {
            xpos *= tc->indent;
            memset(buf, ' ', xpos);
        }
        *startx = xpos;
    }
    else
    {
        return;
    }
    /* extend 2 characters for node displayed */
    children = tc->tree->CountChild(item);

    /* print node */
    if ((TTCS_SHOWNODE & style) && children > 0 && !file)
    {
        if (data.expanded)
        {
            buf[xpos - tc->indent] = '-';
        }
        else
        {
            buf[xpos - tc->indent] = '+';
        }
    }
    /* print text */
    textlen = strlen(data.itemtext);
    if (textlen > 0)
    {
        memcpy(&buf[xpos], data.itemtext, textlen);
    }
    xpos += textlen;

    *endx = *startx + textlen;
    /* copy to output */
    if (!file)
    {
        shifted_right = tc->shifted_right;
    }
    if (shifted_right > 0)
    {
        *endx = TUI_MAX(0, *endx - shifted_right);
        *startx = TUI_MAX(0, *startx - shifted_right);
    }
    memcpy(outtext, &buf[shifted_right], xpos);

    len = strlen(outtext);
    if (len < maxlen)
    {
        memset(&outtext[len], ' ', maxlen - len);
        outtext[maxlen] = 0;
    }
}

tui_i32 _TTC_ExpandAllItemsProc(tui_void* args, tree_iter_t item, const tui_void* node, tui_ui32 size)
{
    TTREEITEMDATA data;
    TWND wnd = (TWND) args;
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    tc->tree->GetItemData(item, &data, sizeof (data));
    data.expanded = TUI_TRUE;
    tc->tree->SetItemData(item, &data, sizeof (data));
    return 0;
}

tui_i32 _TTC_CollapseAllItemsProc(tui_void* args, tree_iter_t item, const tui_void* node, tui_ui32 size)
{
    TTREEITEMDATA data;
    TWND wnd = (TWND) args;
    TTREECTRLSTRUCT* tc = 0;

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);
    tc->tree->GetItemData(item, &data, sizeof (data));
    data.expanded = TUI_FALSE;
    tc->tree->SetItemData(item, &data, sizeof (data));
    return 0;
}

tui_i32 _TTC_PreorderTraverseProc(tui_void* args, tree_iter_t item, const tui_void* node, tui_ui32 size)
{
    tqueue_t* queue = (tqueue_t*) args;
    TTREEVIEWITEM view;

    view.item = item;
    queue->Enqueue(queue, &view, sizeof (view));
    return 0;
}

TVOID _TTC_OnPaint(TWND wnd, TDC dc)
{
    TRECT rc;
    TUI_CHAR buf[TUI_MAX_WNDTEXT + 1];
    TLONG displayableitems = 0;
    TTREECTRLSTRUCT* tc = 0;
    TINT y = 0;
    TTREEITEMDATA data;
    TTREEVIEWITEM view;
    list_iter_t iter;
    TINT count = 0, items = 0;
    TINT ysel = 0;
    TINT xsel = 0;
    TUI_CHAR bufsel[TUI_MAX_WNDTEXT + 1];
    TDWORD style = 0;
    TUI_CHAR filler = 0;
    TUI_BOOL fullrow = TUI_FALSE;
    TINT startx = 0, endx = 0;
    g2t_dword textcolor = g2t_GetTextColor(wnd);
    g2t_dword bgcolor = g2t_GetBgColor(wnd);
    TDWORD htextcolor = g2t_GetSysColor(TREECTRL_HIGHLIGHTED_TEXTCOLOR);
    TDWORD hbgcolor = g2t_GetSysColor(TREECTRL_HIGHLIGHTED_BGCOLOR);

    tc = (TTREECTRLSTRUCT*) g2t_GetWndParam(wnd);

    /* visible window & there is a visible item */
    if (g2t_IsWndVisible(wnd) && tc->firstvisibleitem)
    {
        g2t_GetWndRect(wnd, &rc);
        y = rc.y;

        style = g2t_GetWndStyle(wnd);
        if (TTCS_NOHIGHLIGHT & style)
        {
            htextcolor = textcolor;
            hbgcolor = bgcolor;
        }
        if (TTCS_FULLSECROW & style)
        {
            /*filler = ' ';*/
            fullrow = TUI_TRUE;
        }

        _TTC_AdjustVisibleItems(wnd);
        iter = tc->visibleitems->Begin(tc->visibleitems);
        items = tc->visibleitems->Count(tc->visibleitems);

        /* skip the previous item before the first visible item */
        while (iter != tc->visibleitems->End(tc->visibleitems))
        {
            tc->visibleitems->GetItemData(iter, &view, sizeof (view));
            if (tc->firstvisibleitem == view.item)
            {
                break;
            }
            iter = tc->visibleitems->GetNext(iter);
            ++count;
        }
        /* display items */
        while (iter != tc->visibleitems->End(tc->visibleitems) &&
                displayableitems < rc.lines)
        {
            tc->visibleitems->GetItemData(iter, &view, sizeof (view));
            tc->tree->GetItemData(view.item, &data, sizeof (TTREEITEMDATA));

            memset(buf, filler, sizeof (buf));
            _TTC_GetDisplayedText(wnd, buf, &startx, &endx, view.item, rc.cols, TUI_FALSE);
            if (data.selected)
            {
                if (fullrow)
                {
                    g2t_DrawText(dc, rc.x, y, buf, htextcolor, hbgcolor);
                }
                else
                {
                    buf[endx] = 0;
                    g2t_DrawText(dc, rc.x + startx, y, &buf[startx], htextcolor, hbgcolor);
                }
                ysel = y;
                xsel = rc.x;
                strcpy(bufsel, buf);
            }
            else
            {
                g2t_DrawText(dc, rc.x, y, buf, textcolor, bgcolor);
            }
            /* next line */
            ++y;
            /* limit item displayed */
            ++displayableitems;
            /* move to the next visible item */
            iter = tc->visibleitems->GetNext(iter);
            ++count;
        }
        /* keep the last visible item */
        if (displayableitems > items - count)
        {
            iter = tc->visibleitems->GetPrev(iter);
        }
        tc->visibleitems->GetItemData(iter, &view, sizeof (view));
        tc->lastvisibleitem = view.item;

        /* draw the selected item */
        g2t_DrawText(dc, xsel, ysel, bufsel, htextcolor, hbgcolor);
        g2t_MoveXY(xsel, ysel);
    }
}

TLONG G2TTREECTRLPROC(TWND wnd, TUINT msg, TWPARAM wparam, TLPARAM lparam)
{
    switch (msg)
    {
    case TWM_PAINT:
    {
        _TTC_OnPaint(wnd, g2t_GetMemDC());
        return 0;
    }
    case TWM_CREATE:
    {
        return _TTC_OnCreate(wnd);
    }
    case TTCM_INSERTITEM:
    {
        return (TLONG) _TTC_OnInsertItem(wnd, (TTREEITEM*) wparam, (TTREEITEMDATA*) lparam);
    }
    case TTCM_DELETEITEM:
    {
        return _TTC_OnDeleteItem(wnd, (TTREEITEM*) lparam);
    }
    case TTCM_SETITEM:
    {
        return _TTC_OnSetItem(wnd, (TTREEITEM*) wparam, (TTREEITEMDATA*) lparam);
    }
    case TTCM_GETITEM:
    {
        return _TTC_OnGetItem(wnd, (TTREEITEM*) wparam, (TTREEITEMDATA*) lparam);
    }
    case TTCM_FINDITEM:
    {
        return (TLONG) _TTC_OnFindItem(wnd, (TTREEITEMDATA*) lparam);
    }
    case TTCM_FINDNEXTITEM:
    {
        return (TLONG) _TTC_OnFindNextItem(wnd, (TTREEITEM*) wparam, (TTREEITEMDATA*) lparam);
    }
    case TTCM_EXPANDITEM:
    {
        return _TTC_OnExpandItem(wnd, (TTREEITEM*) lparam);
    }
    case TTCM_COLLAPSEITEM:
    {
        return _TTC_OnCollapseItem(wnd, (TTREEITEM*) lparam);
    }
    case TWM_DESTROY:
    {
        /* release memory of static control */
        _TTC_OnDestroy(wnd);
        return 0;
    }
    case TWM_SETFOCUS:
    {
        _TTC_OnSetFocus(wnd);
        break;
    }
    case TWM_KILLFOCUS:
    {
        return _TTC_OnKillFocus(wnd);
    }
    case TWM_KEYDOWN:
    {
        _TTC_OnKeyDown(wnd, (TLONG) wparam);
        break;
    }
    case TTCM_SETFINDITEMPROC:
    {
        return (TLONG) _TTC_OnSetFindItemProc(wnd, (LPTREEFINDITEMPROC) lparam);
    }
    case TTCM_SETSELITEM:
    {
        return _TTC_OnSetSelItem(wnd, (TTREEITEM*) lparam);
    }
    case TTCM_GETSELITEM:
    {
        return (TLONG) _TTC_OnGetSelItem(wnd);
    }
    case TTCM_EXPORTTOFILE:
    {
        return _TTC_OnExportToFile(wnd, (FILE*) wparam, (LPTREEEXPORTPROC) lparam);
    }
    case TTCM_IMPORTFROMFILE:
    {
        return _TTC_OnImportFromFile(wnd, (FILE*) wparam, (LPTREEIMPORTPROC) lparam);
    }
    case TTCM_COLLAPSEALLITEMS:
    {
        _TTC_OnCollapseAllItems(wnd);
        break;
    }
    case TTCM_EXPANDALLITEMS:
    {
        _TTC_OnExpandAllItems(wnd);
        break;
    }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}

