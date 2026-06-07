// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/rcrc.h>
#include <rat/rbit.h>
#include <rat/extra.h>
#include <assert.h>


static uintmax_t reflect(uintmax_t value, unsigned char bits)
{
    assert(bits <= RBIT_BITSIZE(sizeof(uintmax_t)));

    uintmax_t result = 0;

    for (unsigned char i = 0; i < bits; i++)
    {
        result |= RBIT_FLAG(i, RBIT_FLAG_GET(value, bits - 1 - i));
    }

    return result;
}

static uintmax_t mask(unsigned char bits)
{
    assert(bits <= RBIT_BITSIZE(sizeof(uintmax_t)));
    return bits == RBIT_BITSIZE(sizeof(uintmax_t)) ? RBIT_MASK_FULL : RBIT_MASK(0, bits);
}


typedef struct
{
    uintmax_t poly;
    uintmax_t init;
    uintmax_t xorOut;
    unsigned char bitWidth;
    bool refIn;
    bool refOut;
} algo;

static const algo algos[] =
{
    [RCRC_1_PARITY_BIT] = { 0x01, 0x00, 0x00, 1, false, false },
    [RCRC_3_GSM] = { 0x03, 0x00, 0x07, 3, false, false },
    [RCRC_3_ROHC] = { 0x03, 0x07, 0x00, 3, true, true },
    [RCRC_4_G_704] = { 0x03, 0x00, 0x00, 4, true, true },
    [RCRC_4_INTERLAKEN] = { 0x03, 0x0F, 0x0F, 4, false, false },
    [RCRC_5_EPC_C1G2] = { 0x09, 0x09, 0x00, 5, false, false },
    [RCRC_5_G_704] = { 0x15, 0x00, 0x00, 5, true, true },
    [RCRC_5_USB] = { 0x05, 0x1F, 0x1F, 5, true, true },
    [RCRC_6_CDMA2000_A] = { 0x27, 0x3F, 0x00, 6, false, false },
    [RCRC_6_CDMA2000_B] = { 0x07, 0x3F, 0x00, 6, false, false },
    [RCRC_6_DARC] = { 0x19, 0x00, 0x00, 6, true, true },
    [RCRC_6_G_704] = { 0x03, 0x00, 0x00, 6, true, true },
    [RCRC_6_GSM] = { 0x2F, 0x00, 0x3F, 6, false, false },
    [RCRC_7_MMC] = { 0x09, 0x00, 0x00, 7, false, false },
    [RCRC_7_ROHC] = { 0x4F, 0x7F, 0x00, 7, true, true },
    [RCRC_7_UMTS] = { 0x45, 0x00, 0x00, 7, false, false },
    [RCRC_8_AUTOSAR] = { 0x2F, 0xFF, 0xFF, 8, false, false },
    [RCRC_8_BLUETOOTH] = { 0xA7, 0x00, 0x00, 8, true, true },
    [RCRC_8_CDMA2000] = { 0x9B, 0xFF, 0x00, 8, false, false },
    [RCRC_8_DARC] = { 0x39, 0x00, 0x00, 8, true, true },
    [RCRC_8_DVB_S2] = { 0xD5, 0x00, 0x00, 8, false, false },
    [RCRC_8_GSM_A] = { 0x1D, 0x00, 0x00, 8, false, false },
    [RCRC_8_GSM_B] = { 0x49, 0x00, 0xFF, 8, false, false },
    [RCRC_8_HITAG] = { 0x1D, 0xFF, 0x00, 8, false, false },
    [RCRC_8_I_432_1] = { 0x07, 0x00, 0x55, 8, false, false },
    [RCRC_8_I_CODE] = { 0x1D, 0xFD, 0x00, 8, false, false },
    [RCRC_8_LTE] = { 0x9B, 0x00, 0x00, 8, false, false },
    [RCRC_8_MAXIM_DOW] = { 0x31, 0x00, 0x00, 8, true, true },
    [RCRC_8_MIFARE_MAD] = { 0x1D, 0xC7, 0x00, 8, false, false },
    [RCRC_8_NRSC_5] = { 0x31, 0xFF, 0x00, 8, false, false },
    [RCRC_8_OPENSAFETY] = { 0x2F, 0x00, 0x00, 8, false, false },
    [RCRC_8_ROHC] = { 0x07, 0xFF, 0x00, 8, true, true },
    [RCRC_8_SAE_J1850] = { 0x1D, 0xFF, 0xFF, 8, false, false },
    [RCRC_8_SMBUS] = { 0x07, 0x00, 0x00, 8, false, false },
    [RCRC_8_TECH_3250] = { 0x1D, 0xFF, 0x00, 8, true, true },
    [RCRC_8_WCDMA] = { 0x9B, 0x00, 0x00, 8, true, true },
    [RCRC_10_ATM] = { 0x0233, 0x0000, 0x0000, 10, false, false },
    [RCRC_10_CDMA2000] = { 0x03D9, 0x03FF, 0x0000, 10, false, false },
    [RCRC_10_GSM] = { 0x0175, 0x0000, 0x03FF, 10, false, false },
    [RCRC_11_FLEXRAY] = { 0x0385, 0x001A, 0x0000, 11, false, false },
    [RCRC_11_UMTS] = { 0x0307, 0x0000, 0x0000, 11, false, false },
    [RCRC_12_CDMA2000] = { 0x0F13, 0x0FFF, 0x0000, 12, false, false },
    [RCRC_12_DECT] = { 0x080F, 0x0000, 0x0000, 12, false, false },
    [RCRC_12_GSM] = { 0x0D31, 0x0000, 0x0FFF, 12, false, false },
    [RCRC_12_UMTS] = { 0x080F, 0x0000, 0x0000, 12, false, true },
    [RCRC_13_BBC] = { 0x1CF5, 0x0000, 0x0000, 13, false, false },
    [RCRC_14_DARC] = { 0x0805, 0x0000, 0x0000, 14, true, true },
    [RCRC_14_GSM] = { 0x202D, 0x0000, 0x3FFF, 14, false, false },
    [RCRC_15_CAN] = { 0x4599, 0x0000, 0x0000, 15, false, false },
    [RCRC_15_MPT1327] = { 0x6815, 0x0000, 0x0001, 15, false, false },
    [RCRC_16_ARC] = { 0x8005, 0x0000, 0x0000, 16, true, true },
    [RCRC_16_CDMA2000] = { 0xC867, 0xFFFF, 0x0000, 16, false, false },
    [RCRC_16_CMS] = { 0x8005, 0xFFFF, 0x0000, 16, false, false },
    [RCRC_16_DDS_110] = { 0x8005, 0x800D, 0x0000, 16, false, false },
    [RCRC_16_DECT_R] = { 0x0589, 0x0000, 0x0001, 16, false, false },
    [RCRC_16_DECT_X] = { 0x0589, 0x0000, 0x0000, 16, false, false },
    [RCRC_16_DNP] = { 0x3D65, 0x0000, 0xFFFF, 16, true, true },
    [RCRC_16_EN_13757] = { 0x3D65, 0x0000, 0xFFFF, 16, false, false },
    [RCRC_16_GENIBUS] = { 0x1021, 0xFFFF, 0xFFFF, 16, false, false },
    [RCRC_16_GSM] = { 0x1021, 0x0000, 0xFFFF, 16, false, false },
    [RCRC_16_IBM_3740] = { 0x1021, 0xFFFF, 0x0000, 16, false, false },
    [RCRC_16_IBM_SDLC] = { 0x1021, 0xFFFF, 0xFFFF, 16, true, true },
    [RCRC_16_ISO_IEC_14443_3_A] = { 0x1021, 0xC6C6, 0x0000, 16, true, true },
    [RCRC_16_KERMIT] = { 0x1021, 0x0000, 0x0000, 16, true, true },
    [RCRC_16_LJ1200] = { 0x6F63, 0x0000, 0x0000, 16, false, false },
    [RCRC_16_M17] = { 0x5935, 0xFFFF, 0x0000, 16, false, false },
    [RCRC_16_MAXIM_DOW] = { 0x8005, 0x0000, 0xFFFF, 16, true, true },
    [RCRC_16_MCRF4XX] = { 0x1021, 0xFFFF, 0x0000, 16, true, true },
    [RCRC_16_MODBUS] = { 0x8005, 0xFFFF, 0x0000, 16, true, true },
    [RCRC_16_NRSC_5] = { 0x080B, 0xFFFF, 0x0000, 16, true, true },
    [RCRC_16_OPENSAFETY_A] = { 0x5935, 0x0000, 0x0000, 16, false, false },
    [RCRC_16_OPENSAFETY_B] = { 0x755B, 0x0000, 0x0000, 16, false, false },
    [RCRC_16_PROFIBUS] = { 0x1DCF, 0xFFFF, 0xFFFF, 16, false, false },
    [RCRC_16_RIELLO] = { 0x1021, 0xB2AA, 0x0000, 16, true, true },
    [RCRC_16_SPI_FUJITSU] = { 0x1021, 0x1D0F, 0x0000, 16, false, false },
    [RCRC_16_T10_DIF] = { 0x8BB7, 0x0000, 0x0000, 16, false, false },
    [RCRC_16_TELEDISK] = { 0xA097, 0x0000, 0x0000, 16, false, false },
    [RCRC_16_TMS37157] = { 0x1021, 0x89EC, 0x0000, 16, true, true },
    [RCRC_16_UMTS] = { 0x8005, 0x0000, 0x0000, 16, false, false },
    [RCRC_16_USB] = { 0x8005, 0xFFFF, 0xFFFF, 16, true, true },
    [RCRC_16_XMODEM] = { 0x1021, 0x0000, 0x0000, 16, false, false },
    [RCRC_17_CAN_FD] = { 0x01685B, 0x000000, 0x000000, 17, false, false },
    [RCRC_21_CAN_FD] = { 0x102899, 0x000000, 0x000000, 21, false, false },
    [RCRC_24_BLE] = { 0x00065B, 0x555555, 0x000000, 24, true, true },
    [RCRC_24_FLEXRAY_A] = { 0x5D6DCB, 0xFEDCBA, 0x000000, 24, false, false },
    [RCRC_24_FLEXRAY_B] = { 0x5D6DCB, 0xABCDEF, 0x000000, 24, false, false },
    [RCRC_24_INTERLAKEN] = { 0x328B63, 0xFFFFFF, 0xFFFFFF, 24, false, false },
    [RCRC_24_LTE_A] = { 0x864CFB, 0x000000, 0x000000, 24, false, false },
    [RCRC_24_LTE_B] = { 0x800063, 0x000000, 0x000000, 24, false, false },
    [RCRC_24_OPENPGP] = { 0x864CFB, 0xB704CE, 0x000000, 24, false, false },
    [RCRC_24_OS_9] = { 0x800063, 0xFFFFFF, 0xFFFFFF, 24, false, false },
    [RCRC_30_CDMA] = { 0x2030B9C7, 0x3FFFFFFF, 0x3FFFFFFF, 30, false, false },
    [RCRC_31_PHILIPS] = { 0x04C11DB7, 0x7FFFFFFF, 0x7FFFFFFF, 31, false, false },
    [RCRC_32_AIXM] = { 0x814141AB, 0x00000000, 0x00000000, 32, false, false },
    [RCRC_32_AUTOSAR] = { 0xF4ACFB13, 0xFFFFFFFF, 0xFFFFFFFF, 32, true, true },
    [RCRC_32_BASE91_D] = { 0xA833982B, 0xFFFFFFFF, 0xFFFFFFFF, 32, true, true },
    [RCRC_32_BZIP2] = { 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, 32, false, false },
    [RCRC_32_CD_ROM_EDC] = { 0x8001801B, 0x00000000, 0x00000000, 32, true, true },
    [RCRC_32_CKSUM] = { 0x04C11DB7, 0x00000000, 0xFFFFFFFF, 32, false, false },
    [RCRC_32_ISCSI] = { 0x1EDC6F41, 0xFFFFFFFF, 0xFFFFFFFF, 32, true, true },
    [RCRC_32_ISO_HDLC] = { 0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, 32, true, true },
    [RCRC_32_JAMCRC] = { 0x04C11DB7, 0xFFFFFFFF, 0x00000000, 32, true, true },
    [RCRC_32_MEF] = { 0x741B8CD7, 0xFFFFFFFF, 0x00000000, 32, true, true },
    [RCRC_32_MPEG_2] = { 0x04C11DB7, 0xFFFFFFFF, 0x00000000, 32, false, false },
    [RCRC_32_XFER] = { 0x000000AF, 0x00000000, 0x00000000, 32, false, false },
    [RCRC_32_SATA] = { 0x04C11DB7, 0x52325032, 0x00000000, 32, false, false },
    [RCRC_40_GSM] = { 0x0000000004820009, 0x0000000000000000, 0x000000FFFFFFFFFF, 40, false, false },
    [RCRC_64_ECMA_182] = { 0x42F0E1EBA9EA3693, 0x0000000000000000, 0x0000000000000000, 64, false, false },
    [RCRC_64_GO_ISO] = { 0x000000000000001B, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 64, true, true },
    [RCRC_64_MS] = { 0x259C84CBA6426349, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 64, true, true },
    [RCRC_64_REDIS] = { 0xAD93D23594C935A9, 0x0000000000000000, 0x0000000000000000, 64, true, true },
    [RCRC_64_WE] = { 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 64, false, false },
    [RCRC_64_XZ] = { 0x42F0E1EBA9EA3693, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 64, true, true },
};


void rcrc_initFromId(rcrc *p, rcrc_id id)
{
    assert(p && id < RAT_LEN(algos));

    const algo *algo_p = algos + id;
    rcrc_init(p, algo_p->bitWidth, algo_p->poly, algo_p->init, algo_p->refIn, algo_p->refOut, algo_p->xorOut);
}


void rcrc_init(rcrc *p, unsigned char bitWidth, uintmax_t poly, uintmax_t init, bool refIn, bool refOut, uintmax_t xorOut)
{
    assert(p && bitWidth > 0 && bitWidth <= RBIT_BITSIZE(sizeof(uintmax_t)) &&
        !(poly & ~mask(bitWidth)) && !(init & ~mask(bitWidth)) && !(xorOut & ~mask(bitWidth)));

    p->bitWidth = bitWidth;
    p->poly = poly;
    p->init = init;
    p->refIn = refIn;
    p->refOut = refOut;
    p->xorOut = xorOut;

    p->value = 0;
    p->initOffset = 0;
}

void rcrc_addByte(rcrc *p, unsigned char b)
{
    assert(p && p->bitWidth > 0 && p->bitWidth <= RBIT_BITSIZE(sizeof(uintmax_t)));

    if (p->refIn) b = (unsigned char)reflect(b, CHAR_BIT);

    if (p->initOffset < p->bitWidth)
    {
        p->initOffset += CHAR_BIT;

        if (p->bitWidth <= p->initOffset)
        {
            b ^= (unsigned char)(p->init << (p->initOffset - p->bitWidth));
        }
        else
        {
            b ^= (unsigned char)(p->init >> (p->bitWidth - p->initOffset));
        }
    }

    for (unsigned char i = 0; i < CHAR_BIT; ++i)
    {
        p->value =
            ((p->value << 1) | RBIT_FLAG_GET(b, CHAR_BIT - 1)) ^
            (RBIT_FLAG_GET(p->value, p->bitWidth - 1) * p->poly);

        b <<= 1;
    }
}

