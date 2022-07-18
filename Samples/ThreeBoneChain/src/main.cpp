#include "CharacterIK.h"
#include "Samples.h"


class Target
{
public:
	Target(){
		position = m3::Vec3(1, 4, 2);
	}

	void Update()
	{
		float leftMargin = 70;
		int i = 0;
		auto topMargin = [](int i) {return 170.0f + i * 30; };
		position.x = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20}, "Target X", TextFormat("%f", (float)position.x), position.x, -10, 10);
		position.y = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Target Y", TextFormat("%f", (float)position.y), position.y, -10, 10);
		position.z = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Target Z", TextFormat("%f", (float)position.z), position.z, -10, 10);
	}

	void Draw()
	{
		DrawSphereWires(ToVector3(position), 0.1, 10, 10, RAYWHITE);
		DrawAxis(position, rotation);
	}

	m3::Transform GetTransform() { return m3::Transform(position, rotation, m3::Vec3()); }

	m3::Vec3 position;
	m3::Quat rotation;
};

// ----------------------------------------------------
// Global Variables
// Define the camera to look into our 3d world
Camera3D g_Camera;
NJointChain<4> g_Chain = {
	std::vector<m3::Vec3>{ {0, 0, 0}, {0, 1, 0}, {0, 2, 0}, {0, 1.5, 0} }
};
Target g_Target = {};
cik::FABRIKSolver g_FABRIIKSolver = {4};
float g_ConvergeSpeed = 5;
// ----------------------------------------------------
static void InitProcess()
{
	// TODO: FullScreen
	const int screenWidth = 1600;
	const int screenHeight = 900;
	InitWindow(screenWidth, screenHeight, "Three Bone Chain");
	GuiLoadStyle("../../vendor/raygui/styles/dark/dark.rgs");
	// Define the camera to look into our 3d world
	g_Camera = Camera3D();
	g_Camera.position = Vector3{ 10.0f, 10.0f, 10.0f }; // Camera position
	g_Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };      // Camera looking at point
	g_Camera.up = Vector3{ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
	g_Camera.fovy = 45.0f;                                // Camera field-of-view Y
	g_Camera.projection = CAMERA_PERSPECTIVE;                   // Camera mode type
	SetCameraMode(g_Camera, CAMERA_FREE); // Set a free camera mode
	SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second

	// ---------------------------------------------
	// IK related
	g_FABRIIKSolver.Resize(4);
	// ---------------------------------------------
}

static void UpdateProcess()
{
	UpdateCamera(&g_Camera);          // Update camera
	if (IsKeyDown('Z')) g_Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };

	g_Target.Update();
	// ---------------------------------------------
	// IK related
	for (int i = 0; i < g_Chain.m_JointNum; ++i) g_FABRIIKSolver.SetLocalTransform(i, g_Chain.GetLocalTransform(i));
	bool succeed = g_FABRIIKSolver.Solve(g_Target.GetTransform());
	for (int i = 0; i < g_Chain.m_JointNum; ++i)
	{
		m3::Transform t = g_FABRIIKSolver.GetLocalTransform(i);
		m3::Quat q = m3::Nlerp(g_Chain.GetLocalRotation(i), t.rotation, g_ConvergeSpeed * GetFrameTime());
		g_Chain.SetLocalRotation(i, q);
	}
	// ---------------------------------------------
}

static void Mode3DProcess()
{
	g_Target.Draw();
	g_Chain.Draw();
	DrawGrid(10, 1.0f);
}

static void DrawProcess()
{
	DrawRectangle(10, 10, 320, 133, Fade(BLANK, 0.5f));
	DrawRectangleLines(10, 10, 320, 133, YELLOW);

	DrawText("Free camera default controls:", 20, 20, 10, GRAY);
	DrawText("- Mouse Wheel to Zoom in-out", 40, 40, 10, GRAY);
	DrawText("- Mouse Wheel Pressed to Pan", 40, 60, 10, GRAY);
	DrawText("- Alt + Mouse Wheel Pressed to Rotate", 40, 80, 10, GRAY);
	DrawText("- Alt + Ctrl + Mouse Wheel Pressed for Smooth Zoom", 40, 100, 10, GRAY);
	DrawText("- Z to zoom to (0, 0, 0)", 40, 120, 10, GRAY);
}

int main()
{
	// Initialization
	//--------------------------------------------------------------------------------------
	InitProcess();
	//--------------------------------------------------------------------------------------
	// Main game loop
	while (!WindowShouldClose())        // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		UpdateProcess();
		//----------------------------------------------------------------------------------
		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(g_Camera);
		Mode3DProcess();
		EndMode3D();

		DrawProcess();

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}