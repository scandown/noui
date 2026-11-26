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

typedef enum {
	NOUI_BACKSPACE = 8,
	NOUI_SPACE = 32,
	NOUI_A = 65,    NOUI_B, NOUI_C, NOUI_D, NOUI_E, NOUI_F, NOUI_G, NOUI_H,
	NOUI_I,	NOUI_J, NOUI_K, NOUI_L, NOUI_M, NOUI_N, NOUI_O, NOUI_P, NOUI_Q,
	NOUI_R,	NOUI_S, NOUI_T, NOUI_U, NOUI_V, NOUI_W, NOUI_X, NOUI_Y, NOUI_Z,
} NOUI_KEYS;

typedef struct {
	int x;
	int y;
	int width;
	int height;
} Rect;

typedef struct {
	int pressed_key;
} Config;

typedef struct {
	char *items;
	size_t count;
	size_t capacity;
} String_Dynamic;


typedef struct {
	Event_Type type;
	int pos[2];
	float gizmo_x_percentage;
	float gizmo_y_percentage;
	int width;
	int height;
	int max_height;
	int gizmo_width;
	int gizmo_height;
	bool clicked;
	bool is_right_of;

	String_Dynamic str;
} Event;


Config Global_config = {0};




#define MAX_EVENTS 256
Event event_array[MAX_EVENTS] = {0};
Event event_translated_array[MAX_EVENTS] = {0};
int event_loop_number = 0;

void NOUI_init();
Event event_add(Event event);
Event event_logic();
void update_position();
void event_update_data();
void event_end();
void event_array_free();
void event_free(int index);
void event_print(Event event);
void event_array_print();

float NOUI_clampf(float val, float min, float max);
void DA_APPEND_STR(String_Dynamic *xs, char *string, int string_length);

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

void DA_APPEND_STR(String_Dynamic *xs, char *string, int string_length) {
	if (xs->count > 0 && xs->items[xs->count-1] == '\0') {
		xs->count--;
	}
	for (int i = 0; i < string_length; ++i) {
		if (xs->count >= xs->capacity) {
			if (xs->capacity == 0) xs->capacity = 256;
			else xs->capacity *= 2;
			xs->items = realloc(xs->items, xs->capacity*sizeof(*xs->items));
		}
		xs->items[xs->count++] = string[i];
	}
}





void NOUI_init() {
	for (int i = 0; i < MAX_EVENTS; ++i) {
		event_array[i].type = DUMMY;
	}
}
Event event_add(Event event) {
	Event *current_event = &event_array[event_loop_number];
	Event *prev_event = &event_array[event_loop_number - 1];
	
	static bool insert_mode = false;
	static int tab_count = 0;
	static bool delete_mode = false;
	bool event_initial = false;
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
		event_free(event_loop_number);
		ARRAY_REMOVE(event_array, MAX_EVENTS, event_loop_number);
		while (current_event->type != TAB) {
			ARRAY_REMOVE(event_array, MAX_EVENTS, event_loop_number);
		}
		if (current_event->type == TAB) {
			ARRAY_REMOVE(event_array, MAX_EVENTS, event_loop_number);
			delete_mode = false;
		}
	} else if (event_initial) {
		event_free(event_loop_number);
		event_array[event_loop_number] = event;
	}

	Event new_event = {0};
	update_position();
	new_event = event_logic(event);
	event_loop_number++;


	return new_event;
}

void event_array_free() {
	for (int i = 0; i < MAX_EVENTS; ++i) {
		event_free(i);
	}
}
void event_free(int index) {
	if (event_array[index].str.items != NULL) {
		free(event_array[index].str.items);
	}
}


void update_position() {
	int CONFIG_Y_SPACING = 50;
	int CONFIG_X_SPACING = 10;
	Event *event_current = &event_array[event_loop_number];
	Event dummy = {0};
	Event *event_prev = &dummy;
	if (event_loop_number > 0) {
		event_prev = &event_array[event_loop_number-1];
	}

	if (!event_prev->is_right_of) {
		event_current->pos[1] = event_prev->pos[1] + CONFIG_Y_SPACING;
	} else {
		event_current->pos[0] = event_prev->pos[0] + CONFIG_X_SPACING;
	}
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
		int gizmo_pos[2] = {event->pos[0] + event->width * event->gizmo_x_percentage,
					event->pos[1] + event->height * event->gizmo_y_percentage};
		Rect gizmo_square = {gizmo_pos[0], gizmo_pos[1], event->gizmo_width, event->gizmo_height};
		bool gizmo_collide = AABB_detect_point(gizmo_square, mouse_pos);

		if (gizmo_collide && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			if (event->gizmo_width == event->width) {
				float percentage_along_scrollbar =
					(float)(mouse_pos[1] - event->pos[1] - event->gizmo_height / 2) / event->height;
				percentage_along_scrollbar = NOUI_clampf(percentage_along_scrollbar, 0.0, 1.0);
				event->gizmo_y_percentage = percentage_along_scrollbar;
			} else if (event->gizmo_height == event->height) {
				// get percentage of mouse to scrollbar
				float percentage_along_scrollbar =
					(float)(mouse_pos[0] - event->pos[0] - event->gizmo_width / 2) / event->width;
				percentage_along_scrollbar = NOUI_clampf(percentage_along_scrollbar, 0.0, 1.0);
				event->gizmo_x_percentage = percentage_along_scrollbar;
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

	if (event->type == TEXT_INPUT) {
		Rect square = {event->pos[0], event->pos[1], event->width, event->height};
		bool collide = AABB_detect_point(square, mouse_pos);

		if (collide && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			event->clicked = !event->clicked;
		}


		if (event->clicked) {
			int key_letter = Global_config.pressed_key;
			char string[2] = {0};
			string[0] = (char)key_letter;
			if (key_letter == NOUI_BACKSPACE) {
				if (event->str.count > 0) {
					event->str.count--;
					event->str.items[event->str.count-1] = '\0';
				}
			} else if (key_letter > 0) {
				DA_APPEND_STR(&event->str, string, strlen(string) + 1);
			}
		}
	}

	Event current_event = *event;
	return current_event;
}



void event_end() {
	event_loop_number = 0;
}


bool AABB_detect_point(Rect AABB, int point[2]) {
	bool x_collide = false;
	bool y_collide = false;
	x_collide = (point[0] < AABB.x + AABB.width && point[0] > AABB.x);
	y_collide = (point[1] < AABB.y + AABB.height && point[1] > AABB.y);

	return x_collide && y_collide;
}
void event_print(Event event) {
	char *type;
	switch (event.type) {
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
	if (event.type == DUMMY) {
		return;
	}
	printf("Event_Type = %s\n", type);
}

void event_array_print() {
	int i = 0;
	while (i < event_loop_number) {
		printf("%d: ", i);
		event_print(event_array[i]);
		if (event_array[i].type == DUMMY) {
			return;
		}
		i++;
	}
}

#endif
