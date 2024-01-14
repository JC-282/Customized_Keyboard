#include "main.h"
#include "usb_device.h"
#include "stm32f1xx_hal.h"
#include "stm32f103xb.h"

#include "usbd_hid.h"
#include "decode.h"
#include "eeprom.h"


extern USBD_HandleTypeDef hUsbDeviceFS;



subKeyBoard keyBoardHIDsub = {0,0,0,0,0,0,0,0};


uint8_t rows; // the row being scanned
uint16_t sequence[10][SEQUENCE_LENGTH] = {{0xffff}};

char UI1[] = {"Please make sure you are in text editor and press ENTER to continue >>>>"};
char UI2[] = "\r\r1. Display the key layout "
		"\r2. Display all macro key sequence "
		"\r3. Setup macro key sequence "
		"\r4. Swap key position "
		"\r5. Reset macro key sequence "
		"\r6. Reset key layout ";
char UI3[] = "\rPress ESC to exit OR Press the number to choose >>>>";
char ERR[] = "\rWrong input, try again.";
char messageSav[] = "\rSaving......Please wait!";
char keyout[5][15][7] = {
		{"ESC\t","`/~\t","1\t","2\t","3\t","4\t","5\t","6\t","7\t","8\t","9\t","0\t","-/_\t","=/+\t","BACK\t"},
		{"TAB\t","q\t","w\t","e\t","r\t","t\t","y\t","u\t","i\t","o\t","p\t","[/{\t","]/}\t","\\/|\t","F1\t"},
		{"CAP\t","a\t","s\t","d\t","f\t","g\t","h\t","j\t","k\t","l\t",";/:\t","\'/\"\t","ENTER\t","F2\t","\t"},
		{"SHIFT\t","z\t","x\t","c\t","v\t","b\t","n\t","m\t",",/<\t",".>\t","//?\t","SHIFT\t","UP\t","\t","\t"},
		{"CTRL\t","GUI\t","ALT\t","\t","\t","SPACE\t","\t","\t","\t","\t","ALT\t","CTRL\t","LEFT\t","DOWN\t","RIGHT\t"},
	};

int main(void)
{
	HAL_Init();
	enable_ports();

	init_tim2();

	init_usart1();
	SystemClock_Config();
	MX_USB_DEVICE_Init();



	#define eprog
	#ifdef eprog
	init_eeprom();
	loadLayout();
	loadSeq();
//	char messageaa[] = "ready";
//	print_message(messageaa);
	#endif


	#define key
	#ifdef key
	int countF2 = 0;

	while (1)
	{
		uint32_t c = send_char();

		if (GPIOA->IDR >> 10 & 1) //uart
		{
			if ((c >> (8 + 8)) & 1) // f1
			{
				countF2 = 0;
				display_sequence(c);
			}
			else
			{
				countF2 = 0;
				uart_printChar(c);
			}

		}
		else //usb
		{
			if ((c >> (8 + 8)) & 1) // f1
			{
				countF2 = 0;
				display_sequence(c);
			}
			else if (decode_uart(c) == F2) // f2
			{
				countF2++;
				if (countF2 == 3) {
					UI_modify();
					countF2 = 0;
				}
			}
			else
			{
				countF2 = 0;
				usb_printKey(c);
			}

		}

	}



	#endif
}
void display_sequence(uint32_t c)
{
		if (GPIOA->IDR >> 10 & 1) {
			if (decode_usb(c) == 30) {
				uart_printSeq(sequence[1]);
			} else if (decode_usb(c) == 31) {
				uart_printSeq(sequence[2]);
			} else if (decode_usb(c) == 32) {
				uart_printSeq(sequence[3]);
			} else if (decode_usb(c) == 33) {
				uart_printSeq(sequence[4]);
			} else if (decode_usb(c) == 34) {
				uart_printSeq(sequence[5]);
			} else if (decode_usb(c) == 35) {
				uart_printSeq(sequence[6]);
			} else if (decode_usb(c) == 36) {
				uart_printSeq(sequence[7]);
			} else if (decode_usb(c) == 37) {
				uart_printSeq(sequence[8]);
			} else if (decode_usb(c) == 38) {
				uart_printSeq(sequence[9]);
			} else if (decode_usb(c) == 39) {
				uart_printSeq(sequence[0]);
			}
		} else {
			if (decode_usb(c) == 30) {
				usb_printSeq(sequence[1]);
			} else if (decode_usb(c) == 31) {
				usb_printSeq(sequence[2]);
			} else if (decode_usb(c) == 32) {
				usb_printSeq(sequence[3]);
			} else if (decode_usb(c) == 33) {
				usb_printSeq(sequence[4]);
			} else if (decode_usb(c) == 34) {
				usb_printSeq(sequence[5]);
			} else if (decode_usb(c) == 35) {
				usb_printSeq(sequence[6]);
			} else if (decode_usb(c) == 36) {
				usb_printSeq(sequence[7]);
			} else if (decode_usb(c) == 37) {
				usb_printSeq(sequence[8]);
			} else if (decode_usb(c) == 38) {
				usb_printSeq(sequence[9]);
			} else if (decode_usb(c) == 39) {
				usb_printSeq(sequence[0]);
			}
		}

}

