#pragma once

#include "int.hpp"
#include "maths/maths.hpp"

class DepthBuffer {
private:
	u32 m_fb;
	u32 m_output;

	vec2 m_size;

	mutable vec2 m_old_viewport;
public:
	DepthBuffer(const vec2& size);
	~DepthBuffer();

	void resize(const vec2& size);

	void bind() const;
	void unbind(const vec2& viewport) const;

	inline const vec2& get_size() { return m_size; }
	inline u32 get_output() { return m_output; }

	void bind_output(u32 unit) const;
};
