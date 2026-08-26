# Arduino ↔ LabWindows/CVI — control PID cu două bucle

# 🇷🇴 Română

### Despre proiect

Acest proiect realizează comunicația serială dintre un **Arduino** și o aplicație **LabWindows/CVI** pentru simularea și controlul unui sistem cu **două regulatoare PID discrete** și **două procese de ordinul întâi conectate în serie**.

Proiectul nu este doar un exemplu generic de PID. Codul existent implementează concret următorul flux:

1. LabWindows/CVI citește din interfață referința `Refk`, perioada de eșantionare `Te` și parametrii celor două regulatoare PID.
2. CVI construiește un pachet text cu **12 valori** și îl transmite prin `COM3` la **9600 baud**.
3. Arduino primește pachetul, actualizează perioada `TimerOne` și recalculează coeficienții PID discreți.
4. Prima buclă calculează `yPIDk` din eroarea `Refk - yk`.
5. A doua buclă calculează `y2PIDk` din eroarea `yPIDk - yk2`.
6. Arduino transmite înapoi către CVI cele 12 valori de stare și de configurare.
7. CVI simulează două procese de ordinul întâi:
   - `yk2 = yk2_1 + C2 * (uk2 - yk2_1)`
   - `yk = yk_1 + C1 * (uk - yk_1)`
8. CVI desenează în timp referința, ieșirea procesului și cele două comenzi PID.

### Structura proiectului

```text
Control-PID-cu-Arduino-si-LabWindows-CVI
│
├── arduino/
│   └── Conversie.ino
│    
├── labwindows-cvi/
│   ├── commcallback.c
│   └─── commcallback.h
│
├── ui/
│   └─── commcallback.uir
│
├── simulink/
│   └─── cascada.slx
│
└── README.md
```

### Comunicația serială

Arduino și CVI folosesc un protocol text simplu, cu valorile separate prin spații.

#### CVI → Arduino

Pachetul conține, în această ordine:

| Index | Valoare | Rol |
|---:|---|---|
| 0 | `Referinta` | referința `Refk` |
| 1 | `Perioada_Esantionare` | `Te`, în secunde |
| 2 | `Parametru_Kr` | `Kr` PID 1 |
| 3 | `Parametru_Ti` | `Ti` PID 1 |
| 4 | `Parametru_Td` | `Td` PID 1 |
| 5 | `Parametru_alpha` | `alpha` PID 1 |
| 6 | `yk` | ieșirea procesului 1 calculată în CVI |
| 7 | `Parametru_Kr_2` | `Kr` PID 2 |
| 8 | `Parametru_Ti_2` | `Ti` PID 2 |
| 9 | `Parametru_Td_2` | `Td` PID 2 |
| 10 | `Parametru_alpha_2` | `alpha` PID 2 |
| 11 | `yk2` | ieșirea procesului 2 calculată în CVI |

#### Arduino → CVI

Arduino transmite înapoi:

```text
Refk Te Kr Ti Td alpha yPIDk Kr2 Ti2 Td2 alpha2 y2PIDk
```

CVI așteaptă un pachet de cel puțin **71 de octeți** pentru callback-ul de recepție. În callback, pachetul este citit, valorile sunt extrase cu `Scan()` și sunt afișate în controalele numerice din interfață.

### PID discret

Coeficienții celor două regulatoare sunt calculați în Arduino pe baza parametrilor `Kr`, `Ti`, `Td`, `alpha` și a perioadei `Te`. Forma implementată este o relație recursivă de tip:

```text
u[k] = -a1*u[k-1] - a2*u[k-2] + b0*e[k] + b1*e[k-1] + b2*e[k-2]
```

Pentru a doua buclă sunt utilizați coeficienții `a11`, `a21`, `b01`, `b11`, `b21`.

### Procesele simulate în CVI

CVI nu primește o măsurătoare fizică a procesului. În schimb, codul simulează două procese discrete de ordinul întâi.

Pentru primul proces:

