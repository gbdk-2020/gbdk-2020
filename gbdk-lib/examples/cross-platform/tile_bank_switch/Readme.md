
# Multi-tile-bank image example for GB/GBC, SMS/GG and NES

This example for shows how an image can be split into multiple Tile Data banks when exceeding 256 tiles.

A converted "upper" image is loaded to the first tile bank, and a converted "lower" image to the second.

### The method for display differs slightly depending on platform.

- On GBC and SMS/GG the attribute bit for selecting the second tile bank is set to either 0 or 1. This allows the graphics hardware to show up to 512 background tiles with no extra CPU processing.

- On NES and GB the attribute bit is not available, so a VBL and LCD ISR is used to switch banks mid-frame.
- On NES, this is done with mapper30 CHR-RAM bank-switching.
- On DMG, this is done by flipping the LCDCF_BG8000 bit in LCDC_REG.

To keep the example similar on all platforms the image is of 160x144 resolution, and only two tile banks
are used.

However, it should be noted that the same technique can be used on NES/mapper30 to show a full-screen
256x240 image with multiple bank switches from repeated LCD interrupt events.

