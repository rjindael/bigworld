// SPDX-License-Identifier: GPL-2.0-or-later
#include "ui.h"
#include "shambhala.h"
#include "render/engine.h"
#include <stdio.h>
#include <string.h>

static  char logbuf[64000];
static   int logbuf_updated = 0;
static float bg[3] = { 90, 95, 100 };


static void write_log(const char *text) {
  if (logbuf[0]) { strcat(logbuf, "\n"); }
  strcat(logbuf, text);
  logbuf_updated = 1;
}
// Glyph widths come from microui's atlas (see engine.c / atlas.inl) rather
// than a fixed advance, since the built-in font isn't monospace.
static int text_width_callback(mu_Font font, const char* text, int len)
{
    (void)font;
    if (len == -1) {
        len = strlen(text);
    }

    int width = 0;
    for (int i = 0; i < len; i++) {
        unsigned char ch = (unsigned char)text[i];
        if ((ch & 0xc0) == 0x80) {
            continue; // UTF-8 continuation byte
        }
        int glyph = ch < 127 ? ch : 127;
        width += render_atlas_rect(RENDER_ATLAS_FONT + glyph).w;
    }
    return width;
}

static int text_height_callback(mu_Font font)
{
    (void)font;
    return 18;
}

void ui_init(sb_UI* ui)
{
    memset(ui, 0, sizeof(sb_UI));
    mu_init(&ui->ctx);
    ui->ctx.text_width = text_width_callback;
    ui->ctx.text_height = text_height_callback;
    ui->initialized = 1;
}

void ui_shutdown(sb_UI* ui)
{
    ui->initialized = 0;
}

void ui_begin(sb_UI* ui)
{
    mu_begin(&ui->ctx);
}

void ui_end(sb_UI* ui)
{
    mu_end(&ui->ctx);
}

void ui_render(sb_UI* ui, sb_Renderer* renderer)
{
    mu_Command* cmd = NULL;
    while (mu_next_command(&ui->ctx, &cmd)) {
        switch (cmd->type) {
        case MU_COMMAND_RECT:
            ui_draw_rect(renderer, cmd->rect.rect, cmd->rect.color);
            break;
        case MU_COMMAND_TEXT:
            ui_draw_text(renderer, cmd->text.str, cmd->text.pos, cmd->text.color);
            break;
        case MU_COMMAND_ICON:
            ui_draw_icon(renderer, cmd->icon.id, cmd->icon.rect, cmd->icon.color);
            break;
        case MU_COMMAND_CLIP:
            ui_render_set_clip_rect(renderer, cmd->clip.rect);
            break;
        }
    }
}

// Input handling
void ui_input_mousemove(sb_UI* ui, int x, int y)
{
    mu_input_mousemove(&ui->ctx, x, y);
}

void ui_input_mousedown(sb_UI* ui, int x, int y, int btn)
{
    mu_input_mousedown(&ui->ctx, x, y, btn);
}

void ui_input_mouseup(sb_UI* ui, int x, int y, int btn)
{
    mu_input_mouseup(&ui->ctx, x, y, btn);
}

void ui_input_scroll(sb_UI* ui, int x, int y)
{
    mu_input_scroll(&ui->ctx, x, y);
}

void ui_input_keydown(sb_UI* ui, int key)
{
    mu_input_keydown(&ui->ctx, key);
}

void ui_input_keyup(sb_UI* ui, int key)
{
    mu_input_keyup(&ui->ctx, key);
}

void ui_input_text(sb_UI* ui, const char* text)
{
    mu_input_text(&ui->ctx, text);
}

