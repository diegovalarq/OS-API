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

void os_frame_bitmap(){
    printf("\nFuncion os_frame_bitmap");

    // declaramos en un inicio los contadores
    /// para los frames libres y los ocupados
    int frames_ocupados_count = 0;
    int frames_libres_count = 0;

    // Ahora en vez de acceder a la tabla de PCBs, accedemos al frame bitmap
    // El enunciado nos dice que comienza despues de las paginas de segundo orden
    // esto seria desde el byte 8KB + 128B + 128KB
    int frame_bitmap_start = PCB_START + PCB_SIZE*PCB_ENTRY_SIZE + 128 + 128*1024; 

    // nos movemos al inicio del frame bitmap
    fseek(global_memory_file, frame_bitmap_start, SEEK_SET);

    // el frame tiene un tamano de 8KB = 8192 bytes
    // cada bit representa un frame
    unsigned char frame_bitmap[8192]; // variable temporal para almacenar el frame bitmap

    // ahora se lee el bitmap
    fread(frame_bitmap, 1, 8192, global_memory_file);

    // recorremos el bitmap
    // necesitamos recorrer cada bit
    // por lo que debemos tener en cuenta el little endian
    printf("\nFrame bitmap:\n");
    for (int i=0; i<8192; i++){
        for (int j=0; j<8; j++){
            // obtenemos el bit actual
            unsigned char bit = (frame_bitmap[i] >> j) &1; // sin el ampersand
            // la funcion me arrojaba valores incorrectos para los counts.
            // y tambien valores raros para los bits
            // Aunque por lo que entiendo de como estoy armando codigo, no seria necesario usar el AND.

            // Ahora si entendi por que
            // si no uso el AND, el valor de la variable sera el valor del byte entero despues de aplicar el shift.


            printf("%d", bit);
            // si el bit es 1, el frame esta ocupado
            if (bit == 1){
                frames_ocupados_count++;
            // si no es 1, por el AND va a ser 0, por lo que el frame esta libre
            } else {
                frames_libres_count++;
            }
        }
        printf("|"); // solo lo agrego para separar los bytes
    }
    printf("\nFrames ocupados: %d\n", frames_ocupados_count);
    printf("Frames libres: %d\n", frames_libres_count);
}

void os_tp_bitmap(){
    printf("\nFuncion os_tp_bitmap\n");
    // en vez de acceder al frame bitmap, accedemos al page table bitmap
    // sigue la nocion de la funcion anterior

    // el table page bitmap se compone de 128 bytes, o 1024 bits
    int libre_second_tp = 0;
    int ocupado_second_tp = 0;

    int page_table_bitmap_start = PCB_START + PCB_SIZE*PCB_ENTRY_SIZE;

    fseek(global_memory_file, page_table_bitmap_start, SEEK_SET);

    unsigned char page_table_bitmap[128];
    fread(page_table_bitmap, 1, 128, global_memory_file);

    // es basicamente la misma idea que el bitmap de frames
    // ahora tambien entendi que sacamos el bit por la derecha, o de derecha a izquierda
    // por el little endian

    printf("\nBitman de Tablas de Paginas:\n");
    for (int i=0; i<128; i++){
        for (int j=0; j<8; j++){
            unsigned char bit = (page_table_bitmap[i] >> j) &1;
            printf("%d", bit);
            if (bit == 1){
                ocupado_second_tp++;
            } else {
                libre_second_tp++;
            }
        }
        printf("|");
    }
    printf("\nPage tables de segundo orden ocupadas: %d\n", ocupado_second_tp);
    printf("Page tables de segundo orden libres: %d\n", libre_second_tp);

}

