#include <utility.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include <formatio.h>
#include "commcallback.h"

// Handle-uri pentru cele trei panouri ale interfetei CVI.
static int gHelpPanelHandle;
static int gPanelHandle;
static int gPidPanelHandle;

static char gEventChar4[2];
// Portul serial folosit pentru comunicatia cu Arduino.
int COM_PORT;

// Starea LED-urilor de receptie si transmisie din interfata.
int led_Rx = 0, led_Tx = 0;

// Valorile receptionate si transmise prin portul serial.
float dataRx_1, dataRx_2, dataRx_3, dataRx_4, dataRx_5, dataRx_6, dataRx_7, dataRx_8;
float dataRx_9, dataRx_10, dataRx_11, dataRx_12, dataRx_13, dataRx_14, dataRx_15, dataRx_16;
float dataTx_1, dataTx_2, dataTx_3, dataTx_4, dataTx_5, dataTx_6, dataTx_7, dataTx_8;
float dataTx_9, dataTx_10, dataTx_11, dataTx_12, dataTx_13, dataTx_14, dataTx_15, dataTx_16;
int lungime_pachet;

// Starea procesului si a semnalelor discrete.
double Refk_1, Refk;
double tk_1, tk;
double yk, yk_1, uk;
double yk2, yk2_1, uk2;
double Te;

// Valorile citite din interfata si trimise catre Arduino.
double Referinta, Perioada_Esantionare;
double T1_proces, C1;
double Kr, Ti, Td, alpha, yPIDk, yPIDk_1;
double Parametru_Kr, Parametru_Ti, Parametru_Td, Parametru_alpha, Parametru_yPIDk;
double T2_proces, C2;
double Kr_2, Ti_2, Td_2, alpha_2, y2PIDk, y2PIDk_1;
double Parametru_Kr_2, Parametru_Ti_2, Parametru_Td_2, Parametru_alpha_2, Parametru_y2PIDk;

// Callback-ul apelat de CVI atunci cand sosesc date de la Arduino.
void CVICALLBACK Functie_Receptie_Pe_Intrerupere(int portNo, int eventMask, void *callbackData);

int main(int argc, char *argv[])
{
    if (InitCVIRTE(0, argv, 0) == 0)
        return -1;

    if ((gPanelHandle = LoadPanel(0, "commcallback.uir", PANEL)) < 0)
        return -1;
    if ((gHelpPanelHandle = LoadPanel(0, "commcallback.uir", HELP_PANEL)) < 0)
        return -1;
    if ((gPidPanelHandle = LoadPanel(0, "commcallback.uir", PID_PANEL)) < 0)
        return -1;

    // Initializarea starii simularii.
    Refk_1 = 0;
    Refk = 0;
    tk_1 = 0;
    tk = 0;
    Te = 1.000;
    yk_1 = 0;
    yk = 0;
    yk2_1 = 0;
    yk2 = 0;

    // Constantele proceselor sunt calculate din perioadele de timp introduse in panoul PID.
    GetCtrlVal(gPidPanelHandle, PID_PANEL_T1_PROCES, &T1_proces);
    C1 = Te / (Te + T1_proces);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_T2_PROCES, &T2_proces);
    C2 = Te / (Te + T2_proces);

    DisplayPanel(gPanelHandle);

    // Configurarea portului serial: COM3, 9600 baud, 8N1.
    COM_PORT = 3;
    OpenComConfig(COM_PORT, "COM3", 9600, 0, 8, 1, 512, 512);
    SetCTSMode(COM_PORT, LWRS_HWHANDSHAKE_OFF);
    gEventChar4[0]='\n';
    FlushInQ(COM_PORT);
    FlushOutQ(COM_PORT);

    // CVI asteapta un pachet de cel putin 71 de octeti inainte de a apela callback-ul.
    InstallComCallback(COM_PORT, LWRS_RECEIVE, 71, 0, Functie_Receptie_Pe_Intrerupere, 0);

    FlushInQ(COM_PORT);
    FlushOutQ(COM_PORT);
    RunUserInterface();

    CloseCom(COM_PORT);
    DiscardPanel(gPanelHandle);
    DiscardPanel(gHelpPanelHandle);
    DiscardPanel(gPidPanelHandle);

    return 0;
}

