#include "LauncherLayer.h"
#include "imgui/imgui_internal.h"

#include "RockEngine/ImGui/ImGuiLayer.h"

#include "RockEngine/Utilities/FileSystem.h"

namespace Launcher
{
#define MAX_PROJECT_NAME_LENGTH 255
#define MAX_PROJECT_FILEPATH_LENGTH 512

	static char* s_ProjectNameBuffer = new char[MAX_PROJECT_NAME_LENGTH];
	static char* s_ProjectFilePathBuffer = new char[MAX_PROJECT_FILEPATH_LENGTH];

	static std::string GetDateTimeString(const time_t& input_time, const std::locale& loc, char fmt)
	{
		const std::time_put<char>& tmput = std::use_facet <std::time_put<char> >(loc);

		std::stringstream s;
		s.imbue(loc);

		tm time;
		localtime_s(&time, &input_time);
		tmput.put(s, s, ' ', &time, fmt);

		return s.str();
	}

	static std::string FormatDateAndTime(time_t dateTime) { return GetDateTimeString(dateTime, std::locale(""), 'R') + " " + GetDateTimeString(dateTime, std::locale(""), 'x'); }

	LauncherLayer::LauncherLayer(const LauncherProperties& properties)
		: m_Properties(properties), m_HoveredProjectID(0)
	{
		memset(s_ProjectNameBuffer, 0, MAX_PROJECT_NAME_LENGTH);
		memset(s_ProjectFilePathBuffer, 0, MAX_PROJECT_FILEPATH_LENGTH);
	}

	LauncherLayer::~LauncherLayer()
	{
	}

    void LauncherLayer::OnAttach()
    {
		RockEngine::ImGuiLayer::SetDarkTheme();
    }

	void LauncherLayer::OnImGuiRender()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("Launcher", 0, window_flags);

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		auto largeFont = io.Fonts->Fonts[1];
		ImGui::PopStyleVar(2);

		{
			if ((m_Properties.InstallPath.empty() || !RockEngine::Utils::FileSystem::Exists(m_Properties.InstallPath)) && !ImGui::IsPopupOpen("Select Hazel Install"))
			{
				ImGui::OpenPopup("Select TheRock Install");
				m_Properties.InstallPath.reserve(MAX_PROJECT_FILEPATH_LENGTH);
			}

			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2(700, 0));
			if (ImGui::BeginPopupModal("Select TheRock Install", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
			{
				ImGui::PushFont(boldFont);
				ImGui::TextUnformatted("Failed to find an appropiate TheRock installation!");
				ImGui::PopFont();

				ImGui::TextWrapped("Please select the root folder for the TheRock version you want to use (E.g C:/TheRock)."
					"You should be able to find a file called premake5.lua in the root folder."
					"The install you select will be used when creating new projects.");

				ImGui::Dummy(ImVec2(0, 8));

				ImVec2 label_size = ImGui::CalcTextSize("...", NULL, true);
				auto& style = ImGui::GetStyle();
				ImVec2 button_size = ImGui::CalcItemSize(ImVec2(0, 0), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 10));
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
				ImGui::SetNextItemWidth(700 - button_size.x - style.FramePadding.x * 2.0f - style.ItemInnerSpacing.x - 1);
				ImGui::InputTextWithHint("##install_location", "C:'\'dev", m_Properties.InstallPath.data(), MAX_PROJECT_FILEPATH_LENGTH, ImGuiInputTextFlags_ReadOnly);
				ImGui::SameLine();
				if (ImGui::Button("..."))
				{
					std::string result = RockEngine::Utils::FileSystem::OpenFolderDialog().string();
					m_Properties.InstallPath.assign(result);
				}

				if (ImGui::Button("Confirm"))
				{
#ifdef RE_PLATFORM_WINDOWS
					std::replace(m_Properties.InstallPath.begin(), m_Properties.InstallPath.end(), '/', '\\'); // Only windows
#endif // Windows only
					bool success = RockEngine::Utils::FileSystem::SetEnvironmentVariable("THEROCK_DIR", m_Properties.InstallPath);
					RE_CORE_ASSERT(success, "Failed to set Environment Variable!");
					ImGui::CloseCurrentPopup();
				}

				ImGui::PopStyleVar(2);

				ImGui::EndPopup();

			}
		}
		ImGui::Columns(2, nullptr,false);
		ImGui::SetColumnWidth(0, viewport->Size.x / 1.5f);

