/**************************************************************************/
/* Fisier de includere generat de LabWindows/CVI pentru resursa UIR       */
/*                                                                        */
/* ATENTIE: Continutul acestui fisier este generat automat.               */
/*          Nu il modifica manual.                                        */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  HELP_PANEL                       1
#define  HELP_PANEL_TEXTBOX               2       /* control type: textBox, callback function: (none) */
#define  HELP_PANEL_QUIT_WINDOW           3       /* control type: command, callback function: QuitHelp */

#define  PANEL                            2
#define  PANEL_START_APLICATIE            2       /* control type: command, callback function: Start_Aplicatie */
#define  PANEL_QUITBUTTON                 3       /* control type: command, callback function: Quit */
#define  PANEL_PID                        4       /* control type: command, callback function: Pid */
#define  PANEL_HELP                       5       /* control type: command, callback function: Help */
#define  PANEL_TEXTMSG                    6       /* control type: textMsg, callback function: (none) */
#define  PANEL_REFERINTA                  7       /* control type: scale, callback function: (none) */
#define  PANEL_PID_COMANDA                8       /* control type: graph, callback function: (none) */
#define  PANEL_GRAPH_PROCES               9       /* control type: graph, callback function: (none) */
#define  PANEL_TEXTBOX_2                  10      /* control type: textBox, callback function: (none) */
#define  PANEL_TEXTBOX_1                  11      /* control type: textBox, callback function: (none) */
#define  PANEL_DATARX_16                  12      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_14                  13      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_11                  14      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_15                  15      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_10                  16      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_13                  17      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_7                   18      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_4                   19      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_9                   20      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_12                  21      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_3                   22      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_8                   23      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_6                   24      /* control type: numeric, callback function: (none) */
#define  PANEL_STE                        25      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_2                   26      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_5                   27      /* control type: numeric, callback function: (none) */
#define  PANEL_DATATX_2                   28      /* control type: numeric, callback function: (none) */
#define  PANEL_DATARX_1                   29      /* control type: numeric, callback function: (none) */
#define  PANEL_DATATX_1                   30      /* control type: numeric, callback function: (none) */
#define  PANEL_LEDTX                      31      /* control type: LED, callback function: (none) */
#define  PANEL_REFRESHGRAPH               32      /* control type: command, callback function: Refresh_Graph */
#define  PANEL_LEDRX                      33      /* control type: LED, callback function: (none) */

#define  PID_PANEL                        3
#define  PID_PANEL_QUIT_WINDOW            2       /* control type: command, callback function: QuitPid */
#define  PID_PANEL_ALPHA_2                3       /* control type: numeric, callback function: (none) */
#define  PID_PANEL_TD_2                   4       /* control type: numeric, callback function: (none) */
#define  PID_PANEL_TI_2                   5       /* control type: numeric, callback function: (none) */
#define  PID_PANEL_KR_2                   6       /* control type: numeric, callback function: (none) */
#define  PID_PANEL_ALPHA                  7       /* control type: numeric, callback function: (none) */
#define  PID_PANEL_TD                     8       /* control type: numeric, callback function: (none) */
#define  PID_PANEL_TI                     9       /* control type: numeric, callback function: (none) */
#define  PID_PANEL_KR                     10      /* control type: numeric, callback function: (none) */
#define  PID_PANEL_T1_PROCES              11      /* control type: numeric, callback function: (none) */
#define  PID_PANEL_T2_PROCES              12      /* control type: numeric, callback function: (none) */


     /* Tablouri de controale. */

          /* Nu exista tablouri de controale. */


     /* Bare de meniu si elemente de meniu. */

          /* Nu exista bare de meniu. */


     /* Prototipurile functiilor callback. */

int  CVICALLBACK Help(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Pid(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Quit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QuitPid(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Refresh_Graph(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Start_Aplicatie(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif