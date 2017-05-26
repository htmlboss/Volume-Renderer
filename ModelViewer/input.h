#ifndef INPUT_H
#define INPUT_H

#include <QPoint>
#include <QCursor>

class OGLWidget;
/***********************************************************************************/
class Input {
	friend class OGLWidget;
public:
	enum class InputState : unsigned int {
		Invalid,
		Registered,
		Unregistered,
		Triggered,
		Pressed,
		Released
	};

	static InputState keyState(const Qt::Key key);
	static auto keyTriggered(const Qt::Key key) { return keyState(key) == InputState::Triggered; }
	static auto keyPressed(const Qt::Key key) { return keyState(key) == InputState::Pressed; }
	static auto keyReleased(const Qt::Key key) { return keyState(key) == InputState::Released; }

	static InputState buttonState(const Qt::MouseButton button);
	static auto buttonTriggered(const Qt::MouseButton button) { return buttonState(button) == InputState::Triggered; }
	static auto buttonPressed(const Qt::MouseButton button) { return buttonState(button) == InputState::Pressed; }
	static auto buttonReleased(const Qt::MouseButton button) { return buttonState(button) == InputState::Released; }

	static auto mousePosition() noexcept { return QCursor::pos(); }
	static QPoint mouseDelta() noexcept;

private:
	static void update();
	static void registerKeyPress(const int key);
	static void registerKeyRelease(const int key);
	static void registerMousePress(const Qt::MouseButton button);
	static void registerMouseRelease(const Qt::MouseButton button);
	static void reset() noexcept;
};

#endif // INPUT_H