// Trimite catre Arduino un mesaj simplu pentru verificarea comunicatiei.
int CVICALLBACK Start_Aplicatie(int panel, int control, int event, void *callbackData,
                                int eventData1, int eventData2)
{
    switch (event)
		{
		case EVENT_COMMIT:
			ComWrt (COM_PORT, "Comunicatie_pornita_SUCCES", StringLength ("Comunicatie_pornita_SUCCES"));
			SetCtrlVal(gPanelHandle, PANEL_TEXTBOX_2, "Comunicatie_pornita_SUCCES");SetCtrlVal (gPanelHandle, PANEL_TEXTBOX_2, "\n");
			SetCtrlVal(gPanelHandle, PANEL_LEDTX, 1);
		break;
		}
	return 0;
}

// Proceseaza un pachet primit de la Arduino si pregateste urmatorul pachet de comanda.
void CVICALLBACK Functie_Receptie_Pe_Intrerupere(int portNo, int eventMask, void *callbackData)
{
    char Buf_Receptie[1000], transmitBuf[1000], buffsend1[1000];
    double Val[1000];
    int lungime_octeti_Rx;

    led_Rx = !led_Rx;
    SetCtrlVal(gPanelHandle, PANEL_LEDRX, led_Rx);

    // Acorda timp bufferului serial pentru completarea pachetului text.
    Delay(0.050);
    lungime_octeti_Rx = GetInQLen(COM_PORT);
    ComRd(COM_PORT, Buf_Receptie, lungime_octeti_Rx);

    //Afisare pachet de date Receptionat ca string
    SetCtrlVal(gPanelHandle, PANEL_TEXTBOX_1, Buf_Receptie);
    SetCtrlVal(gPanelHandle, PANEL_TEXTBOX_1, "\n");

    // Pachetul contine 12 valori separate prin spatii.
    Scan(Buf_Receptie, "%s>%12f[x]", Val);

    Refk = Val[0];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_1, Refk);
    Te = Val[1];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_2, Te);
    Kr = Val[2];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_3, Kr);
    Ti = Val[3];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_4, Ti);
    Td = Val[4];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_5, Td);
    alpha = Val[5];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_6, alpha);
    yPIDk = Val[6];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_7, yPIDk);

    Kr_2 = Val[7];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_8, Kr_2);
    Ti_2 = Val[8];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_9, Ti_2);
    Td_2 = Val[9];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_10, Td_2);
    alpha_2 = Val[10];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_11, alpha_2);
    y2PIDk = Val[11];
    SetCtrlVal(gPanelHandle, PANEL_DATARX_12, y2PIDk);

    FlushInQ(COM_PORT);
    FlushOutQ(COM_PORT);

    // Modelul CVI contine doua procese de ordinul intai legate in serie.
    uk2 = y2PIDk;
    yk2 = yk2_1 + C2 * (uk2 - yk2_1);
    uk = yk2;
    yk = yk_1 + C1 * (uk - yk_1);

    // Afisarea referintei, iesirii procesului si a celor doua comenzi PID.
    PlotLine(gPanelHandle, PANEL_GRAPH_PROCES, tk_1, Refk_1, tk, Refk, VAL_GREEN);
    PlotLine(gPanelHandle, PANEL_GRAPH_PROCES, tk_1, yk_1, tk, yk, VAL_RED);
    PlotLine(gPanelHandle, PANEL_PID_COMANDA, tk_1, yPIDk_1, tk, yPIDk, VAL_BLUE);
    PlotLine(gPanelHandle, PANEL_PID_COMANDA, tk_1, y2PIDk_1, tk, y2PIDk, VAL_BLACK);

    // Actualizarea valorilor pentru urmatorul pas de esantionare.
    Refk_1 = Refk;
    tk_1 = tk;
    tk = tk + Te;
    yk_1 = yk;
    yPIDk_1 = yPIDk;
    yk2_1 = yk2;
    y2PIDk_1 = y2PIDk;

    // Pastreaza graficele la o fereastra de 100 de secunde.
    if (tk > 100) {
        DeleteGraphPlot(gPanelHandle, PANEL_GRAPH_PROCES, -1, VAL_IMMEDIATE_DRAW);
        DeleteGraphPlot(gPanelHandle, PANEL_PID_COMANDA, -1, VAL_IMMEDIATE_DRAW);
        tk_1 = 0;
        tk = Te;
    }

    // Citeste din interfata referinta, perioada si parametrii celor doua PID-uri.
    GetCtrlVal(gPanelHandle, PANEL_REFERINTA, &Referinta);
    GetCtrlVal(gPanelHandle, PANEL_STE, &Perioada_Esantionare);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_KR, &Parametru_Kr);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_TI, &Parametru_Ti);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_TD, &Parametru_Td);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_ALPHA, &Parametru_alpha);

    GetCtrlVal(gPidPanelHandle, PID_PANEL_KR_2, &Parametru_Kr_2);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_TI_2, &Parametru_Ti_2);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_TD_2, &Parametru_Td_2);
    GetCtrlVal(gPidPanelHandle, PID_PANEL_ALPHA_2, &Parametru_alpha_2);

    // Construieste pachetul cu cele 12 valori transmise catre Arduino.
    Fmt(buffsend1, "%s<%f[p3]", Referinta);
    strcat(buffsend1, " ");
    strcpy(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Perioada_Esantionare);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_Kr);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_Ti);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_Td);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_alpha);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", yk);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_Kr_2);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_Ti_2);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_Td_2);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", Parametru_alpha_2);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    Fmt(buffsend1, "%s<%f[p3]", yk2);
    strcat(buffsend1, " ");
    strcat(transmitBuf, buffsend1);

    ComWrt(COM_PORT, transmitBuf, StringLength(transmitBuf));
    SetCtrlVal(gPanelHandle, PANEL_TEXTBOX_2, transmitBuf);
    SetCtrlVal(gPanelHandle, PANEL_TEXTBOX_2, "\n");
    led_Tx = !led_Tx;
    SetCtrlVal(gPanelHandle, PANEL_LEDTX, led_Tx);
 
    return 0;
}

