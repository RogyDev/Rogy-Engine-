#ifndef UI_Button_H
#define UI_Button_H

#include "UITransform.h"

class UIButton : public UITransform
{
	BASE_COMPONENT()

	glm::vec3 color = glm::vec3(1.0f);
	glm::vec3 MainColor = glm::vec3(1.0f);
	glm::vec3 IntersectColor = glm::vec3(0.7f);
	glm::vec3 ClickColor = glm::vec3(0.5f);

	glm::vec3 TextColor = glm::vec3(1.0f);
	glm::vec3 TextColorIntersect = glm::vec3(0.5f);
	float alpha = 1.0f;
	std::string text = "Button";
	bool ChangeColorOnClick = false;

	glm::vec2 text_size = glm::vec2(0.5f, 0.5f);
	glm::vec2 text_offset = glm::vec2(10.0f, 5.0f);

	glm::vec3 texCol = glm::vec3(1.0f);

	bool intersecting = false;
	bool clicked = false;
	bool once_click = false;

	bool CheckIntersection(int mouseX, int mouseY, bool LeftClicked)
	{
		if (mouseX < Position.x + Scale.x && mouseX > Position.x - Scale.x
			&& mouseY < Position.y + Scale.y && mouseY > Position.y - Scale.y)
		{
			intersecting = true;
			color = IntersectColor;
			if (once_click) {
				color = ClickColor;
			}
		}
		else {
			intersecting = false;
			color = MainColor;
		}
		if (intersecting && LeftClicked && !once_click)
		{
			clicked = true;
			once_click = true;
			std::cout << "BUTTON Clicked!\n";
		}
		if(once_click && !LeftClicked)
			once_click = false;

		if (once_click && intersecting)
			texCol = MainColor;
		else if (intersecting)
			texCol = TextColorIntersect;
		else
			texCol = TextColor;

		return intersecting;
	}
};


#endif
