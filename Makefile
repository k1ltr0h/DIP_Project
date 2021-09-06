GPP=g++ -Wall -o video
CPP=*.cpp
LIBS=-L "C:\opencv440\install\x64\mingw\lib"
INCLUDE=-I "C:\opencv440\install\include"\
	-lopencv_highgui440 -lopencv_imgcodecs440 -lopencv_core440 -lopencv_videoio440\
	-lopencv_imgproc440
ICON=icon.res
MWIN=-mwindows

all:
	$(GPP) $(CPP) $(LIBS) $(INCLUDE) $(ICON) $(MWIN)
test:
	$(GPP) $(CPP) $(LIBS) $(INCLUDE) $(ICON)