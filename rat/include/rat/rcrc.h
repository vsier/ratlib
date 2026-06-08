// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RCRC_H_
#define RAT_RCRC_H_

#include <rat/common.h>


// Predefined CRC algorithm parameters follow the RevEng CRC Catalogue:
// https://reveng.sourceforge.io/crc-catalogue/all.htm
typedef enum
{
    // Parity bit
    RCRC_1_PARITY_BIT = 0,
    // CRC-3/GSM
    RCRC_3_GSM,
    // CRC-3/ROHC
    RCRC_3_ROHC,
    // CRC-4/G-704, CRC-4/ITU
    RCRC_4_G_704,
    // CRC-4/INTERLAKEN
    RCRC_4_INTERLAKEN,
    // CRC-5/EPC-C1G2, CRC-5/EPC
    RCRC_5_EPC_C1G2,
    // CRC-5/G-704, CRC-5/ITU
    RCRC_5_G_704,
    // CRC-5/USB
    RCRC_5_USB,
    // CRC-6/CDMA2000-A
    RCRC_6_CDMA2000_A,
    // CRC-6/CDMA2000-B
    RCRC_6_CDMA2000_B,
    // CRC-6/DARC
    RCRC_6_DARC,
    // CRC-6/G-704, CRC-6/ITU
    RCRC_6_G_704,
    // CRC-6/GSM
    RCRC_6_GSM,
    // CRC-7/MMC, CRC-7
    RCRC_7_MMC,
    // CRC-7/ROHC
    RCRC_7_ROHC,
    // CRC-7/UMTS
    RCRC_7_UMTS,
    // CRC-8/AUTOSAR
    RCRC_8_AUTOSAR,
    // CRC-8/BLUETOOTH
    RCRC_8_BLUETOOTH,
    // CRC-8/CDMA2000
    RCRC_8_CDMA2000,
    // CRC-8/DARC
    RCRC_8_DARC,
    // CRC-8/DVB-S2
    RCRC_8_DVB_S2,
    // CRC-8/GSM-A
    RCRC_8_GSM_A,
    // CRC-8/GSM-B
    RCRC_8_GSM_B,
    // CRC-8/HITAG
    RCRC_8_HITAG,
    // CRC-8/I-432-1, CRC-8/ITU
    RCRC_8_I_432_1,
    // CRC-8/I-CODE
    RCRC_8_I_CODE,
    // CRC-8/LTE
    RCRC_8_LTE,
    // CRC-8/MAXIM-DOW, CRC-8/MAXIM, DOW-CRC
    RCRC_8_MAXIM_DOW,
    // CRC-8/MIFARE-MAD
    RCRC_8_MIFARE_MAD,
    // CRC-8/NRSC-5
    RCRC_8_NRSC_5,
    // CRC-8/OPENSAFETY
    RCRC_8_OPENSAFETY,
    // CRC-8/ROHC
    RCRC_8_ROHC,
    // CRC-8/SAE-J1850
    RCRC_8_SAE_J1850,
    // CRC-8/SMBUS, CRC-8
    RCRC_8_SMBUS,
    // CRC-8/TECH-3250, CRC-8/AES, CRC-8/EBU
    RCRC_8_TECH_3250,
    // CRC-8/WCDMA
    RCRC_8_WCDMA,
    // CRC-10/ATM, CRC-10, CRC-10/I-610
    RCRC_10_ATM,
    // CRC-10/CDMA2000
    RCRC_10_CDMA2000,
    // CRC-10/GSM
    RCRC_10_GSM,
    // CRC-11/FLEXRAY, CRC-11
    RCRC_11_FLEXRAY,
    // CRC-11/UMTS
    RCRC_11_UMTS,
    // CRC-12/CDMA2000
    RCRC_12_CDMA2000,
    // CRC-12/DECT, X-CRC-12
    RCRC_12_DECT,
    // CRC-12/GSM
    RCRC_12_GSM,
    // CRC-12/UMTS, CRC-12/3GPP
    RCRC_12_UMTS,
    // CRC-13/BBC
    RCRC_13_BBC,
    // CRC-14/DARC
    RCRC_14_DARC,
    // CRC-14/GSM
    RCRC_14_GSM,
    // CRC-15/CAN, CRC-15
    RCRC_15_CAN,
    // CRC-15/MPT1327
    RCRC_15_MPT1327,
    // CRC-16/ARC, ARC, CRC-16, CRC-16/LHA, CRC-IBM
    RCRC_16_ARC,
    // CRC-16/CDMA2000
    RCRC_16_CDMA2000,
    // CRC-16/CMS
    RCRC_16_CMS,
    // CRC-16/DDS-110
    RCRC_16_DDS_110,
    // CRC-16/DECT-R, R-CRC-16
    RCRC_16_DECT_R,
    // CRC-16/DECT-X, X-CRC-16
    RCRC_16_DECT_X,
    // CRC-16/DNP
    RCRC_16_DNP,
    // CRC-16/EN-13757
    RCRC_16_EN_13757,
    // CRC-16/GENIBUS, CRC-16/DARC, CRC-16/EPC, CRC-16/EPC-C1G2, CRC-16/I-CODE
    RCRC_16_GENIBUS,
    // CRC-16/GSM
    RCRC_16_GSM,
    // CRC-16/IBM-3740, CRC-16/AUTOSAR, CRC-16/CCITT-FALSE
    RCRC_16_IBM_3740,
    // CRC-16/IBM-SDLC, CRC-16/ISO-HDLC, CRC-16/ISO-IEC-14443-3-B, CRC-16/X-25, CRC-B, X-25
    RCRC_16_IBM_SDLC,
    // CRC-16/ISO-IEC-14443-3-A, CRC-A
    RCRC_16_ISO_IEC_14443_3_A,
    // CRC-16/KERMIT, CRC-16/BLUETOOTH, CRC-16/CCITT, CRC-16/CCITT-TRUE, CRC-16/V-41-LSB, CRC-CCITT, KERMIT
    RCRC_16_KERMIT,
    // CRC-16/LJ1200
    RCRC_16_LJ1200,
    // CRC-16/M17
    RCRC_16_M17,
    // CRC-16/MAXIM-DOW, CRC-16/MAXIM
    RCRC_16_MAXIM_DOW,
    // CRC-16/MCRF4XX
    RCRC_16_MCRF4XX,
    // CRC-16/MODBUS, MODBUS
    RCRC_16_MODBUS,
    // CRC-16/NRSC-5
    RCRC_16_NRSC_5,
    // CRC-16/OPENSAFETY-A
    RCRC_16_OPENSAFETY_A,
    // CRC-16/OPENSAFETY-B
    RCRC_16_OPENSAFETY_B,
    // CRC-16/PROFIBUS, CRC-16/IEC-61158-2
    RCRC_16_PROFIBUS,
    // CRC-16/RIELLO
    RCRC_16_RIELLO,
    // CRC-16/SPI-FUJITSU, CRC-16/AUG-CCITT
    RCRC_16_SPI_FUJITSU,
    // CRC-16/T10-DIF
    RCRC_16_T10_DIF,
    // CRC-16/TELEDISK
    RCRC_16_TELEDISK,
    // CRC-16/TMS37157
    RCRC_16_TMS37157,
    // CRC-16/UMTS, CRC-16/BUYPASS, CRC-16/VERIFONE
    RCRC_16_UMTS,
    // CRC-16/USB
    RCRC_16_USB,
    // CRC-16/XMODEM, CRC-16/ACORN, CRC-16/LTE, CRC-16/V-41-MSB, XMODEM, ZMODEM
    RCRC_16_XMODEM,
    // CRC-17/CAN-FD
    RCRC_17_CAN_FD,
    // CRC-21/CAN-FD
    RCRC_21_CAN_FD,
    // CRC-24/BLE
    RCRC_24_BLE,
    // CRC-24/FLEXRAY-A
    RCRC_24_FLEXRAY_A,
    // CRC-24/FLEXRAY-B
    RCRC_24_FLEXRAY_B,
    // CRC-24/INTERLAKEN
    RCRC_24_INTERLAKEN,
    // CRC-24/LTE-A
    RCRC_24_LTE_A,
    // CRC-24/LTE-B
    RCRC_24_LTE_B,
    // CRC-24/OPENPGP, CRC-24
    RCRC_24_OPENPGP,
    // CRC-24/OS-9
    RCRC_24_OS_9,
    // CRC-30/CDMA
    RCRC_30_CDMA,
    // CRC-31/PHILIPS
    RCRC_31_PHILIPS,
    // CRC-32/AIXM, CRC-32Q
    RCRC_32_AIXM,
    // CRC-32/AUTOSAR
    RCRC_32_AUTOSAR,
    // CRC-32/BASE91-D, CRC-32D
    RCRC_32_BASE91_D,
    // CRC-32/BZIP2, CRC-32/AAL5, CRC-32/DECT-B, B-CRC-32
    RCRC_32_BZIP2,
    // CRC-32/CD-ROM-EDC
    RCRC_32_CD_ROM_EDC,
    // CRC-32/CKSUM, CKSUM, CRC-32/POSIX
    RCRC_32_CKSUM,
    // CRC-32/ISCSI, CRC-32/BASE91-C, CRC-32/CASTAGNOLI, CRC-32/INTERLAKEN, CRC-32C
    RCRC_32_ISCSI,
    // CRC-32/ISO-HDLC, CRC-32, CRC-32/ADCCP, CRC-32/V-42, CRC-32/XZ, PKZIP
    RCRC_32_ISO_HDLC,
    // CRC-32/JAMCRC, JAMCRC
    RCRC_32_JAMCRC,
    // CRC-32/MEF
    RCRC_32_MEF,
    // CRC-32/MPEG-2
    RCRC_32_MPEG_2,
    // CRC-32/XFER, XFER
    RCRC_32_XFER,
    // CRC-32/SATA
    RCRC_32_SATA,
    // CRC-40/GSM
    RCRC_40_GSM,
    // CRC-64/ECMA-182, CRC-64
    RCRC_64_ECMA_182,
    // CRC-64/GO-ISO
    RCRC_64_GO_ISO,
    // CRC-64/MS
    RCRC_64_MS,
    // CRC-64/REDIS
    RCRC_64_REDIS,
    // CRC-64/WE
    RCRC_64_WE,
    // CRC-64/XZ, CRC-64/GO-ECMA
    RCRC_64_XZ,
} rcrc_id;


