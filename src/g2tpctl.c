/*-------------------------------------------------------------------
 * File name: g2tpctl.c
 * Author: Seree Rakwong
 * Date: 11-DEC-19
 *-----------------------------------------------------------------*/
#include <windows.h>
#include "g2t.h"

struct _TPAGESTRUCT
{
    struct _TPAGESTRUCT*    nextpage;
    struct _TPAGESTRUCT*    prevpage;
    g2t_wnd                 page;       /*page window handle*/
    g2t_long                pageid;     /*page id*/
    g2t_lparam              lparam;     /*multi-purposes used*/
};
typedef struct _TPAGESTRUCT TPAGESTRUCT;

struct _TPAGECTRLSTRUCT
{
    TPAGESTRUCT*            firstpage;
    TPAGESTRUCT*            lastpage;
    TPAGESTRUCT*            activepage;         /*active page*/
    g2t_long                npages;             /*number of pages*/
    g2t_lparam              lparam;             /*multi-purposes used*/
};
typedef struct _TPAGECTRLSTRUCT TPAGECTRLSTRUCT;

/*-------------------------------------------------------------------
 * STATIC functions
 *-----------------------------------------------------------------*/
g2t_void _TPGC_OnPaint(g2t_wnd wnd, g2t_dc dc);
g2t_long _TPGC_OnCreate(g2t_wnd wnd);
g2t_long _TPGC_OnAddPage(g2t_wnd wnd, g2t_string clsname, g2t_frmwndtemplate* templ);
g2t_void _TPGC_OnRemovePage(g2t_wnd wnd, g2t_long pageno);
g2t_void _TPGC_OnRemoveAllPages(g2t_wnd wnd);
g2t_wnd _TPGC_OnGotoFirstPage(g2t_wnd wnd);
g2t_wnd _TPGC_OnGotoLastPage(g2t_wnd wnd);
g2t_wnd _TPGC_OnGotoPage(g2t_wnd wnd, g2t_long pageno);
g2t_wnd _TPGC_OnGotoNextPage(g2t_wnd wnd);
g2t_wnd _TPGC_OnGotoPrevPage(g2t_wnd wnd);
g2t_void _TPGC_OnKeyDown(g2t_wnd wnd, g2t_long ch);


g2t_long G2TPAGECTRLPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);
g2t_long G2TCHILDPAGEPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);

TPAGESTRUCT* _TPGC_GetPageAt(TPAGESTRUCT* begpage, TPAGESTRUCT* endpage, g2t_long at);
TPAGESTRUCT* _TPGC_FindPage(TPAGESTRUCT* begpage, TPAGESTRUCT* endpage, g2t_long pageid);
TPAGESTRUCT* _TPGC_InitPage(g2t_wnd parent, g2t_string clsname, g2t_frmwndtemplate* templ, g2t_long pageid, g2t_lparam lparam);
g2t_void _TPGC_DeletePage(TPAGESTRUCT* delpage);






TPAGESTRUCT* _TPGC_GetPageAt(TPAGESTRUCT* begpage, TPAGESTRUCT* endpage, g2t_long at)
{
    TPAGESTRUCT* curpage = begpage;
    if (at < 0) /*get the first page*/
    {
        return begpage;
    }
    else if (0 == at) /*get the last page*/
    {
        return (endpage->prevpage ? endpage->prevpage : endpage);
    }

    while (curpage && curpage != endpage)
    {
        if (at <= 0)
        {
            break;
        }
        --at;
    }
    return curpage;
}

TPAGESTRUCT* _TPGC_FindPage(TPAGESTRUCT* begpage, TPAGESTRUCT* endpage, g2t_long pageid)
{
    TPAGESTRUCT* curpage = begpage;
    while (curpage && curpage != endpage)
    {
        if (pageid == curpage->pageid)
        {
            break;
        }
    }
    return curpage;
}

TPAGESTRUCT* _TPGC_InitPage(g2t_wnd parent, g2t_string clsname, g2t_frmwndtemplate* templ, g2t_long pageid, g2t_lparam lparam)
{
    TPAGESTRUCT* pg = 0;
    pg = (TPAGESTRUCT*) g2t_malloc(sizeof (TPAGECTRLSTRUCT));
    if (!pg)
    {
        return 0;
    }
    g2t_memset(pg, 0, sizeof(TPAGECTRLSTRUCT));
    pg->page   = g2t_CreatePageWnd(
                    clsname,
                    GWS_CHILD,
                    0,
                    0, 0, 25, 80,
                    parent,
                    pageid,
                    templ,
                    lparam);
    if (!pg->page)
    {
        g2t_free(pg);
        return 0;
    }
    pg->lparam = lparam;
    pg->pageid = pageid;
    return pg;
}

