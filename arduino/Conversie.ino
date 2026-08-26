#include <TimerOne.h>

// Starea celor doua procese si a regulatoarelor PID.
double yProcesk = 0, yProcesk2 = 0;
double uk = 0;
double yk = 0, yk_1 = 0;
double C;
double T_proces = 1;
double uk2 = 0;
double yk2 = 0, yk2_1 = 0;
double C2;
double T2_proces = 10;

// Coeficientii primei functii de transfer PID discrete.
double a1, a2, b0, b1, b2;
double p0, p1, p2, q0, q1, q2;
double epsk_2 = 0, epsk_1 = 0, epsk = 0;
double yPIDk_2 = 0, yPIDk_1 = 0, yPIDk = 0, yPIDk1 = 0, yPIDk2 = 0;

// Parametrii PID primiti de la aplicatia CVI si valorile utilizate curent.
double Kr_nou = 0.0, Ti_nou = 0.0, Td_nou = 0.0, alpha_nou = 0.0;
double Kr_vechi = 1.357, Ti_vechi = 0.826, Td_vechi = -0.074, alpha_vechi = 0.785;

// Coeficientii celui de-al doilea regulator PID discret.
double a11, a21, b01, b11, b21;
double p01, p11, p21, q01, q11, q21;
double epsk2_2 = 0, epsk2_1 = 0, epsk2 = 0;
double y2PIDk_2 = 0, y2PIDk_1 = 0, y2PIDk = 0;
double Kr2_nou = 0.0, Ti2_nou = 0.0, Td2_nou = 0.0, alpha2_nou = 0.0;
double Kr2_vechi = 0.988, Ti2_vechi = 0.203, Td2_vechi = -0.544, alpha2_vechi = 1.0;

// Bufferul si variabilele folosite pentru protocolul serial text.
char Buffer_Receptie[100];
int Lungime_Pachet_Date = 0;
float Val[20];
float data1 = 0, data2 = 0, data3 = 0, data4 = 0;
float data5 = 0, data6 = 0, data7 = 0, data8 = 0;
float data9 = 0, data10 = 0, data11 = 0, data12 = 0;
float data13 = 0, data14 = 0, data15 = 0, data16 = 0;
float Refk, Te;

// TimerOne lucreaza in microsecunde, iar Te este transmis de CVI in secunde.
long Te_arduino_vechi = 300000;
long Te_arduino_nou;

void setup() {
    Serial.begin(9600);
    Serial.flush();

    Te_arduino_nou = Te_arduino_vechi;

    // Calculul coeficientilor PID pentru prima bucla.
    p0 = Kr_vechi * ((4 / (Te * Te)) * (alpha_vechi * Td_vechi * Ti_vechi + Ti_vechi * Td_vechi) + (2 / Te) * (Ti_vechi + alpha_vechi * Td_vechi) + 1);
    p1 = Kr_vechi * (-(8 / (Te * Te)) * (alpha_vechi * Td_vechi * Ti_vechi + Td_vechi * Ti_vechi) + 2);
    p2 = Kr_vechi * ((4 / (Te * Te)) * (alpha_vechi * Td_vechi * Ti_vechi + Ti_vechi * Td_vechi) - (2 / Te) * (Ti_vechi + alpha_vechi * Td_vechi) + 1);

    q0 = (4 / (Te * Te)) * Ti_vechi * alpha_vechi * Td_vechi + (2 / Te) * Ti_vechi;
    q1 = -(8 / (Te * Te)) * Ti_vechi * alpha_vechi * Td_vechi;
    q2 = (4 / (Te * Te)) * Ti_vechi * alpha_vechi * Td_vechi - (2 / Te) * Ti_vechi;

    a1 = q1 / q0;
    a2 = q2 / q0;
    b0 = p0 / q0;
    b1 = p1 / q0;
    b2 = p2 / q0;

    // Calculul coeficientilor PID pentru a doua bucla.
    p01 = Kr2_vechi * ((4 / (Te * Te)) * (alpha2_vechi * Td2_vechi * Ti2_vechi + Ti2_vechi * Td2_vechi) + (2 / Te) * (Ti2_vechi + alpha2_vechi * Td2_vechi) + 1);
    p11 = Kr2_vechi * (-(8 / (Te * Te)) * (alpha2_vechi * Td2_vechi * Ti2_vechi + Td2_vechi * Ti2_vechi) + 2);
    p21 = Kr2_vechi * ((4 / (Te * Te)) * (alpha2_vechi * Td2_vechi * Ti2_vechi + Ti2_vechi * Td2_vechi) - (2 / Te) * (Ti2_vechi + alpha2_vechi * Td2_vechi) + 1);

    q01 = (4 / (Te * Te)) * Ti2_vechi * alpha2_vechi * Td2_vechi + (2 / Te) * Ti2_vechi;
    q11 = -(8 / (Te * Te)) * Ti2_vechi * alpha2_vechi * Td2_vechi;
    q21 = (4 / (Te * Te)) * Ti2_vechi * alpha2_vechi * Td2_vechi - (2 / Te) * Ti2_vechi;

    a11 = q11 / q01;
    a21 = q21 / q01;
    b01 = p01 / q01;
    b11 = p11 / q01;
    b21 = p21 / q01;

    // Pornirea temporizatorului care executa algoritmul la fiecare Te.
    Timer1.initialize(Te_arduino_nou);
    Timer1.attachInterrupt(esantionare);
}

