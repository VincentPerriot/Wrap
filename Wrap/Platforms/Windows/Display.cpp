#include "Display.h"

//--------------------------------------------------------------------------------
//------------ Basic GLFW Display To connect to apps -----------------------------
//--------------------------------------------------------------------------------
Display::Display( u32 _width, u32 _height, const std::string& _title )
	: m_Title( _title )
	, m_Width( _width )
	, m_Height( _height )
{
	createWindow();
}

//--------------------------------------------------------------------------------
Display::~Display()
{
	glfwDestroyWindow( m_pWindow );
	glfwTerminate();
}

//--------------------------------------------------------------------------------
void Display::createWindow()
{
	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	m_pWindow = glfwCreateWindow( m_Width, m_Height, m_Title.c_str(), nullptr, nullptr );

	if ( !m_pWindow )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback( m_pWindow, []( GLFWwindow* window, int key, int scancode, int action, int mods )
		{
			if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
				glfwSetWindowShouldClose( window, GLFW_TRUE );
		}
	);

}

// Could leave these 2 as direct calls in the App class ( see later what is more convenient )
//--------------------------------------------------------------------------------
bool Display::shouldClose() const
{
	return glfwWindowShouldClose( m_pWindow );
}

//--------------------------------------------------------------------------------
void Display::pollEvents()
{
	glfwPollEvents();
}
