.PHONY : all

HOSTNAME := $(shell hostname -s)

#n.b. the '-y' sets autoexec scripts to 'on' so that driver expressions will work
ifeq ($(HOSTNAME), nextchow)
	BLENDER = ~/blender-2.82a-linux64/blender -y
else ifeq ($(HOSTNAME), retchow)
	BLENDER = /Applications/Blender/blender.app/Contents/MacOS/blender -y
else
	BLENDER = blender
endif

EXPORT_MESHES=export-meshes.py
EXPORT_SCENE=export-scene.py

DIST=../dist

all : \
	$(DIST)/hexapod.pnct \
	$(DIST)/hexapod.scene \


$(DIST)/hexapod.scene : hexapod.blend $(EXPORT_SCENE)
	$(BLENDER) --background --python $(EXPORT_SCENE) -- '$<':Main '$@'

$(DIST)/hexapod.pnct : hexapod.blend $(EXPORT_MESHES)
	$(BLENDER) --background --python $(EXPORT_MESHES) -- '$<':Main '$@'