// Sterge graficele si reporneste axa de timp.
int CVICALLBACK Refresh_Graph(int panel, int control, int event,
                              void *callbackData, int eventData1, int eventData2)
{
    switch (event) 
        {
        case EVENT_COMMIT:
        DeleteGraphPlot(gPanelHandle, PANEL_GRAPH_PROCES, -1, VAL_IMMEDIATE_DRAW);
        DeleteGraphPlot(gPanelHandle, PANEL_PID_COMANDA, -1, VAL_IMMEDIATE_DRAW);
        tk_1 = 0;
        tk = Te;
	break; 
        }
	return 0;
}

// Inchide aplicatia CVI.
int CVICALLBACK Quit(int panel, int control, int event,
                     void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
        case EVENT_COMMIT:
	QuitUserInterface(0);
	break;	}
	return 0;
}

// Afiseaza panoul de ajutor.
int CVICALLBACK Help(int panel, int control, int event, void *callbackData,
                     int eventData1, int eventData2)
{
	switch (event) {
	case EVENT_COMMIT:
	InstallPopup (gHelpPanelHandle);
	break;	}
	return 0;
}

// Inchide panoul de ajutor.
int CVICALLBACK QuitHelp(int panel, int control, int event, void *callbackData,
                         int eventData1, int eventData2)
{
	switch (event) {
	case EVENT_COMMIT:
	RemovePopup (0);
	break;	}
	return 0;
}

// Deschide panoul pentru configurarea celor doua regulatoare PID.
int CVICALLBACK Pid(int panel, int control, int event,
                    void *callbackData, int eventData1, int eventData2)
{
	switch (event) {
	case EVENT_COMMIT:
        InstallPopup (gPidPanelHandle);
	break; }
	return 0;
}

// Citeste parametrii PID si constantele celor doua procese la inchiderea panoului PID.
int CVICALLBACK QuitPid(int panel, int control, int event, void *callbackData,
                        int eventData1, int eventData2)
{
	switch (event) 
	{
	case EVENT_COMMIT:
        GetCtrlVal(gPidPanelHandle, PID_PANEL_KR, &Parametru_Kr);
        GetCtrlVal(gPidPanelHandle, PID_PANEL_TI, &Parametru_Ti);
        GetCtrlVal(gPidPanelHandle, PID_PANEL_TD, &Parametru_Td);
        GetCtrlVal(gPidPanelHandle, PID_PANEL_ALPHA, &Parametru_alpha);

        GetCtrlVal(gPidPanelHandle, PID_PANEL_KR_2, &Parametru_Kr_2);
        GetCtrlVal(gPidPanelHandle, PID_PANEL_TI_2, &Parametru_Ti_2);
        GetCtrlVal(gPidPanelHandle, PID_PANEL_TD_2, &Parametru_Td_2);
        GetCtrlVal(gPidPanelHandle, PID_PANEL_ALPHA_2, &Parametru_alpha_2);

        GetCtrlVal(gPidPanelHandle, PID_PANEL_T1_PROCES, &T1_proces);
        C1 = Te / (Te + T1_proces);
        GetCtrlVal(gPidPanelHandle, PID_PANEL_T2_PROCES, &T2_proces);
        C2 = Te / (Te + T2_proces);

        RemovePopup(0);
        break;
    }

    return 0;
}
