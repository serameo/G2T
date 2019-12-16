#ifndef __GUI2TUI_H__
#define __GUI2TUI_H__
/*
File: g2t.h
Purpose: Display Graphic UI as Text UI
Author: Seree Rakwong
Date: 23-SEP-2019
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#define G2T_MIN(a, b)       ((a) < (b) ? (a) : (b))
#define G2T_MAX(a, b)       ((a) > (b) ? (a) : (b))

#define TUI_MIN(a, b)       ((a) < (b) ? (a) : (b))
#define TUI_MAX(a, b)       ((a) > (b) ? (a) : (b))


#define GF_CONSOLAS         "Consolas"
#define GF_COURIER          "Courier"
#define GF_COURIER_NEW      "Courier New"
#define GF_FIXEDSYS         "Fixedsys"
#define GF_LUCIDA_CONSOLE   "Lucida Console"
#define GF_TERMINAL         "Terminal"

#define G2T_SCREEN_LINES    25
#define G2T_SCREEN_COLS     80
#define G2T_HTEXTSPACES     (0)
#define G2T_RGB_BLACK       RGB(  0,   0,   0)
#define G2T_RGB_RED         RGB(255,   0,   0)
#define G2T_RGB_GREEN       RGB(  0, 255,   0)
#define G2T_RGB_BLUE        RGB(  0,   0, 255)
#define G2T_RGB_YELLOW      RGB(255, 255,   0)
#define G2T_RGB_MAGENTA     RGB(255,   0, 255)
#define G2T_RGB_CYAN        RGB(  0, 255, 255)
#define G2T_RGB_WHITE       RGB(255, 255, 255)
#define G2T_RGB_GRAY        RGB(128, 128, 128)

#define ENV_FONT                            "Env.Font"
#define ENV_FONT_HSPACES                    "Env.FontHSpaces"
#define ENV_SCREEN_LINES                    "Env.Screen.Lines"
#define ENV_SCREEN_COLS                     "Env.Screen.Cols"
    /*color schemes */
#define ENV_TEXTCOLOR                       "Env.TextColor"
#define ENV_BGCOLOR                         "Env.BgColor"
#define ENV_DISABLED_TEXTCOLOR              "Env.Disabled.TextColor"
#define ENV_DISABLED_BGCOLOR                "Env.Disabled.BgColor"
#define ENV_HIGHLIGHTED_TEXTCOLOR           "Env.Highlighted.TextColor"
#define ENV_HIGHLIGHTED_BGCOLOR             "Env.Highlighted.BgColor"
#define STATIC_TEXTCOLOR                    "Static.TextColor"
#define STATIC_BGCOLOR                      "Static.BgColor"
#define STATIC_MNEMONIC_TEXTCOLOR           "Static.Mnemonic.TextColor"
#define STATIC_MNEMONIC_BGCOLOR             "Static.Mnemonic.BgColor"
#define EDITBOX_TEXTCOLOR                   "EditBox.TextColor"
#define EDITBOX_BGCOLOR                     "EditBox.BgColor"
#define EDITBOX_DISABLED_TEXTCOLOR          "EditBox.Disabled.TextColor"
#define EDITBOX_DISABLED_BGCOLOR            "EditBox.Disabled.BgColor"
#define LISTBOX_TEXTCOLOR                   "ListBox.TextColor"
#define LISTBOX_BGCOLOR                     "ListBox.BgColor"
#define LISTBOX_ODD_TEXTCOLOR               "ListBox.Odd.TextColor"
#define LISTBOX_ODD_BGCOLOR                 "ListBox.Odd.BgColor"
#define LISTBOX_EVEN_TEXTCOLOR              "ListBox.Even.TextColor"
#define LISTBOX_EVEN_BGCOLOR                "ListBox.Even.BgColor"
#define LISTBOX_DISABLED_TEXTCOLOR          "ListBox.Disabled.TextColor"
#define LISTBOX_DISABLED_BGCOLOR            "ListBox.Disabled.BgColor"
#define LISTBOX_HIGHLIGHTED_TEXTCOLOR       "ListBox.Highlighted.TextColor"
#define LISTBOX_HIGHLIGHTED_BGCOLOR         "ListBox.Highlighted.BgColor"
#define LISTCTRL_TEXTCOLOR                  "ListCtrl.TextColor"
#define LISTCTRL_BGCOLOR                    "ListCtrl.BgColor"
#define LISTCTRL_ODD_TEXTCOLOR              "ListCtrl.Odd.TextColor"
#define LISTCTRL_ODD_BGCOLOR                "ListCtrl.Odd.BgColor"
#define LISTCTRL_EVEN_TEXTCOLOR             "ListCtrl.Even.TextColor"
#define LISTCTRL_EVEN_BGCOLOR               "ListCtrl.Even.BgColor"
#define LISTCTRL_DISABLED_TEXTCOLOR         "ListCtrl.Disabled.TextColor"
#define LISTCTRL_DISABLED_BGCOLOR           "ListCtrl.Disabled.BgColor"
#define LISTCTRL_HIGHLIGHTED_TEXTCOLOR      "ListCtrl.Highlighted.TextColor"
#define LISTCTRL_HIGHLIGHTED_BGCOLOR        "ListCtrl.Highlighted.BgColor"
#define LISTCTRL_HEADER_TEXTCOLOR           "ListCtrl.Header.TextColor"
#define LISTCTRL_HEADER_BGCOLOR             "ListCtrl.Header.BgColor"
#define TREECTRL_TEXTCOLOR                  "TreeCtrl.TextColor"
#define TREECTRL_BGCOLOR                    "TreeCtrl.BgColor"
#define TREECTRL_ODD_TEXTCOLOR              "TreeCtrl.Odd.TextColor"
#define TREECTRL_ODD_BGCOLOR                "TreeCtrl.Odd.BgColor"
#define TREECTRL_EVEN_TEXTCOLOR             "TreeCtrl.Even.TextColor"
#define TREECTRL_EVEN_BGCOLOR               "TreeCtrl.Even.BgColor"
#define TREECTRL_DISABLED_TEXTCOLOR         "TreeCtrl.Disabled.TextColor"
#define TREECTRL_HIGHLIGHTED_TEXTCOLOR      "TreeCtrl.Highlighted.TextColor"
#define TREECTRL_HIGHLIGHTED_BGCOLOR        "TreeCtrl.Highlighted.BgColor"
#define APP_PRICE_TEXTCOLOR                 "App.Price.TextColor"
#define APP_PRICE_BGCOLOR                   "App.Price.BgColor"
#define APP_PRICE_UP_TEXTCOLOR              "App.Price.Up.TextColor"
#define APP_PRICE_UP_BGCOLOR                "App.Price.Up.BgColor"
#define APP_PRICE_DOWN_TEXTCOLOR            "App.Price.Down.TextColor"
#define APP_PRICE_DOWN_BGCOLOR              "App.Price.Down.BgColor"

#define G2T_STATIC                          "G2T_STATIC"
#define G2T_EDITBOX                         "G2T_EDITBOX"
#define G2T_LISTBOX                         "G2T_LISTBOX"
#define G2T_LISTCTRL                        "G2T_LISTCTRL"
#define G2T_TREECTRL                        "G2T_TREECTRL"
#define G2T_TSCHART         "G2T_TIMESERIESCHART"
#define G2T_PAGECTRL                        "G2T_PAGECTRL"
#define G2T_CHILDPAGE                       "G2T_CHILDPAGE"

    /* status */
#define G2T_BYPASS          (0xffff)
#define G2T_CONTINUE        (1)
#define G2T_OK              (0)
#define G2T_ERROR           (-1)
#define G2T_ENOMEM          (-2)
    /* general purposes */
#define G2T_MAX_WNDTEXT     (128)
#define G2T_MAX_NMHDR       (512)
#define G2T_FALSE           (0)
#define G2T_TRUE            (1)
#define G2T_BUFSIZ          (512)

#define TUI_FALSE           G2T_FALSE
#define TUI_TRUE            G2T_TRUE
#define TUI_CONTINUE        G2T_CONTINUE
#define TUI_OK              G2T_OK      
#define TUI_ERROR           G2T_ERROR   
#define TUI_MEM             G2T_ENOMEM  
#define TUI_MAX_WNDTEXT     G2T_MAX_WNDTEXT

    /* windows */
#define GALIGN_LEFT         0
#define GALIGN_CENTER       1
#define GALIGN_RIGHT        2

#define TALIGN_LEFT         GALIGN_LEFT  
#define TALIGN_CENTER       GALIGN_CENTER
#define TALIGN_RIGHT        GALIGN_RIGHT 

#define GW_HIDE             0
#define GW_SHOW             1

#define TW_HIDE             GW_HIDE
#define TW_SHOW             GW_SHOW

#define GW_DISABLE          0
#define GW_ENABLE           1

#define TW_DISABLE          GW_DISABLE
#define TW_ENABLE           GW_ENABLE 

    /* window styles */
#define GWS_WINDOW              0x00000001
#define GWS_CHILD               0x00000002
#define GWS_VISIBLE             0x00000004
#define GWS_DISABLED            0x00000008
#define GWS_BORDER              0x00000010
#define GWS_LEFT                0x00000000             /* shared controls style */
#define GWS_CENTER              0x00010000
#define GWS_RIGHT               0x00020000

#define TWS_WINDOW              GWS_WINDOW  
#define TWS_CHILD               GWS_CHILD   
#define TWS_VISIBLE             GWS_VISIBLE 
#define TWS_DISABLED            GWS_DISABLED
#define TWS_BORDER              GWS_BORDER  
#define TWS_LEFT                GWS_LEFT                 /* shared controls style */
#define TWS_CENTER              GWS_CENTER  
#define TWS_RIGHT               GWS_RIGHT   

    /* window messages */
#define GWM_FIRST               100
#define GWM_CREATE              (GWM_FIRST +    1)
#define GWM_DESTROY             (GWM_FIRST +    2)
#define GWM_INITDIALOG          (GWM_FIRST +    3) /* get when system initialized window frame */
#define GWM_PAINT               (GWM_FIRST +    4) /* paint window        */
#define GWM_SETFOCUS            (GWM_FIRST +    5) /* get focus           */
#define GWM_KILLFOCUS           (GWM_FIRST +    6) /* lost focus          */
#define GWM_KEYDOWN             (GWM_FIRST +    7) /* key down            */
#define GWM_KEYUP               (GWM_FIRST +    8) /* key up              */
#define GWM_CHAR                (GWM_FIRST +    9) /* character input     */
#define GWM_NOTIFY              (GWM_FIRST +   10) /* control sent notify to parent */
#define GWM_ERASEBK             (GWM_FIRST +   11) /* erase background    */
#define GWM_SETTEXT             (GWM_FIRST +   12) /* set window text     */
#define GWM_GETTEXT             (GWM_FIRST +   13) /* get window text     */
#define GWM_SETTEXTALIGN        (GWM_FIRST +   14) /* set text alignment  */
#define GWM_GETTEXTALIGN        (GWM_FIRST +   15) /* set text alignment  */
#define GWM_COMMAND             (GWM_FIRST +   16) /* control sent command to parent */
#define GWM_SETTEXTATTRS        (GWM_FIRST +   17) /* set text attributes */
#define GWM_GETTEXTATTRS        (GWM_FIRST +   18) /* set text attributes */
#define GWM_DRAWITEM            (GWM_FIRST +   19)
#define GWM_SETINFOTEXT         (GWM_FIRST +   20) /* set info text       */
#define GWM_GETINFOTEXT         (GWM_FIRST +   21) /* get info text       */
#define GWM_SHOW                (GWM_FIRST +   22) /* show window         */
#define GWM_ENABLE              (GWM_FIRST +   23) /* enable window       */
#define GWM_GETCURSOR           (GWM_FIRST +   24) /* get cursor          */
#define GWM_SETCURSOR           (GWM_FIRST +   25) /* set cursor          */
#define GWM_INITPAGE            (GWM_FIRST +   26) /* get when system initialized window page */
#define GWM_GETACTIVECHILD      (GWM_FIRST +   27)

#define TWM_FIRST               GWM_FIRST       
#define TWM_CREATE              GWM_CREATE      
#define TWM_DESTROY             GWM_DESTROY     
#define TWM_INITDIALOG          GWM_INITDIALOG   /* get when system initialized window frame */
#define TWM_PAINT               GWM_PAINT        /* paint window        */
#define TWM_SETFOCUS            GWM_SETFOCUS     /* get focus           */
#define TWM_KILLFOCUS           GWM_KILLFOCUS    /* lost focus          */
#define TWM_KEYDOWN             GWM_KEYDOWN      /* key down            */
#define TWM_KEYUP               GWM_KEYUP        /* key up              */
#define TWM_CHAR                GWM_CHAR         /* character input     */
#define TWM_NOTIFY              GWM_NOTIFY       /* control sent notify to parent */
#define TWM_ERASEBK             GWM_ERASEBK      /* erase background    */
#define TWM_SETTEXT             GWM_SETTEXT      /* set window text     */
#define TWM_GETTEXT             GWM_GETTEXT      /* get window text     */
#define TWM_SETTEXTALIGN        GWM_SETTEXTALIGN /* set text alignment  */
#define TWM_GETTEXTALIGN        GWM_GETTEXTALIGN /* set text alignment  */
#define TWM_COMMAND             GWM_COMMAND      /* control sent command to parent */
#define TWM_SETTEXTATTRS        GWM_SETTEXTATTRS /* set text attributes */
#define TWM_GETTEXTATTRS        GWM_GETTEXTATTRS /* set text attributes */
#define TWM_DRAWITEM            GWM_DRAWITEM    
#define TWM_SETINFOTEXT         GWM_SETINFOTEXT  /* set info text       */
#define TWM_GETINFOTEXT         GWM_GETINFOTEXT  /* get info text       */
#define TWM_SHOW                GWM_SHOW         /* show window         */
#define TWM_ENABLE              GWM_ENABLE       /* enable window       */
#define TWM_GETCURSOR           GWM_GETCURSOR    /* get cursor          */
#define TWM_SETCURSOR           GWM_SETCURSOR    /* set cursor          */

    /* application user messages */
#define GWM_USER                (1000)
#define GWM_MSGPOST             (GWM_USER + 10000)
#define GWM_NETMESSAGE          (GWM_USER + 10100)
#define GWM_SCREENREFRESH       (GWM_USER + 10101)

    /* notification control message */
#define GCN_FIRST               (GWM_USER +   50)
#define GSN_FIRST               (GWM_USER +  100)
#define GEN_FIRST               (GWM_USER +  150)
#define GLBN_FIRST              (GWM_USER +  200)
#define GBN_FIRST               (GWM_USER +  250)
#define GLCN_FIRST              (GWM_USER +  300)
#define GTCN_FIRST              (GWM_USER +  350)

#define TWM_USER                GWM_USER

/* notification control message */
#define TSN_FIRST               (TWM_USER +  100)
#define TEDN_FIRST              (TWM_USER +  150)
#define TLBN_FIRST              (TWM_USER +  200)
#define TLCN_FIRST              (TWM_USER +  300)
#define TTCN_FIRST              (TWM_USER +  350)
#define TPCN_FIRST              (TWM_USER +  400)

    /* display info */
#define GCN_DISPLAYINFO         (GCN_FIRST +   1) /* see also DISPLAYINFO */
    /*-------------------------------------------------------------------
    * static control
    *-----------------------------------------------------------------*/
#define GSS_LEFT                GWS_LEFT
#define GSS_CENTER              GWS_CENTER
#define GSS_RIGHT               GWS_RIGHT

