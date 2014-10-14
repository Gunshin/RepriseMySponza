#pragma once
#include <tygra/WindowControlDelegate.hpp>

class MyView;
class MyScene;

class MyController : public tygra::WindowControlDelegate
{
public:
	
    MyController();

    ~MyController();

private:

    void
    windowControlWillStart(std::shared_ptr<tygra::Window> window) override;

    void
    windowControlDidStop(std::shared_ptr<tygra::Window> window) override;

    void
    windowControlViewWillRender(std::shared_ptr<tygra::Window> window) override;

    void
    windowControlMouseMoved(std::shared_ptr<tygra::Window> window,
                            int x,
                            int y) override;	

    void
    windowControlMouseButtonChanged(std::shared_ptr<tygra::Window> window,
                                    int button_index,
                                    bool down) override;

    void
    windowControlMouseWheelMoved(std::shared_ptr<tygra::Window> window,
                                 int position) override;

    void
    windowControlKeyboardChanged(std::shared_ptr<tygra::Window> window,
                                 int key_index,
                                 bool down) override;

    void
    windowControlGamepadAxisMoved(std::shared_ptr<tygra::Window> window,
                                  int gamepad_index,
                                  int axis_index,
                                  float pos) override;

    void
    windowControlGamepadButtonChanged(std::shared_ptr<tygra::Window> window,
                                      int gamepad_index,
                                      int button_index,
                                      bool down) override;

	std::shared_ptr<MyView> view_;
    std::shared_ptr<MyScene> scene_;

    bool camera_turn_mode_;
    bool camera_move_key_[4];
};
