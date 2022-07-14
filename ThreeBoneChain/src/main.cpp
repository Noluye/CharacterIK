#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "CharacterIK.h"
#include "FABRIKSolver.h"

static Vector3 ToVector3(m3::Vec3 v)
{
	return Vector3(v.x, v.y, v.z);
}

static void DrawAxis(const m3::Vec3& pos, const m3::Quat rot, const float scale=0.5f)
{
	m3::Vec3 axis0 = pos + rot * (m3::Vec3(1.0f, 0.0f, 0.0f) * scale);
	m3::Vec3 axis1 = pos + rot * (m3::Vec3(0.0f, 1.0f, 0.0f) * scale);
	m3::Vec3 axis2 = pos + rot * (m3::Vec3(0.0f, 0.0f, 1.0f) * scale);

	DrawLine3D(ToVector3(pos), ToVector3(axis0), RED);
	DrawLine3D(ToVector3(pos), ToVector3(axis1), GREEN);
	DrawLine3D(ToVector3(pos), ToVector3(axis2), BLUE);
}

struct Joint
{
	m3::Vec3 position;
	m3::Quat rotation;
	m3::Vec3 localPosition;
	m3::Quat localRotation;
	int parent;

	Joint() 
		:position(), rotation(), localPosition(m3::Vec3()), localRotation(m3::Quat()), parent(-1)
	{}
	Joint(m3::Vec3 pos, m3::Quat rot, int p)
		:position(), rotation(), localPosition(pos), localRotation(rot), parent(p)
	{}

	void Draw()
	{
		DrawSphereWires(ToVector3(position), 0.1, 10, 10, GOLD);
		DrawAxis(position, rotation);
	}
};

class ThreeBoneChain
{
public:
	ThreeBoneChain()
	{
		joints[0] = Joint(m3::Vec3(), m3::Quat(), -1);
		joints[1] = Joint(m3::Vec3(0, 1, 0), m3::Quat(), 0);
		joints[2] = Joint(m3::Vec3(0, 2, 0), m3::Quat(), 1);
		joints[3] = Joint(m3::Vec3(0, 1.5, 0), m3::Quat(), 2);
	}

	void ForwardKinematics()
	{
		for (int i = 0; i < 4; ++i)
		{
			int p = joints[i].parent;
			if (p != -1)
			{
				joints[i].position = joints[p].position + joints[p].rotation * joints[i].localPosition;
				joints[i].rotation = joints[p].rotation * joints[i].localRotation;
			}
			else
			{
				joints[i].position = joints[i].localPosition;
				joints[i].rotation = joints[i].localRotation;
			}
		}
	}

	void Draw()
	{
		for (int i = 0; i < 4; ++i)
		{
			int p = joints[i].parent;
			if (p != -1) DrawCylinderEx(ToVector3(joints[i].position), ToVector3(joints[p].position), 0.04, 0.04, 10, DARKBLUE);
			joints[i].Draw();
		}
	}

	Joint joints[4];
};

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
		position.x = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20}, "Target X", TextFormat("%i", (float)position.x), position.x, -10, 10);
		position.y = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Target Y", TextFormat("%i", (float)position.y), position.y, -10, 10);
		position.z = GuiSliderBar(Rectangle{ leftMargin, topMargin(i++), 200, 20 }, "Target Z", TextFormat("%i", (float)position.z), position.z, -10, 10);
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
ThreeBoneChain g_Chain = {};
Target g_Target = {};
FABRIKSolver g_FABRIIKSolver = {4};
// ----------------------------------------------------
static void InitProcess()
{
	// TODO: FullScreen
	const int screenWidth = 1600;
	const int screenHeight = 900;
	InitWindow(screenWidth, screenHeight, "Three Bone Chain");
	GuiLoadStyle("../vendor/raygui/styles/dark/dark.rgs");
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
	for (int i = 0; i < 4; ++i)
	{
		m3::Transform t = m3::Transform(g_Chain.joints[i].localPosition, g_Chain.joints[i].localRotation);
		g_FABRIIKSolver.SetLocalTransform(i, t);
	}
	g_FABRIIKSolver.Solve(g_Target.GetTransform());
	for (int i = 0; i < 4; ++i)
	{
		m3::Transform t = g_FABRIIKSolver.GetLocalTransform(i);
		//g_Chain.joints[i].localPosition = t.position;
		g_Chain.joints[i].localRotation = t.rotation;
	}
	// ---------------------------------------------
	// 
	// FABRIK
	//float maxReach = 0;
	//for (int i = 0; i < 4; ++i)
	//{

	//}
	//m3::Vec3 tipToTarget = target.position - chain.joints[3].position;
	//// If the target is unreachable
	//if (LenSq(tipToTarget) > LenSq)

	g_Chain.ForwardKinematics();
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