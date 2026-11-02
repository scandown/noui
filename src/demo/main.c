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



		Event window = {.type = WINDOW,
				.pos = {200, 200},
				.width = 300,
				.height = 300
		};
		event_add(window);

		Event checkbox = {.type = CHECKBOX,
				  .width = 30,
				  .height = 30

		};
		event_add(checkbox);
		event_add(checkbox);

		event_end();

		event_array_print();


		int i = 0;
		while (i < MAX_EVENTS) {
			Event *event = &event_array[i];
			Event *event_prev = &event_array[i-1];

			int previous_offset_x = event_prev->pos[0];
			int previous_offset_y = event_prev->pos[1];
			int previous_width = 0;
			int previous_height = 0;
			if (event_prev->type != WINDOW) {
				previous_width = event_prev->width;
				previous_height = event_prev->height;
			}
			switch (event_array[i].type) {
				case WINDOW:
					DrawRectangle(event->pos[0],
							event->pos[1],
							event->width,
							event->height,
							GRAY);

					break;
				case CHECKBOX:
					event->pos[0] = previous_offset_x + previous_width;
					event->pos[1] = previous_offset_y + previous_height;
					DrawRectangle(event->pos[0],
							event->pos[1],
							event->width,
							event->height,
							DARKGRAY);
					break;

			}
			i++;
		}
		EndDrawing();


	}


	CloseWindow();

	return 0;
}