		static std::string s_ProjectToOpen = "";

		bool showNewProjectPopup = false;
		bool serializePreferences = false;

		// Seach bar and title for recent projects
		{

			std::string searchRecentProjects = "";
			ImGui::SetCursorPosX(40.0f);
			ImGui::SetCursorPosY(40.0f);

			ImGui::PushFont(largeFont);
			ImGui::Text("Recent Projects");
			ImGui::PopFont();
			ImGui::Dummy({ 40.f,40.f});
			ImGui::Indent(40.f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 20);
			ImGui::PushFont(boldFont);
			ImGui::InputTextWithHint("##search_recent_projects", "Search recent projects", searchRecentProjects.data(), MAX_PROJECT_NAME_LENGTH);
			ImGui::PopFont();
			ImGui::PopStyleVar(2);
		}
		// Info Area
		ImGui::BeginChild("info_area");
		{
			float columnWidth = ImGui::GetColumnWidth();
			float columnCenterX = columnWidth / 2.0f;

			float imageSize = 160.0f;
			ImGui::SetCursorPosY(-40.0f);

			ImGui::Separator();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + imageSize / 3.0f);

			ImGui::BeginChild("RecentProjects");

			float projectButtonWidth = columnWidth - 60.0f;
			ImGui::SetCursorPosX(20.0f);
			ImGui::BeginGroup();