```text
C1 = Te / (Te + T1_proces)
yk = yk_1 + C1 * (uk - yk_1)
```

Pentru al doilea proces:

```text
C2 = Te / (Te + T2_proces)
yk2 = yk2_1 + C2 * (uk2 - yk2_1)
```

Cele două procese sunt conectate în serie, astfel încât ieșirea celui de-al doilea proces este folosită ca intrare pentru primul.

### Interfața LabWindows/CVI

Fișierul `commcallback.uir` conține interfața grafică folosită de aplicație. Din aceasta sunt utilizate, printre altele:

- referința semnalului;
- perioada de eșantionare;
- parametrii `Kr`, `Ti`, `Td`, `alpha` pentru PID 1;
- parametrii `Kr`, `Ti`, `Td`, `alpha` pentru PID 2;
- constantele `T1_proces` și `T2_proces`;
- graficul procesului și al referinței;
- graficul celor două comenzi PID;
- indicatorii LED pentru recepție și transmisie;
- ferestrele de ajutor și configurare PID.

Graficul este resetat după aproximativ **100 de secunde** de simulare pentru a evita extinderea nelimitată a istoricului afișat.

### Cerințe

#### Arduino

- Arduino compatibil cu biblioteca `TimerOne`.
- Arduino IDE.
- Biblioteca **TimerOne** instalată.
- Conexiune serială la 9600 baud.

#### LabWindows/CVI

- LabWindows/CVI 2020 sau un mediu compatibil cu proiectul `.uir`.
- Driverul/interfața RS-232 utilizată de LabWindows/CVI.
- Un port serial disponibil. Proiectul este configurat inițial pentru **COM3**.

### Rulare

1. Deschide `arduino/Conversie/Conversie.ino` în Arduino IDE.
2. Instalează `TimerOne` dacă biblioteca nu este deja disponibilă.
3. Selectează placa și portul Arduino și încarcă sketch-ul.
4. Conectează Arduino la PC.
5. Verifică portul serial folosit de sistem. Dacă Arduino nu este pe `COM3`, modifică în `commcallback.c` valoarea și numele portului din `OpenComConfig()`.
6. Deschide proiectul in **NI LabWindows/CVI**.
7. Compilează și pornește aplicația CVI.
8. Folosește panoul PID pentru a modifica `Kr`, `Ti`, `Td`, `alpha`, `T1_proces` și `T2_proces`.
9. Setează referința și perioada de eșantionare din panoul principal.
10. Pornește comunicația și urmărește graficele.

---

# 🇬🇧 English

### About the project

This project implements serial communication between an **Arduino** and a **LabWindows/CVI** application for simulation and control of a system with **two discrete PID controllers** and **two first-order processes connected in series**.

This is not a generic PID example. The existing project implements the following concrete data flow:

1. LabWindows/CVI reads the reference `Refk`, sampling period `Te`, and the parameters of both PID controllers from the GUI.
2. CVI builds a text packet containing **12 values** and sends it through **COM3** at **9600 baud**.
3. Arduino receives the packet, updates the `TimerOne` sampling period, and recalculates the discrete PID coefficients.
4. The first loop computes `yPIDk` from the error `Refk - yk`.
5. The second loop computes `y2PIDk` from the error `yPIDk - yk2`.
6. Arduino sends the 12 state/configuration values back to CVI.
7. CVI simulates two first-order processes:
   - `yk2 = yk2_1 + C2 * (uk2 - yk2_1)`
   - `yk = yk_1 + C1 * (uk - yk_1)`
8. CVI plots the reference, process output, and both PID commands over time.

### Project structure

```text
Control-PID-cu-Arduino-si-LabWindows-CVI
│
├── arduino/
│   └── Conversie.ino
│    
├── labwindows-cvi/
│   ├── commcallback.c
│   └─── commcallback.h
│
├── ui/
│   └─── commcallback.uir
│
├── simulink/
│   └─── cascada.slx
│
└── README.md
```