typedef struct
{
    uintmax_t poly;
    uintmax_t init;
    uintmax_t value;
    uintmax_t xorOut;
    unsigned char bitWidth;
    unsigned char initOffset;
    bool refIn;
    bool refOut;
} rcrc;

// Initializes a custom CRC, adds buf[0..bufSize), and returns the result.
uintmax_t rcrc_calc(unsigned char bitWidth, uintmax_t poly, uintmax_t init, bool refIn, bool refOut, uintmax_t xorOut, const void *buf, size_t bufSize);
// Initializes a predefined CRC, adds buf[0..bufSize), and returns the result.
uintmax_t rcrc_calcFromId(rcrc_id id, const void *buf, size_t bufSize);

// Initializes a custom CRC. bitWidth must be in 1..sizeof(uintmax_t) bits, and
// poly/init/xorOut must fit in that width.
void rcrc_init(rcrc *p, unsigned char bitWidth, uintmax_t poly, uintmax_t init, bool refIn, bool refOut, uintmax_t xorOut);
// Initializes a predefined CRC selected by id.
void rcrc_initFromId(rcrc *p, rcrc_id id);
// Adds one byte to the CRC state.
void rcrc_addByte(rcrc *p, unsigned char b);
// Adds buf[0..bufSize) to the CRC state.
void rcrc_add(rcrc *p, const void *buf, size_t bufSize);
// Returns the current finalized CRC result without modifying p.
uintmax_t rcrc_result(rcrc *p);

#endif