			bool anyFrameHovered = false;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 5));
			auto& recentProjects = m_Properties.UserPreferences->RecentProjects;
			for (auto it = recentProjects.begin(); it != recentProjects.end(); it++)
			{
				time_t lastOpened = it->first;
				auto& recentProject = it->second;

				// Custom button rendering to allow for multiple text elements inside a button
				std::string fullID = "Project_" + recentProject.FilePath;
				ImGuiID id = ImGui::GetID(fullID.c_str());

				bool changedColor = false;
				if (id == m_HoveredProjectID)
				{
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
					changedColor = true;
				}

				ImGui::BeginChildFrame(id, ImVec2(projectButtonWidth, 50));
				{
					float leftEdge = ImGui::GetCursorPosX();

					ImGui::PushFont(boldFont);
					ImGui::TextUnformatted(recentProject.Name.c_str());
					ImGui::PopFont();

					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.85f, 0.85f, 1.0f));
					ImGui::TextUnformatted(recentProject.FilePath.c_str());

					float prevX = ImGui::GetCursorPosX();
					ImGui::SameLine();
					std::string lastOpenedString = FormatDateAndTime(lastOpened);
					ImGui::SetCursorPosX(leftEdge + projectButtonWidth - ImGui::CalcTextSize(lastOpenedString.c_str()).x - ImGui::GetStyle().FramePadding.x * 1.5f);
					ImGui::TextUnformatted(lastOpenedString.c_str());
					ImGui::PopStyleColor();

					ImGui::SetCursorPosX(prevX);

					if (ImGui::IsWindowHovered())
					{
						anyFrameHovered = true;
						m_HoveredProjectID = id;

						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							s_ProjectToOpen = recentProject.FilePath;
							recentProject.LastOpened = time(NULL);
						}
					}

					if (ImGui::BeginPopupContextWindow("project_context_window"))
					{
						bool isStartupProject = m_Properties.UserPreferences->StartupProject == recentProject.FilePath;
						if (ImGui::MenuItem("Set Startup Project", nullptr, &isStartupProject))
						{
							m_Properties.UserPreferences->StartupProject = isStartupProject ? recentProject.FilePath : "";
							serializePreferences = true;
						}

						if (ImGui::MenuItem("Remove From List"))
						{
							if (isStartupProject)
								m_Properties.UserPreferences->StartupProject = "";
							it = recentProjects.erase(it);
						}

						ImGui::EndPopup();
					}
				}
				ImGui::EndChildFrame();

				if (changedColor)
					ImGui::PopStyleColor();
			}
			ImGui::PopStyleVar();

			if (!anyFrameHovered)
				m_HoveredProjectID = 0;

			ImGui::EndGroup();
			ImGui::EndChild();
		}
		ImGui::EndChild();

		ImGui::NextColumn();

		ImGui::BeginChild("general_area");
		{
			float columnWidth = ImGui::GetColumnWidth();
			float buttonWidth = columnWidth / 1.5f;
			float columnCenterX = columnWidth / 2.0f;

			ImGui::SetCursorPosX(columnCenterX - buttonWidth / 2.0f);
			ImGui::SetCursorPosY(40.f);

			ImGui::PushFont(largeFont);
			ImGui::Text("Get Started");
			ImGui::PopFont();

			ImGui::PushFont(boldFont);

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20 + 50);

			ImGui::BeginGroup();

			if (ImGui::Button("New Project", ImVec2(buttonWidth, 50)))
				showNewProjectPopup = true;

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

			if (ImGui::Button("Open Project...", ImVec2(buttonWidth, 50)))
			{
				std::string result = RockEngine::Utils::FileSystem::OpenFileDialog("TheRock Project (*.treproj)\0*.treproj\0").string();
				//AddProjectToRecents(result);
				s_ProjectToOpen = result;
			}

			ImGui::PopFont();
			ImGui::EndGroup();
		}

		//Change installation path
		{
			ImVec2 label_size = ImGui::CalcTextSize("...", NULL, true);
			auto& style = ImGui::GetStyle();
			ImVec2 button_size = ImGui::CalcItemSize(ImVec2(0, 0), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
			ImGui::SetCursorPosY(viewport->Size.y - button_size.y * 4);

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 10));
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
			ImGui::SetNextItemWidth(400 - button_size.x - style.FramePadding.x * 2.0f - style.ItemInnerSpacing.x - 1);
			//ImGui::PushFont(boldFont);

			ImGui::InputTextWithHint("##install_location", "C:'\'dev", m_Properties.InstallPath.data(), MAX_PROJECT_FILEPATH_LENGTH, ImGuiInputTextFlags_ReadOnly);
			ImGui::SameLine();
			if (ImGui::Button("..."))
			{
				std::string result = RockEngine::Utils::FileSystem::OpenFolderDialog().string();
				m_Properties.InstallPath.assign(result);
			}
			//ImGui::PopFont();
			ImGui::PopStyleVar(2);
		}

		ImGui::EndChild();

		if (showNewProjectPopup)
		{
			ImGui::OpenPopup("New Project");
			memset(s_ProjectNameBuffer, 0, MAX_PROJECT_NAME_LENGTH);
			memset(s_ProjectFilePathBuffer, 0, MAX_PROJECT_FILEPATH_LENGTH);
			showNewProjectPopup = false;
		}

		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2{ 700, 325 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 10));
		if (ImGui::BeginPopupModal("New Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 325 / 8);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

			// ...
		}
		ImGui::PopStyleVar(2);


		ImGui::End();

		if (!s_ProjectToOpen.empty())
		{
			//m_Properties.ProjectOpenedCallback(s_ProjectToOpen);
			s_ProjectToOpen = "";
			serializePreferences = true;
		}

		if (serializePreferences)
		{
			RockEngine::UserPreferencesSerializer serializer(m_Properties.UserPreferences);
			serializer.Serialize(m_Properties.UserPreferences->FilePath);
		}
	}
}
