#pragma once

#include "../../Utils/Common.h"
#include "../../Warp/TriangleApp.h"
#include "../../Engine/SwapChain.h"

#include <GLFW/glfw3.h>

namespace Config {
	inline u32 WINDOW_WIDTH = 1920;
	inline u32 WINDOW_HEIGHT = 1080;
}

class Display {

public:

	static Display& Instance( u32 _width, u32 _height, const std::string& _title );
	static Display& Instance();

	Display( const Display& _other ) = delete;
	Display& operator=( const Display& _other ) = delete;

	~Display();

	void createWindow();
	bool shouldClose() const;
	void pollEvents();

	GLFWwindow* getWindowPtr() { return m_pWindow; };

private:
	Display( u32 _width, u32 _height, const std::string& _title );
	const std::string m_Title{};

	u32 m_Width = 0;
	u32 m_Height = 0;

	GLFWwindow* m_pWindow;
	inline static Display* m_SingletonDisplay = nullptr;
};