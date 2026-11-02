#pragma once
// column start - column end (EVENT feature);
// defines how many columns they take up

#include <stdlib.h>
#include <string.h>

typedef enum {
	WINDOW,
	CHECKBOX,
	RADIOBOX,
	SCROLLBAR,
	TEXT_INPUT,
	TEXT,
	DUMMY
} Event_Type;

typedef struct {
	Event_Type type;
	int pos[2];
	int gizmo_pos[2];
	int width;
	int height;
	int gizmo_width;
	int gizmo_height;
	bool clicked;
	bool is_right_of;
} Event;


#define MAX_EVENTS 256
Event event_array[MAX_EVENTS] = {0};
bool event_initial = false;
int event_loop_number = 0;

void NOUI_init();
int event_add(Event event);
void event_end();
void event_array_print();

#define NOUI_IMPLEMENTATION
#ifdef NOUI_IMPLEMENTATION

void NOUI_init() {
	for (int i = 0; i < MAX_EVENTS; ++i) {
		event_array[i].type = DUMMY;
	}
}

int event_add(Event event) {
	if (event_array[0].type == DUMMY) {
		event_initial = true;	
	}
	if (event_loop_number > MAX_EVENTS - 1) {
		fprintf(stderr, "Error: Exceeded MAX_EVENTS\n");
		return -1;
	}


	if (event_initial) {
		Event dummy_event = event;
		event_array[event_loop_number] = dummy_event;
		event_loop_number++;
	}

	if (event_array[0].type != WINDOW) {
		fprintf(stderr, "Error: First event needs to be a window\n");
		return -1;
	}

	return 0;
}



void event_end() {
	event_initial = false;
	event_loop_number = 0;
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
