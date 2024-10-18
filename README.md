# Tarea 2

Diego Valenzuela Arqueros

## Algunos supuestos
Para os_exists, no se considera revisar el byte de validez de la entrada, solo si existe o no. Esto lo decidi escribir asi porque no se especifica en el enunciado.

No verifico que el ultimo bit de algunos bytes sea nulo, ya que no se especifica en el enunciado.

Par la funcion os_ls_files, si se comprueba el byte de validez de la entrada, para evitar errores en el funcionamiento de la funcion.

## Funciones implementadas

- os_mount.
- os_ls processes.
- os_exists.
- os_ls files.
- os_frame bitmap.
- os_tp bitmap.
- os_start process.
- os_finish process.

## Funciones no implementadas
- os_open.
- os_write file.
- os_read file.
- os_close.


## Referencias
Las fuentes de informacion que revise las deje a lo largo del codigo, pero las reuno de todos modos aqui

https://stackoverflow.com/questions/3684450/what-is-the-difference-between-static-and-extern-in-c
https://stackoverflow.com/questions/75620363/difference-between-rb-and-rb-or-wb-and-wb
https://www.fresh2refresh.com/c-programming/c-file-handling/fseek-seek_set-seek_cur-seek_end-functions-c/
https://www.geeksforgeeks.org/unsigned-char-in-c-with-examples/
https://stackoverflow.com/questions/8202897/null-terminated-string-in-c
https://www.ibm.com/docs/es/i/7.5?topic=functions-strcmp-compare-strings
https://stackoverflow.com/questions/2898364/strcpy-vs-memcpy
https://stackoverflow.com/questions/10696845/does-fread-move-the-file-pointer
https://stackoverflow.com/questions/18170410/what-is-the-use-of-fflushstdin-in-c-programming

## Comentarios
- La funcion os_finish_process no esta implementada de forma completa, solo alcance a invalidar el byte de validez de la entrada, pero no alcance a manejar la memoria.

- revise el codigo con valgrind y no deberia haber problemas de memoria.

- Use Copilot a lo largo del desarrollo del codigo. Sin embargo, la mayoria de sugerencias y codigo autocompletado contenia demasiados errores, por lo que en realidad no aporto mucho. Al final solo me ayudo a escribir 'boilerplate' o codigo que ya estaba en funciones anteriores y que necesitaba usar de nuevo. Algo en lo que si me ayudo fue en poder escribir los unsigned de forma correcta

- El main imprime los resultados de cada funcion que implemente, basado en usar mem_filled.bin como memoria principal. EL codigo se basa en como entendi que se debian implementar las funciones.