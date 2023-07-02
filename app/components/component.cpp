#include "imgui.h"
#include "component.hpp"
#include "toolset.hpp"
#include <cstring>
#include <cstdio>
#include <implot.h>
#include <string>

//TODO: handle dynamic component attribution(generating)
namespace Component {
	void Sidebar_Init()
	{
		ImGui::Begin("Plots");
        if (ImGui::BeginTabBar("Stats")) {

            if (ImGui::BeginTabItem("Magnitude")) {
                static const double co = -3;
                if (ImPlot::BeginPlot("##Bode1", ImVec2(-1, -1))) {
                    ImPlot::SetupAxesLimits(1, 500, -100, 10);
                    ImPlot::SetupAxes("Frequency [Hz]", "Magnitude [dB]");
                    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
                    ImPlot::SetNextLineStyle({ 1,1,1,1 });
                    ImPlot::PlotInfLines("##3dB", &co, 1, ImPlotInfLinesFlags_Horizontal);
                    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.250f);
                    /*ImPlot::PlotShaded("##Mag1", result.f.data(), result.mag.data(), (int)result.f.size(), -INFINITY);
                    ImPlot::PlotLine("##Mag2", result.f.data(), result.mag.data(), (int)result.f.size());*/
                   /* ImPlot::Annotation(Fc[0], -3, ImVec4(0.15f, 0.15f, 0.15f, 1), ImVec2(5, -5), true, "Half-Power Point");
                    if (ImPlot::DragLineX(148884, &Fc[0], ImVec4(0.15f, 0.15f, 0.15f, 1)))
                        filt_need_update = true;*/
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Phase")) {
                if (ImPlot::BeginPlot("##Bode2", ImVec2(-1, -1))) {
                    ImPlot::SetupAxesLimits(1, 500, -180, 10);
                    ImPlot::SetupAxes("Frequency [Hz]", "Phase Angle [deg]");
                    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
                    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.250f);
                  /*  ImPlot::PlotShaded("##Phase1", result.f.data(), result.phase.data(), (int)result.f.size(), -INFINITY);
                    ImPlot::PlotLine("##Phase2", result.f.data(), result.phase.data(), (int)result.f.size());
                    if (ImPlot::DragLineX(439829, &Fc[0], ImVec4(0.15f, 0.15f, 0.15f, 1)))
                        filt_need_update = true;*/
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Amplitude")) {
                if (ImPlot::BeginPlot("##Amp", ImVec2(-1, -1))) {
                    ImPlot::SetupAxesLimits(1, 500, 0, 1.0);
                    ImPlot::SetupAxes("Frequency [Hz]", "Amplitude [dB]");
                    ImPlot::SetupLegend(ImPlotLocation_NorthEast);
                    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
                    /*ImPlot::PlotShaded("x(f)", result.f.data(), result.ampx.data(), (int)result.f.size(), -INFINITY);
                    ImPlot::PlotLine("x(f)", result.f.data(), result.ampx.data(), (int)result.f.size());
                    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
                    ImPlot::PlotShaded("y(f)", result.f.data(), result.ampy.data(), (int)result.f.size(), -INFINITY);
                    ImPlot::PlotLine("y(f)", result.f.data(), result.ampy.data(), (int)result.f.size());
                    if (ImPlot::DragLineX(397391, &Fc[0], ImVec4(0.15f, 0.15f, 0.15f, 1)))
                        filt_need_update = true;

                    if (ImPlot::DragLineY(939031, &a[0], ImVec4(0.15f, 0.15f, 0.15f, 1)))
                        signal_need_update = true;
                    if (ImPlot::DragLineY(183853, &a[1], ImVec4(0.15f, 0.15f, 0.15f, 1)))
                        signal_need_update = true;*/
                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Power")) {
                /*static std::vector<double> pxx10, pyy10;
                pxx10.resize(result.pxx.size());
                pyy10.resize(result.pyy.size());*/
            /*    for (int i = 0; i < pxx10.size(); ++i) {
                    pxx10[i] = 10 * std::log10(result.pxx[i]);
                    pyy10[i] = 10 * std::log10(result.pyy[i]);
                }*/
                if (ImPlot::BeginPlot("##Power", ImVec2(-1, -1))) {
                    ImPlot::SetupAxesLimits(1, 500, -100, 0);
                    ImPlot::SetupAxes("Frequency [Hz]", "Power Spectral Density (dB/Hz)");
                    ImPlot::SetupLegend(ImPlotLocation_NorthEast);
                    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
                   /* ImPlot::PlotShaded("x(f)", result.f.data(), pxx10.data(), (int)result.f.size(), -INFINITY);
                    ImPlot::PlotLine("x(f)", result.f.data(), pxx10.data(), (int)result.f.size());*/
                    ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
                    /*ImPlot::PlotShaded("y(f)", result.f.data(), pyy10.data(), (int)result.f.size(), -INFINITY);
                    ImPlot::PlotLine("y(f)", result.f.data(), pyy10.data(), (int)result.f.size());*/

                    ImPlot::EndPlot();
                }
                ImGui::EndTabItem();
            }


            ImGui::EndTabBar();
        }

		ImGui::End();
	}
    void MainView()
    {

        
        Sidebar_Init();
        Toolset::FlasherView();
        
    }
}
