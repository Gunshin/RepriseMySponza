#pragma once

#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class MyScene;

class MyView : public tygra::WindowViewDelegate
{
public:
	
    MyView();
	
    ~MyView();

    void
    setScene(std::shared_ptr<const MyScene> scene);

private:

    void
    windowViewWillStart(std::shared_ptr<tygra::Window> window);
	
    void
    windowViewDidReset(std::shared_ptr<tygra::Window> window,
                       int width,
                       int height);

    void
    windowViewDidStop(std::shared_ptr<tygra::Window> window);
    
    void
    windowViewRender(std::shared_ptr<tygra::Window> window);

    std::shared_ptr<const MyScene> scene_;

};
