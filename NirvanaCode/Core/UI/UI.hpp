#pragma once
struct YcButton {
	class UButton* Button = nullptr;
	bool bWasPressed = false;
};

struct YcCkBox {
	class UCheckBox* Toggle = nullptr;
	bool bWasPressed = false;
};