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
	TEXT,
	TAB
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
Event event_add(Event event);
Event event_logic();
void event_update_pos();
void event_update_data();
void event_end();
void event_array_print();
float NOUI_clampf(float val, float min, float max);

bool AABB_detect_point(Rect AABB, int point[2]);

#define ARRAY_INSERT(array, array_length, index, value) do { \
		for (int i = array_length; i > index; --i) { \
			array[i] = array[i-1]; \
		} \
		array[index] = value; \
	} while (0)

#define ARRAY_REMOVE(array, array_length, index) do { \
		for (int i = index; i < array_length - 1; ++i) { \
			array[i] = array[i + 1]; \
		} \
		memset(array + array_length - 1, 0, sizeof(*array)); \
	} while (0)


#define ARRAY_PRINT(array, array_length, format) do { \
		printf(#array " = {"); \
		const int MAX_LINES = 10; \
		bool has_newline = false; \
		for (int i = 0; i < array_length; ++i) { \
			char *separator = ", "; \
			if (i == array_length - 1) { \
				separator = ""; \
			} \
			printf(format "%s", array[i], separator); \
			if ((i + 1) % MAX_LINES == 0) { \
				printf("\n"); \
				has_newline = true; \
				for (int j = 0; j < sizeof(#array) + sizeof(" ="); ++j) { \
					printf(" "); \
				} \
			} \
			if (i == array_length - 1 && has_newline) { \
				printf("\n"); \
				for (int j = 0; j < sizeof(#array) + sizeof(" "); ++j) { \
					printf(" "); \
				} \
			} \
		} \
		printf("}\n"); \
	} while (0)

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
Event event_add(Event event) {
	Event *current_event = &event_array[event_loop_number];
	Event *next_event = &event_array[event_loop_number + 1];
	Event *prev_event = &event_array[event_loop_number - 1];
	
	static bool insert_mode = false;
	static int tab_count = 0;
	static bool delete_mode = false;
	if (event.type != current_event->type
	    && event.type == TAB) {
		insert_mode = true;
		event = *prev_event;
		event.type = TAB;
	} else if (event.type != current_event->type
	    && current_event->type == TAB) {
		delete_mode = true;
	} else if (event.type != current_event->type) {
		event_initial = true;
	}

	if (insert_mode) {
		//ARRAY_INSERT
		ARRAY_INSERT(event_array, MAX_EVENTS, event_loop_number, event);
		if (event.type == TAB) {
			tab_count++;
			if (tab_count == 2) {
				insert_mode = false;
				tab_count = 0;
			}
		}
	} else if (delete_mode) {
		//ARRAY_DELETE
		ARRAY_REMOVE(event_array, MAX_EVENTS, event_loop_number);
		if (next_event->type == TAB) {
			delete_mode = false;
		}
	} else if (event_initial) {
		event_array[event_loop_number] = event;
	}

	event_update_data();
	Event new_event = event_logic();
	event_loop_number++;


	return new_event;
}

Event event_logic() {
	Vector2 pos = GetMousePosition();
	int mouse_pos[] = {(int)pos.x, (int)pos.y};

	if (IsKeyDown(KEY_M)) {
		event_translated_array[0].pos[0] = mouse_pos[0];
		event_translated_array[0].pos[1] = mouse_pos[1];
	}

	Event *event = &event_array[event_loop_number];

	if (event->type == SCROLLBAR) {
		Rect gizmo_square = {event->gizmo_pos[0], event->gizmo_pos[1], event->gizmo_width, event->gizmo_height};
		bool gizmo_collide = AABB_detect_point(gizmo_square, mouse_pos);

		if (gizmo_collide && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			if (event->gizmo_width == event->width) {
				event->gizmo_pos[1] = mouse_pos[1] - event->gizmo_height / 2;
				event->gizmo_pos[1] = NOUI_clampf(event->gizmo_pos[1],
						event->pos[1], event->pos[1] + event->height - event->gizmo_height);
			} else if (event->gizmo_height == event->height) {
				// get percentage of mouse to scrollbar
				float percentage_along_scrollbar =
					(float)(mouse_pos[0] - event->pos[0] - event->gizmo_width / 2) / event->width;
				percentage_along_scrollbar = NOUI_clampf(percentage_along_scrollbar, 0.0, 1.0);
				float val = event->width * percentage_along_scrollbar;
				event->gizmo_pos[0] = event->pos[0] + val;
			}
		}
	}

	if (event->type == CHECKBOX) {
		Rect square = {event->pos[0], event->pos[1], event->width, event->height};
		bool collide = AABB_detect_point(square, mouse_pos);

		if (collide && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			event->clicked = !event->clicked;
		}
	}

	Event current_event = *event;
	return current_event;
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
			case TAB:
				type = "Tab";
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
