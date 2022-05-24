#include "videocard.h"
#include "board.h"

int main(int argc, char* argv[]){

    lcf_set_language("EN-US");

    lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {

    map_vram(INDEXED_MODE);
    init_graphics_mode(INDEXED_MODE);

    Board board;
    initBoard(&board);

    

    drawBoard(&board);

    sleep(10);
    return return_text_mode();
}
