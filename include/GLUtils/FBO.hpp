#ifndef _FBO_HPP_
#define _FBO_HPP_

#include "GLutils/GLUtils.hpp"

namespace GLUtils {
	class FBO
	{
	public:

		FBO(unsigned int width, unsigned int height) : m_width(width), m_height(height)
		{
			
			glGenTextures(1, &m_texture);
			glBindTexture(GL_TEXTURE_2D, m_texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			
			glGenFramebuffers(1, &m_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
			glDrawBuffer(GL_NONE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

			glClear(GL_COLOR_BUFFER_BIT);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

		}

		~FBO()
		{
			glDeleteFramebuffers(1, &m_fbo);
		}

		void bind() {
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		}

		void unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		unsigned int getWidth() { return m_width; }
		unsigned int getHeight() { return m_height; }

		GLuint getTexture() { return m_texture; }

	private:
		GLuint m_fbo;
		GLuint m_texture;
		unsigned int m_width, m_height;
	};
}

#endif // _FBO_HPP_

/*
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

// Create FBO and attach buffers

glGenFramebuffersEXT(1, &fbo);
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
glDrawBuffer(GL_NONE);
glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture, 0);

glClear(GL_DEPTH_BUFFER_BIT);

glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
glBindTexture(GL_TEXTURE_2D, 0);

//Check for completeness
CHECK_GL_ERRORS();
CHECK_GL_FBO_COMPLETENESS();
*/
