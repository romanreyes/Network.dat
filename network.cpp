#include "network_lib.h"

int main(){
        FILE *f= fopen("C:network_structure.dat", "rb");
    if( f == NULL){
        printf("\n Error");
        return -1;
    }
    int count = Count_Devices(f);  //cuento la cantidad de registros.
    Registro *R= new Registro[count]; //Asigno memoria dinamica a la estructura.
    showIDs(f, R);  //aunque en la consola no muestre los ID's uso la funcion para cargar los ID's en la estructura
    load_network_struct (f, R); // cargo todos los datos de manera ordenada en la estrucura
    menu(f, R);

    //libero memoria y cierro los archivos
    for (int i = 0; i < count; ++i){
        delete[] R[i].LLD_ID;
    }
    delete []R;
    fclose(f);
}