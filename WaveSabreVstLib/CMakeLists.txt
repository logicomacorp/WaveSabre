add_library(WaveSabreVstLib STATIC
	${VSTSDK3_DIR}/public.sdk/source/vst2.x/audioeffect.cpp
	${VSTSDK3_DIR}/public.sdk/source/vst2.x/audioeffectx.cpp
	${VSTSDK3_DIR}/public.sdk/source/vst2.x/vstplugmain.cpp
	${VSTSDK3_DIR}/vstgui.sf/libpng/png.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngerror.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pnggccrd.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngget.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngmem.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngpread.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngread.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngrio.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngrtran.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngrutil.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngset.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngtrans.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngvcrd.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngwio.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngwrite.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngwtran.c
	${VSTSDK3_DIR}/vstgui.sf/libpng/pngwutil.c
	${VSTSDK3_DIR}/vstgui.sf/vstgui/aeffguieditor.cpp
	${VSTSDK3_DIR}/vstgui.sf/vstgui/cfileselector.cpp
	${VSTSDK3_DIR}/vstgui.sf/vstgui/vstcontrols.cpp
	${VSTSDK3_DIR}/vstgui.sf/vstgui/vstgui.cpp
	${VSTSDK3_DIR}/vstgui.sf/vstgui/vstguidebug.cpp
	${VSTSDK3_DIR}/vstgui.sf/zlib/adler32.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/compress.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/crc32.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/deflate.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/gzio.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/infback.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/inffast.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/inflate.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/inftrees.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/minigzip.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/trees.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/uncompr.c
	${VSTSDK3_DIR}/vstgui.sf/zlib/zutil.c
	include/WaveSabreVstLib.h
	src/ImageManager.cpp
	src/NoTextCOptionMenu.cpp
	src/VstEditor.cpp
	src/VstPlug.cpp)

target_include_directories(WaveSabreVstLib PUBLIC
	include
	../WaveSabreCore/include
	${VSTSDK3_DIR}/vstgui.sf/zlib
	${VSTSDK3_DIR}/vstgui.sf/libpng
	${VSTSDK3_DIR}/public.sdk/source/vst2.x
	${VSTSDK3_DIR})

target_compile_definitions(WaveSabreVstLib PUBLIC USE_LIBPNG)

if(MSVC)
	target_compile_definitions(WaveSabreVstLib PUBLIC _CRT_SECURE_NO_WARNINGS)
	target_compile_options(WaveSabreVstLib PUBLIC /EHsc)
	set_source_files_properties(${VSTSDK3_DIR}/vstgui.sf/vstgui/vstgui.cpp
		${VSTSDK3_DIR}/vstgui.sf/zlib/minigzip.c
		PROPERTIES COMPILE_OPTIONS /wd4996)
endif()
