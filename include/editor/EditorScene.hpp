#pragma once
#include <iostream>
#include <anex/modules/gl/GLWindow.hpp>
#include <anex/modules/gl/GLScene.hpp>
#include <anex/modules/gl/fonts/freetype/Freetype.hpp>
#include <anex/SharedLibrary.hpp>
#include <anex/RuntimeAPI.hpp>
#include <anex/modules/gl/entities/Button.hpp>
#include <anex/modules/gl/entities/Dialog.hpp>
#include <anex/modules/gl/entities/Input.hpp>
#include <anex/modules/gl/entities/Toolbar.hpp>
#include <anex/modules/gl/entities/Panel.hpp>
#include <anex/modules/gl/entities/Tabs.hpp>
#include "Project.hpp"
namespace anex::editor
{
	struct EditorScene : GLScene
	{
		glm::vec4 editorClearColor = {0.2, 0.2, 0.2, 1};
		float toolbarHeight;
		float bottomTabsHeight;
		filesystem::File robotoRegularFile;
		modules::gl::fonts::freetype::FreetypeFont robotoRegularFont;
		GLWindow* gameWindowPointer = 0;
		GLWindow* codeWindowPointer = 0;
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
		glm::vec4 gameWindowBorderColor = {0.4, 0.4, 0.7, 1};
		glm::vec4 gameWindowHoveredBorderColor = {0.7, 0.4, 0.4, 1};
		glm::vec4 gameWindowActiveBorderColor = {1, 0, 0, 1};
		std::shared_ptr<entities::Plane> gameWindowBorder;
		std::shared_ptr<entities::PanelMenu> sceneGraphPanelMenu;
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
		Project project;
		explicit EditorScene(GLWindow& window);
		~EditorScene() override;
		void onEntityAdded(const std::shared_ptr<IEntity>& entity) const;
		void setupGameWindow();
		void setupCodeWindow();
		void minimizeWindows();
		void setupToolbarOptions();
		void loadProject();
	};
}