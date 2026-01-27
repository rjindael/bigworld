// SPDX-License-Identifier: GPL-2.0-or-later
#include "ui.h"
#include "shambhala.h"
#include "render/engine.h"
#include <glad/gl.h>
#include <stdio.h>
#include <string.h>

// Simple fixed-width font rendering
#define FONT_WIDTH 8
#define FONT_HEIGHT 14

static int text_width_callback(mu_Font font, const char* text, int len)
{
    (void)font;
    if (len == -1) {
        len = strlen(text);
    }
    return len * FONT_WIDTH;
}

static int text_height_callback(mu_Font font)
{
    (void)font;
    return FONT_HEIGHT;
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

void ui_render(sb_UI* ui)
{
    mu_Command* cmd = NULL;
    while (mu_next_command(&ui->ctx, &cmd)) {
        switch (cmd->type) {
        case MU_COMMAND_RECT:
            ui_draw_rect(cmd->rect.rect, cmd->rect.color);
            break;
        case MU_COMMAND_TEXT:
            ui_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color);
            break;
        case MU_COMMAND_ICON:
            ui_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
            break;
        case MU_COMMAND_CLIP:
            ui_render_set_clip_rect(cmd->clip.rect);
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
}

void ui_draw_scene_window(sb_UI* ui, int object_count)
{
}

void ui_draw_rect(mu_Rect rect, mu_Color color)
{
    render_push_quad(rect, atlas[ATLAS_WHITE], color);
}
