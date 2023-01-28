#include <iostream>

#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"
#include "render.hpp"
void initialize_docking()
{
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    if (opt_fullscreen)
    {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        const ImGuiViewport *imguiViewport = ImGui::GetMainViewport();

        ImGuiID dockspace_id = ImGui::GetID("Cyclops View");


        if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);

            ImGui::DockBuilderSetNodeSize(dockspace_id, imguiViewport->Size);

            ImGuiID dock_main_id =
                dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
            ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id,
                                                               ImGuiDir_Left,
                                                               0.20f,
                                                               NULL,
                                                               &dock_main_id);
            ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id,
                                                                 ImGuiDir_Down,
                                                                 0.20f,
                                                                 NULL,
                                                                 &dock_main_id);
            ImGuiID dock_id_centroid =
                ImGui::DockBuilderSplitNode(dock_main_id,
                                            ImGuiDir_Up,
                                            0.20f,
                                            NULL,
                                            &dock_main_id);
            
            ImGui::DockBuilderDockWindow("Viewport", dock_id_centroid);
            ImGui::DockBuilderDockWindow("Current Stats", dock_id_bottom);
            ImGui::DockBuilderDockWindow("Properties", dock_id_prop);
            ImGui::DockBuilderDockWindow("Mesh", dock_id_prop);
            ImGui::DockBuilderDockWindow("Extra", dock_id_prop);
            ImGui::DockBuilderDockWindow("Phase Graph", dock_id_centroid);

            ImGui::DockBuilderFinish(dockspace_id);
            ImGui::DockBuilderFinish(dock_main_id);
        }
    }
    ImGui::End();
}
void render()
{
    initialize_docking();
}
