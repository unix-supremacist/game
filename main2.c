#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include <raylib.h>
#include <raylib-nuklear.h>
#include <raymath.h>
#include <tmx.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct Player {
	Vector2 position;
	float speed;
	bool canJump;
} Player;

float playerJumpSpd = 125.0f;
float playerJumpMult = 15.0f;
float playerJumpTimeMax = 0.05f;
float playerJumpTimeAdd = 0.5f;
float playerMoveSpd = 100.0f;
float gravity = 325.0f;

float jumpTime = 0;

void UpdatePlayer(Player *player, tmx_map *map, float delta){
	Vector2 *p = &(player->position);
	if (IsKeyDown(KEY_LEFT)) p->x -= playerMoveSpd*delta;
	if (IsKeyDown(KEY_RIGHT)) p->x += playerMoveSpd*delta;
	if (IsKeyDown(KEY_SPACE) && (player->canJump || jumpTime > 0.0f)) {
		player->speed -= playerJumpSpd*playerJumpMult*delta;
		player->canJump = false;
		if (jumpTime < playerJumpTimeMax) jumpTime += (playerJumpTimeAdd*delta);
		else jumpTime = 0;
	} else {
		jumpTime = 0;
	}

	bool landed = false;
	bool headHit = false;
	bool shoulderHit = false;
	bool sideHit = false;
	unsigned long i, j;
	unsigned int gid, flags;
	float x, y, w, h;
	tmx_tileset *ts;
	tmx_layer *layers = map->ly_head;

	while (layers) {
		if (layers->visible && !tmx_get_property(layers->properties, "Intangible")->value.boolean) {
			if (layers->type == L_LAYER) {
				for (i=0; i<map->height; i++) {
					for (j=0; j<map->width; j++) {
						gid = (layers->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
						if (map->tiles[gid] != NULL) {
							ts = map->tiles[gid]->tileset;
							x = (float)(j*ts->tile_width);
							y = (float)(i*ts->tile_height);
							w = (float)(ts->tile_width);
							h = ts->tile_height;

							float shoulder = 6;
							float revx = x + w;
							float revy = y + h;
							float head = p->y - 16 - h;
							float shoulderWidth = p->y - shoulder * 2 - h;
							float left = p->x - shoulder;
							float right = p->x + shoulder;

							//Feet Collision
							if (x <= p->x + shoulder - 1 && revx >= p->x - shoulder + 1 && y >= p->y && y <= p->y + player->speed * delta && !landed) {
								landed = true;
								player->speed = 0.0f;
								p->y = y;
							}

							//Head Collision
							if (x <= p->x + 1 && revx >= p->x - 1 && y <= head && y >= head + player->speed * delta && !headHit) {
								headHit = true;
								player->speed = 0.0f;
								p->y = y + 16 + h;
							}

							//Left Shoulder Collision
							if (x <= left && revx >= left && y <= shoulderWidth && y >= shoulderWidth + player->speed * delta && !shoulderHit) {
								shoulderHit = true;
								p->x = x + shoulder + w;
							}

							//Right Shoulder Collision	 
							if (x <= right && revx >= right && y <= shoulderWidth && y >= shoulderWidth + player->speed * delta && !shoulderHit) {
								shoulderHit = true;
								p->x = x - shoulder;
							}

							//Left Side Collision
							if (y <= p->y - 1 && revy >= p->y - 11 && x <= left && revx >= left && !sideHit) {
								sideHit = true;
								p->x = x + shoulder + w;
							}

							//Right Side Collision
							if (y <= p->y - 1 && revy >= p->y - 11 && x <= right && revx >= right && !sideHit) {
								sideHit = true;
								p->x = x - shoulder;
							}
						}
					}
				}
			}
		}
		layers = layers->next;
	}

	if (!landed){
		player->position.y += player->speed*delta;
		player->speed += gravity*delta;
		player->canJump = false;
	} else player->canJump = true;
}

float yTarget;
float xTarget;

void UpdateCameraTest(Camera2D *camera, Player *player, float delta){
	int width = GetScreenWidth();
	int height = GetScreenHeight();
	static Vector2 bbox = { 0.2f, 0.2f };
	Vector2 bboxWorldMin = GetScreenToWorld2D((Vector2){ (1 - bbox.x)*0.5f*width, (1 - bbox.y)*0.5f*height }, *camera);
	Vector2 bboxWorldMax = GetScreenToWorld2D((Vector2){ (1 + bbox.x)*0.5f*width, (1 + bbox.y)*0.5f*height }, *camera);
	camera->offset = (Vector2){ (1 - bbox.x)*0.5f * width, height/3.0f*2.0f };

	static float minSpeed = 30;
	static float minEffectLength = 10;
	static float fractionSpeed = 1.8f;

	if (player->position.x < bboxWorldMin.x) xTarget = player->position.x;
	if (player->position.y < bboxWorldMin.y) yTarget = player->position.y;
	if (player->position.x > bboxWorldMax.x) xTarget = bboxWorldMin.x + (player->position.x - bboxWorldMax.x);
	if (player->position.y > bboxWorldMax.y) yTarget = bboxWorldMin.y + (player->position.y - bboxWorldMax.y);
	if (player->canJump && (player->speed == 0) && (player->position.y != camera->target.y)) yTarget = player->position.y;

	Vector2 FakePos = {xTarget, yTarget};
	Vector2 diff = Vector2Subtract(FakePos, camera->target);
	float length = Vector2Length(diff);

	if (length > minEffectLength) {
		float speed = fmaxf(fractionSpeed*length, minSpeed);
		camera->target = Vector2Add(camera->target, Vector2Scale(diff, speed*delta/length));
	}
}

void* raylib_tex_loader(const char *path) {
	Texture2D *returnValue = malloc(sizeof(Texture2D));
	*returnValue = LoadTexture(path);
	return returnValue;
}

void raylib_free_tex(void *ptr) {
	UnloadTexture(*((Texture2D*)ptr));
	free(ptr);
}

Color int_to_color(int color) {
	tmx_col_bytes res = tmx_col_to_bytes(color);
	return *((Color*)&res);
}

#define LINE_THICKNESS 2.5

void draw_polyline(double offset_x, double offset_y, double **points, int points_count, Color color) {
	for (int i = 1; i<points_count; i++) DrawLineEx((Vector2){offset_x + points[i-1][0], offset_y + points[i-1][1]},
		(Vector2){offset_x + points[i][0], offset_y + points[i][1]}, LINE_THICKNESS, color);
}

void draw_polygon(double offset_x, double offset_y, double **points, int points_count, Color color) {
	draw_polyline(offset_x, offset_y, points, points_count, color);
	if (points_count > 2) DrawLineEx((Vector2){offset_x + points[0][0], offset_y + points[0][1]},
		(Vector2){offset_x + points[points_count-1][0], offset_y + points[points_count-1][1]}, LINE_THICKNESS, color);
}

void draw_objects(tmx_object_group *objgr) {
	tmx_object *head = objgr->head;
	Color color = int_to_color(objgr->color);

	while (head) {
		if (head->visible) {
			if (head->obj_type == OT_SQUARE)
				DrawRectangleLinesEx((Rectangle){head->x, head->y, head->width, head->height}, LINE_THICKNESS, color);
			else if (head->obj_type	== OT_POLYGON)
				draw_polygon(head->x, head->y, head->content.shape->points, head->content.shape->points_len, color);
			else if (head->obj_type == OT_POLYLINE)
				draw_polyline(head->x, head->y, head->content.shape->points, head->content.shape->points_len, color);
			else if (head->obj_type == OT_ELLIPSE)
				DrawEllipseLines(head->x + head->width/2.0, head->y + head->height/2.0, head->width/2.0, head->height/2.0, color);
		}
		head = head->next;
	}
}

void draw_image_layer(tmx_image *image) {
	Texture2D *texture = (Texture2D*)image->resource_image;
	DrawTexture(*texture, 0, 0, WHITE);
}

void draw_tile(Texture2D image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
				 unsigned int dx, unsigned int dy, int opacity, unsigned int flags) {
	DrawTextureRec(image, (Rectangle) {sx, sy, sw, sh}, (Vector2) {dx, dy}, (Color) {opacity, opacity, opacity, opacity});
}

void draw_layer(tmx_map *map, tmx_layer *layer) {
	unsigned long i, j;
	unsigned int gid, x, y, w, h, flags;
	tmx_tileset *ts;
	tmx_image *im;
	Texture2D* image;
	int op = (int)(layer->opacity*255);

	for (i=0; i<map->height; i++) {
		for (j=0; j<map->width; j++) {
			gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
			if (map->tiles[gid] != NULL) {
				ts = map->tiles[gid]->tileset;
				im = map->tiles[gid]->image;
				x = map->tiles[gid]->ul_x;
				y = map->tiles[gid]->ul_y;
				w = ts->tile_width;
				h = ts->tile_height;
				if (im)
					image = im->resource_image;
				else
					image = ts->image->resource_image;
				flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
				draw_tile(*image, x, y, w, h, j*ts->tile_width, i*ts->tile_height, op, flags);
			}
		}
	}
}

void draw_all_layers(tmx_map *map, tmx_layer *layers) {
	while (layers) {
		if (layers->visible) {
			if (layers->type == L_GROUP)
				draw_all_layers(map, layers->content.group_head);
			else if (layers->type == L_OBJGR)
				draw_objects(layers->content.objgr);
			else if (layers->type == L_IMAGE)
				draw_image_layer(layers->content.image);
			else if (layers->type == L_LAYER)
				draw_layer(map, layers);
		}
		layers = layers->next;
	}
}

void render_map(tmx_map *map) {
	ClearBackground(int_to_color(map->backgroundcolor));
	draw_all_layers(map, map->ly_head);
}

int main(void) {
	tmx_img_load_func = raylib_tex_loader; //tmx image loading
	tmx_img_free_func = raylib_free_tex;   //tmx image freeing

	const int screenWidth = 800;
	const int screenHeight = 600;

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(screenWidth, screenHeight, "Test");

	tmx_map *map = tmx_load("assets/testMap.tmx");
	if (map == NULL) {
		tmx_perror("Cannot load map");
		return 1;
	}

	Player player = { 0 };
	player.position = (Vector2){ 120, 128 };
	player.speed = 0;
	player.canJump = false;

	Camera2D camera = { 0 };
	camera.target = player.position;
	camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 4.0f;

	int fontSize = 20;
	int nkWidth = 225;
	struct nk_context *debug = InitNuklear(fontSize);
	struct nk_context *tutorial = InitNuklear(fontSize);

	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		UpdateNuklear(debug);
		UpdateNuklear(tutorial);

		if (nk_begin(debug, "Debug", nk_rect(screenWidth-10-nkWidth, 10, nkWidth, screenHeight-20),
				NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
			nk_layout_row_static(debug, fontSize+5, nkWidth-30, 1);
			nk_label(debug, TextFormat("X: %f", player.position.x), NK_TEXT_LEFT);
			nk_label(debug, TextFormat("Y: %f", player.position.y), NK_TEXT_LEFT);
			nk_label(debug, TextFormat("JMP SPD: %f", playerJumpSpd), NK_TEXT_LEFT);
			nk_slider_float(debug, 50.0f, &playerJumpSpd, 500.0f, 5.0f);
			nk_label(debug, TextFormat("JMP MULT: %f", playerJumpMult), NK_TEXT_LEFT);
			nk_slider_float(debug, 1.0f, &playerJumpMult, 50.0f, 1.0f);
			nk_label(debug, TextFormat("JMP TIME ADD: %f", playerJumpTimeAdd), NK_TEXT_LEFT);
			nk_slider_float(debug, 0.0f, &playerJumpTimeAdd, 10.0f, 0.1f);
			nk_label(debug, TextFormat("JMP TIME MAX: %f", playerJumpTimeMax), NK_TEXT_LEFT);
			nk_slider_float(debug, 0.0f, &playerJumpTimeMax, 0.25f, 0.01f);
			nk_label(debug, TextFormat("MOVE SPEED: %f", playerMoveSpd), NK_TEXT_LEFT);
			nk_slider_float(debug, 50.0f, &playerMoveSpd, 500.0f, 5.0f);
			nk_label(debug, TextFormat("GRAVITY: %f", gravity), NK_TEXT_LEFT);
			nk_slider_float(debug, 50.0f, &gravity, 1000.0f, 25.0f);
		}
		nk_end(debug);

		if (nk_begin(tutorial, "Controls", nk_rect(10, 10, nkWidth, screenHeight-20),
				NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_CLOSABLE)) {
			nk_layout_row_static(tutorial, fontSize+5, nkWidth-30, 1);
			nk_label(tutorial, "- Right/Left to", NK_TEXT_LEFT);
			nk_label(tutorial, "	move", NK_TEXT_LEFT);
			nk_label(tutorial, "- Space to jump", NK_TEXT_LEFT);
			nk_label(tutorial, "- Mouse Wheel to", NK_TEXT_LEFT);
			nk_label(tutorial, "	Zoom in-out, R to", NK_TEXT_LEFT);
			nk_label(tutorial, "	reset zoom", NK_TEXT_LEFT);
		}
		nk_end(tutorial);

		float deltaTime = GetFrameTime();

		UpdatePlayer(&player, map, deltaTime);

		camera.zoom += ((float)GetMouseWheelMove()*0.05f);

		if (camera.zoom > 8.0f) camera.zoom = 8.0f;
		else if (camera.zoom < 1.0f) camera.zoom = 1.0f;

		if (IsKeyPressed(KEY_R)) {
			camera.zoom = 4.0f;
			player.position = (Vector2){ 120, 128 };
		}

		UpdateCameraTest(&camera, &player, deltaTime);

		BeginDrawing();
			BeginMode2D(camera);
				render_map(map);
				Rectangle playerRect = { player.position.x - 6, player.position.y - 16, 12.0f, 16.0f };
				DrawRectangleRec(playerRect, RED);
			EndMode2D();

			DrawFPS(20,20);

			DrawNuklear(debug);
			DrawNuklear(tutorial);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}