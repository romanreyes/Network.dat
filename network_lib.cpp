#include "network_lib.h"

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

void show_network(FILE *f, Registro *R){
    int count= Count_Devices(f);
    for(int i=0; i<count; i++){
        printf("\n ID: %u", R[i].ID);
        printf("\n Lower Level Devices Count: %u", R[i].LLD_COUNT);
        printf("\n Upper Level Device ID: %u", R[i].ULD_ID);
         // Determinar el tipo de dispositivo y mostrar información correspondiente
        if(R[i].DT_value==0){
            printf("\n Device Type: \tCPU ", R[i].DT.CPU_OR_CONCENTRATOR);
        } else if(R[i].DT_value==3){
            printf("\n Device Type: \tConcentrator ", R[i].DT.CPU_OR_CONCENTRATOR);
        }else if(R[i].DT_value==1){ //PREGUNTO SI ES SENSOR
            if(R[i].DT.SENSOR.TYPE==0){ 
                printf("\n Device Type : \tFlow Sensor ", R[i].DT.SENSOR.TYPE);
            } else if ( R[i].DT.SENSOR.TYPE==1){
                printf("\n Device Type: \tSensor Temp  ", R[i].DT.SENSOR.TYPE);
            }else if (R[i].DT.SENSOR.TYPE==2){
                printf("\n Device Type: \tSensor Presure  ", R[i].DT.SENSOR.TYPE);
            }else if (R[i].DT.SENSOR.TYPE==3){
                printf("\n Device Type: \tSensor Level  ", R[i].DT.SENSOR.TYPE);
            }
        }else if (R[i].DT_value==2){
            if(R[i].DT.ACTUATOR.TYPE==0){
                printf("\n Device Type: \tElectro Valvula ", R[i].DT.ACTUATOR.TYPE);
            }else if(R[i].DT.ACTUATOR.TYPE==1){
                printf("\n Device Type: \tElectro Motor  ", R[i].DT.ACTUATOR.TYPE);
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

int busqueda_lineal(FILE *f, Registro *R, uint16_t ID) { //uso una busqueda lineal pensando en que no tengo la estructura ordenada y ni ganas de ordenarla jaja
    int size_struct = Count_Devices(f);
    for (int i = 0; i < size_struct; ++i) {
        if (R[i].ID == ID) {
            return i;  // termina la funcion cuando encuentre el ID y retorna la posicion ( la variable 'i')
        }
    }
    return -1;  // si el ID no se encuentra, retorna un -1
}

uint16_t validate_ID(FILE *f, Registro *R) {  //este lo uso para que el usuario ingrese un ID y verificar si el ID que ingreso el usuario esta dentro de la estructura 
    int size_struct = Count_Devices(f); //llamo al tamaño de la estructura
    uint16_t ID;
    int index;
    do {
        printf("\n Enter ID: ");
        //printf("\n Ingrese un ID");
        scanf("%d", &ID);
        index = busqueda_lineal(f, R, ID);
        if (index == -1) { //si busqueda lineal retorna un -1 entra en el if()
            printf("\n ID not found. Please enter a valid ID\n");
            //printf("\n ID no encontrado. Por favor ingrese un ID valido.");
        }
    } while (index == -1); //va a buclear hasta que el usuario ingrese un ID correcto
    return ID; //validate ID retorna el ID que sí se encuentra en la estructura.
}

void ID_Connection_Sequence(FILE *f, Registro *R) { 
    int size_struct = Count_Devices(f); // Tamaño de la estructura
    uint16_t ID = validate_ID(f, R);  // llamo al procedimiento para verificar si el ID ingresado es correcto
    int Pos = busqueda_lineal(f, R, ID); //extraigo la posicion del ID el cual sé que es correcto para comenzar a operar
    int v_size = 0; //tamaño del vector que voy a usar para guardar los ID encontrados

    // Bucle para determinar el tamaño del vector de ID's
    while (R[Pos].ULD_ID != 65535) {  //bucleo hasta llegar al ID invalido
        v_size++;                     //aumento el tamaño del vector que almacena los ID's en cada vuelta
        Pos = busqueda_lineal(f, R, R[Pos].ULD_ID); //En vez de mandar como tercer parametro directamente el ID mando el Upper level Device ID (el ID del dispositivo que esta arriba) para saber su posicion.
        if (Pos == -1) {
            break; // salgo del bucle con un break
        }
    }
    // Asignar memoria para el vector 
    int *v = new int[v_size + 1]; // +1 para incluir el ID de nivel superior
    Pos = busqueda_lineal(f, R, ID);
    for (int i = 0; i <= v_size; ++i) {
        v[i] = R[Pos].ID;
        Pos = busqueda_lineal(f, R, R[Pos].ULD_ID);
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

void print_Register(Registro *R) {
    printf("\n ID: %u", R->ID);  // Segun chat: En C++, cuando tenes un puntero a una estructura, se usa -> para acceder a las partes de una estructura. El operador '.' se usa para acceder a los miembros de una estructura cuando tienes una instancia directa de la estructura (la posicion)
    printf("\n Lower Level Devices Count: %u", R->LLD_COUNT); //si debaja los R.LLD_COUNT con un punto en vez de R->LLD_COUNT me procude errores.
    printf("\n Upper Level Device ID: %u", R->ULD_ID);

    if (R->DT_value == 0) {
        printf("\n Device Type: \tCPU ");
    } else if (R->DT_value == 3) {
        printf("\n Device Type: \tConcentrator ", R->DT.CPU_OR_CONCENTRATOR);
    } else if (R->DT_value == 1) { // PREGUNTO SI ES SENSOR
        if (R->DT.SENSOR.TYPE == 0) {
            printf("\n Device Type : \tFlow Sensor ");
        } else if (R->DT.SENSOR.TYPE == 1) {
            printf("\n Device Type: \tSensor Temp  ");
        } else if (R->DT.SENSOR.TYPE == 2) {
            printf("\n Device Type: \tPressure Sensor  ");
        } else if (R->DT.SENSOR.TYPE == 3) {
            printf("\n Device Type: \tLevel Sensor  ");
        }
    } else if (R->DT_value == 2) {
        if (R->DT.ACTUATOR.TYPE == 0) {
            printf("\n Device Type: \tElectro Valve ");
        } else if (R->DT.ACTUATOR.TYPE == 1) {
            printf("\n Device Type: \tElectro Motor  ");
        }
    }
    if (R->LLD_COUNT != 0) {
        for (int j = 0; j < R->LLD_COUNT; j++) {
            printf("\n Lower Level Device ID: %u", R->LLD_ID[j]);
        }
    } else {
        printf("\n Don't have any devices on the lower level.");
    }
    printf("\n\n");
}

Registro getRegister(FILE *f, Registro *R, uint16_t ID) {
    int index = busqueda_lineal(f, R, ID); // Buscar el ID en la estructura
    return R[index]; // Retornar la estructura completa del registro en la posicion del ID seleccionado por el usuario.
}

void menu(FILE *f, Registro *R) {
    int value;
    do {
        printf("\n \t Menu ");
        printf("\n Enter 1 to view each record of all devices in the network.");
        printf("\n Enter 2 to get a full registry from one ID.");
        printf("\n Enter 3 to view the connection sequence of a specific ID.");
        printf("\n Enter 0 to exit.");
        printf("\n Your option: ");
        /*
        printf("\n Ingrese 1 para ver cada registro de todos los dispositivos en la red.");
        printf("\n Ingrese 2 para obtener un registro completo de una ID.");
        printf("\n Ingrese 3 para ver la secuencia de conexión de un ID específico.");
        printf("\n Ingrese 0 para salir.");
        printf("\n Su opcion: ");
        */
        scanf("%d", &value);

        if (value < 0 || value > 3) {
            printf("\n\n Please enter a valid option.\n");
            //printf("\n\n Por favor ingrese una opcion valida.\n")
        }
        if (value == 1) {
            show_network(f, R);
        }else if (value == 2){
            int size_struct = Count_Devices(f);  //extraigo el tamaño de la estructura
            uint16_t ID = validate_ID(f, R);     // valido el ID que ingrese el usario
            Registro reg_aux = getRegister(f, R, ID);//Hago una estructura auxiliar para poder almacenar la estructura retornada por la función de tipo estructura getRegister
            print_Register(&reg_aux); //muestro el registro del ID solicitado enviandon como parametro solo la estructura retornada de getRegister.
        }else if (value == 3) {
            ID_Connection_Sequence(f, R);
        }
    } while (value != 0); 
}