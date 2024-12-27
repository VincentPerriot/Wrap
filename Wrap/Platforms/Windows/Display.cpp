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
Display& Display::Instance( u32 _width, u32 _height, const std::string& _title )
{
	if ( m_SingletonDisplay == nullptr )
	{
		m_SingletonDisplay = new Display( _width, _height, _title );
	}
	return *m_SingletonDisplay;
}

//--------------------------------------------------------------------------------
Display& Display::Instance()
{
	assert( m_SingletonDisplay );
	return *m_SingletonDisplay;
}

//--------------------------------------------------------------------------------
void Display::createWindow()
{
	glfwInit();

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );

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

	glfwSetFramebufferSizeCallback( m_pWindow, []( GLFWwindow* _window, int _width, int _height ) {
		Engine::SwapChain::m_BufferResized = true;
		Config::WINDOW_WIDTH = (u32)_width;
		Config::WINDOW_HEIGHT = (u32)_height;
	} );
}

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

