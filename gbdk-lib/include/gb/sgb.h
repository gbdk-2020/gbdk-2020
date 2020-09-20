/** @file gb/sgb.h
    Super Gameboy definitions.
*/
#ifndef _SGB_H
#define _SGB_H

#define SGB_PAL_01 0x00U
#define SGB_PAL_23 0x01U
#define SGB_PAL_03 0x02U
#define SGB_PAL_12 0x03U
#define SGB_ATTR_BLK 0x04U
#define SGB_ATTR_LIN 0x05U
#define SGB_ATTR_DIV 0x06U
#define SGB_ATTR_CHR 0x07U
#define SGB_SOUND 0x08U
#define SGB_SOU_TRN 0x09U
#define SGB_PAL_SET 0x0AU
#define SGB_PAL_TRN 0x0BU
#define SGB_ATRC_EN 0x0CU
#define SGB_TEST_EN 0x0DU
#define SGB_ICON_EN 0x0EU
#define SGB_DATA_SND 0x0FU
#define SGB_DATA_TRN 0x10U
#define SGB_MLT_REQ 0x11U
#define SGB_JUMP 0x12U
#define SGB_CHR_TRN 0x13U
#define SGB_PCT_TRN 0x14U
#define SGB_ATTR_TRN 0x15U
#define SGB_ATTR_SET 0x16U
#define SGB_MASK_EN 0x17U
#define SGB_OBJ_TRN 0x18U


/** Return a non-null value if running on Super GameBoy */
UINT8 sgb_check(void);

/** Transfers SGB packet */
void sgb_transfer(unsigned char * packet);

/** Transfers SGB packet without waiting */
void sgb_transfer_nowait(unsigned char * packet);

#endif /* _SGB_H */
