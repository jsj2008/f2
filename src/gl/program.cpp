//
// Created by Dillon Yao on 4/25/19.
//

#include <fstream>
#include <sstream>
#include <iostream>
#include "program.h"

namespace gl {

Shader::~Shader() {
    glDeleteShader(_shader);
}

void Shader::from_file(const char *filename) {
    std::ifstream input(filename);
    if (input.fail())
        throw std::runtime_error("Could not open file");

    std::ostringstream output;
    output << input.rdbuf();
    input.close();
    std::string src = output.str();
    const char *c_str = src.c_str();

    glShaderSource(_shader, 1, &c_str, nullptr);
    glCompileShader(_shader);

    GLchar infolog[512];
    GLint success;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        glGetShaderInfoLog(_shader, 512, nullptr, infolog);
        std::string error = "\nError in shader \"";
        error += filename;
        error += "\":\n";
        error += infolog;
        throw std::runtime_error(error);
    };
}

VertexShader::VertexShader() {
    _shader = glCreateShader(GL_VERTEX_SHADER);
}

FragmentShader::FragmentShader() {
    _shader = glCreateShader(GL_FRAGMENT_SHADER);
}



Program::Program() {
    _program = glCreateProgram();
};

void Program::use() {
    glUseProgram(_program);
}

void Program::unuse() {
    glUseProgram(0);
}

bool Program::link() {
    glLinkProgram(_program);
    GLchar infolog[512];
    GLint success;
    glGetProgramiv(_program, GL_LINK_STATUS, &success);

    if (success != GL_TRUE) {
        glGetProgramInfoLog(_program, 512, nullptr, infolog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infolog << std::endl;
        return false;
    }

    int max_len, count, length, size;
    GLenum type;

    glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    char name[max_len];

    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &count);
    for (int i = 0; i < count; ++i) {
        glGetActiveAttrib(_program, (GLuint) i, max_len, &length, &size, &type, name);
        _attrs[name] = i;
    }

    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &count);
    for (int i = 0; i < count; ++i) {
        glGetActiveUniform(_program, (GLuint) i, max_len, &length, &size, &type, name);
        _uniforms[name] = std::make_pair(glGetUniformLocation(_program, name), type);
    }

    return true;
}

void Program::attach_shader(Shader &shader) {
    glAttachShader(_program, shader._shader);
}

void Program::detach_shader(Shader &shader) {
    glDetachShader(_program, shader._shader);
}

void Program::detach_all() {
    GLsizei count;
    GLuint shader[64];

    glGetAttachedShaders(_program, 64, &count, shader);
    for (GLsizei i = 0; i < count; ++i) {
        glDetachShader(_program, shader[i]);
    }
}

bool Program::is_linked() {
    GLint success;
    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    return success == GL_TRUE;
}

void Program::get_attrs(std::vector<gl::ProgramArg> &out) {
    int max_len, count, length, size;
    GLenum type;

    glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &count);

    char name[max_len];
    for (int i = 0; i < count; ++i) {
        glGetActiveAttrib(_program, (GLuint) i, max_len, &length, &size, &type, name);
        out.emplace_back(i, type, (char *) name);
    }
}

void Program::get_uniforms(std::vector<gl::ProgramArg> &out) {
    int max_len, count, length, size;
    GLenum type;

    glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_len);
    glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &count);

    char name[max_len];
    for (int i = 0; i < count; ++i) {
        glGetActiveUniform(_program, (GLuint) i, max_len, &length, &size, &type, name);
        out.emplace_back(glGetUniformLocation(_program, name), type, (char *) name);
    }
}

int Program::get_attr_loc(const std::string &name) {
    auto it = _attrs.find(name);
    if (it == _attrs.end())
        return -1;

    return it->second;
}

int Program::get_uniform_loc(const std::string &name) {
    auto it = _uniforms.find(name);
    if (it == _uniforms.end())
        return -1;

    return it->second.first;
}

void Program::uniform(std::string name, void *value) {

}

}
