#include "mageboot.h"

void init(void)
{
	CLEAR_ZEROREG;
	SP = RAMEND;
	uint8_t flags = MCUSR;
	MCUSR = 0;
	setWDT(WATCHDOG_OFF);
	if (flags & RESET_FLAGS) START_PROGRAM;
}

int main(void)
{	
	SETUP_UART;
	register uint8_t cmd;
	register address_t address = 0;
	register length_t length;
	
	setWDT(WATCHDOG_500MS);

	while(1)
	{
		cmd = read();
		switch(cmd)
		{
			case ADDRESS:
			{
				GETADDRESS(address);
				validate();
			}
			break;
			case READ:
			{
				GETLENGTH(length);
				validate();
				read_mem(address, length);
			}
			break;
			case WRITE:
			{
				GETLENGTH(length);
				validate();
				write_buffer(length);
				write_mem(ram_bfr, address, length);
			}
			break;
			case EXIT:
			{
				validate();
				reset();
			}
			break;
			default:
			{
				write(FAIL);
			}
			break;
		}
		write(OK);
	}
}

uint8_t read(void)
{
	while(WAITING_FOR_RX);
	if(NO_ERROR) RESET_WATCHDOG;
	return UART_DATA;
}

void write(uint8_t data)
{
	while(WAITING_FOR_TX);
	UART_DATA = data;
}

void validate()
{
	if(read() != EXECUTE) reset();
}

void setWDT(uint8_t x)
{
	ENABLE_WDT_CHANGE;
	WDTCSR = x;
}

void reset()
{
	setWDT(WATCHDOG_16MS);
	while(1);
}

static inline void read_mem(address_t address, length_t length)
{
	while(length)
	{
		write(read_pgm_mem(address));
		address++;
		length--;
	}
}

static inline void write_buffer(length_t length)
{
	uint8_t* bufPtr = ram_bfr;
	while(length--) *bufPtr++ = read();
}

void write_mem(uint8_t* bfr, address_t address, length_t length)
{
	uint32_t addrPtr = address;
	
	page_erase(address);
	busy_wait();
	
	while(length)
	{
		uint16_t a = *bfr++;
		a |= (*bfr++) << 8;
		page_fill(addrPtr,a);
		addrPtr += 2;
		length -= 2;
	}
	
	page_write(address);
	busy_wait();
	
	rww_enable();
}