### Serial protocol

Arduino and CVI use a simple text protocol with values separated by spaces.

#### CVI → Arduino

The packet contains the following 12 values, in this exact order:

| Index | Value | Role |
|---:|---|---|
| 0 | `Referinta` | reference `Refk` |
| 1 | `Perioada_Esantionare` | `Te`, in seconds |
| 2 | `Parametru_Kr` | PID 1 `Kr` |
| 3 | `Parametru_Ti` | PID 1 `Ti` |
| 4 | `Parametru_Td` | PID 1 `Td` |
| 5 | `Parametru_alpha` | PID 1 `alpha` |
| 6 | `yk` | process output calculated by CVI |
| 7 | `Parametru_Kr_2` | PID 2 `Kr` |
| 8 | `Parametru_Ti_2` | PID 2 `Ti` |
| 9 | `Parametru_Td_2` | PID 2 `Td` |
| 10 | `Parametru_alpha_2` | PID 2 `alpha` |
| 11 | `yk2` | second process output calculated by CVI |

#### Arduino → CVI

Arduino sends back:

```text
Refk Te Kr Ti Td alpha yPIDk Kr2 Ti2 Td2 alpha2 y2PIDk
```

CVI waits for a packet of at least **71 bytes** before invoking the receive callback. The callback reads the packet, extracts the values with `Scan()`, and displays them in the numeric controls of the GUI.

### Discrete PID

The coefficients of both controllers are calculated on Arduino from `Kr`, `Ti`, `Td`, `alpha`, and `Te`. The implemented controller has the recursive form:

```text
u[k] = -a1*u[k-1] - a2*u[k-2] + b0*e[k] + b1*e[k-1] + b2*e[k-2]
```

The second loop uses `a11`, `a21`, `b01`, `b11`, and `b21`.

### Processes simulated in CVI

CVI does not receive a physical process measurement. Instead, the code simulates two discrete first-order processes.

For the first process:

```text
C1 = Te / (Te + T1_proces)
yk = yk_1 + C1 * (uk - yk_1)
```

For the second process:

```text
C2 = Te / (Te + T2_proces)
yk2 = yk2_1 + C2 * (uk2 - yk2_1)
```

The two processes are connected in series, with the output of the second process feeding the first process.

### LabWindows/CVI interface

The `commcallback.uir` file contains the graphical interface used by the application. The project uses controls for:

- reference signal;
- sampling period;
- `Kr`, `Ti`, `Td`, `alpha` for PID 1;
- `Kr`, `Ti`, `Td`, `alpha` for PID 2;
- `T1_proces` and `T2_proces`;
- process/reference graph;
- the two PID command graphs;
- RX/TX LED indicators;
- help and PID configuration panels.

The graph is cleared after approximately **100 seconds** of simulation to avoid keeping an unlimited history.

### Requirements

#### Arduino

- An Arduino compatible with the `TimerOne` library.
- Arduino IDE.
- The **TimerOne** library installed.
- Serial connection at 9600 baud.

#### LabWindows/CVI

- LabWindows/CVI 2020 or a compatible environment able to open the `.uir` project.
- The RS-232 support used by LabWindows/CVI.
- An available serial port. The project is initially configured for **COM3**.

### Running the project

1. Open `arduino/Conversie/Conversie.ino` in Arduino IDE.
2. Install `TimerOne` if it is not already installed.
3. Select the Arduino board and port and upload the sketch.
4. Connect the Arduino to the PC.
5. Check which COM port is assigned. If the Arduino is not on `COM3`, update the port configuration in `commcallback.c`.
6. Open the project in **NI LabWindows/CVI**.
7. Build and run the CVI application.
8. Use the PID panel to change `Kr`, `Ti`, `Td`, `alpha`, `T1_proces`, and `T2_proces`.
9. Set the reference and sampling period in the main panel.
10. Start communication and observe the graphs.

---

## 👤 Autor / Author

**IonutD**
