#include <glad/glad.h>

#include "depthbuffer.hpp"
#include "logger.hpp"

DepthBuffer::DepthBuffer(const vec2& size) {
	glGenFramebuffers(1, &m_fb);

	glGenTextures(1, &m_output);
	glBindTexture(GL_TEXTURE_2D, m_output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_output, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

DepthBuffer::~DepthBuffer() {
	glDeleteFramebuffers(1, &m_fb);
	glDeleteTextures(1, &m_output);
}

void DepthBuffer::resize(const vec2& size) {
	if (size == m_size) { return; }

	m_size = size;

	if (m_size.x <= 0.0f) {
		m_size.x = 1.0f;
	}
	if (m_size.y <= 0.0f) {
		m_size.y = 1.0f;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_fb);

	glDeleteTextures(1, &m_output);

	glGenTextures(1, &m_output);
	glBindTexture(GL_TEXTURE_2D, m_output);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_output, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unbind(size);
}

void DepthBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_size.x, m_size.y);
}

void DepthBuffer::unbind(const vec2& viewport) const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, viewport.x, viewport.y);
}

void DepthBuffer::bind_output(u32 unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_output);
}
