#pragma once

#include "../../Utils/Common.h"

#include <GLFW/glfw3.h>

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
	static Display* m_SingletonDisplay;
};