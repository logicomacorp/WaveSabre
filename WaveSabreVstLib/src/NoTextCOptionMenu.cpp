#include <WaveSabreVstLib/NoTextCOptionMenu.h>

namespace WaveSabreVstLib
{
	NoTextCOptionMenu::NoTextCOptionMenu(const CRect& size, CControlListener *listener, long tag, CBitmap *background, CBitmap *bgWhenClick, const long style)
		: COptionMenu(size, listener, tag, background, bgWhenClick, style)
	{
	}

	NoTextCOptionMenu::NoTextCOptionMenu(const NoTextCOptionMenu& v)
		: COptionMenu(v)
	{
	}

	NoTextCOptionMenu::~NoTextCOptionMenu()
	{
	}

	void NoTextCOptionMenu::draw(CDrawContext *pContext)
	{
		drawText(pContext, NULL, getFrame()->getFocusView() == this ? bgWhenClick : 0);
	}
}
