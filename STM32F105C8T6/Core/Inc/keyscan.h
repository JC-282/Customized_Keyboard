#ifndef __KEYSCAN_H
#define __KEYSCAN_H

void nano_wait(unsigned int);
void drive_row(int);   // energize one of the column outputs
int  read_col();         // read the four row inputs
void update_history(int col, int rows); // record the buttons of the driven column
uint8_t get_key_event(void); // wait for a button event (press or release)
uint8_t get_keypress(void);  // wait for only a button press event.
float getfloat(void);     // read a floating-point number from keypad
void show_keys(void);     // demonstrate get_key_event()


uint32_t send_char(); // Send key pressed to USART_TX


#endif