void os_start_process(int process_id, char* process_name){
    printf("Funcion os_start_process\n");
    // primero, accedemos a la tabla de PCBs
    fseek(global_memory_file, PCB_START, SEEK_SET);

    // recorremos la tabla de PCBs
    for (int i=0; i<PCB_SIZE; i++){
        // el inicio del codigo es similar a las primera funciones
        // desde las cuales tambien accedemos a la tabla de PCBs

        // creamos una variable temporal para leer la entrada del PCB
        unsigned char pcb_entrada[PCB_ENTRY_SIZE];
        // leemos la entrada del PCB
        //https://stackoverflow.com/questions/10696845/does-fread-move-the-file-pointer
        fread(pcb_entrada, 1, PCB_ENTRY_SIZE, global_memory_file);

        // ahora comenzamos a extraer la informacion de la entrada del PCB
        // nos interesa primero que el proceso este inactivo
        unsigned char status = pcb_entrada[0];
        unsigned char pid = pcb_entrada[1];
        // Si el proceso esta inactivo, su estado es 0x00
        // Junto a esto, verificamos si el pid a asignar ya se encuentra en uso
        if (status == 0x00 && process_id == pid){
            // si el proceso esta inactivo, lo activamos
            pcb_entrada[0] = 0x01;
            // no necesito escribir algo porque el pid ya esta asignado
            printf("Proceso %s con PID %d iniciado\n\n", process_name, process_id);
            return;
        }
    }
    // si no encuentra un proceso en que el pid coincida y no este corriendo
    // entonces significa que podemos escribir el proceso en una nueva entrada
    bool free_pcb = 0;
    for (int i=0; i<PCB_SIZE; i++){
        unsigned char pcb_entrada[PCB_ENTRY_SIZE];
        // leemos la entrada del PCB
        fread(pcb_entrada, 1, PCB_ENTRY_SIZE, global_memory_file);

        
        unsigned char status = pcb_entrada[0];
        unsigned char pid = pcb_entrada[1];        

        if (status == 0x00 && pid == 0x00){ // asumimos que un pid 0 no es valido, o sea que si el pid de una entrada es 0, entonces esta vacia
            free_pcb = 1;
            // dejamos el proceso como activos
            pcb_entrada[0] = 0x01;
            // asignamos el pid
            pcb_entrada[1] = process_id;
            // finalmente asignamos el nombre del proceso
            strncpy((char*)&pcb_entrada[2], process_name, 11);
            fseek(global_memory_file, PCB_START + i*PCB_ENTRY_SIZE, SEEK_SET);
            fwrite(pcb_entrada, 1, PCB_ENTRY_SIZE, global_memory_file);
            fflush(global_memory_file);
             // entiendo que fflush limpia el cache
             // https://stackoverflow.com/questions/18170410/what-is-the-use-of-fflushstdin-in-c-programming
            
            printf("Proceso %s con PID %d iniciado\n\n", process_name, process_id);
            return;
        }
    } 
    if (free_pcb == 0){
        printf("No hay espacio para iniciar el proceso %s con PID %d\n\n", process_name, process_id);        
    }
    printf("No se pudo iniciar el proceso %s con PID %d\n\n", process_name, process_id);
}    

void os_finish_process(int process_id){
    printf("Funcion os_finish_process\n");

    // accedemos a la tabla de PCBs
    fseek(global_memory_file, PCB_START, SEEK_SET);

    unsigned char pcb_entrada[PCB_ENTRY_SIZE];
    bool process_found = 0;

    // recorremos la tabla de PCBs
    for (int i=0; i<PCB_SIZE; i++){
        fread(pcb_entrada, 1, PCB_ENTRY_SIZE, global_memory_file);

        unsigned char status = pcb_entrada[0];
        unsigned char pid = pcb_entrada[1];

        // si encontramos el proceso
        if (pid == process_id){
            process_found = 1;
            if (status == 0x01){
                pcb_entrada[0] = 0x00;
            }
            fseek(global_memory_file, PCB_START + i*PCB_ENTRY_SIZE, SEEK_SET);
            fwrite(pcb_entrada, 1, PCB_ENTRY_SIZE, global_memory_file);
            fflush(global_memory_file);
            printf("Proceso con PID %d finalizado\n\n", process_id);
        }
        

    }
    if (process_found == 0){
        printf("Proceso con PID %d no encontrado\n\n", process_id);
        return;
    }




}