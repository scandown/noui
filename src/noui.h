#pragma once
// column start - column end (EVENT feature);
// defines how many columns they take up

#include <stdlib.h>
#include <string.h>

typedef enum {
	DUMMY = 0,
	WINDOW,
	CHECKBOX,
	RADIOBOX,
	SCROLLBAR,
	TEXT_INPUT,
	TEXT
} Event_Type;

typedef struct {
	int x;
	int y;
	int width;
	int height;
} Rect;

typedef struct {
	Event_Type type;
	int pos[2];
	int gizmo_pos[2];
	int width;
	int height;
	int max_height;
	int gizmo_width;
	int gizmo_height;
	bool clicked;
	bool is_right_of;
} Event;


#define MAX_EVENTS 256
Event event_array[MAX_EVENTS] = {0};
Event event_translated_array[MAX_EVENTS] = {0};
bool event_initial = false;
int event_loop_number = 0;

void NOUI_init();
int event_add(Event event);
void event_logic();
void event_update_pos();
void event_update_data();
void event_end();
void event_array_print();
float NOUI_clampf(float val, float min, float max);

bool AABB_detect_point(Rect AABB, int point[2]);

#define NOUI_IMPLEMENTATION
#ifdef NOUI_IMPLEMENTATION

float NOUI_clampf(float val, float min, float max) {
	if (val < min) val = min;
	else if (val > max) val = max;

	return val;
}

void NOUI_init() {
	for (int i = 0; i < MAX_EVENTS; ++i) {
		event_array[i].type = DUMMY;
	}
}

void event_update_pos() {
	int base_offset[2] = {0};
	for (int i = 0; i <= event_loop_number; ++i) {
		if (event_translated_array[i].type == WINDOW) {
			base_offset[0] = event_translated_array[i].pos[0];
			base_offset[1] = event_translated_array[i].pos[1];
		} else {
			event_translated_array[i] = event_array[i];
			event_translated_array[i].pos[0] = event_array[i].pos[0] + base_offset[0];
			event_translated_array[i].pos[1] = event_array[i].pos[1] + base_offset[1];
			event_translated_array[i].gizmo_pos[0] = event_array[i].gizmo_pos[0] + base_offset[0];
			event_translated_array[i].gizmo_pos[1] = event_array[i].gizmo_pos[1] + base_offset[1];
		}
	}
}

int event_add(Event event) {
	if (event_loop_number > MAX_EVENTS - 1) {
		fprintf(stderr, "Error: Exceeded MAX_EVENTS\n");
		return -1;
	}


	if (event_array[event_loop_number].type != event.type) {
		event_initial = true;	
	}
	if (event_initial) {
		event_array[event_loop_number] = event;
	}

	if (event_array[0].type != WINDOW) {
		fprintf(stderr, "Error: First event needs to be a window\n");
		return -1;
	}

	event_logic();
	event_update_pos();
	event_update_data();
	event_loop_number++;


	return 0;
}

void event_logic() {
	Vector2 pos = GetMousePosition();
	int mouse_pos[] = {(int)pos.x, (int)pos.y};

	if (IsKeyDown(KEY_M)) {
		event_translated_array[0].pos[0] = mouse_pos[0];
		event_translated_array[0].pos[1] = mouse_pos[1];
	}

	Event *event_translated = &event_translated_array[event_loop_number];
	Event *event = &event_array[event_loop_number];

	if (event->type == SCROLLBAR) {
		Rect gizmo_square = {event_translated->gizmo_pos[0], event_translated->gizmo_pos[1], event_translated->gizmo_width, event_translated->gizmo_height};
		bool gizmo_collide = AABB_detect_point(gizmo_square, mouse_pos);

		if (gizmo_collide && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			if (event->gizmo_width == event->width) {
				event->gizmo_pos[1] = mouse_pos[1] - event->gizmo_height / 2;
				event->gizmo_pos[1] = NOUI_clampf(event->gizmo_pos[1],
						event->pos[1], event->pos[1] + event->height - event->gizmo_height);
			} else if (event->gizmo_height == event->height) {
				// get percentage of mouse to scrollbar
				float percentage_along_scrollbar =
					(float)(mouse_pos[0] - event_translated->pos[0] - event_translated->gizmo_width / 2) / event_translated->width;
				percentage_along_scrollbar = NOUI_clampf(percentage_along_scrollbar, 0.0, 1.0);
				float val = event_translated->width * percentage_along_scrollbar;
				event->gizmo_pos[0] = event->pos[0] + val;
			}
		}
	}

	if (event->type == CHECKBOX) {
		Rect square = {event_translated->pos[0], event_translated->pos[1], event_translated->width, event_translated->height};
		bool collide = AABB_detect_point(square, mouse_pos);

		if (collide && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			event->clicked = !event->clicked;
		}
	}
}

void event_update_data() {
	int CONFIG_Y_SPACING = 10;
	int CONFIG_X_SPACING = 10;
	Event *event = &event_array[event_loop_number];
	Event *event_prev = &event_array[event_loop_number-1];

	if (!event_initial) {
		return;
	}

	static int base_x = 0;
	if (event->type == WINDOW) {
		base_x = event->pos[0];
	}

	event->max_height = event->height;

	int previous_offset_x = event_prev->pos[0];
	int previous_offset_y = event_prev->pos[1];
	int previous_x = 0;
	int previous_y = 0;

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

	switch (event_array[event_loop_number].type) {
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
}

void event_end() {
	event_initial = false;
	event_loop_number = 0;
}


bool AABB_detect_point(Rect AABB, int point[2]) {
	bool x_collide = false;
	bool y_collide = false;
	x_collide = (point[0] < AABB.x + AABB.width && point[0] > AABB.x);
	y_collide = (point[1] < AABB.y + AABB.height && point[1] > AABB.y);

	return x_collide && y_collide;
}

void event_array_print() {
	int i = 0;
	while (i < MAX_EVENTS) {
		char *type;
		switch (event_array[i].type) {
			case WINDOW:
				type = "Window";
				break;
			case CHECKBOX:
				type = "Checkbox";
				break;
			case RADIOBOX:
				type = "Radiobox";
				break;
			case SCROLLBAR:
				type = "Scrollbar";
				break;
			case TEXT_INPUT:
				type = "Text_Input";
				break;
			case TEXT:
				type = "Text";
				break;
			case DUMMY:
				type = "Dummy";
				break;
			default:
				type = "N/A";
				break;
		}
		if (event_array[i].type == DUMMY) {
			return;
		}
		printf("%d: Event_Type = %s\n", i, type);
		i++;
	}
}

#endif
