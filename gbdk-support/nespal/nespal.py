#!/usr/bin/env python3
import sys
import argparse
from operator import itemgetter
from pathlib import Path

from typing import Optional, Tuple, List, Sequence, Dict, Set, NewType


def get_ppu_color_name(ppu_color: int) -> str:
    """
    Returns a human-readable name for a PPU color
    
    Naming convention from https://www.nesdev.org/wiki/PPU_palettes#Color_names
    
    With following simplifications:
    'Dark gray' -> 'Gray'
    'Light gray or silver' -> 'Silver'
    
    :param ppu_color: 6-bit NES PPU color
    :return: Human-readable name for PPU color
    """
    ppu_hue_names = [
        'Gray',
        'Azure',
        'Blue',
        'Violet',
        'Magenta',
        'Rose',
        'Red',
        'Orange',
        'Yellow',
        'Chartreuse',
        'Green',
        'Spring',
        'Cyan'
    ]
    ppu_luma_names = [
        'Dark',
        'Medium',
        'Light',
        'Pale'
    ]
    c = ppu_color & 0x3F
    # Special-cases for black/white/grays
    if c in [0x20, 0x30]:
        return 'White'
    elif c in [0x3D]:
        return 'Silver'
    elif c in [0x00, 0x2D]:
        return 'Gray'
    elif c in [0x1D, 0x0E, 0x1E, 0x2E, 0x3E, 0x0F, 0x1F, 0x2F, 0x3F]:
        return 'Black'
    elif c == 0x0D:
        return 'BlackerThanBlack'
    # All normal hues
    ppu_hue_name = ppu_hue_names[c & 0xF]
    ppu_luma_name = ppu_luma_names[(c >> 4) & 0x3]
    return f'{ppu_luma_name}-{ppu_hue_name}'


def get_script_directory() -> Path:
    """
    Return path to current scripts directory.
    (or the executable if built with pyinstaller)
    
    :return: Path to directory of this script
    """
    if getattr(sys, 'frozen', False):
        return Path(sys.executable).parent
    # or a script file (e.g. `.py` / `.pyw`)
    elif __file__:
        return Path(__file__).parent


def nes_closest_palette_entry(rgb: Tuple[int, int, int], NESPaletteRGB: List[Tuple[int, int, int]]) -> int:
    def dist2(rgbA, rgbB):
        return sum((rgbA[i] - rgbB[i])**2 for i in range(3))
    minIndex = min(enumerate([dist2(rgb, nprgb) for i, nprgb in enumerate(NESPaletteRGB)]), key=itemgetter(1))[0]
    return minIndex


def to_triplets(data: List[int]) -> List[Tuple[int, int, int]]:
    """
    Convert a linear list into a triplet list
    
    :param data: List formatted as [0, 1, 2 ... N, N+1, N+2]
    :return: Nested list formatted as [[0, 1, 2] ... [N, N+1, N+2]]
    """
    assert(len(data) % 3 == 0)
    length = len(data) // 3
    data_triplets = []
    for i in range(length):
        data_triplets.append((data[3 * i + 0], data[3 * i + 1], data[3 * i + 2]))
    return data_triplets


def map_to_PPU_colors(target_colors: List[int], rgb_palette_nes: List[int], invalid_colors: List[int]) -> Tuple[List[int], List[int]]:
    """
    Creates a mapping from target colors in RGB format to NES PPU colors
    
    :param target_colors: List of length N, with target colors in RGB format
    :param rgb_palette_nes: List of length 64, giving RGB values for NES colors
    :return: List of length N with each entry indicating the best NES color
    """
    target_colors = to_triplets(target_colors)
    rgb_palette_nes = to_triplets(rgb_palette_nes)
    # Set invalid_colors to large value to prevent being picked
    rgb_palette_nes = rgb_palette_nes[:]
    for c in invalid_colors:
        rgb_palette_nes[c] = (1000000, 1000000, 1000000)
    nes_palette_colors = []
    for rgb_color in target_colors:
        # print(rgb_color)
        closest_palette_entry = nes_closest_palette_entry(rgb_color, rgb_palette_nes)
        nes_palette_colors.append(closest_palette_entry)
    return nes_palette_colors


