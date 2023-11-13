/* 
 * File:   avr_database.h
 * Author: Bartosz Derleta <bartosz@derleta.com>
 */

#ifndef AVR_DATABASE_H
#define	AVR_DATABASE_H

#define AVR_FLAG_SUPPORTED (1 << 0)
#define AVR_FLAG_FUSE_EXTENDED (1 << 1)

typedef const struct {
    const uint32_t signature;
    const char* const name;
    const uint8_t flags;
    const uint8_t fuse_low_factory;
    const uint8_t fuse_high_factory;
    const uint8_t fuse_extended_factory;
    const uint8_t lock_factory;
    const char* const* fuse_low_map;
    const char* const* fuse_high_map;
    const char* const* fuse_extended_map;
    const char* const* lock_map;
} avr_record;

/* ATmega 48A/48PA has no separate Boot Loader section */
static const char* const avr_base_lock[] = {
    "LB1", "LB2", NULL, NULL, NULL, NULL, NULL, NULL
};
static const char* const avr_std_lock[] = {
    "LB1", "LB2", "BLB01", "BLB02", "BLB11", "BLB12", NULL, NULL
};

static const char* const avr_mega8_fuse_high[] = {
    "BOOTRST", "BOOTSZ0", "BOOTSZ1", "EESAVE", "CKOPT", "SPIEN", "WDTON", "RSTDISBL",
};
static const char* const avr_mega8_fuse_low[] = {
    "CKSEL0", "CKSEL1", "CKSEL2", "CKSEL3", "SUT0", "SUT1", "BODEN", "BODLEVEL",
};

static const char* const avr_mega32_fuse_high[] = {
    "BOOTRST", "BOOTSZ0", "BOOTSZ1", "EESAVE", "CKOPT", "SPIEN", "JTAGEN", "OCDEN",
};
#define avr_mega32_fuse_low avr_mega8_fuse_low

#define avr_mega16_fuse_high avr_mega32_fuse_high
#define avr_mega16_fuse_low avr_mega8_fuse_low

static const char* const avr_mega48a_fuse_extended[] = {
    "SELFPRGEN", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};
static const char* const avr_mega88a_fuse_extended[] = {
    "BOOTRST", "BOOTSZ0", "BOOTSZ1", NULL, NULL, NULL, NULL, NULL,
};
#define avr_mega88a_fuse_extended avr_mega88a_fuse_extended
#define avr_mega168a_fuse_extended avr_mega88a_fuse_extended
static const char* const avr_mega328a_fuse_extended[] = {
    "BODLEVEL0", "BODLEVEL1", "BODLEVEL2", NULL, NULL, NULL, NULL, NULL,
};
static const char* const avr_mega48a_fuse_high[] = {
    "BODLEVEL0", "BODLEVEL1", "BODLEVEL2", "EESAVE", "WDTON", "SPIEN", "DWEN", "RSTDISBL"
};
#define avr_mega88a_fuse_high avr_mega48a_fuse_high
#define avr_mega168a_fuse_high avr_mega48a_fuse_high
static const char* const avr_mega328a_fuse_high[] = {
    "BOOTRST", "BOOTSZ0", "BOOTSZ1", "EESAVE", "WDTON", "SPIEN", "DWEN", "RSTDISBL",
};
static const char* const avr_mega48a_fuse_low[] = {
    "CKSEL0", "CKSEL1", "CKSEL2", "CKSEL3", "SUT0", "SUT1", "CKOUT", "CKDIV8",
};
#define avr_mega88a_fuse_low avr_mega48a_fuse_low
#define avr_mega168a_fuse_low avr_mega48a_fuse_low
#define avr_mega328a_fuse_low avr_mega48a_fuse_low

#define avr_mega88_fuse_extended avr_mega88a_fuse_extended
#define avr_mega168_fuse_extended avr_mega88a_fuse_extended
#define avr_mega88_fuse_high avr_mega48a_fuse_high
#define avr_mega168_fuse_high avr_mega48a_fuse_high
#define avr_mega88_fuse_low avr_mega48a_fuse_low
#define avr_mega168_fuse_low avr_mega48a_fuse_low

#define avr_megaX4_fuse_extended avr_mega328a_fuse_extended
static const char* const avr_megaX4_fuse_high[] = {
    "BOOTRST", "BOOTSZ0", "BOOTSZ1", "EESAVE", "WDTON", "SPIEN", "JTAGEN", "OCDEN",
};
#define avr_megaX4_fuse_low avr_mega48a_fuse_low
#define avr_megaX4_definition \
    AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, \
    0x62, 0x99, 0xFF, 0xFF, \
    avr_megaX4_fuse_low, avr_megaX4_fuse_high, avr_megaX4_fuse_extended, avr_std_lock

