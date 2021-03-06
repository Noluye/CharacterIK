#include "CharacterIK.h"
#include "Samples.h"

#include <sstream>

#define JOINT_NUM 4


class Target
{
public:
	Target() {
		position = m3::Vec3(1, 4, 2);
	}

	void Update()
	{
		float leftMargin = 70;
		int i = 0;
		auto topMargin = [](int i) {return 170.0f + i * 30; };
		position.x = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Target X", TextFormat("%f", (float)position.x), position.x, -10, 10);
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
Target g_Target = {};
cik::FABRIKSolver g_FABRIIKSolver = { JOINT_NUM };
float g_ConvergeSpeed = 5;
NJointChain<JOINT_NUM> g_Chain = {
std::vector<m3::Vec3>{ {0, 0, 0}, {0, 2, 0}, {0, 1.5, 0}, {0, 1, 0} }
};
int g_ChosedJointIndex = -1;

std::vector<cik::RotationLimit*> g_RotLimits(JOINT_NUM-1);
cik::RotationLimitHinge g_RotationLimitHinge = {};
m3::Vec3 g_Eulers[JOINT_NUM] = {};
bool g_LimitChecked = true;
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
	g_RotLimits[0] = new cik::RotationLimitAngle(45.0f);
	g_RotLimits[1] = new cik::RotationLimitHinge(-120, 5);
	g_RotLimits[2] = new cik::RotationLimitHinge(-45, 5);
	// ---------------------------------------------
}

static void UpdateProcess()
{
	UpdateCamera(&g_Camera);          // Update camera
	if (IsKeyDown('Z')) g_Camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
	g_Target.Update();
	// ui
	float leftMargin = 70;
	int i = 0;
	auto topMargin = [](int i) {return 280.0f + i * 30; };
	g_LimitChecked = GuiCheckBox(Rectangle{ leftMargin, topMargin(i++), 15, 15 }, "Apply Constraint", g_LimitChecked);
	// ---------------------------------------------
	// IK related
	for (int i = 0; i < g_Chain.m_JointNum; ++i) g_FABRIIKSolver.SetLocalTransform(i, g_Chain.GetLocalTransform(i));
	
	if (g_LimitChecked) g_FABRIIKSolver.Solve(g_Target.GetTransform(), g_RotLimits);  // apply limits
	else  g_FABRIIKSolver.Solve(g_Target.GetTransform());
	
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
	g_Chain.Draw(g_ChosedJointIndex);

	if (g_ChosedJointIndex != -1)
	{
		auto pos = g_Chain[g_ChosedJointIndex].transform.position;
		auto rot = g_Chain[g_ChosedJointIndex].transform.rotation;
		//m3::Vec3 axis = rot * g_Hinge.axis;
		m3::Vec3 axis = rot * m3::Vec3(0.0, 0.0, 1.0);

		//auto rot2 = FromTo(m3::Vec3(0.0, 0.0, 1.0), g_Hinge.axis) * rot;
		auto rot2 = FromTo(m3::Vec3(0.0, 0.0, 1.0), m3::Vec3(0.0, 0.0, 1.0)) * rot;
		DrawCircle3D(ToVector3(pos),
			0.4, ToVector3(GetAxis(rot2)),
			GetAngle(rot2, true), GREEN);

		// range
		DrawLine3D(ToVector3(pos), ToVector3(
			pos + 0.4f * (m3::AngleAxis(axis, g_RotationLimitHinge.m_AngleMin, true) * rot *  m3::Vec3(0.0f, 1.0f, 0.0f))), ORANGE);
		DrawLine3D(ToVector3(pos), ToVector3(
			pos + 0.4f * (m3::AngleAxis(axis, g_RotationLimitHinge.m_AngleMax, true) * rot * m3::Vec3(0.0f, 1.0f, 0.0f))), ORANGE);
		for (int i = g_RotationLimitHinge.m_AngleMin + 5; i < g_RotationLimitHinge.m_AngleMax; i += 5)
		{
			DrawLine3D(ToVector3(pos), ToVector3(
				pos + 0.4f * (m3::AngleAxis(axis, i, true) * rot * m3::Vec3(0.0f, 1.0f, 0.0f))), ORANGE);
		}

	}
	

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

	// joint drop down
	static bool dropDownJointEditMode = false;
	GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
	std::stringstream ss;
	for (int i = 0; i < JOINT_NUM; ++i)
	{
		ss << "Joint " << i;
		if (i != JOINT_NUM - 1) ss << ";";
	}

	if (GuiDropdownBox(Rectangle { GetScreenWidth() - 150.f, 20, 100, 30},
		ss.str().c_str(),
		&g_ChosedJointIndex, dropDownJointEditMode)) dropDownJointEditMode = !dropDownJointEditMode;

	// edit joint rotation
	if (g_ChosedJointIndex >= 0 && g_ChosedJointIndex < JOINT_NUM)
	{
		m3::Vec3& euler = g_Eulers[g_ChosedJointIndex];

		float leftMargin = 70;
		int i = 0;
		auto topMargin = [](int i) {return 170.0f + i * 30; };

		euler.x = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Rotation X", TextFormat("%f", (float)euler.x), euler.x, -180, 180);
		euler.y = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Rotation Y", TextFormat("%f", (float)euler.y), euler.y, -180, 180);
		euler.z = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Rotation Z", TextFormat("%f", (float)euler.z), euler.z, -180, 180);
		
		m3::Vec3 eulerRad = m3::Deg2Rad(euler);
		m3::Quat targetQ = m3::QuatFromEulerXYZ(eulerRad);

		g_Chain.SetLocalRotation(g_ChosedJointIndex, targetQ);  // TODO: outer -> inner
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
	for (auto p : g_RotLimits) delete p;
	//--------------------------------------------------------------------------------------

	return 0;
}