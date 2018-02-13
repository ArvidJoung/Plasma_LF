#include "Global.h"

MEMORY
{
PAGE 0:    /* Program Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE1 for data allocation */
   RAML0       : origin = 0x008000, length = 0x001000     /* on-chip RAM block L0 */
   OTP         : origin = 0x3D7800, length = 0x000400     /* on-chip OTP */
   FLASHD      : origin = 0x3F4000, length = 0x001500     /* on-chip FLASH */
   FLASHC      : origin = 0x3F5500, length = 0x000500     /* on-chip FLASH */
   FLASHA      : origin = 0x3F7000, length = 0x000F80     /* on-chip FLASH */
   CSM_RSVD    : origin = 0x3F7F80, length = 0x000076     /* Part of FLASHA.  Program with all 0x0000 when CSM is in use. */

#if FLASH_LOAD == 0
   BEGIN       : origin = 0x000000, length = 0x000002     /* Part of RAMM0.   Used for "boot to RAM" bootloader mode. */
#else
   BEGIN       : origin = 0x3F7FF6, length = 0x000002     /* Part of RAMM0.   Used for "boot to RAM" bootloader mode. */
#endif

   CSM_PWL     : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASHA.  CSM password locations in FLASHA */
   
   ROM         : origin = 0x3FF000, length = 0x000FC0     /* Boot ROM */
   RESET       : origin = 0x3FFFC0, length = 0x000002     /* part of boot ROM  */
   VECTORS     : origin = 0x3FFFC2, length = 0x00003E     /* part of boot ROM  */

PAGE 1 :   /* Data Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE0 for program allocation */
           /* Registers remain on PAGE1                                                  */

   RAMM0       : origin = 0x000002, length = 0x0003FE     /* on-chip RAM block M0 */
   //BOOT_RSVD   : origin = 0x000400, length = 0x000080     /* Part of M1, BOOT rom will use this for stack */
   RAMM1       : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
   FLASHB      : origin = 0x3F6000, length = 0x001000     /* on-chip FLASH */
}

/* Allocate sections to memory blocks.
   Note:
         codestart user defined section in DSP28_CodeStartBranch.asm used to redirect code
                   execution when booting to flash
         ramfuncs  user defined section to store functions that will be copied from Flash into RAM
*/
 
SECTIONS
{

   /* Allocate program areas: */
   codestart           : > BEGIN       PAGE = 0

#if FLASH_LOAD == 0
   ramfuncs            : > RAML0       PAGE = 0
   .cinit              : > RAML0       PAGE = 0
   .pinit              : > RAML0,      PAGE = 0
   .text               : > RAML0       PAGE = 0
#else
   //ramfuncs            : > FLASHA       PAGE = 0
   ramfuncs            : LOAD = FLASHA,      PAGE = 0
   						 RUN = RAML0,		 PAGE = 0
   						 LOAD_START(_RamfuncsLoadStart),
   						 LOAD_END(_RamfuncsLoadEnd),
   						 RUN_START(_RamfuncsRunStart)
   .cinit              : > FLASHA       PAGE = 0
   .pinit              : > FLASHA,      PAGE = 0
   .text               : > FLASHD       PAGE = 0
#endif

   csmpasswds          : > CSM_PWL     PAGE = 0
   csm_rsvd            : > CSM_RSVD    PAGE = 0
   
   /* Allocate uninitalized data sections: */
   .stack              : > RAMM0       PAGE = 1
   .ebss               : > RAMM1       PAGE = 1
   .esysmem            : > RAMM1       PAGE = 1

   /* Initalized sections go in Flash */
   /* For SDFlash to program these, they must be allocated to page 0 */
#if FLASH_LOAD == 0
   .econst             : > RAML0       PAGE = 0
   .switch             : > RAML0       PAGE = 0
#else
   .econst             : > FLASHA       PAGE = 0
   .switch             : > FLASHA       PAGE = 0
#endif

   /* .reset is a standard section used by the compiler.  It contains the */
   /* the address of the start of _c_int00 for C Code.   /*
   /* When using the boot ROM this section and the CPU vector */
   /* table is not needed.  Thus the default type is set here to  */
   /* DSECT  */
   .reset              : > RESET,      PAGE = 0, TYPE = DSECT
   vectors             : > VECTORS     PAGE = 0, TYPE = DSECT

}
