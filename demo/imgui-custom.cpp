#include "imgui-custom.h"
#include "imgui_internal.h"

namespace ImGui
{
   	bool ImGui::ColorEdit(const char *label, sf::Color *col, bool show_alpha) {
		float col4[4];
		col4[0] = (float)col->r / 255.f;
		col4[1] = (float)col->g / 255.f;
		col4[2] = (float)col->b / 255.f;
		col4[3] = (float)col->a / 255.f;
		const bool value_changed = ColorEdit4(label, col4, show_alpha);
		col->r = (sf::Uint8)(col4[0] * 255.f);
		col->g = (sf::Uint8)(col4[1] * 255.f);
		col->b = (sf::Uint8)(col4[2] * 255.f);
		col->a = (sf::Uint8)(col4[3] * 255.f);
		return value_changed;
	}

	bool ImGui::SliderFloat2(const char *label, sf::Vector2f *vec, float v_min, float v_max, const char *display_format, float power) {
		float vec2[2];
		vec2[0] = vec->x;
		vec2[1] = vec->y;
	    const bool value_changed = SliderFloatN(label, vec2, 2, v_min, v_max, display_format, power);
	    vec->x = vec2[0];
	    vec->y = vec2[1];
	    return value_changed;
	}
}
