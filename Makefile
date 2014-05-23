APP = dungeon
RELEASE = 0

include $(SDK_DIR)/Makefile.defs

OBJS = $(ASSETS).gen.o src/Map.o src/Game.o src/CubeData.o src/Entity.o src/Player.o src/Enemy.o src/Main.o src/AudioPlayer.o
ASSETDEPS += gfx/*.png $(ASSETS).lua
SIFTULATOR_FLAGS = -n 6

# build assets.html to proof stir-processed assets.
# comment out to disable.
ASSETS_BUILD_PROOF := yes

include $(SDK_DIR)/Makefile.rules
