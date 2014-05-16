APP = dungeon

include $(SDK_DIR)/Makefile.defs

OBJS = $(ASSETS).gen.o src/Main.o src/Map.o src/Game.o
ASSETDEPS += gfx/*.png $(ASSETS).lua
SIFTULATOR_FLAGS = -n 5

# build assets.html to proof stir-processed assets.
# comment out to disable.
ASSETS_BUILD_PROOF := yes

include $(SDK_DIR)/Makefile.rules
