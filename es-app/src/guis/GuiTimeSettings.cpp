#include "guis/GuiTimeSettings.h"

#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"
#include "guis/GuiTextEditPopup.h"
#include "utils/StringUtil.h"
#include "Settings.h"
#include "Window.h"

GuiTimeSettings::GuiTimeSettings(Window* window, const char* title) : GuiScreensaverOptions(window, title)
{
	time_t time_now;
	struct tm *curr_time = NULL;
	time(&time_now);
	curr_time = localtime(&time_now);
	//set year
	auto year = std::make_shared<SliderComponent>(mWindow, 1970.f, 2099.f, 1.f, "年");
	year->setValue((float)(curr_time->tm_year + 1900));
	addWithLabel("设置年份", year);

	//set month
	auto month = std::make_shared<SliderComponent>(mWindow, 1.f, 12.f, 1.f, "月");
	month->setValue((float)(curr_time->tm_mon + 1));
	addWithLabel("设置月份", month);
	//set day
	auto mday = std::make_shared<SliderComponent>(mWindow, 1.f, 31.f, 1.f, "日");
	mday->setValue((float)(curr_time->tm_mday));
	addWithLabel("设置日期", mday);
	addSaveFunc([year,month,mday] {
		int timeday=(int)Math::round(year->getValue())*10000+(int)Math::round(month->getValue())*100+(int)Math::round(mday->getValue());
		std::string str="sudo date -s "+std::to_string(timeday);
		system(str.c_str());
	});
	//set hour
	auto hour = std::make_shared<SliderComponent>(mWindow, 0.f, 23.f, 1.f, "时");
	hour->setValue((float)(curr_time->tm_hour));
	addWithLabel("设置小时", hour);
	//set min
	auto minute = std::make_shared<SliderComponent>(mWindow, 0.f, 59.f, 1.f, "分");
	minute->setValue((float)(curr_time->tm_min));
	addWithLabel("设置分钟", minute);
	addSaveFunc([minute,hour] {
		std::string str="sudo date -s "+std::to_string((int)Math::round(hour->getValue()))+":"+std::to_string((int)Math::round(minute->getValue()))+":00";
		system(str.c_str());
	});
}

GuiTimeSettings::~GuiTimeSettings()
{
}
