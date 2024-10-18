#pragma once
#include "../osrms_File/Osrms_File.h"
#include <stdio.h>

extern FILE* global_memory_file;
// https://stackoverflow.com/questions/3684450/what-is-the-difference-between-static-and-extern-in-c

void os_mount(char* memory_path);
void os_ls_processes();
int os_exists(int process_id, char* file_name);
void os_ls_files(int process_id);
void os_frame_bitmap();
void os_tp_bitmap();
void os_start_process(int process_id, char* process_name);
void os_finish_process(int process_id);