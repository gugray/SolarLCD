EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_Microchip_ATtiny:ATtiny85-20PU U1
U 1 1 62A64AD0
P 3900 2850
F 0 "U1" H 3703 3425 50  0000 R CNN
F 1 "ATtiny85" H 4331 3425 50  0000 R CNN
F 2 "Package_DIP:DIP-8_W7.62mm" H 3900 2850 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf" H 3900 2850 50  0001 C CNN
	1    3900 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:Solar_Cell SC1
U 1 1 62A667FF
P 1500 2800
F 0 "SC1" H 1608 2896 50  0000 L CNN
F 1 "Solar_Cell" H 1608 2805 50  0001 L CNN
F 2 "Connector_Wire:SolderWire-0.1sqmm_1x02_P3.6mm_D0.4mm_OD1mm" V 1500 2860 50  0001 C CNN
F 3 "~" V 1500 2860 50  0001 C CNN
F 4 "5V" H 1608 2805 50  0000 L CNN "Voltage"
	1    1500 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:D D1
U 1 1 62A696C5
P 1800 1750
F 0 "D1" H 1800 1533 50  0000 C CNN
F 1 "1N60" H 1800 1624 50  0000 C CNN
F 2 "Diode_THT:D_5W_P12.70mm_Horizontal" H 1800 1750 50  0001 C CNN
F 3 "" H 1800 1624 50  0001 C CNN
	1    1800 1750
	-1   0    0    1   
$EndComp
Wire Wire Line
	1500 2600 1500 1750
Wire Wire Line
	3900 3450 3900 3850
Wire Wire Line
	1500 1750 1650 1750
$Comp
L Device:D_Zener D2
U 1 1 62A6BF92
P 2050 2750
F 0 "D2" V 2004 2830 50  0000 L CNN
F 1 "Zener" V 2095 2830 50  0000 L CNN
F 2 "" H 2050 2750 50  0001 C CNN
F 3 "~" H 2050 2750 50  0001 C CNN
F 4 "5V" V 2188 2830 50  0000 L CNN "Info"
	1    2050 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	1950 1750 2050 1750
Wire Wire Line
	2050 1750 2050 2600
Wire Wire Line
	2050 2900 2050 3850
Wire Wire Line
	2050 3850 1500 3850
$Comp
L Device:CP C1
U 1 1 62A6F382
P 2800 2750
F 0 "C1" H 2918 2796 50  0000 L CNN
F 1 "1.5F" H 2918 2705 50  0000 L CNN
F 2 "" H 2838 2600 50  0001 C CNN
F 3 "~" H 2800 2750 50  0001 C CNN
	1    2800 2750
	1    0    0    -1  
$EndComp
Connection ~ 2050 1750
Wire Wire Line
	3900 1750 3900 2250
Wire Wire Line
	4700 2950 4700 2550
Wire Wire Line
	4800 2650 4800 2850
Wire Wire Line
	5100 2850 5100 2750
Wire Wire Line
	4900 2750 4900 2650
Wire Wire Line
	5500 1750 5500 2250
Connection ~ 3900 1750
$Comp
L Display_Character:HT1621 LCD1
U 1 1 62A78604
P 5500 2750
F 0 "LCD1" H 5554 3228 50  0000 L TNN
F 1 "HT1621" H 5828 2805 50  0001 L CNN
F 2 "" H 5700 3200 50  0001 C CNN
F 3 "" H 5700 3200 50  0001 C CNN
	1    5500 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4900 2650 5100 2650
Wire Wire Line
	4800 2850 5100 2850
Wire Wire Line
	3900 1750 5500 1750
Wire Wire Line
	3900 3850 5500 3850
Wire Wire Line
	4700 2550 5100 2550
Wire Wire Line
	4500 2650 4800 2650
Wire Wire Line
	4500 2750 4900 2750
Wire Wire Line
	4500 2950 4700 2950
Connection ~ 3900 3850
Wire Wire Line
	5500 3850 5500 3050
Connection ~ 2050 3850
Wire Wire Line
	1500 3850 1500 2900
Wire Wire Line
	2050 1750 2800 1750
Wire Wire Line
	2050 3850 2800 3850
Connection ~ 2800 1750
Wire Wire Line
	2800 1750 3900 1750
Wire Wire Line
	2800 2900 2800 3850
Connection ~ 2800 3850
Wire Wire Line
	2800 3850 3900 3850
Wire Wire Line
	2800 1750 2800 2600
$EndSCHEMATC
