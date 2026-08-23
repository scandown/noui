#pragma once
// column start - column end (EVENT feature);
// defines how many columns they take up

#include <stdlib.h>
#include <string.h>

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
	SCROLLBAR_GIZMO,
	CHECKBOX,
	RADIOBOX
} EVENT_TYPE;


#define EVENT_MAX_SIZE 256

typedef struct {
	int mouse_position[2];
	unsigned int columns;
	unsigned int column_size_px;
	bool is_valid;
} Root_Window;

typedef struct {
	EVENT_TYPE type;
	Rect surface;

	// WINDOW type data
	Root_Window root_window;

	bool checked;
} Event_Data;

Event_Data global_events[EVENT_MAX_SIZE];
Rect sub_surfaces[EVENT_MAX_SIZE];

unsigned int sub_surfaces_iter = 0;
unsigned int sub_surfaces_iter_checks = 0;
unsigned int global_events_iter = 0;
unsigned int global_events_iter_checks = 0;


typedef struct {
	Rect *collided_button_id;
	bool is_button_selected;
	bool pressed_once;
} NOUI_CONTEXT;

NOUI_CONTEXT NOUI_CTX = {0};


typedef struct {
	unsigned int padding_x;
	unsigned int element_spacing_y;
} Style;


Style global_default_style = {
	.padding_x = 10,
	.element_spacing_y = 30
};


//Event_Array global_events = {0};

int clamp(int value, int min, int max) {
  const int t = value < min ? min : value;
  return t > max ? max : t;
}


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

unsigned int get_previous_element_offset_x();
unsigned int get_previous_element_offset_y();
unsigned int get_previous_element_offset_width();
unsigned int get_previous_element_offset_height();
void print_global_event_array();
void print_rect(Rect rect);

bool AABB_element_check(Rect AABB, int position[2]);

Event_Data global_events_get_root_window();
unsigned int get_pixel_offset_from_last_node();

Event_Data create_event(EVENT_TYPE type, unsigned int width, unsigned int height);
Rect create_rect(EVENT_TYPE type, unsigned int x, unsigned int y,
		 unsigned int width, unsigned int height);

int addWindow(unsigned int width, unsigned int height, 
		unsigned int mouse_position[2],
		unsigned int pos[2]);
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
		case SCROLLBAR_GIZMO:
			event_type = "SCROLLBAR_GIZMO";
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


unsigned int get_previous_element_offset_x() {
	Event_Data event = global_events[global_events_iter_checks-1];

	if (event.type == WINDOW) return 0;
	return event.surface.x;
}

unsigned int get_previous_element_offset_y() {
	Event_Data event = global_events[global_events_iter_checks-1];

	if (event.type == WINDOW) return 0;
	return event.surface.y;
}

unsigned int get_previous_element_offset_width() {
	Event_Data event = global_events[global_events_iter_checks-1];

	if (event.type == WINDOW) return 0;
	return event.surface.width;
}
unsigned int get_previous_element_offset_height() {
	Event_Data event = global_events[global_events_iter_checks-1];

	if (event.type == WINDOW) return 0;
	return event.surface.height;
}

void print_global_event_array() {
	for (unsigned int i = 0; i < global_events_iter_checks; ++i) {
		print_event(global_events[i]);
	}
}
void print_rect(Rect rect) {
	printf("x: %d, y: %d, width: %d, height: %d\n",
			rect.x, rect.y, rect.width, rect.height);
}

Event_Data global_events_get_root_window() {
	for (int i = global_events_iter - 1; i >= 0; --i) {
		if (global_events[i].type == WINDOW) {
			return global_events[i];
		}
	}

	Event_Data null_window = {0};


	return null_window;
}

unsigned int get_pixel_offset_from_last_node() {
	unsigned int count = global_events_iter_checks;
	if (count > 0) {
		unsigned int accumulated_y_offset = global_events[count-1].surface.y;
		return accumulated_y_offset;
	}
	return 0;
}


