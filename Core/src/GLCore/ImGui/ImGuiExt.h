#pragma once

#include <GLCore.h>
#include <GLCore/Core/Core.h>

#include "ViewportDockspace.h"

namespace ImGuiExt
{
void Help(const char *desc);
bool Knob(const char *label, float *p_value, float v_min = 0.0f, float v_max = 1.0f, float step = 0.0f, const char *fmt = "%.3f");
bool KnobInt(const char *label, float *p_value, int v_min, int v_max, float step = 1.0f, const char *fmt = "%d");

void HelpMarker(const char *desc);

void Separator(float height = 0.0f);

void DisableCursor();
}

