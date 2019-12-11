#include "guis/GuiScraperStart.h"

#include "components/OptionListComponent.h"
#include "components/SwitchComponent.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiScraperMulti.h"
#include "views/ViewController.h"
#include "FileData.h"
#include "SystemData.h"

GuiScraperStart::GuiScraperStart(Window* window) : GuiComponent(window),
	mMenu(window, "下载游戏信息")
{
	addChild(&mMenu);

	// add filters (with first one selected)
	mFilters = std::make_shared< OptionListComponent<GameFilterFunc> >(mWindow, "下载这些游戏的信息", false);
	mFilters->add("所有游戏",
		[](SystemData*, FileData*) -> bool { return true; }, false);
	mFilters->add("无预览图的游戏",
		[](SystemData*, FileData* g) -> bool { return g->metadata.get("image").empty(); }, true);
	mMenu.addWithLabel("游戏类型", mFilters);

	//add systems (all with a platformid specified selected)
	mSystems = std::make_shared< OptionListComponent<SystemData*> >(mWindow, "下载这些平台的信息", true);
	for(auto it = SystemData::sSystemVector.cbegin(); it != SystemData::sSystemVector.cend(); it++)
	{
		if(!(*it)->hasPlatformId(PlatformIds::PLATFORM_IGNORE))
			mSystems->add((*it)->getFullName(), *it, !(*it)->getPlatformIds().empty());
	}
	mMenu.addWithLabel("游戏平台", mSystems);

	mApproveResults = std::make_shared<SwitchComponent>(mWindow);
	mApproveResults->setState(true);
	mMenu.addWithLabel("由用户裁决信息冲突", mApproveResults);

	mMenu.addButton("开始", "start", std::bind(&GuiScraperStart::pressedStart, this));
	mMenu.addButton("后退", "back", [&] { delete this; });

	mMenu.setPosition((Renderer::getScreenWidth() - mMenu.getSize().x()) / 2, Renderer::getScreenHeight() * 0.15f);
}

void GuiScraperStart::pressedStart()
{
	std::vector<SystemData*> sys = mSystems->getSelectedObjects();
	for(auto it = sys.cbegin(); it != sys.cend(); it++)
	{
		if((*it)->getPlatformIds().empty())
		{
			mWindow->pushGui(new GuiMsgBox(mWindow,
				Utils::String::toUpper("警告：某些平台缺少相关信息，所得结果可能很不准确。\n仍然继续吗？"),
				"是", std::bind(&GuiScraperStart::start, this),
				"否", nullptr));
			return;
		}
	}

	start();
}

void GuiScraperStart::start()
{
	std::queue<ScraperSearchParams> searches = getSearches(mSystems->getSelectedObjects(), mFilters->getSelected());

	if(searches.empty())
	{
		mWindow->pushGui(new GuiMsgBox(mWindow,
			"没有符合要求的游戏"));
	}else{
		GuiScraperMulti* gsm = new GuiScraperMulti(mWindow, searches, mApproveResults->getState());
		mWindow->pushGui(gsm);
		delete this;
	}
}

std::queue<ScraperSearchParams> GuiScraperStart::getSearches(std::vector<SystemData*> systems, GameFilterFunc selector)
{
	std::queue<ScraperSearchParams> queue;
	for(auto sys = systems.cbegin(); sys != systems.cend(); sys++)
	{
		std::vector<FileData*> games = (*sys)->getRootFolder()->getFilesRecursive(GAME);
		for(auto game = games.cbegin(); game != games.cend(); game++)
		{
			if(selector((*sys), (*game)))
			{
				ScraperSearchParams search;
				search.game = *game;
				search.system = *sys;

				queue.push(search);
			}
		}
	}

	return queue;
}

bool GuiScraperStart::input(InputConfig* config, Input input)
{
	bool consumed = GuiComponent::input(config, input);
	if(consumed)
		return true;

	if(input.value != 0 && config->isMappedTo("b", input))
	{
		delete this;
		return true;
	}

	if(config->isMappedTo("start", input) && input.value != 0)
	{
		// close everything
		Window* window = mWindow;
		while(window->peekGui() && window->peekGui() != ViewController::get())
			delete window->peekGui();
	}


	return false;
}

std::vector<HelpPrompt> GuiScraperStart::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();
	prompts.push_back(HelpPrompt("b", "后退"));
	prompts.push_back(HelpPrompt("start", "关闭"));
	return prompts;
}