void ui_draw_demo_window(sb_UI* ui)
{
    // All microui window/widget calls must happen between ui_begin()/ui_end()
    // (mu_begin()/mu_end()) each frame -- ui_draw_demo_window() is called from
    // shambhala_update() in exactly that window, which is why this code lives
    // here and not in ui_init() (calling it once at startup, outside any
    // mu_begin()/mu_end() frame, left the container/id stacks in a state
    // mu_begin_window() assumes never happens, tripping the 'n > -1' assert
    // in microui.c).
    if (mu_begin_window(&ui->ctx, "Demo Window", mu_rect(40, 40, 300, 450))) {
        mu_Container* win = mu_get_current_container(&ui->ctx);
        win->rect.w = mu_max(win->rect.w, 240);
        win->rect.h = mu_max(win->rect.h, 300);

        /* window info */
        if (mu_header(&ui->ctx, "Window Info")) {
            mu_Container* win = mu_get_current_container(&ui->ctx);
            char buf[64];
            mu_layout_row(&ui->ctx, 2, (int[]) { 54, -1 }, 0);
            mu_label(&ui->ctx, "Position:");
            sprintf(buf, "%d, %d", win->rect.x, win->rect.y);
            mu_label(&ui->ctx, buf);
            mu_label(&ui->ctx, "Size:");
            sprintf(buf, "%d, %d", win->rect.w, win->rect.h);
            mu_label(&ui->ctx, buf);
        }

        /* labels + buttons */
        if (mu_header_ex(&ui->ctx, "Test Buttons", MU_OPT_EXPANDED)) {
            mu_layout_row(&ui->ctx, 3, (int[]) { 86, -110, -1 }, 0);
            mu_label(&ui->ctx, "Test buttons 1:");
            if (mu_button(&ui->ctx, "Button 1")) {
                write_log("Pressed button 1");
            }
            if (mu_button(&ui->ctx, "Button 2")) {
                write_log("Pressed button 2");
            }
            mu_label(&ui->ctx, "Test buttons 2:");
            if (mu_button(&ui->ctx, "Button 3")) {
                write_log("Pressed button 3");
            }
            if (mu_button(&ui->ctx, "Popup")) {
                mu_open_popup(&ui->ctx, "Test Popup");
            }
            if (mu_begin_popup(&ui->ctx, "Test Popup")) {
                mu_button(&ui->ctx, "Hello");
                mu_button(&ui->ctx, "World");
                mu_end_popup(&ui->ctx);
            }
        }

        /* tree */
        if (mu_header_ex(&ui->ctx, "Tree and Text", MU_OPT_EXPANDED)) {
            mu_layout_row(&ui->ctx, 2, (int[]) { 140, -1 }, 0);
            mu_layout_begin_column(&ui->ctx);
            if (mu_begin_treenode(&ui->ctx, "Test 1")) {
                if (mu_begin_treenode(&ui->ctx, "Test 1a")) {
                    mu_label(&ui->ctx, "Hello");
                    mu_label(&ui->ctx, "world");
                    mu_end_treenode(&ui->ctx);
                }
                if (mu_begin_treenode(&ui->ctx, "Test 1b")) {
                    if (mu_button(&ui->ctx, "Button 1")) {
                        write_log("Pressed button 1");
                    }
                    if (mu_button(&ui->ctx, "Button 2")) {
                        write_log("Pressed button 2");
                    }
                    mu_end_treenode(&ui->ctx);
                }
                mu_end_treenode(&ui->ctx);
            }
            if (mu_begin_treenode(&ui->ctx, "Test 2")) {
                mu_layout_row(&ui->ctx, 2, (int[]) { 54, 54 }, 0);
                if (mu_button(&ui->ctx, "Button 3")) {
                    write_log("Pressed button 3");
                }
                if (mu_button(&ui->ctx, "Button 4")) {
                    write_log("Pressed button 4");
                }
                if (mu_button(&ui->ctx, "Button 5")) {
                    write_log("Pressed button 5");
                }
                if (mu_button(&ui->ctx, "Button 6")) {
                    write_log("Pressed button 6");
                }
                mu_end_treenode(&ui->ctx);
            }
            if (mu_begin_treenode(&ui->ctx, "Test 3")) {
                static int checks[3] = { 1, 0, 1 };
                mu_checkbox(&ui->ctx, "Checkbox 1", &checks[0]);
                mu_checkbox(&ui->ctx, "Checkbox 2", &checks[1]);
                mu_checkbox(&ui->ctx, "Checkbox 3", &checks[2]);
                mu_end_treenode(&ui->ctx);
            }
            mu_layout_end_column(&ui->ctx);

            mu_layout_begin_column(&ui->ctx);
            mu_layout_row(&ui->ctx, 1, (int[]) { -1 }, 0);
            mu_text(&ui->ctx, "Lorem ipsum dolor sit amet, consectetur adipiscing "
                              "elit. Maecenas lacinia, sem eu lacinia molestie, mi risus faucibus "
                              "ipsum, eu varius magna felis a nulla.");
            mu_layout_end_column(&ui->ctx);
        }

        /* background color sliders */
        if (mu_header_ex(&ui->ctx, "Background Color", MU_OPT_EXPANDED)) {
            mu_layout_row(&ui->ctx, 2, (int[]) { -78, -1 }, 74);
            /* sliders */
            mu_layout_begin_column(&ui->ctx);
            mu_layout_row(&ui->ctx, 2, (int[]) { 46, -1 }, 0);
            mu_label(&ui->ctx, "Red:");
            mu_slider(&ui->ctx, &bg[0], 0, 255);
            mu_label(&ui->ctx, "Green:");
            mu_slider(&ui->ctx, &bg[1], 0, 255);
            mu_label(&ui->ctx, "Blue:");
            mu_slider(&ui->ctx, &bg[2], 0, 255);
            mu_layout_end_column(&ui->ctx);
            /* color preview */
            mu_Rect r = mu_layout_next(&ui->ctx);
            mu_draw_rect(&ui->ctx, r, mu_color(bg[0], bg[1], bg[2], 255));
            char buf[32];
            sprintf(buf, "#%02X%02X%02X", (int)bg[0], (int)bg[1], (int)bg[2]);
            mu_draw_control_text(&ui->ctx, buf, r, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
        }

        mu_end_window(&ui->ctx);
    }
}

void ui_draw_scene_window(sb_UI* ui, int object_count)
{
    (void)ui;
    (void)object_count;
}

void ui_draw_rect(sb_Renderer* renderer, mu_Rect rect, mu_Color color)
{
    // Sampling the atlas's opaque ATLAS_WHITE patch gives full coverage, so
    // plain flat-colored rects go through the same textured pipeline as text/icons.
    render_push_quad(renderer, rect, render_atlas_rect(RENDER_ATLAS_WHITE), color);
}

void ui_draw_text(sb_Renderer* renderer, const char* text, mu_Vec2 pos, mu_Color color)
{
    int x = pos.x;
    for (const char* p = text; *p; p++) {
        unsigned char ch = (unsigned char)*p;
        if ((ch & 0xc0) == 0x80) {
            continue; // UTF-8 continuation byte
        }
        int glyph = ch < 127 ? ch : 127;
        mu_Rect src = render_atlas_rect(RENDER_ATLAS_FONT + glyph);
        render_push_quad(renderer, mu_rect(x, pos.y, src.w, src.h), src, color);
        x += src.w;
    }
}

void ui_draw_icon(sb_Renderer* renderer, int id, mu_Rect rect, mu_Color color)
{
    mu_Rect src = render_atlas_rect(id);
    int x = rect.x + (rect.w - src.w) / 2;
    int y = rect.y + (rect.h - src.h) / 2;
    render_push_quad(renderer, mu_rect(x, y, src.w, src.h), src, color);
}

int ui_get_text_width(const char* text, int len)
{
    return text_width_callback(NULL, text, len);
}

int ui_get_text_height(void)
{
    return text_height_callback(NULL);
}

void ui_render_set_clip_rect(sb_Renderer* renderer, mu_Rect rect)
{
    render_set_scissor(renderer, rect);
}
