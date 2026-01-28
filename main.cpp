#include "raylib.h"
#include "raymath.h"

struct Vert
{
	Vector3 position;
	Color color;
	float size;
};

struct MyWave
{
	Vector3 origin;
	double startTime;
	float speed;
	float frequency;
	bool active;
};

void DrawVert(Vector3 pos, float size, Color color);
void UpdateCamera(Camera &camera, const Vector2 &mouseDelta, float deltaTime);

int main()
{
	InitWindow(0, 0, "Economi");

	SetTargetFPS(60);
	// SetWindowState(FLAG_BORDERLESS_WINDOWED_MODE);
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();

	Camera camera = {0};
	camera.position = (Vector3){0.0f, 10.0f, 0.0f};
	camera.target = (Vector3){0.0f, 0.0f, 1.0f};
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};
	camera.fovy = 90.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	Vector2 mouseDelta{0.0f, 0.0f};

	// generate a voxel world
	Vert verts[8000] = {0};
	int dim = 20;
	for (int i = 0; i < dim * dim * dim; i++)
	{
		verts[i].position = Vector3{(float)(i % dim), (float)((i / dim) % dim), (float)(i / (dim * dim))};
		verts[i].color = {
				0,
				255,
				0,
				0};
		// define the voxel at (x, y, z)
		long r = random();
	}

	// Add wave state
	MyWave wave = {0};
	wave.active = false;
	wave.speed = 24.0f;		 // Units per second
	wave.frequency = 4.0f; // Oscillations per unit

	float angle = 0.0f;
	double time = GetTime();
	while (!WindowShouldClose())
	{
		const float deltaTime{GetFrameTime()};
		time += deltaTime;
		// Housekeeping Variables
		// const Vector2 mousePos{GetMousePosition()};
		mouseDelta = GetMouseDelta();
		SetMousePosition(screenWidth / 2, screenHeight / 2);
		HideCursor();

		// Trigger wave on spacebar
		if (IsKeyPressed(KEY_SPACE))
		{
			wave.origin = Vector3Zero(); // Or any arbitrary position
			wave.startTime = time;
			wave.active = true;
		}

		// Then do the rendering
		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);

		// Update and draw verts with wave effect
		if (wave.active)
		{
			double elapsed = time - wave.startTime;
			float waveRadius = elapsed * wave.speed;

			for (int i = 0; i < dim * dim * dim; i++)
			{
				float distance = Vector3Distance(verts[i].position, wave.origin);

				// Calculate wave function: sin based on distance from wavefront
				float waveFront = waveRadius - distance;
				float amplitude = sinf(waveFront * wave.frequency) * 0.5f + 0.5f; // Range [0, 1]

				// Apply falloff so wave is visible only near the front
				float falloff = 0.3f;
				if (waveFront < 0.0f)
				{
					falloff = 0.0f; // Wave hasn't reached yet
				}
				else if (waveFront > 3.0f)
				{
					falloff = expf(-(waveFront - 3.0f) * 0.5f); // Exponential decay after passing
				}

				verts[i].color.a = (unsigned char)(amplitude * falloff * 255.0f);
				DrawVert(verts[i].position, verts[i].color.a * 0.002f, verts[i].color);
			}
		}
		else
		{
			// Original animation when no wave
			angle += (deltaTime * 33.0f);
			for (int i = 0; i < dim * dim * dim; i++)
			{
				verts[i].color.a = (unsigned char)(127 + 127 * sinf(angle * 0.1f + verts[i].position.x));
				DrawVert(verts[i].position, verts[i].color.a * 0.002f, verts[i].color);
			}
		}

		// // Draw the world
		// for (int i = 0; i < dim*dim*dim; i++)
		// {
		// 	DrawCubeWires((Vector3){(float)world[i].position.x, (float)world[i].position.y, (float)world[i].position.z}, 1.0f, 1.0f, 1.0f, world[i].color);
		// }

		// if spacebar is pressed start a wavefront at the origin, and propagate it outward over time

		// angle += (deltaTime * 33.0f);
		// for (int i = 0; i < dim*dim*dim; i++)
		// {
		// 	verts[i].color.a = (unsigned char)(127 + 127 * sinf(angle * 0.1f + verts[i].position.x) * sinf(angle * 0.1f + verts[i].position.y));
		// 	DrawVert(verts[i].position, verts[i].color.a * 0.002f, verts[i].color);
		// }

		// When space is pressed, shoot a projectile from the camera position in the camera direction

		UpdateCamera(camera, mouseDelta, deltaTime);

		EndMode3D();

		// DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 10, 60, 10, GRAY);
		// DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 10, 75, 10, GRAY);
		// DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 10, 90, 10, GRAY);
		if (wave.active)
		{
			DrawText("Wave Active - Press SPACE to restart", 10, 105, 10, GREEN);
		}
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

void DrawVert(Vector3 pos, float size, Color color)
{
	DrawCircle3D(pos, size, (Vector3){0.0f, 1.0f, 0.0f}, 00.0f, color);
	// DrawCircle3D(pos, size, (Vector3){0.0f, 1.0f, 0.0f}, 45.0f, color);
	// DrawCircle3D(pos, size, (Vector3){0.0f, 1.0f, 0.0f}, 90.0f, color);
}

void UpdateCamera(Camera &camera, const Vector2 &mouseDelta, float deltaTime)
{
	const float cameraMoveSpeed = 10.0f;
	const float cameraRotateSpeed = 1.0f;
	// update camera based on mouse
	Vector3 cameraDirection = Vector3Subtract(camera.target, camera.position);
	float pitch = Clamp(-mouseDelta.y * cameraRotateSpeed * deltaTime, -89.0f, 89.0f);
	float yaw = Clamp(-mouseDelta.x * cameraRotateSpeed * deltaTime, -89.0f, 89.0f);
	cameraDirection = Vector3RotateByAxisAngle(cameraDirection, camera.up, yaw);
	Vector3 right = Vector3CrossProduct(cameraDirection, camera.up);
	cameraDirection = Vector3RotateByAxisAngle(cameraDirection, right, pitch);

	// update camera position based on keyboard
	Vector3 forward = Vector3Normalize((Vector3){cameraDirection.x, cameraDirection.y, cameraDirection.z});
	Vector3 rightDir = Vector3Normalize((Vector3){right.x, 0.0f, right.z});
	if (IsKeyDown(KEY_W))
		camera.position = Vector3Add(camera.position, Vector3Scale(forward, cameraMoveSpeed * deltaTime));
	if (IsKeyDown(KEY_S))
		camera.position = Vector3Subtract(camera.position, Vector3Scale(forward, cameraMoveSpeed * deltaTime));
	if (IsKeyDown(KEY_A))
		camera.position = Vector3Subtract(camera.position, Vector3Scale(rightDir, cameraMoveSpeed * deltaTime));
	if (IsKeyDown(KEY_D))
		camera.position = Vector3Add(camera.position, Vector3Scale(rightDir, cameraMoveSpeed * deltaTime));

	camera.target = Vector3Add(camera.position, cameraDirection);
}
