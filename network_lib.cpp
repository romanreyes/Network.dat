#include "network_lib.h"
/**
 * @brief Cuenta el número total de dispositivos en el archivo.
 * 
 * Esta función lee el archivo y cuenta el número de headers en el archivo. 
 * Restablece el puntero del archivo al principio después de contar (rewid(f)).
 * 
 * @param f  Manipulador asociado al archivo network_structure.dat.
 * @return  El número de dispositivos totales en el archivo.
 */
int Count_Devices(FILE *f){
    int count=0;
    uint64_t header;
    uint32_t LLDC;
    uint64_t aux;
    while (fread(&header, sizeof(uint64_t), 1, f)){
        aux = header >> 16;
        LLDC = aux & 0xFFFF;

        fseek(f, LLDC * sizeof(uint16_t), SEEK_CUR);
        count++;
    }
    rewind(f);
    return count;
}
/**
 * @brief Carga y muestra los IDs de cada dispositivo en la estructura.
 * 
 * Esta función lee los IDs del archivo y los almacena en la estructura Registro. 
 * Omite los demas datos del dispositivo que no son necesarios para esta función.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void showIDs(FILE *f, Registro *R){
    int count = 0;
    uint16_t LLDC;
    uint16_t ID;
    while (fread(&ID, sizeof(uint16_t), 1, f)){
        R[count].ID = ID;
        fread(&LLDC, sizeof(uint16_t), 1, f);
        fseek(f, sizeof(uint32_t), SEEK_CUR);
        fseek(f, LLDC * sizeof(uint16_t), SEEK_CUR);
        count++;
    }
    //for(int i=0; i<count; i++){
    //    printf("\n ID: %u",R[i].ID);         //Reemplace el mostrar ID por la funcion que hice para mostrar toda la estructura, pero uso esta funcion para solo cargar los ID de cada equipo en la estructura
    //}
    rewind(f);
}
/**
 * @brief Carga la estructura registro con datos del archivo.
 * 
 * Esta función lee los datos del archivo y los carga en la estructura donde corresponde para su posterior lectura.
 * Asigna memoria dinámicamente para los IDs de los dispositivos de nivel inferior (LLD_ID) segundo el header.
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void load_network_struct (FILE *f, Registro *R){ 
    uint16_t ID;
    int count=0;
    uint8_t Dev_Type; 
    uint16_t LLDC;
    uint8_t info;
    uint16_t Upper_level_device_ID;
    while (fread(&ID, sizeof(uint16_t), 1,f)){
        fread(&LLDC, sizeof(uint16_t), 1, f); //Extraigo la cantidad de lower level device para asignar memoria dinamia para el punto *LLD_ID
        R[count].LLD_COUNT = LLDC;
        R[count].LLD_ID = new uint16_t[LLDC]; 

        fread(&Dev_Type, sizeof(uint8_t), 1, f); //extraigo los bits del 32 al 39 del device type

        fread(&info, sizeof(uint8_t), 1, f); //extraigo los bits del 40 al 47 que le pertenecen los bits de info, si el device type me da un 01 o 10 (1 o 2 en base dec) entonces este campo me es relevan, si me da un 00 o 11 (0 o)
        
        Dev_Type = (Dev_Type<<6)>>6; //acá quiero extraer solo los bits del 32 al 33 del header (header es de 64 bits) que le pertenecen al Device Type
        R[count].DT_value = Dev_Type;
        if(Dev_Type == 0 || Dev_Type == 3){
            R[count].DT.CPU_OR_CONCENTRATOR=Dev_Type; //solo puedo asignar 0 o 3 si es cpu o concentrator
        } else if(Dev_Type==1){ //pregunto si los bits estraidos es un 01 entonces es un sensor
            info = (info<<2)>>6;
            R[count].DT.SENSOR.TYPE = info;
        }else if(Dev_Type==2){ //si device type es igual a 2 (los bit son 10) entonces es un actuador.
            info = info>>7;
            R[count].DT.ACTUATOR.TYPE = info;
        }
        fread(&Upper_level_device_ID, sizeof(uint16_t), 1, f);
        R[count].ULD_ID = Upper_level_device_ID;
        for (int i = 0; i < LLDC; i++){
            fread(&R[count].LLD_ID[i], sizeof(uint16_t), 1, f);    
        }
        count++;
    }
    rewind(f);
}

/**
 * @brief Muestra la estructura de cada dispositivo conectado a la red.
 *  Esta función muestra el ID, ID del dispositivo conectado en la parte superior, que tipo de dispositivo, etc, cada dispositivo en la red. 
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void show_network(FILE *f, Registro *R){
    int count= Count_Devices(f);
    for(int i=0; i<count; i++){
        printf("\n ID: %u", R[i].ID);
        if(R[i].LLD_COUNT == 0){
            printf("\n Lower Level Devices Count: %u", R[i].LLD_COUNT);
        } else{
            printf("\n Lower Level Devices Count: %u", R[i].LLD_COUNT);
        }
        printf("\n Upper Level Device ID: %u", R[i].ULD_ID);
        if(R[i].DT_value==0){
            printf("\n Device Type: CPU \t(%u)", R[i].DT.CPU_OR_CONCENTRATOR);
        } else if(R[i].DT_value==3){
            printf("\n Device Type: Concentrator \t(%u)", R[i].DT.CPU_OR_CONCENTRATOR);
        }else if(R[i].DT_value==1){ //PREGUNTO SI ES SENSOR
            if(R[i].DT.SENSOR.TYPE==0){ 
                printf("\n Device Type : Flow Sensor \t(%u)", R[i].DT.SENSOR.TYPE);
            } else if ( R[i].DT.SENSOR.TYPE==1){
                printf("\n Device Type: Sensor Temp  \t(%u)", R[i].DT.SENSOR.TYPE);
            }else if (R[i].DT.SENSOR.TYPE==2){
                printf("\n Device Type: Presure Sensor  \t(%u)", R[i].DT.SENSOR.TYPE);
            }else if (R[i].DT.SENSOR.TYPE==3){
                printf("\n Device Type: Level Sensor  \t(%u)", R[i].DT.SENSOR.TYPE);
            }
        }else if (R[i].DT_value==2){
            if(R[i].DT.ACTUATOR.TYPE==0){
                printf("\n Device Type: Electro Valvula \t(%u)", R[i].DT.ACTUATOR.TYPE);
            }else if(R[i].DT.ACTUATOR.TYPE==1){
                printf("\n Device Type: Electro Motor  \t(%u)", R[i].DT.ACTUATOR.TYPE);
            }
        }
        if(R[i].LLD_COUNT!=0){
            for (int j = 0; j < R[i].LLD_COUNT; j++){
                printf("\n Lower Level Device ID: %u", R[i].LLD_ID[j]);
            }
        }else{
            printf("\n Don't any devices on the lower level.");
            //printf("\n No hay dispositivos conectados en el nivel inferior");
        }
        printf("\n\n");
    }
    rewind(f);
}
/**
 * @brief Realiza una búsqueda lineal de un ID en los registros.
 * 
 * @param R Puntero a los registros cargados en la estructura registro.
 * @param size_struct Tamaño de la estructura de registros.
 * @param ID ID a buscar.
 * @return int La posición del ID en la estructura o -1 si no se encuentra.
 */
