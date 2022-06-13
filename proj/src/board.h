#ifndef BOARD_H
#define BOARD_H


#include "piece.h"
#include "videocard.h"
#include <lcom/lcf.h>

#define get_position(x,y) (x + y*8)

typedef struct
{
    char * mem_board;
    char * mem_pieces;
    Piece* board[64];
}Board;

/**
 *  @brief Board Constructor:  create video mem buffers that board use and all pieces of the game
 *  @param board    board to init
 * 
*/
void initBoard(Board* board);

/**
 * draw board squares in board_mem 
 * @param board board to be drawn
 * 
 */
void drawBoard(Board* board);

/**
 * @brief draw board and pieces in the video_mem_buffer
 * 
 * @param board     board to be drawn
 */
void drawBoardPieces(Board* board);

/**
 *  @brief convert col into screen x
 * 
 *  @param col  board col 
 *  @return screen x
 */
int getScreenX( int col);

/**
 *  @brief convert row into screen y 
 * 
 *  @param row  board row 
 *  @return screen y
 */
int getScreenY( int row);

/**
 *  @brief convert screen x into board x 
 * 
 *  @param absoluteX screen x 
 *  @return board x
 */
int getBoardX(int absoluteX);

/**
 *  @brief convert screen y into board y 
 * 
 *  @param absoluteY screen y 
 *  @return board y
 */
int getBoardY(int absoluteY);

/**
 * @brief update all the pieces in the board
 * 
 * @param board         board to update
 * @param animation     if the animations are active or not
 */
void updateBoard(Board* board, bool animation);

/**
 * @brief calcule the possibles positions for the piece to move, and paint them in board with other color
 * @param board     where the piece is
 * @param piece     piece to calcule positions
 * @param x         x of the piece in the board
 * @param y         y of the piece in the board
 * 
 */
void possiblePosition(Board* board, Piece* piece,int x, int y);

/**
 * @brief change priority of the piece, if true the board will draw this piece in the last place
 * @param p         piece to be changed
 * @param priority  new priority of the piece
 */
void changePiecePriority(Piece* p, bool priority);


#endif
