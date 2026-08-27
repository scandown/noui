#include <stdio.h>
#include <raylib.h>
#define NOUI_IMPLEMENTATION
#include "../noui.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 800

int main() {

	NOUI_CTX.is_button_selected = false;
	NOUI_CTX.collided_button_id = NULL;
	NOUI_CTX.pressed_once = false;

	InitWindow(SCR_WIDTH, SCR_HEIGHT, "UI demo");


	while (!WindowShouldClose()) {

		ClearBackground(BLACK);

		BeginDrawing();

		char *word = "Hello, Sailour";

		int font_height = global_default_style.row_height;
		int font_width = MeasureText(word, font_height);

		DrawText(word, (SCR_WIDTH - font_width) / 2,
			(SCR_HEIGHT - font_height)/2, font_height, WHITE);

		EndDrawing();

		bool mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
		int mouse_position[2] = {GetMouseX(), GetMouseY()};
		NOUI_CTX.mouse_position[0] = mouse_position[0];
		NOUI_CTX.mouse_position[1] = mouse_position[1];
		sub_surfaces_iter_checks = 0;
		global_events_iter_checks = 0;

		if (!mouse_down) {
			NOUI_CTX.is_button_selected = false;
			NOUI_CTX.collided_button_id = NULL;
			NOUI_CTX.pressed_once = false;
		}
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			NOUI_CTX.is_button_selected = true;
		}


		addWindow(200, 200, (unsigned int []){20, 20});
		addScroll(10, 10);

		addWindow(200, 200, (unsigned int []){300, 20});
		addScroll(10, 10);

		print_global_event_array();


		for (unsigned int i = 0; i < global_events_iter; ++i) {
			Event_Data base_event = global_events[i];
			Rect base_rect = base_event.surface;
			Color colour = GRAY;
			if (base_event.type == WINDOW) {
				colour = LIGHTGRAY;
			}

			else if (base_event.type == CHECKBOX) {
				colour = RED;
				if (base_event.checked) {
					colour = GREEN;
				}
			} else if (base_event.type == RADIOBOX) {
				colour = ORANGE;
				if (base_event.checked) {
					colour = BLUE;
				}
			}
			if (base_event.type == TEXT) {
				unsigned int font_height = base_event.surface.height;
				DrawText(base_event.text,
					base_event.surface.x,
					base_event.surface.y,
					font_height, WHITE);
				
			} else {
				DrawRectangle(base_rect.x, base_rect.y, 
					base_rect.width, base_rect.height,
						colour);
			}

			for (unsigned int j = 0; j < sub_surfaces_iter; ++j) {
				colour = BLUE;
				Rect sub_rect = sub_surfaces[j];
				DrawRectangle(sub_rect.x, sub_rect.y,
					sub_rect.width, sub_rect.height,
						colour);
			}
		}

		NOUI_init = false;

	}


	CloseWindow();

	return 0;
}
