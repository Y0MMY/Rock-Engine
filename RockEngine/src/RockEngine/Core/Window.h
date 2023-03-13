#pragma once

namespace RockEngine
{
	struct WindowProps{
		std::string Title;
		uint32_t Width, Height;

		WindowProps(const std::string& title = "Window", unsigned int width = 900, unsigned int height = 900)
			: Title(title), Width(width), Height(height) {}
	};

	class Window
	{
	public:
		virtual ~Window(){}

		// Windows attributes
		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		// Instance of window
		static Window* Create(const WindowProps& props = WindowProps());
	};
}