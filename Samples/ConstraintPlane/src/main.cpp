#include "CharacterIK.h"
#include "Samples.h"
#include "rlgl.h"

#define JOINT_NUM 4
// ----------------------------------------------------
// Global Variables
// Define the camera to look into our 3d world
Camera3D g_Camera;
Model g_ModelPlane;
Texture2D g_Texture;
m3::Vec3 g_Position = { 0.0f, 0.1f, 0.0f };
m3::Vec3 g_Euler = {};
m3::Vec3 g_RotEdit = {};
m3::Quat g_Rotation = {};
float g_Scale = 0.1f;
bool g_LimitChecked = true;

// limits
cik::RotationLimitHinge g_RotationLimitHinge = {};
cik::RotationLimitAngle g_RotationLimitAngle = {};
int g_ChosedLimitID = 0;

int g_AxisX = 0;
int g_AxisY = 0;
int g_AxisZ = 0;
bool g_AxisXEditMode = false;
bool g_AxisYEditMode = false;
bool g_AxisZEditMode = false;
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

	// plane model
	g_ModelPlane = LoadModel("../../vendor/raylib/examples/models/resources/models/obj/plane.obj");  // Load model
	g_Texture = LoadTexture("../../vendor/raylib/examples/models/resources/models/obj/plane_diffuse.png"); // Load model texture
	g_ModelPlane.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = g_Texture;            // Set map diffuse texture
	rlSetLineWidth(4);
}

static void UpdateProcess()
{
	UpdateCamera(&g_Camera);          // Update camera
	if (IsKeyDown('Z')) g_Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
}

