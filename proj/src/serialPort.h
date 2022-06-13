#pragma once
#include <lcom/lcf.h>

#include "serialConst.h"
#include "queue.h"


/**
 *  @brief Configures UART to place the specified number of bits per char
 *  @param val number of bits per cher to configurate in the UART
 *  @return 0 in case of sucess, 1 otherwise
 */
int sp_set_number_of_bits_per_char(uint8_t number_of_bits);


/**
 *  @brief set the  bitrate
 *  @param bitrate Bitrate to set
 *  @return 0 if successful, -1 otherwise
 */
int sp_set_bitrate(int bitrate);

/**
 *  @brief enable or disable the bit in the port given
 * 
 *  @param port  where to change
 *  @param bit bit to change
 *  @param enable true to enable the interrupt, false otherwise
 *  @return 0 if successful, -1 otherwise
 */
int sp_enable(uint8_t port ,uint8_t bit, bool enable);

/**
 *  @brief enable all interrupts, set the number of bits per menssage, and bitrate
 *  @return 0 if successful
 */
int sp_configure_init();

/**
 *  @brief disable all interrupts of Serial Port
 *  @return 0 if successful, -1 otherwise
 */
int sp_configure_end();

/**
 *  @brief subscribe serial port interrupst
 *  @param bit_no store Serial Port IRQ line
 *  @return 0 if successful, -1 otherwise
 */
int (sp_subscribe_int)(uint8_t *bit_no);

/**
 *  @brief unsubscribe interrupt serial port
 *  @return 0 if successful, -1 otherwise
 */
int (sp_unsubscribe_int)();

/**
 *  @brief check if can write in the serial port 
 *  @return true if can write, false otherwise
 */
bool sp_check_write();

/**
 *  @brief check if there is something to read in the serial port
 *  @return true if there is, false otherwise
 */
bool sp_check_read();

/**
 *  @brief send a byte to serial port from transmit queue
 * @return 0 if no error  occurred
 */
int sp_write();

/**
 * @brief Reads a byte from the serial port and adds it to the receive queue
 * @return 0 if no error  occurred
 */
int sp_read();

/**
 *  @brief Serial port interrup
 *  @return 0 if no error  occurred
 */
uint8_t (sp_ih)();

/**
 *  @brief returns the first element from the receiver queue and removes it 
 *  @return  the first element of the receiver queue 
 */
uint8_t readFromQueue();

/**
 * @brief While there are messages to be received through the serial port, receive clear them. Also clears the receive queue.
 */
void sp_clear();

/**
 *  @brief Adds an element to the transmit queue
 *  @param data element to add to the transmit queue
 */
void addToTransmitQueue(uint8_t data);

/**
* sends all the data in the transmit queue to the serial port
*/
void sp_emptyTransmitQueue();
