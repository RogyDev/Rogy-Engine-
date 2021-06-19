#ifndef UI_CHECKER_H
#define UI_CHECKER_H

#include "UITransform.h"

class UIChecker : public UITransform
{
	BASE_COMPONENT()

	bool checked = false;
	float alpha = 1.0f;

	bool intersecting = false;
	bool clicked = false;
	bool once_click = false;

	bool CheckIntersection(int mouseX, int mouseY, bool LeftClicked)
	{
		if (mouseX < Position.x + Scale.x && mouseX > Position.x - Scale.x
			&& mouseY < Position.y + Scale.y && mouseY > Position.y - Scale.y)
		{
			intersecting = true;
		}
		else intersecting = false;

		if (intersecting && LeftClicked && !once_click)
		{
			clicked = true;
			checked = !checked;
			once_click = true;
			std::cout << "Check : " << checked << std::endl;
		}
		if(once_click && !LeftClicked)
			once_click = false;

		return intersecting;
	}
};


#endif
