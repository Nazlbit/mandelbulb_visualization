#pragma once
#include "i_safe_destructible.h"

class i_renderer : public i_safe_destructible
{
public:
	virtual void on_framebuffer_resize(int x, int y) = 0;
	virtual void render(const double deltaTime) = 0;
	virtual ~i_renderer() = 0 {};
};
