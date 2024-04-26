

ifndef OUTDIR
	OUTDIR = out
endif
RES = res
REF = ref


# PNG2ASSET = ~/gbdev/GBDK_releases/GBDK-2020_4.2.0/gbdk/bin/png2asset

ifndef PNG2ASSET
	PNG2ASSET = ../png2asset
endif

all: test

test: generate-assets compare-output

update:
	${MAKE} generate-assets OUTDIR=ref

compare-output:
	diff --report-identical-files --recursive --suppress-common-lines --side-by-side $(REF) $(OUTDIR)

clean:
	rm -rf $(OUTDIR)

generate-assets:
	rm -rf $(OUTDIR)
	mkdir -p $(OUTDIR)
# ZGB:
	# Metasprite
	# - CGB
	$(PNG2ASSET) $(RES)/sprite_slon.png -c $(OUTDIR)/zgb_sprite_1.c -use_structs -b 255 -spr8x16 -px 0 -py 0 -pw 32 -ph 24 -sw 64 -sh 48
	# - SMS/GG
	$(PNG2ASSET) $(RES)/sprite_slon.png -c $(OUTDIR)/zgb_sprite_2.c -use_structs -b 255 -spr8x16 -px 0 -py 0 -pw 32 -ph 24 -sw 64 -sh 48 -noflip -pack_mode sms -bpp 4 -keep_palette_order

	# SGB
	$(PNG2ASSET) $(RES)/SGB/sgb_border.png -c $(OUTDIR)/zgb_sgb_border_1.c -b 255 -map -bpp 4 -max_palettes 4 -use_map_attributes -use_structs -pack_mode sgb

	# Map
	# - GB
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/zgb_largemap_1.c -b 255 -map -use_structs -noflip
	# - SMS/GG
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/zgb_largemap_2.c -b 255 -map -use_structs -noflip -pack_mode sms -bpp 4 -keep_palette_order


# Entity and Source tileset interaction
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_1.c -map
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_2.c -map -source_tileset $(RES)/nums8x8_9_to_0.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_3.c -map -entity_tileset $(RES)/nums8x8_30_to_39.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_4.c -map -source_tileset $(RES)/nums8x8_9_to_0.png -entity_tileset $(RES)/nums8x8_30_to_39.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_5.c -map
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_6.c -map -keep_duplicate_tiles -source_tileset $(RES)/nums8x8_9_to_0.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_7.c -map -keep_duplicate_tiles -entity_tileset $(RES)/nums8x8_30_to_39.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/nums_map_8.c -map -keep_duplicate_tiles -source_tileset $(RES)/nums8x8_9_to_0.png -entity_tileset $(RES)/nums8x8_30_to_39.png
# ZGB flavor of above
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_1.c -map -use_structs
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_2.c -map -use_structs -source_tileset $(RES)/nums8x8_9_to_0.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_3.c -map -use_structs -entity_tileset $(RES)/nums8x8_30_to_39.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_4.c -map -use_structs -source_tileset $(RES)/nums8x8_9_to_0.png -entity_tileset $(RES)/nums8x8_30_to_39.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_5.c -map -use_structs
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_6.c -map -use_structs -keep_duplicate_tiles -source_tileset $(RES)/nums8x8_9_to_0.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_7.c -map -use_structs -keep_duplicate_tiles -entity_tileset $(RES)/nums8x8_30_to_39.png
	$(PNG2ASSET) $(RES)/nums8x8_map_0_to_39.png -o $(OUTDIR)/zgb_nums_map_8.c -map -use_structs -keep_duplicate_tiles -source_tileset $(RES)/nums8x8_9_to_0.png -entity_tileset $(RES)/nums8x8_30_to_39.png


# keep_palette_order + source_tileset + noflip + map (gbphoto)
	$(PNG2ASSET) $(RES)/remote.png      -c $(OUTDIR)/remote.c      -keep_palette_order -map -noflip
	$(PNG2ASSET) $(RES)/remote_down.png -c $(OUTDIR)/remote_down.c -keep_palette_order -map -noflip -source_tileset $(RES)/remote.png

