//**************************************************************************
// Description: mageboot bootloader
// Author: CJ Vaughter
//**************************************************************************
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
//**************************************************************************

#ifndef _MAGEBOOT_H_
#define _MAGEBOOT_H_

#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>

#define SW_VER		2.0

#define OK			0xF0
#define FAIL		0xFF
#define EXECUTE		0x0A
#define SIGNATURE	0x01
#define ADDRESS		0x02
#define READ		0x03
#define WRITE		0x04
#define EXIT		0x05

#if defined (__AVR_ATmega328P__) //MCU Certification Board
#warning "MAKE SURE MISCELLANEOUS LINKER FLAGS ARE:  -Wl,--section-start=.text=0x7E00"
typedef uint8_t length_t;
typedef uint16_t address_t;
#define F_CPU					 16000000L
#define read_pgm_mem(address)	 pgm_read_byte_near(address)
#define GETLENGTH(length)		 read(); length = read()
#define GETADDRESS(addr)		 read(); read(); addr = (((address_t)(read())<<8)|(read()))<<1

#elif defined (__AVR_ATmega2560__) //MAGE 2 PIU
typedef uint16_t length_t;
typedef uint32_t address_t;
#define F_CPU					 16000000L
#define read_pgm_mem(address)	 pgm_read_byte_far(address)
#define GETLENGTH(length)		 length = ((length_t)read())<<8; length |= read()
#define GETADDRESS(addr)		 addr = (((address_t)(read())<<24)|((address_t)(read())<<16)|((address_t)(read())<<8)|(read()))<<1
#define OTA_ENABLED
#define BAUD_RATE_OTA			 38400L
#define BAUD_OTA				 (uint8_t)( (F_CPU + BAUD_RATE_OTA * 4L) / (BAUD_RATE_OTA * 8L) - 1 )
#define SETUP_UART_OTA			 UCSR1A = 0x02; UCSR1B = 0x18; UCSR1C = 0x06; UBRR1L = BAUD_OTA
#define NO_ERROR_OTA			 !(UCSR1A & _BV(FE1))
#define WAITING_FOR_RX_OTA		 !(UCSR1A & _BV(RXC1))
#define WAITING_FOR_TX_OTA		 !(UCSR1A & _BV(UDRE1))
#define UART_DATA_OTA			 UDR1
#endif

#define BAUD_RATE				 115200L
#define BAUD					 (uint8_t)( (F_CPU + BAUD_RATE * 4L) / (BAUD_RATE * 8L) - 1 )
#define SETUP_UART				 UCSR0A = 0x02; UCSR0B = 0x18; UCSR0C = 0x06; UBRR0L = BAUD
#define NO_ERROR				 !(UCSR0A & _BV(FE0))
#define WAITING_FOR_RX			 !(UCSR0A & _BV(RXC0))
#define WAITING_FOR_TX			 !(UCSR0A & _BV(UDRE0))
#define UART_DATA				 UDR0

#define RESET_FLAGS				 (_BV(WDRF) | _BV(BORF) | _BV(PORF))
#define START_PROGRAM			 asm volatile ("clr r2\n clr r30\n" "clr r31\n" "ijmp\n")
#define ram_bfr					 ((uint8_t*)(RAMSTART))

#define ENABLE_WDT_CHANGE		 WDTCSR = _BV(WDCE) | _BV(WDE)
#define RESET_WATCHDOG			 asm volatile ("wdr\n")
#define WATCHDOG_OFF			 (0)
#define WATCHDOG_16MS			 (_BV(WDE))
#define WATCHDOG_125MS			 (_BV(WDP1) | _BV(WDP0) | _BV(WDE))
#define WATCHDOG_2S				 (_BV(WDP2) | _BV(WDP1) | _BV(WDP0) | _BV(WDE))

#define page_fill(address, data) boot_page_fill(address, data)
#define page_erase(address)      boot_page_erase(address)
#define page_write(address)      boot_page_write(address)
#define busy_wait()				 boot_spm_busy_wait()
#define rww_enable()             boot_rww_enable()

int main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9")));

uint8_t __attribute__((noinline)) read(void);
void	__attribute__((noinline)) write(uint8_t data);
void	__attribute__((noinline)) validate();
void	__attribute__((noinline)) setWDT(uint8_t x);
void	__attribute__((noinline)) reset();

static inline void read_mem(address_t address, length_t length);
static inline void write_buffer(length_t length);
static inline void write_mem(uint8_t* bfr, address_t address, length_t length);

#endif
