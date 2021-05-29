#pragma once

#include "int.hpp"
#include "maths/maths.hpp"

class RenderTarget {
private:
	u32 m_fb, m_rb;
	u32 m_output;

	vec2 m_size;

	mutable vec2 m_old_viewport;
public:
	RenderTarget(const vec2& size);
	~RenderTarget();

	void resize(const vec2& size);

	void bind() const;
	void unbind(const vec2& viewport) const;

	inline const vec2& get_size() { return m_size; }
	inline u32 get_output() { return m_output; }

	void bind_output(u32 unit) const;
};
