/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "../common/applet.h"
#include <board.h>
#include <board_lowlevel.h>
#include <board_memories.h>
#include <pio/pio.h>
#include <dbgu/dbgu.h>
//#include <utility/assert.h>
//#include <utility/trace.h>

#include <string.h>
//#include <stdio.h>

//------------------------------------------------------------------------------
//         External definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------

#define EXTRAM_ADDR AT91C_EBI_SDRAM
#define EXTRAM_SIZE BOARD_SDRAM_SIZE

/// External RAM is SDRAM
#define TYPE_SDRAM 0

//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------

/// Marks the end of program space.
extern unsigned int end;

//------------------------------------------------------------------------------
//         Local functions
//------------------------------------------------------------------------------

#define PATTERN1	0x55AA55AA
#define PATTERN2	0xAA55AA55
#define PATTERN3	0x00000000
#define PATTERN4	0xFFFFFFFF
#define PATTERN5	0xAAAAAAAA
#define PATTERN6	0x55555555

//------------------------------------------------------------------------------
//         Board reset defines
//------------------------------------------------------------------------------

#define ATMEL_BASE_RSTC         0xfffffd00    
#define AT91_RSTC_KEY           0xA5000000
  
#define AT91_RSTC_CR_PROCRST    0x00000001
#define AT91_RSTC_CR_PERRST     0x00000004
#define AT91_RSTC_CR_EXTRST     0x00000008
      
typedef struct at91_rstc {
  unsigned int     cr;     /* Reset Controller Control Register */
  unsigned int     sr;     /* Reset Controller Status Register */
  unsigned int     mr;     /* Reset Controller Mode Register */
} at91_rstc_t;

/*static unsigned char RAMTestPattern(unsigned int patt, unsigned int wsize)
{
    unsigned int i;
    unsigned int *ptr = (unsigned int *) EXTRAM_ADDR;

    printf("Starting memory address: 0x%X \n\r", ptr);
    while (1) {
    	printf("Writing %d bytes of data... \n\r", wsize); 
    	for (i = 0; i < wsize; ++i) {
        	ptr[i] = patt;
    	}
    }

    printf("------------------------\n\r");
    printf("Reading %d bytes of data... \n\r", wsize);
    for (i = 0; i < wsize; ++i) {
        printf("Memory address: 0x%X\t Write: 0x%X\t Read: 0x%X \n\r",&ptr[i],patt,ptr[i]);
    }
    
    return 0;
}*/


/*
 * ADDRESS test. 
 * 
 * Return codes:
 * 	- 0 if successfull
 * 	- 1 if unsuccessfull
 */ 

static unsigned char RAMTestAddress(unsigned int wsize)
{
    unsigned int i;
    unsigned char c;
    unsigned int *ptr = (unsigned int *) EXTRAM_ADDR;

    printf("Starting memory address: 0x%X \n\r", ptr);
    printf("Writing %d bytes of data... \n\r", wsize);
    for (i = 0; i < wsize; i++) {
        ptr[i] = i;
    }
    
    printf("------------------------\n\r");
    printf("Reading %d bytes of data... \n\r", wsize);
    for (i = 0; i < wsize; i++) {
	if (ptr[i] != i) {
	    printf("Memory address: 0x%X\t Write: 0x%X\t Read: 0x%X \n\r",&ptr[i],i,ptr[i]);
	    //return 1;
	}
    }
    return 0;
}

/*
 * DATA test. 
 * 
 * Return codes:
 * 	- 0 if successfull
 * 	- 1 if unsuccessfull
 */ 

