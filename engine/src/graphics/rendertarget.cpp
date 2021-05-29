#include <glad/glad.h>

#include "rendertarget.hpp"
#include "logger.hpp"

RenderTarget::RenderTarget(const vec2& size) {
	glGenFramebuffers(1, &m_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fb);

	glGenTextures(1, &m_output);
	glBindTexture(GL_TEXTURE_2D, m_output);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_output, 0);

	glGenRenderbuffers(1, &m_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rb);
}

RenderTarget::~RenderTarget() {
	glDeleteTextures(1, &m_output);
	glDeleteRenderbuffers(1, &m_rb);
	glDeleteFramebuffers(1, &m_fb);
}

void RenderTarget::resize(const vec2& size) {
	if (size == m_size) { return; }

	m_size = size;

	if (m_size.x <= 0.0f) {
		m_size.x = 1.0f;
	}
	if (m_size.y <= 0.0f) {
		m_size.y = 1.0f;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
	glBindTexture(GL_TEXTURE_2D, m_output);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.x, m_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glBindRenderbuffer(GL_RENDERBUFFER, m_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.x, m_size.y);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rb);

	unbind(size);
}

void RenderTarget::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fb);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rb);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, m_size.x, m_size.y);
}

void RenderTarget::unbind(const vec2& viewport) const {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, viewport.x, viewport.y);
}

void RenderTarget::bind_output(u32 unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, m_output);
}
