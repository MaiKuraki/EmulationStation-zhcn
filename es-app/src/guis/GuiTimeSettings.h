#pragma once
#ifndef ES_APP_GUIS_GUI_TIMESETTINGS_H
#define ES_APP_GUIS_GUI_TIMESETTINGS_H

#include "GuiScreensaverOptions.h"

class GuiTimeSettings : public GuiScreensaverOptions
{
public:
	GuiTimeSettings(Window* window, const char* title);
	virtual ~GuiTimeSettings();
};

#endif // ES_APP_GUIS_GUI_TIMESETTINGS_H