g2t_void _TPGC_DeletePage(TPAGESTRUCT* delpage)
{
    TPAGESTRUCT* prevpage = 0;
    TPAGESTRUCT* nextpage = 0;
    /*unlink*/
    prevpage = delpage->prevpage;
    nextpage = delpage->nextpage;

    if (prevpage)
    {
        prevpage->nextpage = nextpage;
    }
    if (nextpage)
    {
        nextpage->prevpage = prevpage;
    }

    /* destroy page */
    g2t_DestroyWnd(delpage->page);
    g2t_free(delpage);
}

g2t_void _TPGC_OnRemovePage(g2t_wnd wnd, g2t_long pageno)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* delpage = _TPGC_GetPageAt(pgc->firstpage, pgc->lastpage, pageno);
    if (!delpage || delpage == pgc->lastpage)
    {
        return; /*not page found*/
    }
    _TPGC_DeletePage(delpage);
    /*decrement counter*/
    --pgc->npages;
}

g2t_wnd _TPGC_OnGotoPage(g2t_wnd wnd, g2t_long pageno)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = _TPGC_GetPageAt(pgc->firstpage, pgc->lastpage, pageno);
    if (pg && pg != pgc->lastpage)
    {
        pgc->activepage = pg;
        return pg->page;
    }
    return 0;
}

g2t_wnd _TPGC_OnGotoFirstPage(g2t_wnd wnd)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = pgc->firstpage;
    TPAGESTRUCT* activepg = pgc->activepage;

    if (activepg)
    {
        g2t_SendMsg(activepg->page, TCPM_SHOWPAGE, GW_HIDE, 0);
    }
    pgc->activepage = pg;
    g2t_SendMsg(pg->page, TCPM_SHOWPAGE, GW_SHOW, 0);
    g2t_SendMsg(pg->page, TCPM_ACTIVATEPAGE, 0, 0);
    return pg->page;
}

g2t_wnd _TPGC_OnGotoPrevPage(g2t_wnd wnd)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = (pgc->activepage ? pgc->activepage : pgc->lastpage);

    if (pg && pg->prevpage)
    {
        g2t_SendMsg(pg->page, TCPM_SHOWPAGE, GW_HIDE, 0);
        pg = pg->prevpage;
        pgc->activepage = pg;
        g2t_SendMsg(pg->page, TCPM_SHOWPAGE, GW_SHOW, 0);
        g2t_SendMsg(pg->page, TCPM_ACTIVATEPAGE, 0, 0);
        return pg->page;
    }
    return 0;
}


g2t_wnd _TPGC_OnGotoNextPage(g2t_wnd wnd)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = (pgc->activepage ? pgc->activepage : pgc->firstpage);

    if (pg && pg->nextpage)
    {
        g2t_SendMsg(pg->page, TCPM_SHOWPAGE, GW_HIDE, 0);
        pg = pg->nextpage;
        pgc->activepage = pg;
        g2t_SendMsg(pg->page, TCPM_SHOWPAGE, GW_SHOW, 0);
        g2t_SendMsg(pg->page, TCPM_ACTIVATEPAGE, 0, 0);
        return pg->page;
    }
    return 0;
}

g2t_wnd _TPGC_OnGotoLastPage(g2t_wnd wnd)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = pgc->lastpage;
    TPAGESTRUCT* activepg = pgc->activepage;

    if (activepg)
    {
        g2t_SendMsg(activepg->page, TCPM_SHOWPAGE, GW_HIDE, 0);
    }
    pgc->activepage = pg;
    g2t_SendMsg(pg->page, TCPM_SHOWPAGE, GW_SHOW, 0);
    g2t_SendMsg(pg->page, TCPM_ACTIVATEPAGE, 0, 0);
    return pg->page;
}

g2t_void _TPGC_OnRemoveAllPages(g2t_wnd wnd)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = pgc->firstpage;
    TPAGESTRUCT* delpage = pg;
    while (pg && pg != pgc->lastpage)
    {
        pg = pg->nextpage;
        _TPGC_DeletePage(delpage);
        delpage = pg;
    }
    pgc->npages = 0;
    pgc->firstpage = pgc->lastpage;
}

g2t_long _TPGC_OnAddPage(g2t_wnd wnd, g2t_string clsname, g2t_frmwndtemplate* templ)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = pgc->lastpage;
    g2t_long pageid = pgc->npages + 1;
    /*find the page number*/
    if (pg != pgc->firstpage)
    {
        pg = pg->prevpage;
        pageid = pg->pageid + 1;
    }
    /* initial the new page*/
    pg = _TPGC_InitPage(wnd, clsname, templ, pageid, 0);
    if (!pg)
    {
        return -1;
    }
    /* add page by linking pages */
    pg->nextpage = pgc->lastpage;
    pg->prevpage = pgc->lastpage->prevpage;
    if (pgc->firstpage != pgc->lastpage)
    {
        pgc->lastpage->prevpage->nextpage = pg;
    }
    else
    {
        pgc->firstpage = pg;
    }
    pgc->lastpage->prevpage = pg;
    /*increment counter*/
    ++pgc->npages;
    return pageid;
}