Rect create_rect(EVENT_TYPE type, unsigned int local_x, unsigned int local_y,
		 unsigned int width, unsigned int height) {

	unsigned int Y_OFFSET_GAP, X_OFFSET_GAP;

	if (type == WINDOW) {
		Rect rect = {
			.x = local_x,
			.y = local_y,
			.width = width,
			.height = height
		};

		return rect;
	} else {
		unsigned int initial_offset_y = get_previous_element_offset_height();

		Y_OFFSET_GAP = initial_offset_y + global_default_style.element_spacing_y;
		X_OFFSET_GAP = global_default_style.padding_x;
		Event_Data root_window_event = global_events_get_root_window();
		unsigned int accumulated_y_offset = get_pixel_offset_from_last_node();
		Rect rect = {
			.x = local_x + root_window_event.surface.x + X_OFFSET_GAP,
			.y = local_y + accumulated_y_offset + Y_OFFSET_GAP,
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
		};
		return default_event;
	} else {
		Event_Data default_event = {
			.type = type,
			.surface = create_rect(type, 0, 0, width, height),
		};
		return default_event;
	}
}

int addWindow(unsigned int width, unsigned int height, 
		unsigned int mouse_position[2],
		unsigned int pos[2]) {

	Event_Data event = create_event(WINDOW, width, height);
	event.surface.x = pos[0];
	event.surface.y = pos[1];
	event.root_window.is_valid = true;
	event.root_window.column_size_px = 50;

	memcpy(event.root_window.mouse_position, mouse_position,
			sizeof(unsigned int) * 2);

	global_events[global_events_iter] = event;
	global_events[global_events_iter_checks] = event;
	
	return 0;
}

bool AABB_element_check(Rect AABB, int position[2]) {
	bool x = false;
	bool y = false;
	if (position[0] > AABB.x && position[0] < AABB.x + AABB.width) {
		x = true;
	}
	if (position[1] > AABB.y && position[1] < AABB.y + AABB.height) {
		y = true;
	}

	return x && y;
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


	int *mouse_pos = root_window.mouse_position;

	unsigned int element_offset = root_window_event.surface.x +
		global_default_style.padding_x;



	unsigned int gizmo_position_x = 
		clamp(mouse_pos[0] - element_offset - GIZMO_WIDTH/2,
			0, event.surface.width - GIZMO_WIDTH / 2);

	Rect scrollbar_gizmo = create_rect(SCROLLBAR_GIZMO,
			gizmo_position_x, 0, GIZMO_WIDTH, GIZMO_HEIGHT);


	sub_surfaces[sub_surfaces_iter] = scrollbar_gizmo;

	bool collision = AABB_element_check(
			sub_surfaces[sub_surfaces_iter_checks],
			mouse_pos);

	Rect *current_rect_id = &sub_surfaces[sub_surfaces_iter_checks];
	if (collision && NOUI_CTX.collided_button_id == NULL) {
		NOUI_CTX.collided_button_id = current_rect_id;
	}

	if (NOUI_CTX.is_button_selected &&
		NOUI_CTX.collided_button_id == current_rect_id) {

		sub_surfaces[sub_surfaces_iter_checks] = scrollbar_gizmo;
	}
	global_events[global_events_iter] = event;


	return 0;
}

int addCheckbox(){

	const unsigned int BOX_SIZE = 20;

	Event_Data root_window_event = global_events_get_root_window();
	Root_Window root_window = root_window_event.root_window;

	if (!root_window.is_valid) {
		fprintf(stderr, "Error: No root_window\n");
		return -1;
	}

	Event_Data event = create_event(CHECKBOX, BOX_SIZE, BOX_SIZE);


	int *mouse_pos = root_window.mouse_position;



	global_events[global_events_iter] = event;

	bool collision = AABB_element_check(
			global_events[global_events_iter_checks].surface,
			(int *)mouse_pos);

	Rect *current_rect_id = &global_events[global_events_iter_checks].surface;

	if (collision && NOUI_CTX.collided_button_id == NULL) {
		NOUI_CTX.collided_button_id = current_rect_id;
	}

	if (NOUI_CTX.is_button_selected &&
		NOUI_CTX.collided_button_id == current_rect_id &&
		!NOUI_CTX.pressed_once) {

		NOUI_CTX.pressed_once = true;


		event.checked = !event.checked;

		bool flipped_check = !global_events[global_events_iter_checks].checked;
		event.checked = flipped_check;
		global_events[global_events_iter_checks] = event;
	}


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
