#ifndef __NOTEXTCOPTIONMENU_H__
#define __NOTEXTCOPTIONMENU_H__

#include "Common.h"

namespace WaveSabreVstLib
{
	class NoTextCOptionMenu : public COptionMenu
	{
	public:
		NoTextCOptionMenu(const CRect& size, CControlListener *listener, long tag, CBitmap *background = 0, CBitmap *bgWhenClick = 0, const long style = 0);
		NoTextCOptionMenu(const NoTextCOptionMenu& v);
		virtual ~NoTextCOptionMenu();

		virtual void draw(CDrawContext *pContext);
	};
}

#endif
