#include "../osrms_API/osrms_API.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{

    // montar la memoria
    os_mount((char *)argv[1]);

    //resto de instrucciones
    os_ls_processes();
    //manejamos distintos casos
    os_exists(117, "facebook.png");
    // os_exists(117, "facebook.txt");
    // os_exists(118, "facebook.png");

    os_ls_files(228);
    os_frame_bitmap();
    os_tp_bitmap();

    os_start_process(200, "process200");
    os_ls_processes(); // repito la funcion para poder apreciar los cambios

    os_finish_process(200);
    os_ls_processes(); // lo mismo aqui 

    // cerramos el archivo de memoria que teniamos montado
    fclose(global_memory_file);
    return 0;

    // clear && make && make clean && make
    // valgrind --leak-check=full --show-leak-kinds=all ./osrms mem_filled.bin

}
