/*-------------------------------------------------------------------
 * File name: g2ttschrt.c
 * Author: Seree Rakwong
 * Date: 24-SEP-18
 *-----------------------------------------------------------------*/
#include <windows.h>
#include "g2t.h"


struct _timeseries_t
{
    g2t_double      open;
    g2t_double      high;
    g2t_double      low;
    g2t_double      close;
    g2t_long        date;
    g2t_long        time;
};
typedef struct _timeseries_t timeseries_t;

struct _tsdata_t
{
    timeseries_t        data;
    struct _tsdata_t*   prev;
    struct _tsdata_t*   next;
};
typedef struct _tsdata_t tsdata_t;


enum
{
    TS_CANDLE,
    TS_OHLC,
    TS_LINE,
    TS_DOT
};

struct _TIMESERIES
{
    tsdata_t*   first;
    tsdata_t*   last;
    g2t_long    style; /* chart style */
};
typedef struct _TIMESERIES TIMESERIES;


g2t_long G2TTSCHARTPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);
g2t_long _GTS_OnCreate(g2t_wnd wnd);
g2t_void _GTB_OnPaint(g2t_wnd wnd, g2t_dc dc);


g2t_long G2TTSCHARTPROC(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    switch (msg)
    {
        case GWM_CREATE:
        {
            return _GTS_OnCreate(wnd);
        }
        case GWM_PAINT:
        {
            _GTB_OnPaint(wnd, g2t_GetMemDC());
            return 0;
        }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}

g2t_long _GTS_OnCreate(g2t_wnd wnd)
{
    return G2T_CONTINUE;
}

g2t_void _GTB_OnPaint(g2t_wnd wnd, g2t_dc dc)
{
}