def get_pal_file_path(pal_file_path: str) -> Path:
    """
    Convert a string path to Pathlib path.
    If None, use default path and print warning message.
    If file is missing, print error.

    :param pal_file_path: path to .pal file
    :return: path as pathlib Path
    """
    default_pal_file_path = get_script_directory() / 'palettes' / 'palgen.pal'
    if pal_file_path is None:
        print(f'WARNING: Palette file not specified - falling back to default palette file {str(default_pal_file_path)}')
        return default_pal_file_path
    elif not Path(pal_file_path).exists():
        print(f'WARNING: Palette file {str(Path(pal_file_path))} does not exist - falling back to default palette file {str(default_pal_file_path)}')
        return default_pal_file_path
    else:
        return Path(pal_file_path)


def write_c_lut(lut: List[int], filename: Path):
    """
    Writes NES color mapping as a C-array rgb_to_nes
    
    :param lut: Lookup table of 64 values, indexed as BBGGRR
    :param filename: Source text file to write
    """
    with open(filename, 'wt') as f:
        print('// File auto-generated file by nespal.py', file=f)
        print(f'unsigned char rgb_to_nes[{len(lut)}] = {{', file=f)
        for i, c in enumerate(lut):
            r = ((i >> 0) & 0x3)
            g = ((i >> 2) & 0x3)
            b = ((i >> 4) & 0x3)
            print(f'    0x{c:0{2}X},   // RGB({r},{g},{b}) -> {c:0{2}X}   ({get_ppu_color_name(c)})', file=f)
        print(f'}};', file=f)


def write_c_macro(lut: List[int], filename: Path):
    """
    Writes NES color mapping as a C-macro RGB_TO_NES(c)
    
    :param lut: Lookup table of 64 values, indexed as BBGGRR
    :param filename: Source text file to write
    """
    with open(filename, 'wt') as f:
        print('// File auto-generated file by nespal.py', file=f)
        print('#ifndef __RGB_TO_NES_MACRO_H__', file=f)
        print('#define __RGB_TO_NES_MACRO_H__', file=f)
        print(f'#define RGB_TO_NES(c) \\', file=f)
        for i, c in enumerate(lut):
            print(f'    (c == 0x{i:0{2}X}) ? 0x{c:0{2}X} : \\', file=f)
        print(f'                  0xFF // out-of-range value - set to 0xFF', file=f)
        print('#endif', file=f)


def main(palette_file: Path,
         invalid_colors: List[int],
         output_c_lut_path: Path,
         output_c_macro_path: Path) -> int:
    # Read NES palette mapping file
    with open(palette_file, 'rb') as f:
        nes_palette = f.read(192)
    # Use a 6-bit RGB palette in BBGGRR format
    gbdk_palette = []
    for i in range(64):
        r = (((i >> 0) & 0x3) / 3.0) * 255.0
        g = (((i >> 2) & 0x3) / 3.0) * 255.0
        b = (((i >> 4) & 0x3) / 3.0) * 255.0
        gbdk_palette.append(r)
        gbdk_palette.append(g)
        gbdk_palette.append(b)
    nes_palette_colors = map_to_PPU_colors(gbdk_palette, nes_palette, invalid_colors)
    if output_c_lut_path is not None:
        write_c_lut(nes_palette_colors, output_c_lut_path)
    if output_c_macro_path is not None:
        write_c_macro(nes_palette_colors, output_c_macro_path)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=f'Generates lookup tables to convert RGB to NES PPU compatible colors.')
    parser.add_argument('--pal_file', type=str,
                        default=None,
                        help='Binary 192-byte file specifying a particular NES palette flavor.')
    parser.add_argument('--output_c_lut', type=str,
                        default='rgb_to_nes_lut.c',
                        help='Output .c file with generated rgb_to_nes lookup table.')
    parser.add_argument('--output_c_macro', type=str,
                        default='rgb_to_nes_macro.h',
                        help='Output .h file with generated RGB_TO_NES C macro.')
    parser.add_argument('--invalid_colors', type=str,
                        nargs='+',
                        default=['0D', '0E', '1E', '2E', '3E', '0F', '1F', '2F', '3F'],
                        help='Specify which NES PPU colors should be considered invalid.')
    args = parser.parse_args()
    # Call main program
    rc = main(get_pal_file_path(args.pal_file),
              [int(s, 16) for s in args.invalid_colors],
              Path(args.output_c_lut),
              Path(args.output_c_macro))
    sys.exit(rc)