void uart_printSeq(uint16_t* sequence)
{
	int idx = 0;
		while (sequence[idx] != 0xffff)
		{
			uart_printChar(sequence[idx]);
			idx++;
		}

		return;
}

void uart_printChar(uint32_t c)
{
	while (!(USART1->SR & USART_SR_TXE)) {}
	USART1->DR = decode_uart(c);
}


void usb_printKey(uint16_t output)
{
	keyBoardHIDsub.MODIFIER = (output >> 8) & 0xff;
	keyBoardHIDsub.KEYCODE1 = decode_usb(output & 0xff);

	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &keyBoardHIDsub, sizeof(keyBoardHIDsub));
	HAL_Delay(HAL_DELAY);

	keyBoardHIDsub.MODIFIER = 0x00;
	keyBoardHIDsub.KEYCODE1 = 0;
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &keyBoardHIDsub, sizeof(keyBoardHIDsub));
	HAL_Delay(HAL_DELAY*2);

	return;
}

void usb_printChar(uint16_t c)
{
	if((c >> 8 & 1) == 1)
	{
		keyBoardHIDsub.MODIFIER = 1 << 1;
	}
	else
	{
		keyBoardHIDsub.MODIFIER = 0;
	}
	keyBoardHIDsub.KEYCODE1 = c & 0xff;

	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &keyBoardHIDsub, sizeof(keyBoardHIDsub));
	HAL_Delay(HAL_DELAY);

	keyBoardHIDsub.MODIFIER = 0x00;
	keyBoardHIDsub.KEYCODE1 = 0;
	USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t*) &keyBoardHIDsub, sizeof(keyBoardHIDsub));
	HAL_Delay(HAL_DELAY*2);
	return;
}

void usb_printSeq(uint16_t *sequence)
{
	int idx = 0;
	while (sequence[idx] != 0xffff)
	{
		usb_printKey(sequence[idx]);
		idx++;
	}

	return;
}


void UI_modify()
{
	char escape[] = "\rFinish setup. Bye Bye :)\r";
	print_message(UI1);
	uint32_t enter;
	do
	{
		enter = send_char();
	}while(decode_uart(enter) != '\r');
	print_message(UI2);
	do
	{
		print_message(UI3);
		uint32_t sel = send_char();
		uint8_t num = decode_uart(sel);
		switch (num)
		{
		case 0x31:
			display_layout();
			break;
		case 0x32:
			display_key_sequence();
			break;
		case 0x33:
			setup_sequence();
			break;
		case 0x34:
			swap_key();
			break;
		case 0x35:
			reset_sequence();
			break;
		case 0x36:
			reset_layout();
			break;
		case 0x1b:

			//print_message(messageSav);
			//saveLayout();
			//saveSeq();
			//char messageD[] = "\rDone";
			//print_message(messageD);
			print_message(escape);
			return;
			break;

		default:
			print_message(ERR);
			break;
		}
	} while (1);
	return;
}

void display_layout()
 {

	char newline[] = { "\r" };
	print_message(newline);
	uint8_t (*map)[15] = get_keyMap();
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 15; j++) {
			print_message(keyout[(map[i][j] & 0xf0) >> 4][map[i][j] & 0xf]);
		}
		print_message(newline);
	}
	return;
}


void display_key_sequence()
{

	for(int i = 0; i < 10; i++)
		{
			usb_printKey(0x2c); //enter
			usb_printChar(lookup_char(0x30+i));  //num
			usb_printKey(0x39); //"."
			usb_printKey(0x45); //" "

			usb_printSeq(sequence[i]);
		}
}

void setup_sequence()
{

	uint8_t num;
	uint32_t entry;
	uint8_t input;

	int count = 0;
	do {
		char message[] = "\rChoose the sequence to modify. From 0 - 9 >>>>";
		print_message(message);
		entry = send_char();
		num = decode_uart(entry);
		if (num < 0x30 || num > 0x39) {
			print_message(ERR);
		}
	} while (num < 0x30 || num > 0x39);

	char message1[] = "\rStart Setting number ";
	print_message(message1);
	usb_printKey(entry);

	char message3[] = " Please enter the sequence end with ENTER:\rNo more than 15 characters!!!\r";
	print_message(message3);

	do
	{
		entry = send_char();
		input = decode_uart(entry);
		if(input != '\r')
		{
			sequence[num & 0xf][count++] = entry & 0xffff;
			usb_printKey(entry);
		}

	}while (input != '\r' && (count != SEQUENCE_LENGTH-1));
	sequence[num & 0xf][count] = 0xffff;

	print_message(messageSav);
	saveSeq(num - 0x30);
	char messageD[] = "\rDone";
	print_message(messageD);
	return;
}




void reset_sequence()
{
	char message[] = "\rStart resetting sequence......";
	print_message(message);
	for(int i = 0; i < 10; i++)
	{

			sequence[i][0] = 0xffff;
			saveSeq(i);
	}
	char message1[] = "\rDone";
	print_message(message1);
}