int busqueda_lineal(Registro *R, int size_struct, uint64_t ID) { //uso una busqueda lineal pensando en que no tengo la estructura ordenada y ni ganas de ordenarla jaja
    for (int i = 0; i < size_struct; ++i) {
        if (R[i].ID == ID) {
            return i;  // ID encontrado en la posición i y la retorno
        }
    }
    return -1;  // ID no encontrado
}
/**
 * @brief Valida el ID ingresado si este está en los registros.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 * @return uint16_t El ID validado.
 */
uint16_t validate_ID(FILE *f, Registro *R) {  //este lo uso para verificar si el ID que ingrese el usuario esta dentro de la estructura 
    int size_struct = Count_Devices(f);
    uint16_t ID;
    int index;
    do {
        printf("\n Enter ID: ");
        //printf("\n Ingrese un ID");
        scanf("%d", &ID);
        index = busqueda_lineal(R, size_struct, ID);
        if (index == -1) {
            printf("\n ID not found. Please enter a valid ID\n");
            //printf("\n ID no encontrado. Por favor ingrese un ID valido.");
        }
    } while (index == -1); //va a buclear hasta que el usuario ingrese un ID correcto
    return ID;
}
/**
 * @brief Encuentra la secuencia de conexiones de un ID ingresado por el usuario hasta el nivel superior (ID 65535).
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void find_my_ID(FILE *f, Registro *R) {
    int size_struct = Count_Devices(f); // Tamaño de la estructura
    uint16_t ID = validate_ID(f, R);  // llamo al procedimiento para verificar si el ID ingresado es correcto
    int Pos = busqueda_lineal(R, size_struct, ID); //extraigo la posicion del ID el cual sé que es correcto para comenzar a operar
    int v_size = 0; //tamaño del vector que voy a usar para guardar los ID encontrados

    // Bucle para determinar el tamaño del vector de ID's
    while (R[Pos].ULD_ID != 65535) {  //bucleo hasta llegar al ID invalido
        v_size++;                     //aumento el tamaño del vector que almacena los ID's en cada vuelta
        Pos = busqueda_lineal(R, size_struct, R[Pos].ULD_ID);
        if (Pos == -1) {
            break; // salgo del bucle con un break
        }
    }
    // Asignar memoria para el vector 
    int *v = new int[v_size + 1]; // +1 para incluir el ID de nivel superior
    Pos = busqueda_lineal(R, size_struct, ID);
    for (int i = 0; i <= v_size; ++i) {
        v[i] = R[Pos].ID;
        Pos = busqueda_lineal(R, size_struct, R[Pos].ULD_ID);
    }
    printf("\n");
    // Imprimir la cadena
    for (int i = v_size; i >= 0; --i) {
        printf("ID %d", v[i]);
        if (i > 0) {
            printf(" -> ");
        }
    }
    printf("\n");

    delete[] v; // Libero la memoria del vector en el cual almacene la secuencia de ID's
}
/**
 * @brief Muestra el menú para que el usuario elija entre ver todos los registros o buscar una ID.
 * 
 * @param f Manipulador asociado al archivo network_structure.dat.
 * @param R Puntero a los registros cargados en la estructura registro.
 */
void menu(FILE *f, Registro *R) {
    int value;
    do {
        printf("\n \t Menu ");
        printf("\n Enter 1 to view each record of all devices in the network.");
        printf("\n Enter 2 to view the connection sequence of a specific ID.");
        printf("\n Enter 0 to exit.");
        printf("\n Your option: ");
        /*
        printf("\n Ingrese 1 para ver cada registro de todos los dispositivos en la red.");
        printf("\n Ingrese 2 para ver la secuencia de conexión de un ID específico.");
        printf("\n Ingrese 0 para salir.");
        printf("\n Su opcion: ");
        */
        scanf("%d", &value);

        if (value != 1 && value != 2 && value != 0) {
            printf("\n\n Please enter a valid option.\n");
            //printf("\n\n Por favor ingrese una opcion valida.\n")
        }
        if (value == 1) {
            show_network(f, R);
        } else if (value == 2) {
            find_my_ID(f, R);
    }
    } while (value != 0); 
}