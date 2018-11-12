#include "Component.h"
#include "../../Maths/Matrix2D.h"
#include "../../OpenGL/Shader.h"
#include "../../Maths/Vector2D.h"


class Transform : public Component
{
	public:
	Transform();
	~Transform();

	void Update();
	
	void Serialize(JSONObject obj);

	Component* Create();
	
	public:
	Vector2D mPosition;
	Vector2D mScale;
	float mAngle;
};