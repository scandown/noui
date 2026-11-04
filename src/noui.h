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
bool event_initial = false;
int event_loop_number = 0;
int event_loop_number_inspection = 0;

void NOUI_init();
int event_add(Event event);
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

int event_add(Event event) {
	if (event_array[event_loop_number_inspection].type != event.type) {
		event_initial = true;	
	}
	if (event_loop_number > MAX_EVENTS - 1) {
		fprintf(stderr, "Error: Exceeded MAX_EVENTS\n");
		return -1;
	}



	if (event_initial) {
		event_array[event_loop_number] = event;
	}

	if (event_array[0].type != WINDOW) {
		fprintf(stderr, "Error: First event needs to be a window\n");
		return -1;
	}

	event_loop_number_inspection++;
	event_loop_number++;

	return 0;
}



void event_end() {
	event_initial = false;
	event_loop_number = 0;
	event_loop_number_inspection = 0;
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
