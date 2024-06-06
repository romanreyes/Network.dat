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

/**
 * @brief Cuenta el número total de dispositivos en el archivo.
 * 
 * Esta función lee el archivo y cuenta el número de headers en el archivo. 
 * Restablece el puntero del archivo al principio después de contar (rewid(f)).
 * 
 * @param f  Manipulador asociado al archivo network_structure.dat.
 * @return  El número de dispositivos totales en el archivo.
 */
int Count_Devices(FILE *f);

/**
 * @brief Carga y muestra los IDs de cada dispositivo en la estructura.
 * 
 * Esta función lee los IDs del archivo y los almacena en la estructura Registro. 
 * Omite los demas datos del dispositivo que no son necesarios para esta función.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void showIDs(FILE *f, Registro *R);

/**
 * @brief Carga la estructura registro con datos del archivo.
 * 
 * Esta función lee los datos del archivo y los carga en la estructura donde corresponde para su posterior lectura.
 * Asigna memoria dinámicamente para los IDs de los dispositivos de nivel inferior (LLD_ID) segundo el header.
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void load_network_struct (FILE *f, Registro *R); //cargo la estructura

/**
 * @brief Muestra la estructura de cada dispositivo conectado a la red.
 *  Esta función muestra el ID, ID del dispositivo conectado en la parte superior, que tipo de dispositivo, etc, cada dispositivo en la red. 
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 **/
void show_network(FILE *f, Registro *R); //muestra todos los elementos de la estructura

/**
 * @brief Realiza una búsqueda lineal de un ID en los registros.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 * @param ID ID a buscar.
 * @return int La posición del ID en la estructura o -1 si no se encuentra.
 */
int busqueda_lineal(FILE *f, Registro *R, uint64_t ID);

/**
 * @brief Valida el ID ingresado si este está en los registros.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 * @return uint16_t El ID validado.
 */
uint16_t validate_ID(FILE *f, Registro *R);

/**
 * @brief Encuentra la secuencia de conexiones de un ID ingresado por el usuario hasta el nivel superior (ID 65535).
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void ID_Connection_Sequence(FILE *f, Registro *R);

/**
 * @brief Imprime todos los datos de recolectados por un registro.
 * 
 * Esta función toma como parametro el puntero del Registro imprime en consola todos los datos del registro.
 * 
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void print_Register(Registro *R);

/**
 * @brief Busca un registro por su ID y retorna la estructura.
 * 
 * Esta función de tipo estructura busca un registro por su ID y retorna 
 * la estructura completa en función del ID que desea buscar el usuario para su posterior impresion.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 * @param ID ID del equipo que desea saber su registro completo.
 * @return La estructura del registro al cual le corresponde la ID.
 */
Registro getRegister(FILE *f, Registro *R, uint16_t ID);

/**
 * @brief Muestra el menú para que el usuario interactue y busque lo que desea ver.
 * 
 * En este procedimiento el usuario tiene la opcion de ver todos los registros de la red,
 * ver un registro completo de un equipo segun su ID o ver la secuencia de conecciones de equipos
 * segun un ID seleccionado.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void menu(FILE *f, Registro *R);