#define TSS_LEFT                GSS_LEFT  
#define TSS_CENTER              GSS_CENTER
#define TSS_RIGHT               GSS_RIGHT 

    /*-------------------------------------------------------------------
    * edit control
    *-----------------------------------------------------------------*/

#define GES_LEFT                GWS_LEFT
#define GES_CENTER              GWS_CENTER
#define GES_RIGHT               GWS_RIGHT
#define GES_NUMBER              0x00040000
#define GES_UPPERCASE           0x00080000
#define GES_LOWERCASE           0x00100000
#define GES_PASSWORD            0x00200000
#define GES_APPENDMODE          0x00400000
#define GES_AUTOHSCROLL         0x00800000
#define GES_DECIMAL             0x01000000
#define GES_AUTODECIMALCOMMA    0x02000000
#define GES_UNDERLINE           0x04000000
#define GES_A2Z                 0x08000000
#define GES_AUTOSUFFIX          0x10000000
#define GES_NOTIFYKEYPRESSED    0x20000000

#define GEM_LIMITTEXT           (GWM_USER  +    1)
#define GEM_SETPASSWDCHAR       (GWM_USER  +    2)
#define GEM_SHOWPASSWDCHAR      (GWM_USER  +    3)
#define GEM_SETDECWIDTH         (GWM_USER  +    4)
#define GEM_SETVALIDSTRING      (GWM_USER  +    5)
#define GEM_SETVALIDMINMAX      (GWM_USER  +    6)

    /* edit notified message */
#define GEN_CHANGED             (GEN_FIRST  +   0)
#define GEN_SETFOCUS            (GEN_FIRST  +   1)
#define GEN_KILLFOCUS           (GEN_FIRST  +   2)
#define GEN_KEYPRESSED          (GEN_FIRST  +   3)

/*-------------------------------------------------------------------
* edit control
*-----------------------------------------------------------------*/

#define TES_LEFT                TWS_LEFT
#define TES_CENTER              TWS_CENTER
#define TES_RIGHT               TWS_RIGHT
#define TES_NUMBER              0x00040000
#define TES_UPPERCASE           0x00080000
#define TES_LOWERCASE           0x00100000
#define TES_PASSWORD            0x00200000
#define TES_APPENDMODE          0x00400000
#define TES_AUTOHSCROLL         0x00800000
#define TES_DECIMAL             0x01000000
#define TES_AUTODECIMALCOMMA    0x02000000
#define TES_UNDERLINE           0x04000000
#define TES_A2Z                 0x08000000
#define TES_AUTOSUFFIX          0x10000000
#define TES_NOTIFYKEYPRESSED    0x20000000

#define TEM_LIMITTEXT           (TWM_USER  +    1)
#define TEM_SETPASSWDCHAR       (TWM_USER  +    2)
#define TEM_SHOWPASSWDCHAR      (TWM_USER  +    3)
#define TEM_SETDECWIDTH         (TWM_USER  +    4)
#define TEM_SETVALIDSTRING      (TWM_USER  +    5)
#define TEM_SETVALIDMINMAX      (TWM_USER  +    6)

/* edit notified message */
#define TEN_CHANGED             (TEDN_FIRST  +   0)
#define TEN_SETFOCUS            (TEDN_FIRST  +   1)
#define TEN_KILLFOCUS           (TEDN_FIRST  +   2)
#define TEN_KEYPRESSED          (TEDN_FIRST  +   3)

/*-------------------------------------------------------------------
* listbox control
*-----------------------------------------------------------------*/

#define GLB_OK                   G2T_OK
#define GLB_ERROR                G2T_ERROR
#define GLB_UNCHECKED            G2T_FALSE
#define GLB_CHECKED              G2T_TRUE

#define GLBS_LEFT               GWS_LEFT
#define GLBS_CENTER             GWS_CENTER
#define GLBS_RIGHT              GWS_RIGHT
#define GLBS_OWNERDRAW          0x00100000
#define GLBS_CHECKBOX           0x00200000
#define GLBS_RADIOBOX           0x00400000
#define GLBS_MULTICOLOR         0x00800000

#define GLBM_FIRST              (GWM_USER)
#define GLBM_ADDITEM            (GLBM_FIRST  +    1)
#define GLBM_DELETEITEM         (GLBM_FIRST  +    2)
#define GLBM_GETCURSEL          (GLBM_FIRST  +    3)
#define GLBM_SETCURSEL          (GLBM_FIRST  +    4)
#define GLBM_DELETEALLITEMS     (GLBM_FIRST  +    5)
#define GLBM_GETITEMCOUNT       (GLBM_FIRST  +    6)
#define GLBM_SETITEMDATA        (GLBM_FIRST  +    7)
#define GLBM_GETITEMDATA        (GLBM_FIRST  +    8)
#define GLBM_SETITEMTEXT        (GLBM_FIRST  +    9)
#define GLBM_GETITEMTEXT        (GLBM_FIRST  +   10)
#define GLBM_SETITEMCHECKED     (GLBM_FIRST  +   11)
#define GLBM_GETITEMCHECKED     (GLBM_FIRST  +   12)
#define GLBM_COUNTITEMCHECKED   (GLBM_FIRST  +   13)
#define GLBM_INSERTFIRSTITEM    (GLBM_FIRST  +   14)
#define GLBM_LAST               (GLBM_FIRST  +   50)

    /* listbox notified message */
#define GLBN_SETFOCUS           (GLBN_FIRST  +    0)
#define GLBN_KILLFOCUS          (GLBN_FIRST  +    1)
#define GLBN_SELCHANGED         (GLBN_FIRST  +    2)
#define GLBN_ENTERITEM          (GLBN_FIRST  +    3)

/*-------------------------------------------------------------------
* listctrl control
*-----------------------------------------------------------------*/
#define TLCS_NOHEADER           0x00010000
#define TLCS_EDITABLE           0x00020000
#define TLCS_LINEEDIT           0x00040000
#define TLCS_NOSELECTION        0x00080000
#define TLCS_NOBORDER           0x00100000
#define TLCS_MULTICOLOR         0x00800000

#define TLCM_FIRST              (TWM_USER     +   0)
#define TLCM_ADDCOLUMN          (TLCM_FIRST   +   1)
#define TLCM_DELETECOLUMN       (TLCM_FIRST   +   2)
#define TLCM_DELETEALLCOLUMNS   (TLCM_FIRST   +   3)
#define TLCM_ADDITEM            (TLCM_FIRST   +   4)
#define TLCM_DELETEITEM         (TLCM_FIRST   +   5)
#define TLCM_DELETEALLITEMS     (TLCM_FIRST   +   6)
#define TLCM_SETITEM            (TLCM_FIRST   +   7)
#define TLCM_GETITEM            (TLCM_FIRST   +   8)
#define TLCM_GETITEMCOUNT       (TLCM_FIRST   +   9)
#define TLCM_INVALIDATEITEM     (TLCM_FIRST   +  10)
#define TLCM_SETEDITSTYLE       (TLCM_FIRST   +  11)
#define TLCM_GETEDITSTYLE       (TLCM_FIRST   +  12)
#define TLCM_GETCURROW          (TLCM_FIRST   +  13)
#define TLCM_SETCURROW          (TLCM_FIRST   +  14)
#define TLCM_SETCOLWIDTH        (TLCM_FIRST   +  15)
#define TLCM_GETEDITBOX         (TLCM_FIRST   +  16)
#define TLCM_SETCURPAGE         (TLCM_FIRST   +  17)
#define TLCM_GETCURPAGE         (TLCM_FIRST   +  18)
#define TLCM_GETITEMSPERPAGE    (TLCM_FIRST   +  19)
#define TLCM_SETEDITABLECOLS    (TLCM_FIRST   +  20)
#define TLCM_INSERTFIRSTITEM    (TLCM_FIRST   +  21)
#define TLCM_LAST               (TLCM_FIRST   +  50)
/*
#define TLCN_FIRST              (TWM_USER +  300)
*/
/* listctrl states */

