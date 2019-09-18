#include <WaveSabreVstLib/ImageManager.h>
#include <WaveSabreVstLib/Common.h>

#include "../../Data/resource.h"

using namespace std;

namespace WaveSabreVstLib
{
	CBitmap *ImageManager::Get(ImageIds imageId)
	{
		ImageManager *im = get();

		auto ret = im->bitmaps.find(imageId);
		if (ret != im->bitmaps.end()) return ret->second;

		CBitmap *b;
		switch (imageId)
		{
		case ImageIds::Background: b = new CBitmap(IDB_PNG1); break;
		case ImageIds::Knob1: b = new CBitmap(IDB_PNG2); break;
		case ImageIds::TinyButton: b = new CBitmap(IDB_PNG3); break;
		case ImageIds::OptionMenuUnpressed: b = new CBitmap(IDB_PNG4); break;
		case ImageIds::OptionMenuPressed: b = new CBitmap(IDB_PNG5); break;
		}
		b->remember();
		im->bitmaps.emplace(imageId, b);
		return b;
	}

	ImageManager::~ImageManager()
	{
		for (auto i = bitmaps.begin(); i != bitmaps.end(); i++)
			i->second->forget();
	}

	ImageManager *ImageManager::get()
	{
		static ImageManager im;

		return &im;
	}
}