g2t_long _TPGC_OnCreate(g2t_wnd wnd)
{
    TPAGECTRLSTRUCT* pgc = 0;
    /* initial memory for a page control */
    pgc = (TPAGECTRLSTRUCT*) g2t_malloc(sizeof (TPAGECTRLSTRUCT));
    if (!pgc)
    {
        return G2T_ERROR;
    }
    g2t_memset(pgc, 0, sizeof(TPAGECTRLSTRUCT));
    pgc->firstpage = pgc->lastpage = _TPGC_InitPage(wnd, G2T_CHILDPAGE, 0, 0, 0);
    if (!pgc->lastpage)
    {
        g2t_free(pgc);
        return G2T_ERROR;
    }
    pgc->activepage = 0;
    pgc->lparam = 0;
    pgc->npages = 0;

    g2t_SetWndParam(wnd, (g2t_lparam) pgc);
    g2t_SetTextColor(wnd, G2T_RGB_WHITE);
    g2t_SetBgColor(wnd, G2T_RGB_BLACK);

    return G2T_CONTINUE;
}

g2t_void _TPGC_OnPaint(g2t_wnd wnd, g2t_dc dc)
{
    if (!g2t_IsWndVisible(wnd))
    {
        return;
    }
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = pgc->activepage;

    if (pg)
    {
        g2t_InvalidateWnd(pg->page);
    }
}
g2t_void _TPGC_OnKeyDown(g2t_wnd wnd, g2t_long ch)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = pgc->activepage;

    if (pg)
    {
        g2t_SendMsg(pg->page, GWM_KEYDOWN, (g2t_wparam)ch, 0);
    }
}

g2t_wnd _TPGC_OnGetActiveChild(g2t_wnd wnd)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = pgc->activepage;

    if (pg)
    {
        return g2t_SendMsg(pg->page, GWM_GETACTIVECHILD, 0, 0);
    }
    return 0;
}

g2t_long G2TPAGECTRLPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    TPAGECTRLSTRUCT* pgc = (TPAGECTRLSTRUCT*)g2t_GetWndParam(wnd);
    TPAGESTRUCT* pg = (pgc ? pgc->activepage : 0);

    switch (msg)
    {
        case GWM_GETACTIVECHILD:
        {
            return (g2t_long)_TPGC_OnGetActiveChild(wnd);
        }
        case GWM_PAINT:
        {
            _TPGC_OnPaint(wnd, g2t_GetMemDC());
            return 0;
        }
        case GWM_CREATE:
        {
            return _TPGC_OnCreate(wnd);
        }/*
        case GWM_KEYDOWN:
        {
            _TPGC_OnKeyDown(wnd, wparam);
            break;
        }*/
        case TPCM_ADDPAGE:
        {
            return _TPGC_OnAddPage(wnd, (g2t_string)wparam, (g2t_frmwndtemplate*)lparam);
        }
        case TPCM_REMOVEPAGE:
        {
            _TPGC_OnRemovePage(wnd, wparam);
            return 0;
        }
        case TPCM_REMOVEALLPAGES:
        {
            _TPGC_OnRemoveAllPages(wnd);
            return 0;
        }
        case TPCM_GOTOPAGE:
        {
            return (g2t_long)_TPGC_OnGotoPage(wnd, wparam);
        }
        case TPCM_GOTOFIRSTPAGE:
        {
            return (g2t_long)_TPGC_OnGotoFirstPage(wnd);
        }
        case TPCM_GOTOLASTPAGE:
        {
            return (g2t_long)_TPGC_OnGotoLastPage(wnd);
        }
        case TPCM_GOTOPREVPAGE:
        {
            return (g2t_long)_TPGC_OnGotoPrevPage(wnd);
        }
        case TPCM_GOTONEXTPAGE:
        {
            return (g2t_long)_TPGC_OnGotoNextPage(wnd);
        }
    }
    if (pg)
    {
        g2t_SendMsg(pg->page, msg, wparam, lparam);
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}

/*------------------------*/
g2t_long G2TCHILDPAGEPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    return g2t_DefPageWndProc(wnd, msg, wparam, lparam);
}

g2t_int32 g2t_DefPageWndProc(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    switch (msg)
    {
        case GWM_INITPAGE:
        {
            return G2T_CONTINUE;
        }
        case TCPM_SHOWPAGE:
        {
            g2t_ShowWnd(wnd, (g2t_bool)wparam);
            break;
        }
        case TCPM_ACTIVATEPAGE:
        {
            return 0;
        }
    }
    g2t_wnd activechild = 0;/*g2t_SendMsg(wnd, GWM_GETACTIVECHILD, 0, 0);*/
    if (activechild)
    {
        /*g2t_SendMsg(activechild, msg, wparam, lparam);*/
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}
