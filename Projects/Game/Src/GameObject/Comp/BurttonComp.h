#include "../Object.h"
#include "Input/Input.h"

class BurttonComp : public IComp {
public:
	using IComp::IComp;

	~BurttonComp() = default;

public:
	void Init() override;

	void FirstUpdate() override;

	const Lamb::Flg& Pushed();
	const Lamb::Flg& LongPushed();
	const Lamb::Flg& Released();

public:
	Gamepad::Button gButton = Gamepad::Button::A;
	size_t keyButton = DIK_SPACE;
	Mouse::Button mButton = Mouse::Button::Left;

private:
	Lamb::SafePtr<Gamepad> gamePad_;
	Lamb::SafePtr<KeyInput> keyInput_;
	Lamb::SafePtr<Mouse> mouse_;

	Lamb::Flg isPushed_;
	Lamb::Flg isLongPushed_;
	Lamb::Flg isReleased_;
};