#define TLCT_VIEW                   0x0000
#define TLCT_BEGINEDITING           0x0001
#define TLCT_EDITING                0x0002
#define TLCT_ENDEDITING             0x0000
#define TLCT_BEGININSERTING         0x0001
#define TLCT_INSERTING              0x0002
#define TLCT_ENDINSERTING           0x0000
#define TLCT_BEGINMOVING            0x0001
#define TLCT_MOVINGCURSOR           0x0002
#define TLCT_ENDMOVING              0x0000



#define TLCN_SETFOCUS               (TLCN_FIRST  +    0)
#define TLCN_KILLFOCUS              (TLCN_FIRST  +    1)
#define TLCN_SELCHANGED             (TLCN_FIRST  +    2)
#define TLCN_BEGINMOVING            (TLCN_FIRST  +    3)
#define TLCN_MOVINGCURSOR           (TLCN_FIRST  +    4)
#define TLCN_ENDMOVING              (TLCN_FIRST  +    5)
#define TLCN_BEGINEDITROW           (TLCN_FIRST  +    6)
#define TLCN_ENDEDITROWOK           (TLCN_FIRST  +    7)
#define TLCN_ENDEDITROWCANCEL       (TLCN_FIRST  +    8)
#define TLCN_BEGINEDIT              (TLCN_FIRST  +    9)
#define TLCN_ENDEDITOK              (TLCN_FIRST  +   10)
#define TLCN_ENDEDITCANCEL          (TLCN_FIRST  +   11)
#define TLCN_BEGININSERTROW         (TLCN_FIRST  +   12)
#define TLCN_ENDINSERTROWOK         (TLCN_FIRST  +   13)
#define TLCN_ENDINSERTROWCANCEL     (TLCN_FIRST  +   14)


#define TLC_ENDEDITOK               1
#define TLC_ENDEDITCANCEL           0

#define TLCFM_TEXT                  0x0001
#define TLCFM_ATTRS                 0x0002
#define TLCFM_DATA                  0x0004

/*-------------------------------------------------------------------
* tree ctrl
*-----------------------------------------------------------------*/
#define TTCS_NOHIGHLIGHT            0x00100000
#define TTCS_FULLSECROW             0x00200000
#define TTCS_SHOWNODE               0x00400000
#define TTCS_MULTICOLOR             0x00800000

#define TTCM_FIRST                  (TWM_USER   +     0)
#define TTCM_INSERTITEM             (TTCM_FIRST +     1)
#define TTCM_DELETEITEM             (TTCM_FIRST +     2)
#define TTCM_SETITEM                (TTCM_FIRST +     3)
#define TTCM_GETITEM                (TTCM_FIRST +     4)
#define TTCM_FINDITEM               (TTCM_FIRST +     5)
#define TTCM_FINDNEXTITEM           (TTCM_FIRST +     6)
#define TTCM_SETFINDITEMPROC        (TTCM_FIRST +     7)
#define TTCM_COLLAPSEITEM           (TTCM_FIRST +     8)
#define TTCM_EXPANDITEM             (TTCM_FIRST +     9)
#define TTCM_SETSELITEM             (TTCM_FIRST +    10)
#define TTCM_GETSELITEM             (TTCM_FIRST +    11)
#define TTCM_EXPORTTOFILE           (TTCM_FIRST +    12)
#define TTCM_IMPORTFROMFILE         (TTCM_FIRST +    13)
#define TTCM_COLLAPSEALLITEMS       (TTCM_FIRST +    14)
#define TTCM_EXPANDALLITEMS         (TTCM_FIRST +    15)
#define TTCM_SETINDENTTEXT          (TTCM_FIRST +    16)
#define TTCM_GETINDENTTEXT          (TTCM_FIRST +    17)
#define TTCM_SETSHIFTEDTEXT         (TTCM_FIRST +    18)
#define TTCM_GETSHIFTEDTEXT         (TTCM_FIRST +    19)

/* Tree control notification messages */
#define TTCN_ITEMCHANGING           (TTCN_FIRST +     0)
#define TTCN_ITEMCHANGED            (TTCN_FIRST +     1)
#define TTCN_ITEMEXPANDING          (TTCN_FIRST +     2)
#define TTCN_ITEMEXPANDED           (TTCN_FIRST +     3)
#define TTCN_ITEMCOLLAPSING         (TTCN_FIRST +     4)
#define TTCN_ITEMCOLLAPSED          (TTCN_FIRST +     5)
#define TTCN_KILLFOCUS              (TTCN_FIRST +     6)
#define TTCN_SETFOCUS               (TTCN_FIRST +     7)
#define TTCN_ENTERITEM              (TTCN_FIRST  +    8)

/*-------------------------------------------------------------------
* page control
*-----------------------------------------------------------------*/
#define TPCM_FIRST                  (TWM_USER   +     0)
#define TPCM_ADDPAGE                (TPCM_FIRST +     1)
#define TPCM_REMOVEPAGE             (TPCM_FIRST +     2)
#define TPCM_REMOVEALLPAGES         (TPCM_FIRST +     3)
#define TPCM_REMOVEPAGEID           (TPCM_FIRST +     4)
#define TPCM_GOTOPAGE               (TPCM_FIRST +     5)
#define TPCM_GOTOFIRSTPAGE          (TPCM_FIRST +     6)
#define TPCM_GOTOLASTPAGE           (TPCM_FIRST +     7)
#define TPCM_GOTOPREVPAGE           (TPCM_FIRST +     8)
#define TPCM_GOTONEXTPAGE           (TPCM_FIRST +     9)
#define TPCM_GETPAGEID              (TPCM_FIRST +    10)
/*child page*/
#define TCPM_FIRST                  (TWM_USER   +     0)
#define TCPM_SHOWPAGE               (TCPM_FIRST +     1) /* get when system initialized window page */
#define TCPM_ACTIVATEPAGE           (TCPM_FIRST +     2)

/* define types */

struct _g2t_env;
typedef struct _g2t_env* g2t_env;

struct _g2t_wnd;
typedef struct _g2t_wnd* g2t_wnd;

typedef HDC g2t_dc;

typedef VOID        g2t_void;
typedef CHAR        g2t_int8;
typedef SHORT       g2t_int16;
typedef LONG        g2t_int32;
typedef FLOAT       g2t_float32;
typedef DOUBLE      g2t_float64;

typedef BYTE        g2t_uint8;
typedef WORD        g2t_uint16;
typedef DWORD       g2t_uint32;

typedef INT         g2t_bool;
typedef BYTE        g2t_byte;
typedef CHAR        g2t_char;
typedef CHAR*       g2t_string;
typedef DWORD       g2t_dword;
typedef INT         g2t_int;
typedef LONG        g2t_long;
typedef FLOAT       g2t_float;
typedef DOUBLE      g2t_double;
typedef VOID*       g2t_buffer;

typedef g2t_uint32  g2t_lparam;
typedef g2t_uint32  g2t_wparam;

typedef g2t_uint32  g2t_status;

typedef g2t_int     TINT;
typedef g2t_long    TLONG;
typedef g2t_wparam  TWPARAM;
typedef g2t_lparam  TLPARAM;
typedef g2t_dword   TDWORD;
typedef g2t_wnd     TWND;
typedef g2t_dc      TDC;
typedef g2t_uint32  TUINT;
typedef const g2t_string TLPCSTR;
typedef g2t_char    TUI_CHAR;
typedef g2t_bool    TBOOL;
typedef g2t_void    TVOID;
typedef g2t_void*   TLPVOID;
typedef g2t_bool    TUI_BOOL;

typedef g2t_void    tui_void;
typedef g2t_uint32  tui_ui32;
typedef g2t_int32   tui_i32;

struct _g2t_pos
{
    g2t_int y;
    g2t_int x;
};
typedef struct _g2t_pos g2t_pos;

struct _g2t_rect
{
    g2t_int y;
    g2t_int x;
    g2t_int lines;
    g2t_int cols;
};
typedef struct _g2t_rect g2t_rect;
typedef struct _g2t_rect TRECT;



