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

		if (IsKeyDown(KEY_RIGHT)) {

			event_add(checkbox);
			checkbox.is_right_of = true;
			event_add(checkbox);
			checkbox.is_right_of = false;
			event_add(checkbox);
			event_add(checkbox);

			event_add(scrollbar);
		} else {
			event_add(text_box);

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

		}


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
			i++;

		}
		EndDrawing();
		event_end();


	}

	event_array_free();

	CloseWindow();

	return 0;
}