static unsigned char RAMTestData(void)
{
    unsigned int i;
    volatile unsigned int *ptr = (volatile unsigned int *) EXTRAM_ADDR;

    printf("Starting memory address: 0x%X \n\r", ptr);
//    while (1) {
//    printf("------------------------\n\r");
    printf("Writing %d of data ...\n\r", EXTRAM_SIZE);
    for (i = 0; i < (EXTRAM_SIZE); ++i) {
        if (i & 1) {
            ptr[i] = PATTERN5;
            //printf("Memory address: %X\t Write: %X \n\r",&ptr[i],PATTERN5);
        }
        else {
            ptr[i] = PATTERN6;
            //printf("Memory address: %X\t Write: %X \n\r",&ptr[i],PATTERN6);
        }
    }

 //  printf("------------------------\n\r");
   printf("Reading %d of data ...\n\r", EXTRAM_SIZE);
   for (i = 0; i < (EXTRAM_SIZE); ++i) {
        if (i & 1) {
            if (ptr[i] != PATTERN5) {
	   // if (ptr[i] != (PATTERN5 | (1 << i))) {
            	printf("Memory address: 0x%X\t Write: 0x%X\t Read: 0x%X \n\r",&ptr[i],PATTERN5,ptr[i]);
       //     	return 1;
            }
        }
        else {
            if (ptr[i] != PATTERN6) {
          //  if (ptr[i] != (PATTERN6 | (1 << i))) {
            	printf("Memory address: 0x%X\t Write: 0x%X\t Read: 0x%X \n\r",&ptr[i],PATTERN6,ptr[i]);
        //        return 1;
            }
        }
    }
 //  }
    return 0;
}

//------------------------------------------------------------------------------
/// Applet code for initializing the external RAM.
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{

    // Configure the DBGU
    const Pin pinsDbgu[] = {PINS_DBGU};
    PIO_Configure(pinsDbgu, PIO_LISTSIZE(pinsDbgu));
    DBGU_Configure(DBGU_STANDARD, 115200, BOARD_MCK);

    // Initialize PMC
    LowLevelInit();

    // Enable User Reset
    AT91C_BASE_RSTC->RSTC_RMR |= AT91C_RSTC_URSTEN | (0xA5<<24);

    //printf("Initializing SDRAM Controller\n\r");
    //int dataBusWidth = 32;

    //BOARD_ConfigureSdram(dataBusWidth);

/*
    volatile int i = 0;
    volatile int j = 0;
    for ( i = 0; i < 2000; i++ ) {
    while (1) {
	    DBGU_PutChar(42);
	    for ( j = 0; j < 100000; j++) {}
	    DBGU_PutChar(10);
	    for ( j = 0; j < 100000; j++) {}
	    DBGU_PutChar(13);
	    for ( j = 0; j < 100000; j++) {}
	    printf("Iteracija: %d \n\r", i);
    }
*/
    printf("Initializing SDRAM Controller\n\r");

    volatile unsigned int i = 0;
    for ( i = 0; i < 1000000; i++) {}

    int dataBusWidth = 32;
    BOARD_ConfigureSdram(dataBusWidth);


#ifdef __TESTTYPE_SERIAL
    printf("Test type: SERIAL\n\r");
#endif
  
    
#ifdef __TESTTYPE_ADDRESS
    printf("\n\rSDRAM memory test. (c) 2010/2011 Evo-Teh\n\r");
    printf("Test type: ADDRESS\n\r");
    unsigned int addr_line = 0;
    unsigned int addr_space = 1;
    unsigned char error_code = 0;
    
    while (addr_space <= (EXTRAM_SIZE / 4 ) ) {
      error_code = RAMTestAddress(addr_space);
//      if(error_code != 0){
//	// Bad memory detected? Stop testing and report error code...
//	printf("Result: FAIL.\n\r");
//	break;
//      }
      addr_space = addr_space * 2;
      addr_line +=1;
    }
//    if (error_code == 0)
//	printf("Result: SUCCESS.\n\r");
    
    printf("DONE.\n\r");
#endif

#ifdef __TESTTYPE_DATA
    printf("\n\rSDRAM memory test. (c) 2010/2011 Evo-Teh\n\r");
    
    while (1) {
        printf("Test type: DATA\n\r");
        RAMTestData();
    }
    //if ( RAMTestData() == 0)    
    //  printf("Result: SUCCESS.\n\r");
    //else
    //  printf("Result: FAIL.\n\r");

    printf("DONE.\n\r");
#endif
//        
//  /* Reset the cpu by telling the reset controller to do so */
//    at91_rstc_t *rstc = (at91_rstc_t *) ATMEL_BASE_RSTC;
//    
//    rstc->cr = AT91_RSTC_KEY
// 	  | AT91_RSTC_CR_PROCRST  /* Processor Reset */
// 	  | AT91_RSTC_CR_EXTRST   /* External Reset (assert nRST pin) */
// 	  | AT91_RSTC_CR_PERRST;   /* Peripheral Reset */
//    
//   printf("\n\rNEVER REACHED THIS!!");
//    while (1);
    return(0);
}

