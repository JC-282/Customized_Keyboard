#include "stm32f1xx.h"
#include <string.h> // for memmove()
#include "keyscan.h"
#include "decode.h"
void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

//=============================================================================
// Debounced keypad scanning.
//=============================================================================

// 64 history bytes.  Each byte represents the last 8 samples of a button.
uint8_t hist[5][15];
uint8_t queue[2];  // A two-entry queue of button press/release events.
uint8_t press;
int qin;        // Which queue entry is next for input
int qout;       // Which queue entry is next for output


uint16_t func; // f1 gui alt shift control gui alt shift control


// const char keymap[4][16] = {"DCBA#9630852*741", "DCBA#9630852*741", "DCBA#9630852*741", "DCBA#9630852*741"};
/*
const uint8_t keymap[5][15] = {{27, 0,   49,  50,  51,  52,  53,  54,  55,  56,  57,  48, 45, 61, 8},
							   {9,  113, 119, 101, 114, 116, 121, 117, 105, 111, 112, 91, 93, 92, 0},
							   {0,  97,  115, 100, 102, 103, 104, 106, 107, 108, 59,  39, 13, 0,  0},
							   {0,  122, 120, 67,  118, 98,  110, 109, 44,  46,  47,  0,  0,  0,  0},
							   {0,  0,   0,   0,   0,   32,  0,   0,   0,   0,   0,   0,  0,  0,  0}};*/


void push_queue(int n) {
    queue[qin] = n;
    qin ^= 1;
}

uint8_t pop_queue() {
	uint8_t tmp = queue[qout];
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}



void update_history(int c, int rows)
{

	// maybe c should be rows
    for(int i = 0; i < 15; i++) {
    	//if (i == 2) continue;
        hist[rows][i] = (hist[rows][i]<<1) + ((c>>i)&1);


        if (hist[rows][i] == 0x01)
        {
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 225)
        	{
        		func |= 1 << 5; //left shift
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 224)
        	{
        		func |= 1 << 4;//left ctrl
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 227)
        	{
        		func |= 1 << 7; //left GUI
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 226)
        	{
        		func |= 1 << 6; //left alt
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 230)
        	{
        		func |= 1 << 2; //right alt
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 228)
        	{
        		func |= 1 << 0; //right ctrl
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 229)
        	{
        		func |= 1 << 1; //right shift
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 0xff)
        	{
        	    func |= 1 << 8; //f1
        	}


//        	push_queue(0x80 | (((rows << 4) & 0x70) | (0xf & i )));
        	press = (0x80 | (((rows << 4) & 0x70) | (0xf & i )));
        }
        else if (hist[rows][i] == 0xfe)
        {
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 225)
        	{
        		func &= ~(1 << 5);
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 224)
        	{
        		func &= ~(1 << 4);//left ctrl
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 227)
        	{
        		func &= ~(1 << 7); //left GUI
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 226)
        	{
        		func &= ~(1 << 6); //left alt
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 230)
        	{
				func &= ~(1 << 2); //right alt
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 228)
        	{
        		func &= ~(1 << 0); //right ctrl
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 229)
        	{
        		func &= ~(1 << 1); //right shift
        	}
        	if(decode_usb((0xf0 & (rows << 4)) | (0xf & i)) == 0xff)
        	{
        	    func &= ~(1 << 8); //f1
        	}


//        	push_queue((((rows << 4) & 0x70) | (0xf & i)));
//        	press = ((((rows << 4) & 0x70) | (0xf & i)));
        	press = 0;


        }
    }
}

void drive_row(int c)
{
    GPIOA->BSRR = (0x1F & ~(1 << c)) | (1 << (c + 16));

    //GPIOA->BSRR = 0x00ffff;
}

int read_col()
{
//    return ((~GPIOB->IDR)>>3) & 0xff;
	int re = ~GPIOB->IDR;

	return (re & 0x3) | ((re & 0xfff8) >> 1);
	//return (~GPIOB->IDR);
}

uint8_t get_key_event(void) {
    for(;;) {
        asm volatile ("wfi" : :);   // wait for an interrupt
//        if (queue[qout] != 0)
        if (press != 0)
            break;
    }
//    return pop_queue();
    return press;
}

uint8_t get_keypress() {
	uint8_t event;
    for(;;) {
        // Wait for every button event...
        event = get_key_event();
        press = 0;
        // ...but ignore if it's a release.
        if (event & 0x80){
        	return event & 0x7f;
        }


    }

}



uint32_t send_char()
{
	uint8_t key = get_keypress();
	return ((func << 8) | (0xff & key));
}
