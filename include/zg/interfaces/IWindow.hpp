#pragma once
#include <functional>
#include <map>
#include <queue>
#include <thread>
#include <zg/glm.hpp>
#include "./IScene.hpp"
namespace zg
{
	struct Window;
	struct IWindow
	{
		float windowWidth;
		float windowHeight;
		float windowX;
		float windowY;
		uint32_t framerate = 60;
#if defined(WINDOWS) || defined(LINUX)
		std::shared_ptr<std::thread> windowThread;
#endif
		using Runnable = std::function<void(Window&)>;
		std::queue<Runnable> runnables;
		using Key = uint32_t;
		using Button = uint32_t;
		using EventIdentifier = uint32_t;
		using KeyPressHandler = std::function<void(bool)>;
		using KeyUpdateHandler = std::function<void()>;
		using AnyKeyPressHandler = std::function<void(const Key&, bool)>;
		using MousePressHandler = std::function<void(bool)>;
		using MouseMoveHandler = std::function<void(glm::vec2)>;
		std::unordered_map<Key, int> keys;
		std::unordered_map<Button, int> buttons;
		std::unordered_map<Key, std::pair<EventIdentifier, std::map<EventIdentifier, KeyPressHandler>>> keyPressHandlers;
		std::unordered_map<Key, std::pair<EventIdentifier, std::map<EventIdentifier, KeyUpdateHandler>>> keyUpdateHandlers;
		std::pair<EventIdentifier, std::map<EventIdentifier, AnyKeyPressHandler>> anyKeyPressHandlers;
		std::unordered_map<Button, std::pair<EventIdentifier, std::map<EventIdentifier, MousePressHandler>>>
			mousePressHandlers;
		std::pair<EventIdentifier, std::map<EventIdentifier, MouseMoveHandler>> mouseMoveHandlers;
		using ViewResizeHandler = std::function<void(glm::vec2)>;
		std::pair<EventIdentifier, std::map<EventIdentifier, ViewResizeHandler>> viewResizeHandlers;
		using FocusHandler = std::function<void(bool)>;
		std::pair<EventIdentifier, std::map<EventIdentifier, FocusHandler>> focusHandlers;
		std::shared_ptr<IScene> scene;
		bool open = true;
		std::chrono::steady_clock::time_point lastFrameTime;
		float deltaTime = 0;
		bool justWarpedPointer = false;
		bool borderless = false;
		bool minimized = false;
		bool maximized = false;
		bool focused = false;
		using OnEntityAddedFunction = std::function<void(const std::shared_ptr<IEntity>&)>;
		OnEntityAddedFunction onEntityAdded;
		IWindow(float windowWidth, float windowHeight, float windowX, float windowY, bool borderless, uint32_t framerate);
		virtual ~IWindow() = default;
		void run();
		virtual void startWindow() = 0;
		void update();
		virtual void preRender();
		void render();
		virtual void postRender();
		// Keyboard
		virtual void updateKeyboard() = 0;
		EventIdentifier addKeyPressHandler(Key key, const KeyPressHandler& callback);
		void removeKeyPressHandler(Key key, EventIdentifier& id);
		EventIdentifier addKeyUpdateHandler(Key key, const KeyUpdateHandler& callback);
		void removeKeyUpdateHandler(Key key, EventIdentifier& id);
		EventIdentifier addAnyKeyPressHandler(const AnyKeyPressHandler& callback);
		void removeAnyKeyPressHandler(EventIdentifier& id);
		void callKeyPressHandler(Key key, int pressed);
		void callKeyUpdateHandler(Key key);
		void callAnyKeyPressHandler(Key key, bool pressed);
		// Mouse
		virtual void updateMouse() = 0;
		EventIdentifier addMousePressHandler(Button button, const MousePressHandler& callback);
		void removeMousePressHandler(Button button, EventIdentifier& id);
		EventIdentifier addMouseMoveHandler(const MouseMoveHandler& callback);
		void removeMouseMoveHandler(EventIdentifier& id);
		void callMousePressHandler(Button button, int pressed);
		void callMouseMoveHandler(glm::vec2 coords);
		// resize
		EventIdentifier addResizeHandler(const ViewResizeHandler& callback);
		void removeResizeHandler(EventIdentifier& id);
		void callResizeHandler(glm::vec2 newSize);
		// focus
		EventIdentifier addFocusHandler(const FocusHandler& callback);
		void removeFocusHandler(EventIdentifier& id);
		void callFocusHandler(bool focused);
		std::shared_ptr<IScene> setIScene(const std::shared_ptr<IScene>& scene);
		void runOnThread(const Runnable& runnable);
		void runRunnables();
		void updateDeltaTime();
		void resize(glm::vec2 newSize);
		void registerOnEntityAddedFunction(const OnEntityAddedFunction& function);
		virtual void close();
		virtual void minimize();
		virtual void maximize();
		virtual void restore();
		virtual void drawLine(int x0, int y0, int x1, int y1, uint32_t color) = 0;
		virtual void drawRectangle(int x, int y, int w, int h, uint32_t color) = 0;
		virtual void drawCircle(int x, int y, int radius, uint32_t color) = 0;
		virtual void drawText(int x, int y, const char* text, int scale, uint32_t color) = 0;
		virtual void warpPointer(glm::vec2 coords);
		virtual void setXY(float x, float y);
		virtual void setWidthHeight(float width, float height);
		virtual Window& createChildWindow(const char* title, IScene& scene, float windowWidth, float windowHeight,
																			float windowX, float windowY, bool NDCFramebufferPlane);
	};
} // namespace zg