/* message structure */
struct _g2t_msg
{
    g2t_wnd     wnd;         /* window handle  */
    g2t_uint32  msg;         /* window message */
    g2t_wparam  wparam;      /* parameter      */
    g2t_wparam  lparam;      /* parameter      */
};
typedef struct _g2t_msg g2t_msg;

/* notify message structure */
struct _g2t_notify
{
    g2t_uint32  id;            /* control id     */
    g2t_uint32  code;          /* notify code    */
    g2t_wnd     ctl;           /* control handle */
};
typedef struct _g2t_notify g2t_nhdr;

/* notify message structure */
struct _NMHDRSTRUCT
{
    TUINT       id;            /* control id     */
    TUINT       code;          /* notify code    */
    TWND        ctl;           /* control handle */
};
typedef struct _NMHDRSTRUCT TNMHDR;

struct _g2t_dispinfo
{
    g2t_nhdr    hdr;
    g2t_char    text[G2T_MAX_WNDTEXT + 1];
};
typedef struct _g2t_dispinfo g2t_dispinfo;

struct _VALIDMINMAXSTRUCT
{
    g2t_int        min;
    g2t_int        max;
};
typedef struct _VALIDMINMAXSTRUCT g2t_validminmax;

struct _DRAWITEMSTRUCT
{
    g2t_int  idx;
    g2t_rect rcitem;
};
typedef struct _DRAWITEMSTRUCT g2t_drawitem;

struct _NMHDRITEMSTRUCT
{
    TNMHDR      hdr;          /* notification header */
    TINT        row;
    TINT        col;
};
typedef struct _NMHDRITEMSTRUCT TNMHDRITEMSTRUCT;


struct _TREE_ITER_STRUCT;
typedef struct _TREE_ITER_STRUCT TTREEITEM;

struct _TTREEITEMDATASTRUCT
{
    TUI_CHAR    itemtext[TUI_MAX_WNDTEXT + 1];
    TLPARAM     lparam;
    TBOOL       expanded;
    TBOOL       selected;
    TLONG       children;
    TBOOL       populated;
};
typedef struct _TTREEITEMDATASTRUCT TTREEITEMDATA;

struct _NMHDRTREEITEMSTRUCT
{
    TNMHDR hdr;          /* notification header */
    TTREEITEM* item;
};
typedef struct _NMHDRTREEITEMSTRUCT TNMHDRTREEITEMCTRL;

/*typedef TLONG (*LPTREEFINDITEMPROC)(const TVOID*, const TVOID*);*/
typedef tui_i32(*LPTREEFINDITEMPROC)(const void*, const void*);
typedef TLONG(*LPTREEEXPORTPROC)(FILE*, TTREEITEMDATA*);
typedef TLONG(*LPTREEIMPORTPROC)(TTREEITEMDATA*, TLPCSTR);


typedef CRITICAL_SECTION g2t_lockable;
g2t_status g2t_InitLockedObject(g2t_lockable*);
g2t_void   g2t_Lock(g2t_lockable*);
g2t_void   g2t_UnLock(g2t_lockable*);
g2t_void   g2t_ReleaseLockedObject(g2t_lockable*);

/* string */
g2t_uint32 g2t_strlen(g2t_string str);
g2t_string g2t_strcpy(g2t_string dst, g2t_string src);
g2t_int    g2t_strcmp(g2t_string dst, g2t_string src);
g2t_string g2t_strnew(g2t_uint32 len);
g2t_void   g2t_strdel(g2t_string str);
g2t_string g2t_strcat(g2t_string dst, g2t_string src);
g2t_string g2t_strncpy(g2t_string dst, g2t_string src, g2t_uint32 len);
g2t_string g2t_strstr(g2t_string src, g2t_string searchstr);
g2t_string g2t_strchr(g2t_string src, g2t_char searchch);
g2t_long   g2t_strcpywo(g2t_string dst, g2t_string src, g2t_char ignoredch);

/* simple memory */
g2t_buffer g2t_malloc(g2t_uint32 len);
g2t_void   g2t_free(g2t_buffer buf);
g2t_status g2t_memcpy(g2t_buffer dst, g2t_buffer src, g2t_uint32 len);
g2t_int    g2t_memcmp(g2t_buffer dst, g2t_buffer src, g2t_uint32 len);
g2t_status g2t_memset(g2t_buffer dst, g2t_int val, g2t_uint32 len);