static void Mode3DProcess()
{
	DrawModelEx(g_ModelPlane, ToVector3(g_Position), ToVector3(m3::GetAxis(g_Rotation)), m3::GetAngle(g_Rotation, true), Vector3(g_Scale, g_Scale, g_Scale), WHITE);
	
	DrawAxis(g_Position, g_Rotation, 5);
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

	const float leftMargin = 90;
	// edit joint rotation
	{
		int i = 0;
		auto topMargin = [](int i) {return 190.0f + i * 30; };

		GuiGroupBox(Rectangle{ 10, topMargin(0) - 30, 320, 150 }, "Rotation Operation");
		float operationSpeed = 40 * GetFrameTime();
		g_RotEdit.x = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Axis X  -", "+", g_RotEdit.x, -operationSpeed, operationSpeed);
		if (g_RotEdit.x != 0)
		{
			g_Rotation = g_Rotation * m3::AngleAxis(m3::Vec3(1.0, 0.0, 0.0), g_RotEdit.x, true);
			g_Euler = m3::ToEuler(g_Rotation, "XYZ", true);
			g_RotEdit.x = 0;
		}
		g_RotEdit.y = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Axis Y  -", "+", g_RotEdit.y, -operationSpeed, operationSpeed);
		if (g_RotEdit.y != 0)
		{
			g_Rotation = g_Rotation * m3::AngleAxis(m3::Vec3(0.0, 1.0, 0.0), g_RotEdit.y, true);
			g_Euler = m3::ToEuler(g_Rotation, "XYZ", true);
			g_RotEdit.y = 0;
		}
		g_RotEdit.z = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Axis Z  -", "+", g_RotEdit.z, -operationSpeed, operationSpeed);
		if (g_RotEdit.z != 0)
		{
			g_Rotation = g_Rotation * m3::AngleAxis(m3::Vec3(0.0, 0.0, 1.0), g_RotEdit.z, true);
			g_Euler = m3::ToEuler(g_Rotation, "XYZ", true);
			g_RotEdit.z = 0;
		}
	}
	// euler
	{
		int i = 0;
		auto topMargin = [](int i) {return 360.0f + i * 30; };

		GuiGroupBox(Rectangle{ 10, topMargin(0) - 30, 320, 150 }, "Euler Angles");
		auto eulerEdit = g_Euler;
		eulerEdit.x = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Euler X", TextFormat("%3.1f", (float)eulerEdit.x), eulerEdit.x, -180, 180);
		eulerEdit.y = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Euler Y", TextFormat("%3.1f", (float)eulerEdit.y), eulerEdit.y, -180, 180);
		eulerEdit.z = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Euler Z", TextFormat("%3.1f", (float)eulerEdit.z), eulerEdit.z, -180, 180);
		if (g_Euler != eulerEdit) g_Rotation = m3::QuatFromEulerXYZ(m3::Deg2Rad(eulerEdit));
		g_Euler = eulerEdit;
	}
	// constraint
	{
		int i = 0;
		auto topMargin = [](int i) {return 530.0f + i * 30; };
		GuiGroupBox(Rectangle{ 10, topMargin(0) - 30, 320, 250 }, "Joint Limits");

		g_LimitChecked = GuiCheckBox(Rectangle{ leftMargin, topMargin(i++), 15, 15}, "Apply Constraint", g_LimitChecked);
		
		if (g_ChosedLimitID == 0)
		{
			// hinge
			g_RotationLimitHinge.m_AngleMin = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Angle Min", TextFormat("%3.1f", (float)g_RotationLimitHinge.m_AngleMin), g_RotationLimitHinge.m_AngleMin, -180, 180);
			g_RotationLimitHinge.m_AngleMax = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Angle Max", TextFormat("%3.1f", (float)g_RotationLimitHinge.m_AngleMax), g_RotationLimitHinge.m_AngleMax, -180, 180);
			// axis
			g_AxisX = (int)g_RotationLimitHinge.axis.x;
			g_AxisY = (int)g_RotationLimitHinge.axis.y;
			g_AxisZ = (int)g_RotationLimitHinge.axis.z;
			if (GuiValueBox(Rectangle{ 30,  topMargin(i), 60, 20 }, "X", &g_AxisX, 0, 10, g_AxisXEditMode)) g_AxisXEditMode = !g_AxisXEditMode;
			if (GuiValueBox(Rectangle{ 130, topMargin(i), 60, 20 }, "Y", &g_AxisY, 0, 10, g_AxisYEditMode)) g_AxisYEditMode = !g_AxisYEditMode;
			if (GuiValueBox(Rectangle{ 230, topMargin(i++), 60, 20 }, "Z", &g_AxisZ, 0, 10, g_AxisZEditMode)) g_AxisZEditMode = !g_AxisZEditMode;
			g_RotationLimitHinge.axis = m3::Normalized(m3::Vec3((float)g_AxisX, (float)g_AxisY, (float)g_AxisZ));
		
			if (g_LimitChecked)
			{
				m3::Quat editRotation = g_RotationLimitHinge.Apply(g_Rotation);
				if (editRotation != g_Rotation) g_Euler = m3::ToEuler(editRotation, "XYZ", true);
				g_Rotation = editRotation;
			}
		}
		else
		{
			// angle
			g_RotationLimitAngle.m_SwingLimit = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Swing Limit", TextFormat("%3.1f", (float)g_RotationLimitAngle.m_SwingLimit), g_RotationLimitAngle.m_SwingLimit, 0, 180);
			// axis
			g_AxisX = (int)g_RotationLimitAngle.axis.x;
			g_AxisY = (int)g_RotationLimitAngle.axis.y;
			g_AxisZ = (int)g_RotationLimitAngle.axis.z;
			if (GuiValueBox(Rectangle{ 30,  topMargin(i), 60, 20 }, "X", &g_AxisX, 0, 10, g_AxisXEditMode)) g_AxisXEditMode = !g_AxisXEditMode;
			if (GuiValueBox(Rectangle{ 130, topMargin(i), 60, 20 }, "Y", &g_AxisY, 0, 10, g_AxisYEditMode)) g_AxisYEditMode = !g_AxisYEditMode;
			if (GuiValueBox(Rectangle{ 230, topMargin(i++), 60, 20 }, "Z", &g_AxisZ, 0, 10, g_AxisZEditMode)) g_AxisZEditMode = !g_AxisZEditMode;
			g_RotationLimitAngle.axis = m3::Normalized(m3::Vec3((float)g_AxisX, (float)g_AxisY, (float)g_AxisZ));

			if (g_LimitChecked)
			{
				m3::Quat editRotation = g_RotationLimitAngle.Apply(g_Rotation);
				if (editRotation != g_Rotation) g_Euler = m3::ToEuler(editRotation, "XYZ", true);
				g_Rotation = editRotation;
			}
		}
		GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
		
		// Limit drop down
		static bool dropDownJointEditMode = false;
		if (GuiDropdownBox(Rectangle{ leftMargin, topMargin(i), 150, 30 },
			"Hinge Limit;Angle Limit",
			&g_ChosedLimitID, dropDownJointEditMode)) dropDownJointEditMode = !dropDownJointEditMode;
	}
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