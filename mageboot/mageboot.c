// Copyright 2015 Oklahoma State University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "mageboot.h"

#ifdef OTA_ENABLED
register uint8_t OTA asm("r2");
#endif

int main(void)
{
	uint8_t flags = MCUSR;
	MCUSR = 0;
	setWDT(WATCHDOG_OFF);
	if (flags & RESET_FLAGS) START_PROGRAM;
	
	SETUP_UART;
	setWDT(WATCHDOG_500MS);
	
	#ifdef OTA_ENABLED
	if(OTA)
	{
		SETUP_UART_OTA;
		setWDT(WATCHDOG_2S);
	}
	#endif
	
	register uint8_t cmd;
	register address_t address = 0;
	register length_t length;

	while(1)
	{
		cmd = read();
		switch(cmd)
		{
			case SIGNATURE:
			{
				validate();
				write(SIGNATURE_0);
				write(SIGNATURE_1);
				write(SIGNATURE_2);
			}
			break;
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
				#ifdef OTA_ENABLED
				if(OTA == 0)
				#endif
					write(FAIL);
			}
			break;
		}
		#ifdef OTA_ENABLED
		if(OTA == 0)
		#endif
			write(OK);
	}
}

uint8_t read(void)
{
	#ifdef OTA_ENABLED
	if(OTA)
	{
		while(WAITING_FOR_RX_OTA);
		if(NO_ERROR_OTA) RESET_WATCHDOG;
		return UART_DATA_OTA;
	}
	#endif
	
	while(WAITING_FOR_RX);
	if(NO_ERROR) RESET_WATCHDOG;
	return UART_DATA;
}

void write(uint8_t data)
{
	#ifdef OTA_ENABLED
	if(OTA)
	{
		while(WAITING_FOR_TX_OTA);
		UART_DATA_OTA = data;
		return;
	}
	#endif
	
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

