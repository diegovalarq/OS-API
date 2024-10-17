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

    return 0;

}
