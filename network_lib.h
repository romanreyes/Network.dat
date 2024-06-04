#pragma once
#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct Sensor{
    uint8_t TYPE;      //sensor.type==0 flow  o sensor de caudal, if == 1sensor de temperatura, if == 2 sensor de presion, if == 3 sensor de nivel
};

struct Actuator{
    uint8_t TYPE;     //if == 0 valve o electro valvula, if==1 motor
};

struct Device_Type{
    uint8_t CPU_OR_CONCENTRATOR;      //if type==0 cpu || type==3 concentrator
    struct Sensor SENSOR;
    struct Actuator ACTUATOR;
};

struct registro{
    uint16_t ID;        // if(ID==65535) significa que es un ID invalido  
    uint16_t *LLD_ID;   //lower level device ID pero con el 'lowe level device count' calculo el tamaño de mi vector para el lower level device ID
    struct Device_Type DT; 
    uint16_t ULD_ID;    //upper level device ID
    uint16_t LLD_COUNT; //me sirve para saber el tamaño de del vector *LLD_ID
    uint16_t DT_value;
};

typedef struct registro Registro;

int Count_Devices(FILE *f);
void showIDs(FILE *f, Registro *R);
void load_network_struct (FILE *f, Registro *R); //cargo la estructura
void show_network(FILE *f, Registro *R); //muestra todos los elementos de la estructura
int busqueda_lineal(Registro *R, int size_struct, uint64_t ID);
void find_my_ID(FILE *f, Registro *R);
void menu(FILE *f, Registro *R);
