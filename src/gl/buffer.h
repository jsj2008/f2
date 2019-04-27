//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_BUFFER_H
#define F2_DUP_BUFFER_H

#include <glad/glad.h>

namespace gl {

class Buffer {
public:
    Buffer();
    ~Buffer();

    void bind();
    void unbind();
    void data(unsigned int size, const void *data, GLenum usage);

private:
    GLuint _buffer;
};

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    void bind();
    void unbind();
    void vertex_attr(unsigned int location, int size, GLenum type, GLboolean normalize, int stride,
                     uint64_t offset);

private:
    GLuint _vao;
};

}

#endif //F2_DUP_BUFFER_H