// Arduino nu are nevoie de logica in bucla principala; procesarea este temporizata.
void loop() {
}

// Executa un ciclu complet: receptie, actualizare PID, calculul proceselor si transmisie.
void esantionare() {
    // Primeste de la CVI: Refk, Te, parametrii PID1, yk, parametrii PID2 si yk2.
    if (Serial.available() > 0) {
        delay(20);
        Lungime_Pachet_Date = Serial.available();
        Serial.readBytes(Buffer_Receptie, Lungime_Pachet_Date);
        Serial.flush();

        int p1 = 0;
        int p2 = 0;
        int j = 0;

        // Separarea valorilor din pachetul text pe baza caracterului spatiu.
        for (int i = 0; i < Lungime_Pachet_Date; i++) {
            if ((Buffer_Receptie[i] == ' ') || (Buffer_Receptie[i] == '\0')) {
                p2 = i;
                String Buff_string_Rx = String(Buffer_Receptie);
                String Data_string = Buff_string_Rx.substring(p1, p2);
                Val[j] = Data_string.toFloat();

                if (Val[j] >= 1000) {
                    Val[j] = 999.999;
                }

                p1 = i + 1;
                j = j + 1;
            }
        }

        // Asocierea celor 12 valori primite cu variabilele aplicatiei.
        Refk = Val[0];
        Te = Val[1];
        Te_arduino_nou = (long)(1000000 * Te);

        Kr_nou = Val[2];
        Ti_nou = Val[3];
        Td_nou = Val[4];
        alpha_nou = Val[5];
        yk = Val[6];

        Kr2_nou = Val[7];
        Ti2_nou = Val[8];
        Td2_nou = Val[9];
        alpha2_nou = Val[10];
        yk2 = Val[11];
    } else {
        // Daca nu exista un pachet nou, se sterg starile de calcul pentru urmatorul ciclu.
        data1 = 0; data2 = 0; data3 = 0; data4 = 0; data5 = ; data6 = 0;
        data7 = 0; data8 = 0; data9 = 0; data10 = 0; data11 = 0;

        yPIDk = 0; yPIDk_1 = 0; yPIDk_2 = 0;
        epsk = 0; epsk_1 = 0; epsk_2 = 0;
        y2PIDk = 0; y2PIDk_1 = 0; y2PIDk_2 = 0;
        epsk2 = 0; epsk2_1 = 0; epsk2_2 = 0;
    }

    // Recalculeaza perioada de esantionare si coeficientii PID daca CVI a schimbat parametrii.
    if ((Te_arduino_vechi != Te_arduino_nou) || (Kr_vechi != Kr_nou) || (Ti_vechi != Ti_nou) || (Td_vechi != Td_nou) || (alpha_vechi != alpha_nou) || (Kr2_vechi != Kr2_nou) || (Ti2_vechi != Ti2_nou) || (Td2_vechi != Td2_nou) || (alpha2_vechi != alpha2_nou)) {

        Timer1.initialize(Te_arduino_nou);
        Te_arduino_vechi = Te_arduino_nou;

        Kr_vechi = Kr_nou;
        Ti_vechi = Ti_nou;
        Td_vechi = Td_nou;
        alpha_vechi = alpha_nou;

        p0 = Kr_vechi * ((4 / (Te * Te)) * (alpha_vechi * Td_vechi * Ti_vechi + Ti_vechi * Td_vechi) + (2 / Te) * (Ti_vechi + alpha_vechi * Td_vechi) + 1);
        p1 = Kr_vechi * (-(8 / (Te * Te)) * (alpha_vechi * Td_vechi * Ti_vechi + Td_vechi * Ti_vechi) + 2);
        p2 = Kr_vechi * ((4 / (Te * Te)) * (alpha_vechi * Td_vechi * Ti_vechi + Ti_vechi * Td_vechi) - (2 / Te) * (Ti_vechi + alpha_vechi * Td_vechi) + 1);

        q0 = (4 / (Te * Te)) * Ti_vechi * alpha_vechi * Td_vechi + (2 / Te) * Ti_vechi;
        q1 = -(8 / (Te * Te)) * Ti_vechi * alpha_vechi * Td_vechi;
        q2 = (4 / (Te * Te)) * Ti_vechi * alpha_vechi * Td_vechi - (2 / Te) * Ti_vechi;

        a1 = q1 / q0;
        a2 = q2 / q0;
        b0 = p0 / q0;
        b1 = p1 / q0;
        b2 = p2 / q0;

        Kr2_vechi = Kr2_nou;
        Ti2_vechi = Ti2_nou;
        Td2_vechi = Td2_nou;
        alpha2_vechi = alpha2_nou;

        p01 = Kr2_vechi * ((4 / (Te * Te)) * (alpha2_vechi * Td2_vechi * Ti2_vechi + Ti2_vechi * Td2_vechi) + (2 / Te) * (Ti2_vechi + alpha2_vechi * Td2_vechi) + 1);
        p11 = Kr2_vechi * (-(8 / (Te * Te)) * (alpha2_vechi * Td2_vechi * Ti2_vechi + Td2_vechi * Ti2_vechi) + 2);
        p21 = Kr2_vechi * ((4 / (Te * Te)) * (alpha2_vechi * Td2_vechi * Ti2_vechi + Ti2_vechi * Td2_vechi) - (2 / Te) * (Ti2_vechi + alpha2_vechi * Td2_vechi) + 1);

        q01 = (4 / (Te * Te)) * Ti2_vechi * alpha2_vechi * Td2_vechi + (2 / Te) * Ti2_vechi;
        q11 = -(8 / (Te * Te)) * Ti2_vechi * alpha2_vechi * Td2_vechi;
        q21 = (4 / (Te * Te)) * Ti2_vechi * alpha2_vechi * Td2_vechi - (2 / Te) * Ti2_vechi;

        a11 = q11 / q01;
        a21 = q21 / q01;
        b01 = p01 / q01;
        b11 = p11 / q01;
        b21 = p21 / q01;
    }

    // Prima bucla PID: eroarea este diferenta dintre referinta si iesirea procesului.
    epsk = Refk - yk;
    yPIDk = -a1 * yPIDk_1 - a2 * yPIDk_2 + b0 * epsk + b1 * epsk_1 + b2 * epsk_2;

    // A doua bucla PID foloseste iesirea primei bucle ca referinta.
    epsk2 = yPIDk - yk2;
    y2PIDk = -a11 * y2PIDk_1 - a21 * y2PIDk_2 + b01 * epsk2 + b11 * epsk2_1 + b21 * epsk2_2;

    // Memoreaza valorile precedente necesare relatiei recursive.
    epsk_2 = epsk_1;
    epsk_1 = epsk;
    yPIDk_2 = yPIDk_1;
    yPIDk_1 = yPIDk;

    epsk2_2 = epsk2_1;
    epsk2_1 = epsk2;
    y2PIDk_2 = y2PIDk_1;
    y2PIDk_1 = y2PIDk;

    // Trimite inapoi catre CVI cele 12 valori, separate prin spatii.
    String Buffer_Transmisie = String(Refk, 3) + ' ' + String(Te, 3) + ' ' + String(Kr_vechi, 3) + ' ' +
        String(Ti_vechi, 3) + ' ' + String(Td_vechi, 3) + ' ' + String(alpha_vechi, 3) + ' ' +
        String(yPIDk, 3) + ' ' + String(Kr2_vechi, 3) + ' ' + String(Ti2_vechi, 3) + ' ' +
        String(Td2_vechi, 3) + ' ' + String(alpha2_vechi, 3) + ' ' + String(y2PIDk, 3);

    for (int i = 0; i < Buffer_Transmisie.length(); i++) {
        Serial.write(Buffer_Transmisie[i]);
    }
}
