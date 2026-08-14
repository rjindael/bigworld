/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef UI_H
#define UI_H

#include <microui.h>

typedef struct sb_Renderer sb_Renderer;

typedef struct sb_UI {
    mu_Context ctx;
    int initialized;
} sb_UI;

// Lifecycle
void ui_init(sb_UI* ui);
void ui_shutdown(sb_UI* ui);
void ui_begin(sb_UI* ui);
void ui_end(sb_UI* ui);
void ui_render(sb_UI* ui, sb_Renderer* renderer);

// Input handling
void ui_input_mousemove(sb_UI* ui, int x, int y);
void ui_input_mousedown(sb_UI* ui, int x, int y, int btn);
void ui_input_mouseup(sb_UI* ui, int x, int y, int btn);
void ui_input_scroll(sb_UI* ui, int x, int y);
void ui_input_keydown(sb_UI* ui, int key);
void ui_input_keyup(sb_UI* ui, int key);
void ui_input_text(sb_UI* ui, const char* text);

// Sample UI windows
void ui_draw_demo_window(sb_UI* ui);
void ui_draw_scene_window(sb_UI* ui, int object_count);

// rendering callbacks for microui
void ui_draw_rect(sb_Renderer* renderer, mu_Rect rect, mu_Color color);
void ui_draw_text(sb_Renderer* renderer, const char* text, mu_Vec2 pos, mu_Color color);
void ui_draw_icon(sb_Renderer* renderer, int id, mu_Rect rect, mu_Color color);
int ui_get_text_width(const char* text, int len);
int ui_get_text_height(void);
void ui_render_set_clip_rect(sb_Renderer* renderer, mu_Rect rect);

#endif // UI_H
