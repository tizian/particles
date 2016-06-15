#include <SFML/Graphics/Color.hpp>

#include <imgui.h>

namespace ImGui
{
    bool ColorEdit(const char *label, sf::Color *col, bool show_alpha = true);
    bool SliderFloat2(const char *label, sf::Vector2f *vec, float v_min, float v_max, const char *display_format = "%.3f", float power = 1.0f);
}