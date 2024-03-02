all: clean fonts fonts-bold
.PHONY: clean fonts fonts-bold

FONT_PATH ?= /mnt/c/Users/rosmo/Downloads
FONT ?= AlegreyaSans-Medium.ttf
FONT_NAME ?= alegreya
SIZES := 32 36 40

BOLD_FONT ?= AlegreyaSans-Bold.ttf
BOLD_FONT_NAME ?= alegreya_bold
BOLD_SIZES := 32 40 42 96

clean:
	rm -f main/$(FONT_NAME)*.h
	rm -f main/$(BOLD_FONT_NAME)*.h

fonts:
	echo -n "" > main/fonts.h
	for size in $(SIZES) ; do \
		echo "#include \"$(FONT_NAME)$$size.h\"" >> main/fonts.h ; \
		lv_font_conv --font $(FONT_PATH)/$(FONT) -r 0x20-0xFF --size $$size --format lvgl --lv-include "lvgl.h" --bpp 1 -o main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])glyph_bitmap/\1glyph_bitmap_$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])cache/\1cache_$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])glyph_dsc/\1glyph_dsc_$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])cmaps/\1cmaps_$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])font_dsc/\1font_dsc_$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_pair_glyph_ids/\1kern_pair_glyph_ids_$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_pair_values/\1kern_pair_values$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_pairs/\1kern_pairs$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_classes/\1kern_classes$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_class_values/\1kern_class_values$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_left_class_mapping/\1kern_left_class_mapping$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_right_class_mapping/\1kern_right_class_mapping$$size/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.glyph_bitmap_$$size/.glyph_bitmap/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.glyph_dsc_$$size/.glyph_dsc/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.cmaps_$$size/.cmaps/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.cache_$$size/.cache/g" main/$(FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.kern_classes$$size/.kern_classes/g" main/$(FONT_NAME)$$size.h ; \
	done
	echo "#define APP_FONT(size) &$(FONT_NAME)##size" >> main/fonts.h 

fonts-bold:
	for size in $(BOLD_SIZES) ; do \
		echo "#include \"$(BOLD_FONT_NAME)$$size.h\"" >> main/fonts.h ; \
		lv_font_conv --font $(FONT_PATH)/$(BOLD_FONT) -r 0x20-0xFF --size $$size --format lvgl --lv-include "lvgl.h" --bpp 1 -o main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])glyph_bitmap/\1glyph_bitmap_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])cache/\1cache_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])glyph_dsc/\1glyph_dsc_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])cmaps/\1cmaps_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])font_dsc/\1font_dsc_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_pair_glyph_ids/\1kern_pair_glyph_ids_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_pair_values/\1kern_pair_values_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_pairs/\1kern_pairs_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_classes/\1kern_classes_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_class_values/\1kern_class_values_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_left_class_mapping/\1kern_left_class_mapping_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/([^_])kern_right_class_mapping/\1kern_right_class_mapping_bold_$$size/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.glyph_bitmap_bold_$$size/.glyph_bitmap/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.glyph_dsc_bold_$$size/.glyph_dsc/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.cmaps_bold_$$size/.cmaps/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.cache_bold_$$size/.cache/g" main/$(BOLD_FONT_NAME)$$size.h ; \
		sed -ri -e "s/\.kern_classes_bold_$$size/.kern_classes/g" main/$(BOLD_FONT_NAME)$$size.h ; \
	done
	echo "#define APP_FONT_BOLD(size) &$(BOLD_FONT_NAME)##size" >> main/fonts.h 
