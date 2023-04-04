#pragma once

#include "RockEngine/Core/Application.h"
#include "RockEngine/Core/Log.h"
#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/Buffer.h"
#include "RockEngine/Core/Input.h"

#include "RockEngine/Core/Events/Event.h"
#include "RockEngine/Core/Events/ApplicationEvent.h"
#include "RockEngine/Core/Events/KeyEvent.h"
#include "RockEngine/Core/Events/MouseEvent.h"

//---------------------------------------------

// --- TheRock Renderer API -------------------

//#include "RockEngine/Renderer/RendererAPI.h"
#include "ImGui/imgui.h"

#include "RockEngine/Renderer/Renderer.h"
#include "RockEngine/Renderer/IndexBuffer.h"
#include "RockEngine/Renderer/VertexBuffer.h"
#include "RockEngine/Renderer/Shader.h"
#include "RockEngine/Renderer/Framebuffer.h"
#include "RockEngine/Renderer/Texture.h"
#include "RockEngine/Renderer/Pipeline.h"
#include "RockEngine/Renderer/Camera.h"
#include "RockEngine/Renderer/Mesh.h"
#include "RockEngine/Renderer/Material.h"
#include "RockEngine/Renderer/RenderPass.h"
#include "RockEngine/Renderer/SceneRenderer.h"
#include "RockEngine/Renderer/Renderer2D.h"
//---------------------------------------------


// Scenes
#include "RockEngine/Scene/Scene.h"
#include "RockEngine/Scene/Entity.h"

// Editor
#include "RockEngine/Editor/SceneHierarchyPanel.h"