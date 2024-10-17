#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "osrms_API.h"


//Parametros basados en enunciado
#define PCB_SIZE 32 // Esta separada en 32 entradas,
#define PCB_START 0 // Los primeros 8 KB de la memoria corresponde a espacio reservado exclusivamente para la Tabla de PCBs.
#define PCB_ENTRY_SIZE 256 //Tamano de entrada: 256 Bytes.

FILE* global_memory_file = NULL;

void os_mount(char* memory_path) {
    printf("Funcion os_mount\n");
    global_memory_file = fopen(memory_path, "rb+");
    // https://stackoverflow.com/questions/75620363/difference-between-rb-and-rb-or-wb-and-wb
    if (global_memory_file == NULL) {
        printf("Error al abrir el archivo de memoria '%s'\n\n", memory_path);
        exit(EXIT_FAILURE);
    }

    printf("Archivo de memoria '%s' montado correctamente\n\n", memory_path);
}

void os_ls_processes() {
    printf("Funcion os_ls_processes\n");
    int count = 0;

    fseek(global_memory_file, PCB_START, SEEK_SET);
    // https://www.fresh2refresh.com/c-programming/c-file-handling/fseek-seek_set-seek_cur-seek_end-functions-c/

    for (int i = 0; i < PCB_SIZE; i++) {

        // creamos una variable temporal para leer la entrada del PCB
       unsigned char pcb[PCB_ENTRY_SIZE];

        // leemos la entrada del PCB
       fread(pcb, 1, PCB_ENTRY_SIZE, global_memory_file);

       unsigned char status = pcb[0];
        // Si el proceso corre, su estado es 0x01
        if (status == 0x01) {
            count++;
            // extaremos el PID del proceso
            //https://www.geeksforgeeks.org/unsigned-char-in-c-with-examples/
            // si no le coloco el unsigned, el valor de pid se imprime como un numero negativo y equivocado
            unsigned char pid = pcb[1];

            // Luego su nombre se encuentra en los siguientes 11 bytes
            char process_name[11];
            strncpy(process_name, (char*)&pcb[2], 12);
            //https://stackoverflow.com/questions/8202897/null-terminated-string-in-c
            //no se menciona que se deba agregar el caracter nulo al final de la cadena
            // process_name[PCB_SIZE] = '\0';

            // Imprimimos la informacion del proceso
            printf("Proceso encontrado numero %d\n", count);
            printf("PID: %d, Nombre: %s\n", pid, process_name);
        }
    }
    printf("\n");
}

int os_exists(int process_id, char* file_name){
    printf("Funcion os_exists\n");
    // empezamos accediendo a la tabla de PCBs
    fseek(global_memory_file, PCB_START, SEEK_SET);

    // recorremos la tabla de PCBs
    for (int i=0; i<PCB_SIZE; i++){
        // al igual que en la funcion anterior, creamos una variable temporal
        // para leer la entrada del PCB
        unsigned char pcb_entrada[PCB_ENTRY_SIZE];

        // leemos la entrada del PCB
        fread(pcb_entrada, 1, PCB_ENTRY_SIZE, global_memory_file);

        // ahora comenzamos a extraer la informacion de la entrada del PCB
        // nos interesa primero que el PID sea igual al que estamos buscando

        unsigned char pid = pcb_entrada[1]; // el PID
        // se encuentra en el segundo byte de la entrada del PCB

        // si el PID es igual al que estamos buscando
        if (process_id == pid){
            // ahora nos interesa encontrar el nombre del archivo

            // variable temporal para almacenar la tabla de archivos
            // del proceso que estampos revisando
            unsigned char* file_table = &pcb_entrada[13]; // la tabla de archivos
            // comienza en el byte 13 de la entrada del PCB

            // recorremos la tabla de archivos
            for (int j=0; j<5; j++){ //la tabla de archivos contiene 5 entradas
                // cada entrada tiene 23 bytes
                unsigned char* file_table_entry = &file_table[j*23]; // especie de offset para acceder a la entrada que vamos recorriendo

                char tmp_file_name[14]; // el nombre del archivo tiene 14 bytes
                strncpy(tmp_file_name, (char*)&file_table_entry[1], 14); // copiamos el nombre del archivo a una variable temporal

                //comparamos si el nombre del archivo es igual al que estamos buscando
                if (strcmp(tmp_file_name, file_name) == 0){ //https://www.ibm.com/docs/es/i/7.5?topic=functions-strcmp-compare-strings
                    printf("Archivo encontrado!\nNombre: %s, PID %d\n\n", file_name, process_id);
                    return 1; // si encontramos el archivo, retornamos 1
                }
            }
        // si el pid coincide pero no logramos encontrar el archivo, retorna 0
        printf("Archivo no encontrado\nNombre: %s, PID %d\n\n", file_name, process_id);
        return 0;
        } 
    }
    // si no encontramos el proceso, imprime un mensaje
    printf("Proceso con PID %d no encontrado\n\n", process_id);
    // deje este return 1, aunque por enunciado no especifica este caso
    return 1;
}

void os_ls_files(int process_id){
// esta funcion sigue la misma nocion de la funcion anterior
// debemos recorrer la tabla PCBs para encontrar el proceso
// luego, acceder a la tabla de archivos
// y aqui recorremos los archivos para obtener su nombre y tamaño
    printf("Funcion os_ls_files\n");

    // accedo a la tabla de PCBs
    fseek(global_memory_file, PCB_START, SEEK_SET);

    // al igual que antes, se recorre la tabla de PCBs
    for (int i=0; i<PCB_SIZE; i++){
        
        unsigned char pcb_entrada[PCB_ENTRY_SIZE];
        fread(pcb_entrada, 1, PCB_ENTRY_SIZE, global_memory_file);

        // guardamos el PID para compararlo
        unsigned char pid = pcb_entrada[1];
        // si el PID coincide con el que estamos buscando
        if(process_id==pid){
            printf("Archivos del proceso %d:\n", process_id);
            unsigned char* file_table = &pcb_entrada[13];
            // recorremos su tabla de archivos

            for (int j=0; j<5; j++){
                unsigned char* file_table_entry = &file_table[j*23];

                if (file_table_entry[0] == 0x01){// si el archivo es valido
                    char tmp_file_name[14];
                    strncpy(tmp_file_name, (char*)&file_table_entry[1], 14);
                    // obtenemos el tamaño del archivo
                    unsigned int file_size = 0;
                    
                    // https://stackoverflow.com/questions/2898364/strcpy-vs-memcpy
                    memcpy(&file_size, &file_table_entry[15], 4); //el tamaño del archivo se representa en 4 bytes
                    // imprimimos la informacion del archivo
                    printf("Nombre: %s, Tamaño: %d Bytes\n", tmp_file_name, file_size);
                }
                }
            // no coloco numero porque la funcion no retorna nada
            return;
            }
        // return;
        }
    // si no encontramos el proceso, imprime un mensaje
    // printf("Proceso con PID %d no encontrado\n\n", process_id);
    // return;
    }