void rcrc_add(rcrc *p, const void *buf, size_t bufSize)
{
    assert(p && (buf || !bufSize));

    for (size_t i = 0; i < bufSize; ++i)
    {
        rcrc_addByte(p, ((const unsigned char *)buf)[i]);
    }
}

uintmax_t rcrc_result(rcrc *p)
{
    assert(p && p->bitWidth > 0 && p->bitWidth <= RBIT_BITSIZE(sizeof(uintmax_t)));

    uintmax_t result = p->value;

    for (unsigned char i = 0; i < p->bitWidth; ++i)
    {
        result =
            (result << 1) ^
            (RBIT_FLAG_GET(result, p->bitWidth - 1) * p->poly);
    }
    result &= mask(p->bitWidth);

    if (p->refOut)
        result = reflect(result, p->bitWidth);

    result ^= p->xorOut;

    return result;
}



uintmax_t rcrc_calc(unsigned char bitWidth, uintmax_t poly, uintmax_t init, bool refIn, bool refOut, uintmax_t xorOut, const void *buf, size_t bufSize)
{
    assert(buf || !bufSize);

    rcrc state;
    rcrc_init(&state, bitWidth, poly, init, refIn, refOut, xorOut);
    rcrc_add(&state, buf, bufSize);
    return rcrc_result(&state);
}

uintmax_t rcrc_calcFromId(rcrc_id id, const void *buf, size_t bufSize)
{
    assert(id < RAT_LEN(algos) && (buf || !bufSize));

    rcrc state;
    rcrc_initFromId(&state, id);
    rcrc_add(&state, buf, bufSize);
    return rcrc_result(&state);
}
