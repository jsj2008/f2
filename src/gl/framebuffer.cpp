//
// Created by Dillon Yao on 4/25/19.
//

#include <glad/glad.h>
#include "framebuffer.h"

namespace gl {

Framebuffer::Framebuffer(bool init) : _fbo(0) {
  if (init)
    glGenFramebuffers(1, &_fbo);
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &_fbo);
}

void Framebuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

void Framebuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}
