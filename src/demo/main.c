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
				.pos = {0, 0},
				.width = 300,
				.height = 300
		};

		Event checkbox = {.type = CHECKBOX,
				  .width = 30,
				  .height = 30

		};

		Event scrollbar = {.type = SCROLLBAR,
				   .width = 80,
				   .height = 30,
				   .gizmo_width = 30,//scrollbar.width,
				   .gizmo_height = scrollbar.height
		};


		event_add(window);

		if (IsKeyDown(KEY_RIGHT)) {

			event_add(checkbox);
			checkbox.is_right_of = true;
			event_add(checkbox);
			checkbox.is_right_of = false;
			event_add(checkbox);
			event_add(checkbox);

			event_add(scrollbar);
		} else {

			event_add(scrollbar);
			scrollbar.is_right_of = true;
			event_add(scrollbar);
			scrollbar.is_right_of = false;

			checkbox.is_right_of = true;
			event_add(checkbox);
			event_add(checkbox);
			event_add(checkbox);

			event_add(scrollbar);
			checkbox.is_right_of = false;
			event_add(checkbox);
			event_add(scrollbar);

		}


		// drawing
		int i = 0;
		while (i < event_loop_number) {
			Color colour = RED;
			Color gizmo_colour = GREEN;
			Event *event = &event_translated_array[i];
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
				DrawRectangle(event->gizmo_pos[0],
						event->gizmo_pos[1],
						event->gizmo_width,
						event->gizmo_height,
						gizmo_colour);
			}
			i++;

		}
		EndDrawing();
		event_end();


	}


	CloseWindow();

	return 0;
}
