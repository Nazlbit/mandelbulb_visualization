#pragma once

class i_safe_destructible
{
protected:
	virtual void safe_destroy() noexcept = 0;
public:
	virtual ~i_safe_destructible() = 0 {};
};
