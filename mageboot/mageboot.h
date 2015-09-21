#ifndef _MAGEBOOT_H_
#define _MAGEBOOT_H_

#include <avr/io.h>

#define OK			0xF0
#define FAIL		0xFF
#define EXECUTE		0x0A
#define ADDRESS		0x01
#define READ		0x02
#define WRITE		0x03
#define EXIT		0x04

typedef uint16_t length_t;
typedef uint32_t address_t;
#define BAUD_RATE 38400L
#define BAUD (uint8_t)( (F_CPU + BAUD_RATE * 4L) / (BAUD_RATE * 8L) - 1 );
#define RESET_FLAGS (_BV(WDRF) | _BV(BORF) | _BV(PORF))
#define SETUP_UART UCSR0A = 0x02; UCSR0B = 0x18; UCSR0C = 0x06; UBRR0L = BAUD
#define CLEAR_ZEROREG asm volatile ("clr __zero_reg__")
#define START_PROGRAM asm volatile ("clr r30\n" "clr r31\n" "ijmp\n")
#define NO_ERROR !(UCSR0A & _BV(FE0))
#define WAITING_FOR_RX !(UCSR0A & _BV(RXC0))
#define WAITING_FOR_TX !(UCSR0A & _BV(UDRE0))
#define UART_DATA UDR0
#define ENABLE_WDT_CHANGE WDTCSR = _BV(WDCE) | _BV(WDE)
#define RESET_WATCHDOG asm volatile ("wdr\n")
#define WATCHDOG_OFF    (0)
#define WATCHDOG_16MS   (_BV(WDE))
#define WATCHDOG_500MS  (_BV(WDP2) | _BV(WDP0) | _BV(WDE))
#define ram_bfr    ((uint8_t*)(RAMSTART))
#define GETLENGTH(len) len = ((length_t)read())<<8; len |= read()
#define GETADDRESS(addr) addr = (((address_t)(read())<<24)|((address_t)(read())<<16)|((address_t)(read())<<8)|(read()))<<1

#define MAGEBOOT_MAJVER 1
#define MAGEBOOT_MINVER 0
const unsigned int __attribute__((section(".version"))) mageboot_version = MAGEBOOT_MAJVER<<8 | MAGEBOOT_MINVER;

void init(void) __attribute__((naked)) __attribute__((section(".init3")));
int main(void) __attribute__ ((OS_main)) __attribute__ ((section (".init9")));

uint8_t __attribute__((noinline)) read(void);
void	__attribute__((noinline)) write(uint8_t data);
void	__attribute__((noinline)) validate();
void	__attribute__((noinline)) setWDT(uint8_t x);
void	__attribute__((noinline)) reset();

static inline void read_mem(address_t address, length_t length);
static inline void write_buffer(length_t length);
static inline void write_mem(uint8_t* bfr, address_t address, length_t length);

//The code below is from <avr/boot.h>

#define __BOOT_PAGE_ERASE         (_BV(SPMEN) | _BV(PGERS))
#define __BOOT_PAGE_WRITE         (_BV(SPMEN) | _BV(PGWRT))
#define __BOOT_PAGE_FILL          _BV(SPMEN)
#define __BOOT_RWW_ENABLE         (_BV(SPMEN) | _BV(RWWSRE))

#define read_pgm_mem(addr)     \
(__extension__({                    \
	uint32_t __addr32 = (uint32_t)(addr); \
	uint8_t __result;               \
	__asm__ __volatile__            \
	(                               \
	"out %2, %C1" "\n\t"        \
	"movw r30, %1" "\n\t"       \
	"elpm %0, Z+" "\n\t"        \
	: "=r" (__result)           \
	: "r" (__addr32),           \
	"I" (_SFR_IO_ADDR(RAMPZ)) \
	: "r30", "r31"              \
	);                              \
	__result;                       \
}))

#define page_erase(address)      \
(__extension__({                                 \
	__asm__ __volatile__                         \
	(                                            \
	"movw r30, %A3\n\t"                      \
	"sts  %1, %C3\n\t"                       \
	"sts %0, %2\n\t"                         \
	"spm\n\t"                                \
	:                                        \
	: "i" (_SFR_MEM_ADDR(SPMCSR)),        \
	"i" (_SFR_MEM_ADDR(RAMPZ)),            \
	"r" ((uint8_t)(__BOOT_PAGE_ERASE)),    \
	"r" ((uint32_t)(address))              \
	: "r30", "r31"                           \
	);                                           \
}))

#define page_fill(address, data) \
(__extension__({                                 \
	__asm__ __volatile__                         \
	(                                            \
	"movw  r0, %4\n\t"                       \
	"movw r30, %A3\n\t"                      \
	"sts %1, %C3\n\t"                        \
	"sts %0, %2\n\t"                         \
	"spm\n\t"                                \
	"clr  r1\n\t"                            \
	:                                        \
	: "i" (_SFR_MEM_ADDR(SPMCSR)),        \
	"i" (_SFR_MEM_ADDR(RAMPZ)),            \
	"r" ((uint8_t)(__BOOT_PAGE_FILL)),     \
	"r" ((uint32_t)(address)),             \
	"r" ((uint16_t)(data))                 \
	: "r0", "r30", "r31"                     \
	);                                           \
}))

#define page_write(address)      \
(__extension__({                                 \
	__asm__ __volatile__                         \
	(                                            \
	"movw r30, %A3\n\t"                      \
	"sts %1, %C3\n\t"                        \
	"sts %0, %2\n\t"                         \
	"spm\n\t"                                \
	:                                        \
	: "i" (_SFR_MEM_ADDR(SPMCSR)),        \
	"i" (_SFR_MEM_ADDR(RAMPZ)),            \
	"r" ((uint8_t)(__BOOT_PAGE_WRITE)),    \
	"r" ((uint32_t)(address))              \
	: "r30", "r31"                           \
	);                                           \
}))

#define rww_enable()                      \
(__extension__({                                 \
	__asm__ __volatile__                         \
	(                                            \
	"sts %0, %1\n\t"                         \
	"spm\n\t"                                \
	:                                        \
	: "i" (_SFR_MEM_ADDR(SPMCSR)),        \
	"r" ((uint8_t)(__BOOT_RWW_ENABLE))     \
	);                                           \
}))

#define busy_wait() while(SPMCSR & (uint8_t)_BV(SPMEN))

#endif