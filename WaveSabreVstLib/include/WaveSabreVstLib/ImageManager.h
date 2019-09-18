#ifndef __WAVESABREVSTLIB_IMAGEMANAGER_H__
#define __WAVESABREVSTLIB_IMAGEMANAGER_H__

#include "Common.h"

namespace WaveSabreVstLib
{
	class ImageManager
	{
	public:
		enum class ImageIds
		{
			Background,
			Knob1,
			TinyButton,
			OptionMenuUnpressed,
			OptionMenuPressed
		};

		static CBitmap *Get(ImageIds imageId);

	private:
		~ImageManager();

		static ImageManager *get();

		std::map<ImageIds, CBitmap *> bitmaps;
	};
}

#endif
