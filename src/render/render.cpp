#include <iostream>

#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"
#include "render.hpp"

void render(){
    ImGui::Begin("Test");
    ImGui::Text("Test text");
    ImGui::End();
}