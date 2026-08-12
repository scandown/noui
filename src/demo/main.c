#include <stdio.h>
#include <raylib.h>
#define NOUI_IMPLEMENTATION
#include "../noui.h"

#define SCR_WIDTH 800
#define SCR_HEIGHT 800

void draw_text_game();

int main() {

	InitWindow(SCR_WIDTH, SCR_HEIGHT, "UI demo");


	while (!WindowShouldClose()) {

		ClearBackground(BLACK);

		BeginDrawing();

		draw_text_game();

		EndDrawing();

	}

	printf("Hello, World\n");

	CloseWindow();

	return 0;
}

void draw_text_game() {
	char *word = "Hello, Sailour";

	int font_height = 96;
	int font_width = MeasureText(word, font_height);

	DrawText(word, (SCR_WIDTH - font_width) / 2 , (SCR_HEIGHT - font_height)/2, font_height, WHITE);
}
