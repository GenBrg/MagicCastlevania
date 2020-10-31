
OS := $(shell uname)
ProjectName=MagicCastlevania

ifeq ($(OS), Linux)
	GIMP=gimp
else
	GIMP=/Applications/GIMP.app/Contents/MacOS/gimp
endif


../dist/$(ProjectName).png ../dist/$(ProjectName).atlas : $(ProjectName).xcf $(ProjectName).list trade.xcf trade-font.list extract-sprites.py pack-sprites
	rm -rf $(ProjectName)
	./extract-sprites.py $(ProjectName).list $(ProjectName) --gimp='$(GIMP)'
	./extract-sprites.py trade-font.list $(ProjectName) --gimp='$(GIMP)'
	./pack-sprites ../dist/$(ProjectName) $(ProjectName)/*
