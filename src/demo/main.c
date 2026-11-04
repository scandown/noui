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

		Event checkbox = {.type = CHECKBOX,
				  .width = 30,
				  .height = 30

		};

		Event scrollbar = {.type = SCROLLBAR,
				   .width = 30,
				   .height = 80,
				   .gizmo_width = scrollbar.width,
				   .gizmo_height = 30//scrollbar.height
		};



		if (IsKeyDown(KEY_RIGHT)) {
			event_add(window);

			event_add(checkbox);
			checkbox.is_right_of = true;
			event_add(checkbox);
			checkbox.is_right_of = false;
			event_add(checkbox);
			event_add(checkbox);

			event_add(scrollbar);
		} else {
			event_add(window);

			event_add(scrollbar);
			scrollbar.is_right_of = true;
			event_add(scrollbar);
			scrollbar.is_right_of = false;

			checkbox.is_right_of = true;
			event_add(checkbox);
			checkbox.is_right_of = false;
			event_add(checkbox);
			event_add(checkbox);

			event_add(scrollbar);

		}

		Vector2 pos = GetMousePosition();
		int mouse_pos[] = {(int)pos.x, (int)pos.y};

		int i = 0;
		int CONFIG_Y_SPACING = 10;
		int CONFIG_X_SPACING = 10;
		// position setting
		while (i < MAX_EVENTS) {
			Event *event = &event_array[i];
			Event *event_prev = &event_array[i-1];

			int previous_offset_x = event_prev->pos[0];
			int previous_offset_y = event_prev->pos[1];
			int previous_x = 0;
			int previous_y = 0;


			if (event->type == SCROLLBAR) {
				Rect gizmo_square = {event->gizmo_pos[0], event->gizmo_pos[1], event->gizmo_width, event->gizmo_height};
				bool gizmo_collide = AABB_detect_point(gizmo_square, mouse_pos);

				if (gizmo_collide && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
					if (event->gizmo_width == event->width) {
						event->gizmo_pos[1] = mouse_pos[1] - event->gizmo_height / 2;
						event->gizmo_pos[1] = NOUI_clampf(event->gizmo_pos[1],
								event->pos[1], event->pos[1] + event->height - event->gizmo_height);
					} else if (event->gizmo_height == event->height) {
						event->gizmo_pos[0] = mouse_pos[0] - event->gizmo_width / 2;
						event->gizmo_pos[0] = NOUI_clampf(event->gizmo_pos[0],
								event->pos[0], event->pos[0] + event->width - event->gizmo_width);
					}
				}
			}

			if (!event_initial) {
				i++;
				continue;	
			}

			static int base_x = 0;
			if (event->type == WINDOW) {
				base_x = event->pos[0];
			}

			event->max_height = event->height;

			if (event_prev->type != WINDOW) {
				if (event_prev->max_height > event->max_height) {
					event->max_height = event_prev->max_height;
				}
				if (event_prev->is_right_of) {
					previous_x = event_prev->width + CONFIG_X_SPACING;
				} else {
					previous_offset_x = base_x;
					previous_y = event_prev->max_height + CONFIG_Y_SPACING;
					event->max_height = event->height;
				}
			}

			switch (event_array[i].type) {
				case WINDOW:
					break;
				case SCROLLBAR:
					event->gizmo_pos[0] = previous_offset_x + previous_x;
					event->gizmo_pos[1] = previous_offset_y + previous_y;
					// fall through
				default:
					event->pos[0] = previous_offset_x + previous_x;
					event->pos[1] = previous_offset_y + previous_y;
					break;
			}
			i++;
		}









		// drawing
		i = 0;
		while (i < MAX_EVENTS) {
			Color colour = RED;
			Color gizmo_colour = GREEN;
			Event *event = &event_array[i];
			switch (event->type) {
				case WINDOW:
					colour = GRAY;
					break;
				case CHECKBOX:
					colour = DARKGRAY;
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