typedef g2t_long(*g2t_wndproc)(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
typedef g2t_int32(*g2t_validateproc)(g2t_wnd, g2t_string);

struct _g2t_frmwndtemplate
{
    g2t_string      clsname;        /* window class   */
    g2t_string      text;           /* window text    */
    g2t_uint32      id;             /* window id      */
    g2t_int         y;              /* y-position     */
    g2t_int         x;              /* x-position     */
    g2t_int         lines;          /* window lines   */
    g2t_int         cols;           /* window columns */
    g2t_dword       style;          /* window style   */
    g2t_dword       exstyle;        /* window style   */
    g2t_validateproc validateproc;  /* to validate value before exiting */
                                    /* the edit box return TUI_CONTINUE */
                                    /* if 2nd parameter is accepted     */
                                    /* otherwise return TUI_ERROR       */
    g2t_string      infotext;       /* to display information in a      */
                                    /* specific control                 */
};
typedef struct _g2t_frmwndtemplate g2t_frmwndtemplate;

/* accelerator */
struct _g2t_accel
{
    g2t_long        vkey;
    g2t_long        ctrlkey;
    g2t_long        shiftkey;
    g2t_uint32      cmd;
};
typedef struct _g2t_accel g2t_accel;


struct _HEADERITEMSTRUCT
{
    g2t_string      caption;
    TINT            cols;
    TINT            align;      /* column alignment         */
    TDWORD          textcolor;      /* header text attributes   */
    TDWORD          bgcolor;      /* header text attributes   */
    TDWORD          editstyle;  /* edit style, see TES_XXX  */
    TINT            decwidth;   /* TES_DECIMAL or TES_AUTODECIMALCOMMA, default 6 */
};
typedef struct _HEADERITEMSTRUCT THEADERITEM;


struct _SUBITEMSTRUCT
{
    TINT            col;      /* column index, zero based */
    TINT            idx;      /* row index, zero based    */
    g2t_string      text;
    TDWORD          textcolor;      /* header text attributes   */
    TDWORD          bgcolor;      /* header text attributes   */
    g2t_lparam      data;     /* user data                */
};
typedef struct _SUBITEMSTRUCT TSUBITEM;

/* function interfaces */
/*
g2t_status g2t_StartUp();
    Start g2t environment. This function MUST BE called first.
*/
g2t_status g2t_StartUp(HINSTANCE hInst);

/*
g2t_InitApplication();
    Create the main screen
*/
g2t_status g2t_InitApplication(g2t_string szClsName, g2t_string szTitle);
g2t_status g2t_LoadScreen(g2t_string scrproc, g2t_string scrname, g2t_frmwndtemplate* templ);
g2t_long   g2t_Run();

/*
g2t_void g2t_ShutDown();
    Shut down g2t environment. This function MUST BE called last.
*/
g2t_void g2t_ShutDown();

HWND g2t_GetHwnd();
HWND g2t_GetHwndParent();
g2t_void g2t_ReleaseEnvWnd();
g2t_status g2t_AllocateEnvWnd(
    g2t_dword dwExStyle,
    g2t_dword dwStyle,
    g2t_int   x,
    g2t_int   y,
    g2t_int   cx,
    g2t_int   cy,
    HWND      hwndParent,
    UINT      nID);
g2t_long g2t_GetMsgWndProc(UINT msg, WPARAM wparam, LPARAM lparam);
/*
g2t_env g2t_GetEnv();
    Get g2t environment.
*/
g2t_env g2t_GetEnv();
g2t_bool g2t_NewCaret();
g2t_bool g2t_SetCaretPos(g2t_int x, g2t_int y);
g2t_bool g2t_ShowCaret(g2t_bool show);
g2t_bool g2t_DelCaret();

g2t_long    g2t_GetScreenColumns();
g2t_long    g2t_GetScreenLines();
g2t_status  g2t_LoadProperties(g2t_string filename);
g2t_dword   g2t_GetSysColor(g2t_string);
g2t_dword   g2t_GetSysTextColor();
g2t_dword   g2t_GetSysBgColor();
g2t_dword   g2t_GetSysDisabledTextColor();
g2t_dword   g2t_GetSysDisabledBgColor();
g2t_dword   g2t_GetSysHighlightedTextColor();
g2t_dword   g2t_GetSysHighlightedBgColor();
g2t_dword   g2t_GetReverseColor(g2t_dword color);

/*
* g2t_DefWndProc()
*   All windows MUST be called at its window procedure.
*   g2t_wnd - window handle
*   g2t_uint32 - window message
*   g2t_wparam - first parameter
*   g2t_lparam - second parameter
*/
g2t_int32 g2t_DefWndProc(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
g2t_int32 g2t_DefPageWndProc(g2t_wnd, g2t_uint32, g2t_wparam, g2t_lparam);
/*
* g2t_RegisterCls()
*   All window classes MUST be registered before creating its window instance
*   clsname - a unique name in an application
*   wndproc - window procedure
*/
g2t_status g2t_RegisterCls(g2t_string, g2t_wndproc);

/*
* g2t_GetMsg()
*   Synchronously get message input from screen
*   msg - output from user input
*/
g2t_status g2t_GetMsg(g2t_msg* msg);
/*
* g2t_DispatchMsg()
*   Dispatch message from application and user input
*   msg - input/output from user input
*/
g2t_status g2t_DispatchMsg(g2t_msg* msg);
/*
* g2t_TranslateMsg()
*   Translate message from the application and user input
*   msg - input/output from user input
*/
g2t_status g2t_TranslateMsg(g2t_msg* msg);
/*
* g2t_PostQuitMsg()
*   Post message to quit an application
*/
g2t_status g2t_PostQuitMsg(g2t_int exitcode);
/*
* g2t_SendMsg()
*   Synchronously send message to window handle
*/
g2t_status g2t_SendMsg(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);
/*
* g2t_PostMsg()
*   Asynchronously send message to window handle
* The message will be dispatched once the application call TuiDispatchMsg()
*
* This function useful when the application does not need to wait response immediately
*/
g2t_status g2t_PostMsg(g2t_wnd wnd, g2t_uint32 msg, g2t_wparam wparam, g2t_lparam lparam);

g2t_wnd g2t_CreateWnd(
    g2t_string   clsname,
    g2t_string   wndname,
    g2t_dword    style,
    g2t_dword    exstyle,
    g2t_int      y,
    g2t_int      x,
    g2t_int      lines,
    g2t_int      cols,
    g2t_wnd      parent,
    g2t_uint32   id,
    g2t_string   infotext,
    g2t_void*    param
    );
g2t_wnd g2t_CreateFrameWnd(
    g2t_string   clsname,
    g2t_string   wndname,
    g2t_dword    style,
    g2t_dword    exstyle,
    g2t_int      y,
    g2t_int      x,
    g2t_int      lines,
    g2t_int      cols,
    g2t_frmwndtemplate* templs,
    g2t_void*    param
    );

g2t_wnd g2t_CreatePageWnd(
    g2t_string clsname,
    g2t_dword style,
    g2t_dword exstyle,
    g2t_int y,
    g2t_int x,
    g2t_int lines,
    g2t_int cols,
    g2t_wnd parent,
    g2t_uint32 id,
    g2t_frmwndtemplate* templs,
    g2t_lparam lparam);
/*
* g2t_ShowWnd()
*   Show window object
*   show - TW_SHOW or TW_HIDE
*/
g2t_status   g2t_ShowWnd(g2t_wnd wnd, g2t_bool show);
/*
* g2t_EnableWnd()
*   Show window object
*   show - TW_ENABLE or TW_DISABLE
*/
g2t_status   g2t_EnableWnd(g2t_wnd wnd, g2t_bool enable);

/*
* g2t_IsWndEnabled()
*   Check if window object is enabled
*/
g2t_bool   g2t_IsWndEnabled(g2t_wnd wnd);
/*
* g2t_IsWndVisible()
*   Check if window object is visible
*/
g2t_bool   g2t_IsWndVisible(g2t_wnd wnd);

/*
* g2t_GetWndRect()
*   Get window rectangle
*/
g2t_status   g2t_GetWndRect(g2t_wnd wnd, g2t_rect* rect);

/*
* g2t_GetWndID()
*   Get window id
*/
g2t_uint32   g2t_GetWndID(g2t_wnd wnd);

/*
* g2t_GetParent()
*   Get window parent
*/
g2t_dword g2t_SetBgColor(g2t_wnd wnd, g2t_dword color);
g2t_dword g2t_SetTextColor(g2t_wnd wnd, g2t_dword color);
g2t_dword g2t_GetBgColor(g2t_wnd wnd);
g2t_dword g2t_GetTextColor(g2t_wnd wnd);
/*
g2t_int32 g2t_ShowWnd(g2t_wnd wnd, g2t_bool show);
g2t_int32 g2t_EnableWnd(g2t_wnd wnd, g2t_bool enable);
*/
g2t_lparam g2t_SetWndParam(g2t_wnd wnd, g2t_lparam lparam);
g2t_lparam g2t_GetWndParam(g2t_wnd wnd);
g2t_long   g2t_GetClassName(g2t_wnd, g2t_string clsname, g2t_long len);
g2t_bool   g2t_LockUpdate(g2t_wnd, g2t_bool locked);

g2t_wnd     g2t_GetWndItem(g2t_wnd wnd, g2t_uint32 id);
g2t_wnd     g2t_GetParent(g2t_wnd wnd);
g2t_wnd     g2t_GetParent(g2t_wnd wnd);
g2t_wnd     g2t_GetFirstActiveChildWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetLastActiveChildWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetPrevActiveChildWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetActiveChildWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetNextActiveChildWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetNextActiveWnd(g2t_wnd);
g2t_wnd     g2t_GetPrevActiveWnd(g2t_wnd);
g2t_wnd     g2t_GetActiveWnd();
g2t_wnd     g2t_GetFirstWnd();
g2t_wnd     g2t_GetLastWnd();
g2t_wnd     g2t_GetFirstChildWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetLastChildWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetNextWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetPrevWnd(g2t_wnd wnd);
g2t_wnd     g2t_GetFrameWnd(g2t_wnd wnd);
g2t_wndproc g2t_SetPreTranslateMsg(g2t_wndproc, g2t_void*);
HWND        g2t_GetEnvWnd();
g2t_void    g2t_MoveEnvWnd(g2t_int x, g2t_int y, g2t_int cx, g2t_int cy);
g2t_dword   g2t_GetWndTextAttrs(g2t_wnd wnd);
g2t_void    g2t_SetWndText(g2t_wnd wnd, g2t_string text);
g2t_long    g2t_GetWndText(g2t_wnd wnd, g2t_string text, g2t_long cb);
g2t_wnd     g2t_GetFocus(g2t_wnd wnd);
g2t_status  g2t_SetFocus(g2t_wnd wnd);
g2t_dword   g2t_GetWndStyleEx(g2t_wnd wnd);
g2t_dword   g2t_GetWndStyle(g2t_wnd wnd);
g2t_void    g2t_DestroyWnd(g2t_wnd wnd);
g2t_validateproc g2t_SetWndValidateProc(g2t_wnd wnd, g2t_validateproc validateproc);
g2t_long    g2t_LoadAccel(g2t_accel* accel);
g2t_status  g2t_InvalidateWnd(g2t_wnd wnd);
g2t_long    g2t_IsWndValidate(g2t_wnd wnd, g2t_string text);
g2t_dword   g2t_SetWndStyleEx(g2t_wnd wnd, g2t_dword newstyle);
g2t_dword   g2t_SetWndStyle(g2t_wnd wnd, g2t_dword newstyle);

g2t_long       g2t_MoveWnd(g2t_wnd wnd, g2t_int y, g2t_int x, g2t_int lines, g2t_int cols);


/*
* g2t_GetDC()
*   Get window device context
*/
g2t_dc  g2t_GetDC();
g2t_dc  g2t_GetMemDC();
/*
* g2t_DrawText()
*   Draw a string on screen
*/
g2t_status g2t_TextToScreen(g2t_pos* spos, g2t_pos* tpos);
g2t_status g2t_ScreenToText(g2t_pos* tpos, g2t_pos* spos);
g2t_status g2t_DrawText(g2t_dc dc, g2t_int x, g2t_int y, g2t_string text, g2t_dword textcolor, g2t_dword bgcolor);
g2t_status g2t_MoveXY(g2t_int x, g2t_int y);
g2t_long g2t_PrintTextAlignment(g2t_string out, g2t_string in, g2t_long limit, g2t_long align);
g2t_long g2t_PutChar(
    g2t_dc dc,
    g2t_int x,
    g2t_int y,
    g2t_char   ch,
    g2t_dword  textcolor,
    g2t_dword  bgcolor);

/*
g2t_DrawFrameChar()
    dc - device context
    x  - x-pos
    y  - y-pos
    ch is one of (G2T_UPPER_LEFT, G2T_UPPER_RIGHT, G2T_LOWER_LEFT, G2T_LOWER_RIGHT, G2T_VERTICAL, G2T_HORIZONTAL)
*/
#define G2T_UPPER_LEFT      (1)
#define G2T_UPPER_RIGHT     (2)
#define G2T_LOWER_LEFT      (3)
#define G2T_LOWER_RIGHT     (4)
#define G2T_VERTICAL        (5)
#define G2T_HORIZONTAL      (6)
#define G2T_VERT_LEFT       (7)
#define G2T_VERT_RIGHT      (8)
#define G2T_HORZ_DOWN       (9)
#define G2T_HORZ_UP         (10)
#define G2T_VERT_HORZ       (11)

g2t_status g2t_DrawFrameChar(g2t_dc dc, g2t_int x, g2t_int y, g2t_int ch, g2t_dword textcolor, g2t_dword bgcolor);
g2t_status g2t_DrawVLine(g2t_dc dc, g2t_int x, g2t_int y, g2t_int blocks, g2t_dword textcolor, g2t_dword bgcolor);
g2t_status g2t_DrawHLine(g2t_dc dc, g2t_int x, g2t_int y, g2t_int blocks, g2t_dword textcolor, g2t_dword bgcolor);
g2t_status g2t_DrawFrame(g2t_dc dc, g2t_rect* rect, g2t_dword textcolor, g2t_dword bgcolor);
g2t_status g2t_DrawHFrames(g2t_dc dc, g2t_rect* rect, g2t_int* heights, g2t_int frames, g2t_dword textcolor, g2t_dword bgcolor);
g2t_status g2t_DrawVFrames(g2t_dc dc, g2t_rect* rect, g2t_int* widths, g2t_int frames, g2t_dword textcolor, g2t_dword bgcolor);



/*-------------------------------------------------------------------
* edit control macros
*-----------------------------------------------------------------*/

#define TEDT_LimitText(edt, lim)    \
    g2t_SendMsg(edt, GEM_LIMITTEXT, (g2t_wparam)lim, (g2t_lparam)0)
#define TEDT_SetPasswdChar(edt, ch)    \
    g2t_SendMsg(edt, GEM_SETPASSWDCHAR, (g2t_wparam)ch, (g2t_lparam)0)
#define TEDT_ShowPasswdChar(edt, show)    \
    g2t_SendMsg(edt, GEM_SHOWPASSWDCHAR, (g2t_wparam)show, (g2t_lparam)0)
#define TEDT_SetDecimalWidth(edt, wid)    \
    g2t_SendMsg(edt, GEM_SETDECWIDTH, (g2t_wparam)wid, (g2t_lparam)0)
#define TEDT_SetValidString(edt, val)    \
    g2t_SendMsg(edt, GEM_SETVALIDSTRING, (g2t_wparam)0, (g2t_lparam)val)


#define TEDT_SetValidMinMax(edt, mn, mx, on)    \
do \
{ \
    \
    g2t_validminmax val; \
    val.min = mn; \
    val.max = mx; \
    g2t_SendMsg((edt), GEM_SETVALIDMINMAX, \
    (g2t_wparam)((on) == GW_ENABLE ? GW_ENABLE : GW_DISABLE), (g2t_lparam)&val); \
} while (0)



        /*-------------------------------------------------------------------
        * listbox control macros
        *-----------------------------------------------------------------*/

#define TLB_InsertFirstItem(lbx, text)    \
    g2t_SendMsg(lbx, GLBM_INSERTFIRSTITEM, (g2t_wparam)0, (g2t_lparam)text)
#define TLB_AddItem(lbx, text)    \
    g2t_SendMsg(lbx, GLBM_ADDITEM, (g2t_wparam)0, (g2t_lparam)text)
#define TLB_DeleteItem(lbx, idx)  \
    g2t_SendMsg(lbx, GLBM_DELETEITEM, (g2t_wparam)idx, (g2t_lparam)0)
#define TLB_GetCurSel(lbx)        \
    g2t_SendMsg(lbx, GLBM_GETCURSEL, (g2t_wparam)0, (g2t_lparam)0)
#define TLB_SetCurSel(lbx, idx)   \
    g2t_SendMsg(lbx, GLBM_SETCURSEL, (g2t_wparam)idx, (g2t_lparam)0)
#define TLB_DeleteAllItems(lbx)   \
    g2t_SendMsg(lbx, GLBM_DELETEALLITEMS, (g2t_wparam)0, (g2t_lparam)0)
#define TLB_GetItemCount(lbx)   \
    g2t_SendMsg(lbx, GLBM_GETITEMCOUNT, (g2t_wparam)0, (g2t_lparam)0)
#define TLB_SetItemData(lbx, idx, data)   \
    g2t_SendMsg(lbx, GLBM_SETITEMDATA, (g2t_wparam)idx, (g2t_lparam)data)
#define TLB_GetItemData(lbx, idx)   \
    (g2t_lparam)g2t_SendMsg(lbx, GLBM_GETITEMDATA, (g2t_wparam)idx, (g2t_lparam)0)
#define TLB_SetItemText(lbx, idx, text)   \
    g2t_SendMsg(lbx, GLBM_SETITEMTEXT, (g2t_wparam)idx, (g2t_lparam)text)
#define TLB_GetItemText(lbx, idx, text)   \
    g2t_SendMsg(lbx, GLBM_GETITEMTEXT, (g2t_wparam)idx, (g2t_lparam)text)
#define TLB_SetItemChecked(lbx, idx, check)   \
    g2t_SendMsg(lbx, GLBM_SETITEMCHECKED, (g2t_wparam)idx, (g2t_lparam)check)
#define TLB_GetItemChecked(lbx, idx)   \
    g2t_SendMsg(lbx, GLBM_GETITEMCHECKED, (g2t_wparam)idx, (g2t_lparam)0)
#define TLB_CountItemChecked(lbx)   \
    g2t_SendMsg(lbx, GLBM_COUNTITEMCHECKED, (g2t_wparam)0, (g2t_lparam)0)

/*-------------------------------------------------------------------
* listctrl control macros
*-----------------------------------------------------------------*/

#define TLC_AddColumn(lc, text, width, al)    \
do\
{\
    \
    THEADERITEM hdr; \
    hdr.caption = text; \
    hdr.cols = width; \
    hdr.align = al; \
    hdr.textcolor = g2t_GetSysColor(LISTCTRL_HEADER_TEXTCOLOR); \
    hdr.bgcolor = g2t_GetSysColor(LISTCTRL_HEADER_BGCOLOR); \
    hdr.editstyle = 0; \
    hdr.decwidth = 2; \
    g2t_SendMsg(lc, TLCM_ADDCOLUMN, (TWPARAM)0, (TLPARAM)&hdr); \
} while (0)

#define TLC_AddColumnEx(lc, text, width, al, tc, bc, es, dec)    \
    do\
    {\
        \
        THEADERITEM hdr; \
        hdr.caption = text; \
        hdr.cols = width; \
        hdr.align = al; \
        hdr.textcolor = tc; \
        hdr.bgcolor = bc; \
        hdr.editstyle = es; \
        hdr.decwidth = dec; \
        g2t_SendMsg(lc, TLCM_ADDCOLUMN, (TWPARAM)0, (TLPARAM)&hdr); \
    }\
        while (0)

#define TLC_DeleteColumn(lc, col)    \
    g2t_SendMsg(lc, TLCM_DELETECOLUMN, (TWPARAM)col, (TLPARAM)0)
#define TLC_DeleteAllColumns(lc)    \
    g2t_SendMsg(lc, TLCM_DELETEALLCOLUMNS, (TWPARAM)0, (TLPARAM)0)
#define TLC_InsertFirstItem(lc, text, nitems)    \
    g2t_SendMsg(lc, TLCM_INSERTFIRSTITEM, (TWPARAM)nitems, (TLPARAM)text)
#define TLC_AddItem(lc, text, nitems)    \
    g2t_SendMsg(lc, TLCM_ADDITEM, (TWPARAM)nitems, (TLPARAM)text)
#define TLC_DeleteItem(lc, idx)    \
    g2t_SendMsg(lc, TLCM_DELETEITEM, (TWPARAM)0, (TLPARAM)idx)
#define TLC_DeleteAllItems(lc)    \
    g2t_SendMsg(lc, TLCM_DELETEALLITEMS, (TWPARAM)0, (TLPARAM)0)
#define TLC_SetItem(lc, flags, item)    \
    g2t_SendMsg(lc, TLCM_SETITEM, (TWPARAM)flags, (TLPARAM)item)
#define TLC_GetItem(lc, flags, item)    \
    g2t_SendMsg(lc, TLCM_GETITEM, (TWPARAM)flags, (TLPARAM)item)
#define TLC_GetItemCount(lc)    \
    g2t_SendMsg(lc, TLCM_GETITEMCOUNT, (TWPARAM)0, (TLPARAM)0)
#define TLC_SetEditStyle(lc, col, es)    \
    g2t_SendMsg(lc, TLCM_SETEDITSTYLE, (TWPARAM)col, (TLPARAM)es)
#define TLC_GetEditStyle(lc, col)    \
    (TDWORD)g2t_SendMsg(lc, TLCM_GETEDITSTYLE, (TWPARAM)col, (TLPARAM)0)
#define TLC_GetCurRow(lc)    \
    g2t_SendMsg(lc, TLCM_GETCURROW, (TWPARAM)0, (TLPARAM)0)
#define TLC_SetCurRow(lc, idx)    \
    g2t_SendMsg(lc, TLCM_SETCURROW, (TWPARAM)0, (TLPARAM)idx)
#define TLC_SetCurPage(lc, pg)    \
    g2t_SendMsg(lc, TLCM_SETCURPAGE, (TWPARAM)pg, (TLPARAM)0)
#define TLC_GetCurPage(lc)    \
    g2t_SendMsg(lc, TLCM_GETCURPAGE, (TWPARAM)0, (TLPARAM)0)
#define TLC_GetItemsPerPage(lc)    \
    g2t_SendMsg(lc, TLCM_GETITEMSPERPAGE, (TWPARAM)0, (TLPARAM)0)
#define TLC_GetEditBox(lc)    \
    (TWND)g2t_SendMsg(lc, TLCM_GETEDITBOX, (TWPARAM)0, (TLPARAM)0)
#define TLC_SetEditableCols(lc, ncols, edtcols)    \
    g2t_SendMsg(lc, TLCM_SETEDITABLECOLS, (TWPARAM)ncols, (TLPARAM)edtcols)

/*-------------------------------------------------------------------
* tree ctrl macros
*-----------------------------------------------------------------*/

#define TTC_InsertItem(tc, parentitem, insertitem)    \
    (TTREEITEM*)g2t_SendMsg(tc, TTCM_INSERTITEM, \
    (TWPARAM)(TTREEITEM*)(parentitem), \
    (TLPARAM)(TTREEITEMDATA*)(insertitem))
#define TTC_DeleteItem(tc, treeitem)    \
    g2t_SendMsg(tc, TTCM_DELETEITEM, (TWPARAM)0, (TLPARAM)(TTREEITEM*)(treeitem))
#define TTC_SetItem(tc, item, treeitem)    \
    g2t_SendMsg(tc, TTCM_SETITEM, \
    (TWPARAM)(TTREEITEM*)(item), \
    (TLPARAM)(TTREEITEMDATA*)(treeitem))
#define TTC_GetItem(tc, item, treeitem)    \
    g2t_SendMsg(tc, TTCM_GETITEM, \
    (TWPARAM)(TTREEITEM*)(item), \
    (TLPARAM)(TTREEITEMDATA*)(treeitem))
#define TTC_FindItem(tc, finditem)    \
    (TTREEITEM*)g2t_SendMsg(tc, TTCM_FINDITEM, (TWPARAM)0, (TLPARAM)(TTREEITEMDATA*)(finditem))
#define TTC_FindNextItem(tc, previtem)    \
    (TTREEITEM*)g2t_SendMsg(tc, TTCM_FINDNEXTITEM, \
    (TWPARAM)(TTREEITEM*)(previtem), \
    (TLPARAM)(TTREEITEMDATA*)(finditem))
#define TTC_SetFindItemProc(tc, finditemproc)    \
    (LPTREEFINDITEMPROC)g2t_SendMsg(tc, TTCM_SETFINDITEMPROC, (TWPARAM)0, (TLPARAM)(LPFINDITEMPROC)(finditemproc))
#define TTC_ExpandItem(tc, treeitem)    \
    g2t_SendMsg(tc, TTCM_EXPANDITEM, (TWPARAM)0, (TLPARAM)(TTREEITEM*)(&treeitem))
#define TTC_CollapseItem(tc, treeitem)    \
    g2t_SendMsg(tc, TTCM_COLLAPSEITEM, (TWPARAM)0, (TLPARAM)(TTREEITEM*)(&treeitem))
#define TTC_SetSelItem(tc, setitem)    \
    g2t_SendMsg(tc, TTCM_SETSELITEM, \
    (TWPARAM)0, \
    (TLPARAM)(TTREEITEM*)(setitem))
#define TTC_GetSelItem(tc, getitem)    \
    (TLONG)g2t_SendMsg(tc, TTCM_GETSELITEM, \
    (TWPARAM)0, \
    (TLPARAM)0)
#define TTC_ExportToFile(tc, fp, proc)\
    (TLONG)g2t_SendMsg(tc, TTCM_EXPORTTOFILE, \
    (TWPARAM)(FILE*)(fp), \
    (TLPARAM)(LPTREEEXPORTPROC)(proc))
#define TTC_ImportFromFile(tc, fp, proc)\
    (TLONG)g2t_SendMsg(tc, TTCM_IMPORTFROMFILE, \
    (TWPARAM)(FILE*)(fp), \
    (TLPARAM)(LPTREEIMPORTPROC)proc)
#define TTC_ExpandAllItems(tc)    \
    g2t_SendMsg(tc, TTCM_EXPANDALLITEMS, (TWPARAM)0, (TLPARAM)(0))
#define TTC_CollapseAllItems(tc)    \
    g2t_SendMsg(tc, TTCM_COLLAPSEALLITEMS, (TWPARAM)0, (TLPARAM)(0))


#ifdef __cplusplus
}
#endif



#endif /* __GUI2TUI_H__ */
