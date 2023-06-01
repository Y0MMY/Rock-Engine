#include "pch.h"
#include "Window.h"
#include "RockEngine/Platform/Windows/WindowsWindow.h"

namespace RockEngine
{
	Window* Window::Create(const WindowSpecification& props /* = WindowProps() */)
	{
		return new WindowsWindow(props);
	}
}