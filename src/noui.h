#pragma once
// column start - column end (EVENT feature);
// defines how many columns they take up

#include <stdlib.h>

typedef struct {
	int x;
	int y;
	int width;
	int height;
} Rect;

typedef struct {
	Rect surface;
} Window;

typedef enum {
	WINDOW,
	SCROLLBAR,
	CHECKBOX,
	RADIOBOX
} EVENT_TYPE;

typedef struct {
	Rect *items;
	unsigned int count;
	unsigned int capacity;
} Rect_Array;

typedef struct {
	unsigned int columns;
	unsigned int column_size_px;
	bool is_valid;
} Root_Window;

typedef struct {
	EVENT_TYPE type;
	Rect surface;
	Rect_Array sub_surfaces;

	// WINDOW type data
	Root_Window root_window;
} Event_Data;

typedef struct {
	Event_Data *items;
	unsigned int count;
	unsigned int capacity;
} Event_Array;


typedef struct {
	unsigned int padding;
	unsigned int element_spacing_y;
} Style;


Style global_default_style = {
	.padding = 10,
	.element_spacing_y = 30
};


Event_Array global_events = {0};

#define UNUSED(x) (void)x

#define DA_PUSH(arr, number) \
	do { \
	if (arr.count >= arr.capacity) { \
		if (arr.capacity == 0) { \
			arr.capacity = 256; \
		} \
		else { \
			arr.capacity *= 2; \
		} \
		arr.items = realloc(arr.items, arr.capacity * sizeof(*arr.items)); \
	} \
	arr.items[arr.count++] = number; \
	} while (0)


Event_Data global_events_get_root_window();
unsigned int get_pixel_offset_from_last_node();

Event_Data create_event(EVENT_TYPE type, unsigned int width, unsigned int height);
Rect create_rect(EVENT_TYPE type, unsigned int x, unsigned int y,
		 unsigned int width, unsigned int height);

int addWindow(unsigned int width, unsigned int height, unsigned int pos[2]);
int addScroll(unsigned int lower_bounds, unsigned int upper_bounds);



#define NOUI_IMPLEMENTATION
#ifdef NOUI_IMPLEMENTATION

void print_event(Event_Data event) {
	char *event_type;
	switch (event.type) {
		case WINDOW:
			event_type = "WINDOW"; 
			break;
		case SCROLLBAR:
			event_type = "SCROLLBAR";
			break;
		case CHECKBOX:
			event_type = "CHECKBOX"; 
			break;
		case RADIOBOX: 
			event_type = "RADIOBOX"; 
			break;
	}
	printf("{EVENT_TYPE: %s, ", event_type);
	printf("surface: {x: %u, y: %u, width: %u, height: %u}\n",
			event.surface.x, event.surface.y,
			event.surface.width, event.surface.height);
}

void print_global_event_array() {
	for (unsigned int i = 0; i < global_events.count; ++i) {
		print_event(global_events.items[i]);
	}
}

Event_Data global_events_get_root_window() {
	for (int i = global_events.count - 1; i >= 0; --i) {
		if (global_events.items[i].type == WINDOW) {
			return global_events.items[i];
		}
	}

	Event_Data null_window = {0};


	return null_window;
}

unsigned int get_pixel_offset_from_last_node() {
	unsigned int count = global_events.count;
	if (count > 0) {
		unsigned int accumulated_y_offset = global_events.items[count-1].surface.y;
		return accumulated_y_offset;
	}
	return 0;
}

Rect create_rect(EVENT_TYPE type, unsigned int x, unsigned int y,
		 unsigned int width, unsigned int height) {


	unsigned int count = global_events.count;
	unsigned int Y_OFFSET_GAP, X_OFFSET_GAP;

	if (type == WINDOW) {
		Rect rect = {
			.x = x,
			.y = y,
			.width = width,
			.height = height
		};

		return rect;
	} else {
		unsigned int initial_offset_y;
		if (global_events.items[count-1].type == WINDOW) {
			initial_offset_y = 0;
		} else {
			initial_offset_y = global_events.items[count-1].surface.height;
		}
		Y_OFFSET_GAP = initial_offset_y + global_default_style.element_spacing_y;
		X_OFFSET_GAP = global_default_style.padding;
		Event_Data root_window_event = global_events_get_root_window();
		unsigned int accumulated_y_offset = get_pixel_offset_from_last_node();
		Rect rect = {
			.x = root_window_event.surface.x + X_OFFSET_GAP,
			.y = accumulated_y_offset + Y_OFFSET_GAP,
			.width = width,
			.height = height
		};
		return rect;
	}
}

Event_Data create_event(EVENT_TYPE type, unsigned int width, unsigned int height) {

	if (type == WINDOW) {
		Event_Data default_event = {
			.type = type,
			.surface = create_rect(type, 0, 0, width, height),
			.sub_surfaces = {0}
		};
		return default_event;
	} else {
		Event_Data default_event = {
			.type = type,
			.surface = create_rect(type, 0, 0, width, height),
			.sub_surfaces = {0}
		};
		return default_event;
	}
}

int addWindow(unsigned int width, unsigned int height, unsigned int pos[2]) {

	Event_Data event = create_event(WINDOW, width, height);
	event.surface.x = pos[0];
	event.surface.y = pos[1];
	event.root_window.is_valid = true;
	event.root_window.column_size_px = 50;

	DA_PUSH(global_events, event);
	
	return 0;
}

int addScroll(unsigned int lower_bounds, unsigned int upper_bounds) {

	UNUSED(lower_bounds);
	UNUSED(upper_bounds);

	const unsigned int SCROLLBAR_HEIGHT = 20;
	const unsigned int GIZMO_WIDTH = 10;
	const unsigned int GIZMO_HEIGHT = SCROLLBAR_HEIGHT;

	Event_Data root_window_event = global_events_get_root_window();
	Root_Window root_window = root_window_event.root_window;

	if (!root_window.is_valid) {
		fprintf(stderr, "Error: No root_window\n");
		return -1;
	}
	unsigned int width = root_window.column_size_px;

	Event_Data event = create_event(SCROLLBAR, width, SCROLLBAR_HEIGHT);


	Rect scrollbar_gizmo = {
		.width = GIZMO_WIDTH,
		.height = GIZMO_HEIGHT
	};

	DA_PUSH(event.sub_surfaces, scrollbar_gizmo);
	DA_PUSH(global_events, event);

	return 0;
}

/*
int addCheckbox(Window window_num, unsigned int number_of_boxes);
int addRadiobox(Window window_num, unsigned int number_of_boxes);
*/


/*
// events
event_list[] -- stores each event with rects to draw
{ EVENT_TYPE event, Rect *rect_array, int number_of_rects};

for (int i = 0; i < number_of_events; ++i) {
	switch (event_list[i].event) {
		case SCROLL: {
				     printf("SCOLL event\n");
			     } break;
		case CHECKBOX: {
				       printf("CHECKBOX event\n");
			       } break;
		case RADIOBOX: {
				       printf("RADIOBOX event\n");
			       } break;
	}
}
*/


/*

   UI has a tree structure, this means that each node on that
   tree is a separate interface that the user can iteract with.

   This can range from:
	- Checkboxes
	- Radioboxes
	- Scrollbars
	- Graphs
	- Images

   Each node inherits the previous nodes position. This allows
   for the UI to be both simple and extensible as you won't need
   to worry about specific positions of UI elements.
   (It will be implicit from the code structure)


   Tabs can be implemented by an if statement.
   */
#endif