void print_message(char *msg)
{
	int idx = 0;
	while(msg[idx] != 0)
		{
			uint16_t result = lookup_char(msg[idx++]);
			usb_printChar(result);
		}
}
void swap_key()
{
	uint32_t first;
	uint32_t second;
	//do {
	uint8_t (*map)[15] = get_keyMap();
		char message[] = "\rChoose first key to swap: ";
		print_message(message);
		first = send_char();
		int i, j;
		i = (first & 0xf0) >> 4;
		j = first & 0xf;
		print_message(keyout[(map[i][j] & 0xf0) >> 4][map[i][j] & 0xf]);

		char message2[] = "\rChoose second key to swap: ";
		print_message(message2);
		second = send_char();
		i = (second & 0xf0) >> 4;
		j = second & 0xf;
		print_message(keyout[(map[i][j] & 0xf0) >> 4][map[i][j] & 0xf]);

		//if (((first & 0xff) == 0x2d) || ((second & 0xff) == 0x2d)) {
		//	char message3[] = "\rENTER cannot be swapped, try again";
		//	print_message(message3);
		//}
	//} while (((first & 0xff) == 0x2d) || ((second & 0xff) == 0x2d));

	swap((first & 0xff), (second & 0xff));

	char messageS[] = "\rSaving......Please wait!";
		print_message(messageS);
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < SEQUENCE_LENGTH; j++)
		{
			if ((sequence[i][j] & 0xff) == (first & 0xff))
			{
				sequence[i][j] = (sequence[i][j] & 0xff00) | (second & 0xff);
				continue;
			}
			if ((sequence[i][j] & 0xff) == (second & 0xff))
			{
				sequence[i][j] = (sequence[i][j] & 0xff00) | (first & 0xff);
			}

		}
	}

	int ax,ay,bx,by;
	ax = (first & 0xf0) >> 4;
	ay = first & 0xf;
	bx = (second & 0xf0) >> 4;
	by = second & 0xf;
	eeprom_byteWrite(map[ax][ay], ax*15+ay);
	eeprom_byteWrite(map[bx][by], bx*15+by);
	//saveLayout();

	char messageD[] = "\rDone";
	print_message(messageD);
}

void saveSeq(int entry)
{

	//for(int i = 0; i < 10; i++)
	//{
		uint8_t temp[SEQUENCE_LENGTH*2];

		for(int j = 0; j <SEQUENCE_LENGTH; j++)
		{
			for(int k = 0; k < 2; k++)
			{
				temp[j*2 + k] = (k == 0) ? ((sequence[entry][j] & 0xff00) >> 8 ) : (sequence[entry][j] & 0xff);
			}
		}
		eeprom_pageWrite(temp, KEY_SEQUENCE_PAGE_START + SEQUENCE_LENGTH*2 * entry, SEQUENCE_LENGTH*2);
	//}

	return;
}

void loadSeq()
{
	uint8_t temp[10*SEQUENCE_LENGTH*2];
	eeprom_sequentialRead(temp, KEY_SEQUENCE_PAGE_START, 10*SEQUENCE_LENGTH*2);
/*
	for(int i = 0; i< 10*SEQUENCE_LENGTH*2; i++)
	{
		while (!(USART1->SR & USART_SR_TXE)) {}
		USART1->DR = temp[i];
	}
	*/
	for(int i = 0; i < 10; i++)
		{

			for(int j = 0; j <SEQUENCE_LENGTH; j++)
			{
				uint8_t temp1;
				uint8_t temp2;
				for(int k = 0; k < 2; k++)
				{
					if (k == 0) temp1 = temp[i*SEQUENCE_LENGTH*2 + j * 2 + k];
					else temp2 = temp[i*SEQUENCE_LENGTH*2 + j * 2 + k];
				}

				sequence[i][j] = ((temp1 << 8) | temp2) & 0xffff;
			}

		}

	return;
}
/*
void saveLayout()
{
	uint8_t (*keyarr)[15] = get_keyMap();
	uint8_t arr[32] = {};
	int count = 0;
	for(int i = 0; i<5; i++)
	{
		for(int j = 0; j<15; j++)
		{

			arr[count] = keyarr[i][j];
			if(count == 31)
				{

				eeprom_pageWrite(arr, i*15+j - 31, 32);

				count = 0;
				continue;
			}
			count++;
		}
	}
	eeprom_pageWrite(arr, KEY_LAYOUT_LAST, 11);


	return;
}
*/


void loadLayout()
{
	uint8_t (*keyarr)[15] = get_keyMap();
	uint8_t temp[75];
	eeprom_sequentialRead(temp, 0, 75);
	for(int i = 0; i<5; i++)
	{
		for(int j = 0; j<15; j++)
		{
			keyarr[i][j] = temp[i*15+j];
		}

	}
	return;
}
//============================================================================
// The Timer 2 ISR
//============================================================================

void TIM2_IRQHandler()
{
    TIM2->SR &= ~1;
    int col = read_col();
    update_history(col, rows);
    rows = rows + 1 <= 4 ? (rows + 1) : 0;
    drive_row(rows);
}