static const avr_record database[] = {
    /**
     * ATmega8/8A
     * https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2486-8-bit-AVR-microcontroller-ATmega8_L_datasheet.pdf
     * https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega8A-Data-Sheet-DS40001974B.pdf
     * 
     */
    {
        0x1E9307, "ATmega8/ATmega8A", AVR_FLAG_SUPPORTED,
        0xE1, 0xD9, 0xFF, 0xFF, 
        avr_mega8_fuse_low, avr_mega8_fuse_high, NULL, avr_std_lock,
    },
    
    /**
     * ATmega16/16A
     * https://ww1.microchip.com/downloads/en/devicedoc/doc2466.pdf
     * https://ww1.microchip.com/downloads/en/devicedoc/atmel-8154-8-bit-avr-atmega16a_datasheet.pdf
     */
    {
        0x1E9403, "ATmega16/ATmega16A", AVR_FLAG_SUPPORTED,
        0xE1, 0x99, 0xFF, 0xFF,
        avr_mega16_fuse_low, avr_mega16_fuse_high, NULL, avr_std_lock
    },
    
    /**
     * ATmega32/32A
     * https://ww1.microchip.com/downloads/en/DeviceDoc/doc2503.pdf
     * https://ww1.microchip.com/downloads/en/DeviceDoc/Atmega32A-DataSheet-Complete-DS40002072A.pdf
     */
    {
        0x1E9502, "ATmega32/ATmega32A", AVR_FLAG_SUPPORTED,
        0xE1, 0x99, 0xFF, 0xFF,
        avr_mega32_fuse_low, avr_mega32_fuse_high, NULL, avr_std_lock
    },
    
    /*
     * ATmega88/ATmega168
     * https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-9365-Automotive-Microcontrollers-ATmega88-ATmega168_Datasheet.pdf
     */
    {
        0x1E930A, "ATmega88", 0x00, 
        0x62, 0xDF, 0xF9, 0xFF,
        avr_mega88_fuse_low, avr_mega88_fuse_high, avr_mega88_fuse_extended, avr_std_lock
    },
    {
        0x1E9406, "ATmega168", 0x00, 
        0x62, 0xDF, 0xF9, 0xFF,
        avr_mega168_fuse_low, avr_mega168_fuse_high, avr_mega168_fuse_extended, avr_std_lock    
    },
    
    /**
     * ATmega48A/PA/88A/PA/168A/PA/328/P
     * https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf
     */
    {
        0x1E9205, "ATmega48A", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xDF, 0xFF, 0xFF,
        avr_mega48a_fuse_low, avr_mega48a_fuse_high, avr_mega48a_fuse_extended, avr_base_lock        
    },
    {
        0x1E920A, "ATmega48PA", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xDF, 0xFF, 0xFF,
        avr_mega48a_fuse_low, avr_mega48a_fuse_high, avr_mega48a_fuse_extended, avr_base_lock   
    },
    {
        0x1E930A, "ATmega88A", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xDF, 0xF9, 0xFF, 
        avr_mega88a_fuse_low, avr_mega88a_fuse_high, avr_mega88a_fuse_extended, avr_std_lock
    },
    {
        0x1E930F, "ATmega88PA", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xDF, 0xF9, 0xFF, 
        avr_mega88a_fuse_low, avr_mega88a_fuse_high, avr_mega88a_fuse_extended, avr_std_lock
    },
    {
        0x1E9406, "ATmega168A", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xDF, 0xF9, 0xFF, 
        avr_mega168a_fuse_low, avr_mega168a_fuse_high, avr_mega168a_fuse_extended, avr_std_lock
    },
    {
        0x1E940B, "ATmega168PA", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xDF, 0xF9, 0xFF, 
        avr_mega168a_fuse_low, avr_mega168a_fuse_high, avr_mega168a_fuse_extended, avr_std_lock
    },
    {
        0x1E9514, "ATmega328", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xD9, 0xFF, 0xFF,
        avr_mega328a_fuse_low, avr_mega328a_fuse_high, avr_mega328a_fuse_extended, avr_std_lock
    },
    {
        0x1E950F, "ATmega328P", AVR_FLAG_SUPPORTED | AVR_FLAG_FUSE_EXTENDED, 
        0x62, 0xD9, 0xFF, 0xFF,
        avr_mega328a_fuse_low, avr_mega328a_fuse_high, avr_mega328a_fuse_extended, avr_std_lock
    },
       
    /**
     *  ATmega164A/164PA/324A/324PA/644A/644PA/1284/1284P 
     *  ATmega644
     *  ATmega164P/V/324P/V/644P/V
     *  https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/DataSheets/ATmega164A_PA-324A_PA-644A_PA-1284_P_Data-Sheet-40002070B.pdf
     *  https://ww1.microchip.com/downloads/en/DeviceDoc/doc2593.pdf
     *  https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega164P-324P-644P-Data-Sheet-40002071A.pdf
     *  
     */
    {0x1E940F, "ATmega164A", avr_megaX4_definition},
    {0x1E940A, "ATmega164P/ATmega164PA", avr_megaX4_definition},
    {0x1E9515, "ATmega324A", avr_megaX4_definition},
    {0x1E9508, "ATmega324P", avr_megaX4_definition},
    {0x1E9511, "ATmega324PA", avr_megaX4_definition},
    {0x1E9609, "ATmega644/ATmega644A", avr_megaX4_definition},
    {0x1E960A, "ATmega644P/ATmega644PA", avr_megaX4_definition},
    {0x1E9706, "ATmega1284", avr_megaX4_definition},
    {0x1E9705, "ATmega1284P", avr_megaX4_definition},
};

#endif	/* AVR_DATABASE_H */

