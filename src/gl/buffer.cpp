//
// Created by Dillon Yao on 4/25/19.
//

#include "buffer.h"

namespace gl {

Buffer::Buffer() {
    glGenBuffers(1, &_buffer);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &_buffer);
}

void Buffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
}

void Buffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffer::data(unsigned int size, const void *data, GLenum usage) {
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}



VertexArray::VertexArray() {
    glGenVertexArrays(1, &_vao);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &_vao);
}

void VertexArray::bind() {
    glBindVertexArray(_vao);
}

void VertexArray::unbind() {
    glBindVertexArray(0);
}

void VertexArray::vertex_attr(unsigned int location, int size, GLenum type, GLboolean normalize, int stride,
                              uint64_t offset) {
    glVertexAttribPointer(location, size, type, normalize, stride, (void *) offset);
    glEnableVertexAttribArray(location);
}

}