# Logo example: attributes, various platforms, different images for each
	$(PNG2ASSET) $(RES)/DMG/GBDK_2020_logo.png  -o $(OUTDIR)/gbdklogo_1.c -map                     -bpp 2 -max_palettes 1 -pack_mode gb -noflip
	$(PNG2ASSET) $(RES)/CGB/GBDK_2020_logo.png  -o $(OUTDIR)/gbdklogo_2.c -map -use_map_attributes -bpp 2 -max_palettes 8 -pack_mode gb
	$(PNG2ASSET) $(RES)/NES/GBDK_2020_logo.png  -o $(OUTDIR)/gbdklogo_3.c -map -use_nes_attributes -bpp 2 -max_palettes 4 -pack_mode nes -noflip  -use_nes_colors
	$(PNG2ASSET) $(RES)/SEGA/GBDK_2020_logo.png -o $(OUTDIR)/gbdklogo_4.c -map -use_map_attributes -bpp 4 -max_palettes 2 -pack_mode sms

# Large Map: attributes, various platforms, noflip
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/largemap_1.c -map -bpp 2 -max_palettes 4                     -pack_mode gb  -noflip
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/largemap_2.c -map -bpp 2 -max_palettes 8 -use_map_attributes -pack_mode gb
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/largemap_3.c -map -bpp 2 -max_palettes 4                     -pack_mode gb  -noflip
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/largemap_4.c -map -bpp 2 -max_palettes 4                     -pack_mode gb  -noflip
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/largemap_5.c -map -bpp 4 -max_palettes 2 -use_map_attributes -pack_mode sms
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/largemap_6.c -map -bpp 4 -max_palettes 2 -use_map_attributes -pack_mode sms
	$(PNG2ASSET) $(RES)/largemap.png -c $(OUTDIR)/largemap_7.c -map -bpp 2 -max_palettes 4 -use_nes_attributes -pack_mode nes -noflip -b 0

# Metasprites, various sprite sizing, output options
	$(PNG2ASSET) $(RES)/sprite_slon.png -c $(OUTDIR)/sprite_slon_1.c -sh 48 -spr8x8  -noflip
	$(PNG2ASSET) $(RES)/sprite_slon.png -c $(OUTDIR)/sprite_slon_2.c -sh 48 -spr8x16 -noflip
	$(PNG2ASSET) $(RES)/sprite_slon.png -c $(OUTDIR)/sprite_slon_3.c -sh 48 -spr8x16 -noflip -metasprites_only
	$(PNG2ASSET) $(RES)/sprite_slon.png -c $(OUTDIR)/sprite_slon_4.c -sh 48 -spr8x16 -noflip -tiles_only

# hblank copy: noflip
	$(PNG2ASSET) $(RES)/hblank_copy.png -o $(OUTDIR)/hblankcopy_1.c -map -noflip -tiles_only -keep_duplicate_tiles -b 255

# apa image style
	$(PNG2ASSET) $(RES)/apa_image.png -c $(OUTDIR)/apaimage_1.c -map -keep_duplicate_tiles -bpp 2 -tiles_only

# SGB Border
	$(PNG2ASSET) $(RES)/SGB/sgb_border.png -c $(OUTDIR)/sgb_border_1.c -map -bpp 4 -max_palettes 4 -pack_mode sgb -use_map_attributes
	$(PNG2ASSET) $(RES)/SGB/sgb_border.png -c $(OUTDIR)/sgb_border_2.c -map -bpp 4 -max_palettes 4 -pack_mode sgb -use_map_attributes -no_palettes

# -bin output
	$(PNG2ASSET) $(RES)/gameboy.png -c $(OUTDIR)/binout.1.bin -bin -tiles_only
	$(PNG2ASSET) $(RES)/gameboy.png -c $(OUTDIR)/binout.2.bin -bin -maps_only
	$(PNG2ASSET) $(RES)/gameboy.png -c $(OUTDIR)/binout.3.bin -bin -maps_only -no_palettes
