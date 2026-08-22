#include <stdio.h>
#include <raylib.h>
#define NOUI_IMPLEMENTATION
#include "../noui.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 800

void draw_text_game();

int main() {

	InitWindow(SCR_WIDTH, SCR_HEIGHT, "UI demo");


	while (!WindowShouldClose()) {

		ClearBackground(BLACK);

		BeginDrawing();

		draw_text_game();

		EndDrawing();

		bool mouse_pressed = false;
		unsigned int mouse_position[2] = {GetMouseX(), GetMouseY()};
		//global_events_iter = 0;
		//sub_surfaces_iter = 0;
		sub_surfaces_iter_checks = 0;
		global_events_iter_checks = 0;

		static bool init = true;
		addWindow(400, 600, mouse_position, mouse_pressed, (unsigned int []){20, 20});
		global_events_iter_checks++;
		if (init) {
			global_events_iter++;
		}
		addScroll(10, 10);
		sub_surfaces_iter_checks++;
		global_events_iter_checks++;
		if (init) {
			global_events_iter++;
			sub_surfaces_iter++;
		}

		addScroll(10, 10);
		sub_surfaces_iter_checks++;
		global_events_iter_checks++;
		if (init) {
			global_events_iter++;
			sub_surfaces_iter++;
		}


		for (unsigned int i = 0; i < global_events_iter; ++i) {
			Event_Data base_event = global_events[i];
			Rect base_rect = base_event.surface;
			Color colour = GRAY;
			if (base_event.type == WINDOW) {
				colour = LIGHTGRAY;
			}
			DrawRectangle(base_rect.x, base_rect.y, 
					base_rect.width, base_rect.height,
					colour);

			for (unsigned int j = 0; j < sub_surfaces_iter; ++j) {
				colour = BLUE;
				Rect sub_rect = sub_surfaces[j];
				DrawRectangle(sub_rect.x, sub_rect.y,
					sub_rect.width, sub_rect.height,
						colour);
			}
		}
		init = false;

		/*
		global_events_iter = 0;
		sub_surfaces_iter = 0;
		*/


	}


	CloseWindow();

	for (unsigned int i = 0; i < global_events_iter; ++i) {
		//Event_Data base_event = global_events[i];
		for (unsigned int j = 0; j < sub_surfaces_iter; ++j) {
			//Rect_Array sub_event = sub_surfaces;
			//free(sub_event.items);
		}
		//free(global_events.items);
	}

	return 0;
}

void draw_text_game() {
	char *word = "Hello, Sailour";

	int font_height = 96;
	int font_width = MeasureText(word, font_height);

	DrawText(word, (SCR_WIDTH - font_width) / 2 , (SCR_HEIGHT - font_height)/2, font_height, WHITE);
}
