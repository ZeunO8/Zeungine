#pragma once
#include <zg/Scene.hpp>
#include <zg/Window.hpp>
#include <zg/entities/AssetBrowser.hpp>
#include <zg/entities/Button.hpp>
#include <zg/entities/Console.hpp>
#include <zg/entities/Dialog.hpp>
#include <zg/entities/Input.hpp>
#include <zg/entities/Panel.hpp>
#include <zg/entities/StatusText.hpp>
#include <zg/entities/Tabs.hpp>
#include <zg/entities/Toolbar.hpp>
#include <zg/fonts/freetype/Freetype.hpp>
#include "Hotswapper.hpp"
#include "Project.hpp"
namespace zg::editor
{
	using namespace zg;
	struct EditorScene : Scene
	{
		glm::vec4 editorClearColor = {0.2, 0.2, 0.2, 1};
		float toolbarHeight;
		float bottomTabsHeight;
		filesystem::File robotoRegularFile;
		fonts::freetype::FreetypeFont robotoRegularFont;
		Window* gameWindowPointer = nullptr;
		Window* codeWindowPointer = nullptr;
		float gameWindowBorderWidth;
		float gameWindowWidth;
		float gameWindowHeight;
		float gameWindowX;
		float gameWindowY;
		float codeWindowWidth;
		float codeWindowHeight;
		float codeWindowX;
		float codeWindowY;
		glm::vec4 toolbarColor;
		std::shared_ptr<entities::Toolbar> toolbar;
		std::shared_ptr<entities::TabsBar> bottomTabsBar;
		std::shared_ptr<entities::StatusText> status;
		glm::vec4 gameWindowBorderColor = {0.4, 0.4, 0.7, 1};
		glm::vec4 gameWindowHoveredBorderColor = {0.7, 0.4, 0.4, 1};
		glm::vec4 gameWindowActiveBorderColor = {1, 0, 0, 1};
		std::shared_ptr<entities::Plane> gameWindowBorder;
		std::shared_ptr<entities::PanelMenu> sceneGraphPanelMenu;
		float resourcePanelMenuHeight;
		std::shared_ptr<entities::PanelMenu> resourcePanelMenu;
		std::shared_ptr<entities::Console> resourceConsole;
		std::shared_ptr<entities::AssetBrowser> resourceAssetBrowser;
		std::shared_ptr<entities::TabsBar> resourcePanelTabs;
		std::shared_ptr<Entity> activeResourcePanelEntity;
		IWindow::EventIdentifier resizeID = 0;
		IWindow::EventIdentifier gameWindowBorderHoverID = 0;
		IWindow::EventIdentifier gameWindowBorderPressID = 0;
		IWindow::EventIdentifier gameWindowESCPressID = 0;
		float dialogWidth;
		float dialogHeight;
		float closeDialogButtonWidth;
		float closeDialogButtonHeight;
		float okayDialogButtonWidth;
		float okayDialogButtonHeight;
		std::shared_ptr<entities::Button> closeDialogButton;
		std::shared_ptr<entities::Button> okayDialogButton;
    std::shared_ptr<entities::Input> projectNameInput;
    std::shared_ptr<entities::Input> projectDirectoryInput;
    std::shared_ptr<entities::Dialog> newProjectDialog;
		std::shared_ptr<entities::Dialog> activeDialog;
		std::shared_ptr<hs::Hotswapper> hotswapper;
		Project project;
		std::function<void(Window &)> OnLoad;
		std::function<void(Window &, hscpp::AllocationResolver &)> OnHotswapLoad;
		std::function<void(Window &)> OnUnLoad;
		bool loaded = false;
		explicit EditorScene(Window& window);
		~EditorScene() override;
		void onEntityAdded(const std::shared_ptr<IEntity>& entity);
		void setupGameWindow();
		void setupCodeWindow();
		void minimizeWindows();
		void removeActiveResourceEntity() const;
		void setupToolbarOptions();
		void newProject(std::string_view projectName, std::string_view projectDirectory);
		void loadProject(std::string_view projectDirectory);
	};
}