#pragma once

#include "../Utils/Common.h"

#include <GLFW/glfw3.h>

class Display {

public:
	Display( u32 _width, u32 _height, const std::string& _title );
	Display() = delete;

	~Display();

	void createWindow();
	bool shouldClose() const;
	void pollEvents();

	void cleanUp();

private:
	const std::string m_Title{};

	u32 m_Width = 0;
	u32 m_Height = 0;

	GLFWwindow* m_pWindow;
};