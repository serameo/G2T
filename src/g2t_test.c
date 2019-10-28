#include <windows.h>
#include "g2t.h"


#define IDC_TLISTBOX1     204
#define IDC_TLISTBOX2     205
#define IDC_TREECTRL1     218
#define IDC_STATUSBAR1    220
#define IDC_STATUSBAR2    221
#define IDC_STATUSBAR3    222
#define IDC_STATUSBAR4    223
#define IDC_STATUSBAR5    224
#define IDC_STATUSBAR6    225
#define IDC_NAME1         226
#define IDC_NAME2         227
#define IDC_NAME3         228
#define IDC_NAME4         229
#define IDC_STATUSBAR7    230

g2t_long mywndproc(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);
g2t_frmwndtemplate frame1[] = {
    { G2T_STATIC, "A&2Z", IDC_STATUSBAR1, 1, 0, 1, 20, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_EDITBOX, "A2Z", IDC_NAME1, 1, 22, 1, 20, GWS_CHILD | GWS_VISIBLE|GES_AUTOHSCROLL|GES_A2Z|GES_UPPERCASE , 0, 0, "" },
    { G2T_STATIC, "&NUMBER", IDC_STATUSBAR2, 2, 0, 1, 20, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_EDITBOX, "NUMBER", IDC_NAME2, 2, 22, 1, 20, GWS_CHILD | GWS_VISIBLE|GES_AUTOHSCROLL|GES_NUMBER, 0, 0, "" },
    { G2T_STATIC, "DEC&IMAL", IDC_STATUSBAR3, 3, 0, 1, 20, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_EDITBOX, "DECIMAL", IDC_NAME3, 3, 22, 1, 20, GWS_CHILD | GWS_VISIBLE|GES_AUTOHSCROLL|GES_DECIMAL, 0, 0, "" },
    { G2T_STATIC, "PASSWOR&D", IDC_STATUSBAR4, 4, 0, 1, 20, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_EDITBOX, "PASSWORD", IDC_NAME4, 4, 22, 1, 20, GWS_CHILD | GWS_VISIBLE|GES_AUTOHSCROLL|GES_PASSWORD, 0, 0, "" },
    { G2T_STATIC, "IT&EM", IDC_STATUSBAR5, 5, 0, 1, 20, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_LISTBOX, "", IDC_TLISTBOX1, 5, 22, 5, 20,
        GWS_CHILD | GWS_VISIBLE | GWS_BORDER|GLBS_MULTICOLOR, 0, 0, "List1" },
    { G2T_STATIC, "ST&OCK", IDC_STATUSBAR6, 2, 44, 1, 20, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_LISTCTRL, "", IDC_TLISTBOX2, 3, 44, 10, 60,
        TWS_CHILD | TWS_VISIBLE | TLCS_NOBORDER|TLCS_EDITABLE|TLCS_MULTICOLOR , 0, 0, "List1" },
    { G2T_STATIC, "T&REE", IDC_STATUSBAR7, 11, 0, 1, 20, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_TREECTRL, "", IDC_TREECTRL1, 12, 0, 10, 40,
        TWS_CHILD | TWS_VISIBLE | TTCS_SHOWNODE | TTCS_FULLSECROW|TTCS_MULTICOLOR/*|TTCS_NOHIGHLIGHT*/, 0, 0, "" },
    /* the last is the end-of-controls */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

#define IDC_LABEL1              (1000)
#define IDC_STOCK1              (1001)
#define IDC_BBO1                (1002)
#define IDC_OHLC1               (1003)
#define IDC_LASTSALE1           (1004)
#define IDC_BBO2                (1005)
#define IDC_BBO3                (1006)

g2t_long StockInPlayProc(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);
g2t_frmwndtemplate StockInPlayFrame[] = {
    { G2T_STATIC, "STOCK BY PRICE:", IDC_LABEL1, 0, 0, 1, 15, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_STATIC, "PTT", IDC_STOCK1,   0, 16, 1, 65, GWS_CHILD | GWS_VISIBLE, 0, 0, "" },
    { G2T_LISTCTRL, "", IDC_BBO1,      1,  0, 6, 32, TWS_CHILD | TWS_VISIBLE | TWS_DISABLED | TLCS_NOBORDER, 0, 0, "" },
    { G2T_LISTCTRL, "", IDC_OHLC1,     1, 32, 2, 48, TWS_CHILD | TWS_VISIBLE | TWS_DISABLED | TLCS_NOBORDER, 0, 0, "" },
    { G2T_LISTCTRL, "", IDC_LASTSALE1, 3, 51, 6, 30, TWS_CHILD | TWS_VISIBLE | TWS_DISABLED | TLCS_NOBORDER|TLCS_NOHEADER, 0, 0, "" },
    { G2T_LISTCTRL, "", IDC_BBO2,      8,  0, 9, 40, TWS_CHILD | TWS_VISIBLE | TWS_DISABLED | TLCS_NOBORDER, 0, 0, "" },
    { G2T_LISTCTRL, "", IDC_BBO3,      8, 40, 9, 40, TWS_CHILD | TWS_VISIBLE | TWS_DISABLED | TLCS_NOBORDER, 0, 0, "" },
    /* the last is the end-of-controls */
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

HINSTANCE g_hinst = 0;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("HelloWin");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;
    long rc;

    g2t_StartUp(hInstance);
    g2t_LoadProperties("C:\\workarea\\g2t.env");

    g2t_InitApplication(szAppName, TEXT("The Hello Program"));

    g2t_RegisterCls("mywndproc", mywndproc);
    g2t_RegisterCls("StockInPlayProc", StockInPlayProc);

    g2t_LoadScreen("mywndproc", "Hello G2T", frame1);
    /*g2t_LoadScreen("StockInPlayProc", "Hello G2T", StockInPlayFrame);*/
    rc = g2t_Run();

    g2t_ShutDown();
    return rc;
}

g2t_long mywndproc(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    g2t_wnd listbox = 0;
    g2t_int i = 0;
    g2t_char buf[G2T_MAX_WNDTEXT + 1];
    TWND list = 0;
    /*MSGBOXPARAM param;*/
    TSUBITEM item;
    TNMHDR* nmhdr = 0;
    TWND tctl = 0;
    g2t_dword textcolor = G2T_RGB_BLACK;
    g2t_dword bgcolor = G2T_RGB_CYAN;

    switch (msg)
    {
        case GWM_INITDIALOG:
        {
            listbox = g2t_GetWndItem(wnd, IDC_TLISTBOX1);
            for (i = 0; i < 20; ++i)
            {
                sprintf(buf, "Item %d", (i));
                TLB_AddItem(listbox, buf);
            }
            list = g2t_GetWndItem(wnd, IDC_TLISTBOX2);

            TLC_AddColumnEx(list, "STOCK", 20, TALIGN_LEFT,  G2T_RGB_BLACK, G2T_RGB_CYAN, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "DATE", 16, TALIGN_CENTER, G2T_RGB_BLACK, G2T_RGB_CYAN, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "OPEN", 16, TALIGN_RIGHT,  G2T_RGB_BLACK, G2T_RGB_CYAN, TES_DECIMAL | TES_AUTOHSCROLL, 2);
            TLC_AddColumnEx(list, "HIGH", 16, TALIGN_RIGHT,  G2T_RGB_BLACK, G2T_RGB_CYAN, TES_DECIMAL | TES_AUTOHSCROLL, 2);
            TLC_AddColumnEx(list, "LOW", 16, TALIGN_RIGHT,   G2T_RGB_BLACK, G2T_RGB_CYAN, TES_DECIMAL | TES_AUTOHSCROLL, 2);
            TLC_AddColumnEx(list, "CLOSE", 16, TALIGN_RIGHT, G2T_RGB_BLACK, G2T_RGB_CYAN, TES_DECIMAL | TES_AUTOHSCROLL, 2);

            TLC_AddItem(list, "SCC\t01-01-2018\t560.00\t563.50\t560.00\t562.00\t", 6);
            TLC_AddItem(list, "PTTEP\t01-01-2018\t160.00\t163.50\t160.00\t162.00\t", 6);
            TLC_AddItem(list, "PTTGC\t01-01-2018\t70.00\t73.50\t70.00\t72.00\t", 6);
            TLC_AddItem(list, "PTT\t01-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);

            TLC_AddItem(list, "PTT\t05-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t06-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t07-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t08-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t09-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t10-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t11-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t12-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t13-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t14-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t15-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t16-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t17-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t18-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t19-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t20-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t21-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t22-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t23-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t24-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t25-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t26-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t27-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t28-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t29-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);
            TLC_AddItem(list, "PTT\t30-01-2018\t60.00\t63.50\t60.00\t62.00\t", 6);

            TLC_InsertFirstItem(list, "TTW\t31-12-2017\t60.00\t63.50\t60.00\t62.00\t", 6);

            item.col = 0;
            item.idx = 0;
            item.textcolor = G2T_RGB_GREEN;
            item.bgcolor = G2T_RGB_MAGENTA;
            TLC_SetItem(list, TLCFM_ATTRS, &item);

            item.col = 1;
            item.idx = 1;
            item.textcolor = G2T_RGB_YELLOW;
            item.bgcolor = G2T_RGB_BLUE;
            TLC_SetItem(list, TLCFM_ATTRS, &item);

            item.col = 2;
            item.idx = 2;
            item.textcolor = G2T_RGB_YELLOW;
            item.bgcolor = G2T_RGB_RED;
            TLC_SetItem(list, TLCFM_ATTRS, &item);

            item.col = 3;
            item.idx = 3;
            item.textcolor = G2T_RGB_GREEN;
            item.bgcolor = G2T_RGB_BLUE;
            TLC_SetItem(list, TLCFM_ATTRS, &item);

            item.col = 4;
            item.idx = 4;
            item.textcolor = G2T_RGB_CYAN;
            item.bgcolor = G2T_RGB_MAGENTA;
            TLC_SetItem(list, TLCFM_ATTRS, &item);

            item.col = 5;
            item.idx = 14;
            item.textcolor = G2T_RGB_CYAN;
            item.bgcolor = G2T_RGB_MAGENTA;
            TLC_SetItem(list, TLCFM_ATTRS, &item);

            tctl = g2t_GetWndItem(wnd, IDC_TREECTRL1);
            FILE* fp = fopen("C:\\workarea\\mytree.txt", "r");
            if (fp)
            {
                TTC_ImportFromFile(tctl, fp, 0);
                fclose(fp);
            }

            return G2T_CONTINUE;
        }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}

/*
#define IDC_LABEL1              (1000)
#define IDC_STOCK1              (1001)
#define IDC_BBO1                (1002)
#define IDC_OHLC1               (1003)
#define IDC_LASTSALE1           (1004)
*/
g2t_long StockInPlayProc(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam)
{
    g2t_wnd list = 0;
    g2t_dword textcolor = G2T_RGB_BLACK;
    g2t_dword bgcolor = G2T_RGB_CYAN;
    g2t_wnd label = 0;

    switch (msg)
    {
        case GWM_INITDIALOG:
        {
            /**/
            label = g2t_GetWndItem(wnd, IDC_STOCK1);
            g2t_SetWndText(label, "PTT      LAST38.50-10    -20.62%ENERG  24449.59-1703.75 -6.51%");

            list = g2t_GetWndItem(wnd, IDC_BBO1);
            TLC_AddColumnEx(list, "VOLUME", 8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "BID",    8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "OFFER",  8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "VOLUME", 8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);

            TLC_AddItem(list, "100\t38.50\t39.00\t300\t", 4);
            TLC_AddItem(list, "100\t38.00\t     \t   \t", 4);
            TLC_AddItem(list, "100\t37.50\t     \t   \t", 4);
            TLC_AddItem(list, "100\t37.00\t     \t   \t", 4);
            TLC_AddItem(list, "100\t36.50\t     \t   \t", 4);

            list = g2t_GetWndItem(wnd, IDC_OHLC1);
            TLC_AddColumnEx(list, "PRIOR",  8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "OPEN",   8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "HIGH",   8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "LOW",    8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "AVG",    8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "FLAG",   8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);

            TLC_AddItem(list, "48.50\t \t38.50\t35.50\t36.44\t \t", 6);

            list = g2t_GetWndItem(wnd, IDC_LASTSALE1);
            TLC_AddColumnEx(list, "LAST", 48, TALIGN_RIGHT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            
            TLC_AddItem(list, "200 -38.50 10:07:15\t", 1);
            TLC_AddItem(list, "200 -37.50 10:06:32\t", 1);
            TLC_AddItem(list, "200    +37 10:06:13\t", 1);
            TLC_AddItem(list, "200 +36.50 10:06:08\t", 1);
            TLC_AddItem(list, "200    -36 10:06:08\t", 1);

            list = g2t_GetWndItem(wnd, IDC_BBO2);
            TLC_AddColumnEx(list, "STOCK",  8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "VOLUME", 8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "BID",    8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "OFFER",  8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "VOLUME", 8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);

            TLC_AddItem(list, "PTTGC\t100\t38.50\t39.00\t300\t", 5);
            TLC_AddItem(list, "PTTEP\t100\t38.00\t     \t   \t", 5);
            TLC_AddItem(list, "BBL\t100\t37.50\t     \t   \t", 5);
            TLC_AddItem(list, "KBANK\t100\t37.00\t     \t   \t", 5);
            TLC_AddItem(list, "ASP\t100\t36.50\t     \t   \t", 5);

            list = g2t_GetWndItem(wnd, IDC_BBO3);
            TLC_AddColumnEx(list, "STOCK",  8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "VOLUME", 8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "BID",    8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "OFFER",  8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);
            TLC_AddColumnEx(list, "VOLUME", 8, TALIGN_LEFT, textcolor, bgcolor, TES_UPPERCASE | TES_AUTOHSCROLL, 0);

            TLC_AddItem(list, "LPN\t100\t38.50\t39.00\t300\t", 5);
            TLC_AddItem(list, "LH\t100\t38.00\t     \t   \t", 5);
            TLC_AddItem(list, "JCT\t100\t37.50\t     \t   \t", 5);
            TLC_AddItem(list, "TTW\t100\t37.00\t     \t   \t", 5);
            TLC_AddItem(list, "KGI\t100\t36.50\t     \t   \t", 5);

            return TUI_CONTINUE;
        }
    }
    return g2t_DefWndProc(wnd, msg, wparam, lparam);
}
