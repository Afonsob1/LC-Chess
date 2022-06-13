#pragma once

#include <lcom/lcf.h>
#include "videocard.h"

typedef enum PieceType{
    b_pawn,
    b_knight,
    b_bishop,
    b_rook,
    b_queen,
    b_king,
    w_pawn,
    w_knight,
    w_bishop,
    w_rook,
    w_queen,
    w_king,
} PieceType;

typedef struct Piece
{
    uint16_t absolute_x;    // screen x 
    uint16_t absolute_y;    // screen y
    uint16_t x;             // board x 
    uint16_t y;             // board y
    uint16_t target_x;      // target x 
    uint16_t target_y;      // target y 
    xpm_image_t image;
    PieceType type;
    bool is_moving;
    bool priority_draw;     // if true only draw after all other pieces
    bool change;            // if any atribute change
    bool pawn_moved;
} Piece;
/**
 * @brief initialize a piece with a specific type in a given x and y
 * 
 * @param piece piece to be initialize
 * @param piece_type piece's type
 * @param absolute_x x where the piece belongs
 * @param absolute_y y where the piece belongs
 */
void initPiece(Piece* piece, PieceType piece_type,  int absolute_x, int absolute_y);

/**
 * @brief  draws the piece in the piece's position
 * 
 * @param mem buffer where we draw the image
 * @param piece piece that we want to draw
 */
void drawPieces(char* mem, Piece* piece);

/**
 * @brief sets the targe (target_x and target_y) position of the piece to the position where we want to move the piecewq
 * 
 * @param piecpiece that we want to movee 
 * @param x where we want to move the piecex 
 * @param y where we want to move the piecey 
 */
void movePiece(Piece* piece,int x, int y);

/**
 * @brief Set the Piece's position to the position given
 * 
 * @param piece piece that we want to set the position
 * @param x x of the position that we want to set
 * @param y y of the position that we want to set
 */

void setPiecePosition(Piece* piee,int x, int y);
/**
 * @brief updates the position by 1 until the piece reaches the target position
 * 
 * @param piece piece that we want to update
 * @return true if the piece reaches the target position and it was moving prior
 * @return false otherwise
 */

bool updatePiece(Piece* piece);
/**
 * @brief update the position to the target position
 * 
 * @param piece piece that we want to update
 * @return true if the piece was moving prior
 * @return false otherwise
 */

bool updatePiece_no_animation(Piece* piece);


