#include <stdio.h>
#include <raylib.h>
#define NOUI_IMPLEMENTATION
#include "../noui.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 800

int main() {

	InitWindow(SCR_WIDTH, SCR_HEIGHT, "UI demo");

	NOUI_init();

	while (!WindowShouldClose()) {

		ClearBackground(BLACK);

		BeginDrawing();

		char *word = "Hello, Sailour";

		int font_height = 30;
		int font_width = MeasureText(word, font_height);

		DrawText(word, (SCR_WIDTH - font_width) / 2,
			(SCR_HEIGHT - font_height)/2, font_height, WHITE);


		Global_config.pressed_key = GetCharPressed();
		if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
			Global_config.pressed_key = NOUI_BACKSPACE;
		} else if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
			Global_config.pressed_key = NOUI_KEY_LEFT;
		} else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
			Global_config.pressed_key = NOUI_KEY_RIGHT;
		}

		Event window = {.type = WINDOW,
				.pos = {0, 0},
				.width = 300,
				.height = 300
		};

		Event text_box = {.type = TEXT_INPUT,
				.width = 80,
				.height = 30
		};

		Event checkbox = {.type = CHECKBOX,
				  .width = 30,
				  .height = 30

		};
		Event tab = {.type = TAB};

		Event scrollbar = {.type = SCROLLBAR,
				   .width = 80,
				   .height = 30,
				   .gizmo_width = 30,//scrollbar.width,
				   .gizmo_height = scrollbar.height
		};

		static int val = 0;

		event_add(window);

		Event text_event = {0};
		text_event = event_add(text_box);

		event_add(scrollbar);
		Event check = event_add(checkbox);
		if (check.clicked) {
			event_add(tab);
			if (event_add(checkbox).clicked) {
				event_add(tab);
				event_add(scrollbar);
				checkbox.clicked = val;
				val = event_add(checkbox).clicked;
				event_add(tab);
			}
			event_add(checkbox);
			event_add(tab);
		}
		event_add(scrollbar);


		printf("%d\n", text_event.cursor_x);


		// drawing
		int i = 0;
		while (i < event_loop_number) {
			Color colour = RED;
			Color gizmo_colour = GREEN;
			Event *event = &event_array[i];
			switch (event->type) {
				case WINDOW:
					colour = GRAY;
					break;
				case CHECKBOX:
					if (event->clicked) {
						colour = LIGHTGRAY;
					} else {
						colour = DARKGRAY;
					}
					break;
				case TEXT_INPUT:
					colour = LIGHTGRAY;
					break;
				default:
					colour = RED;
					break;
			}


			DrawRectangle(event->pos[0],
					event->pos[1],
					event->width,
					event->height,
					colour);

			if (event->type == SCROLLBAR) {
				DrawRectangle(event->pos[0] + event->width * event->gizmo_x_percentage,
						event->pos[1] + event->height * event->gizmo_y_percentage,
						event->gizmo_width,
						event->gizmo_height,
						gizmo_colour);
			}

			if (event->type == TEXT_INPUT) {
				int font_size = event->height;
				int text_width = MeasureText(event->str.items, font_size);
				int text_offset = event->width - text_width;
				if (text_offset > 0) {
					text_offset = 0;
				}
				int text_x = event->pos[0] + text_offset;

				BeginScissorMode(event->pos[0], event->pos[1], event->width, event->height);
				DrawText(event->str.items, text_x, event->pos[1], font_size, BLACK);
				EndScissorMode();

				// cursor
				int cursor_width = 10;
				int cursor_height = event->height;
				Color cursor_colour = DARKGRAY;
				DrawRectangle(text_x + text_width, event->pos[1], cursor_width, cursor_height, cursor_colour);
			}
			i++;

		}
		EndDrawing();
		event_end();


	}

	event_array_free();

	CloseWindow();

	return 0;
}
