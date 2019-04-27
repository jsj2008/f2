//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_SHADER_H
#define F2_DUP_SHADER_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include <map>

namespace gl {

class Shader {
public:
    ~Shader();

    void from_file(const char *filename);

protected:
    GLuint _shader;

    friend class Program;
};

class VertexShader : public Shader {
public:
    VertexShader();
};

class FragmentShader : public Shader {
public:
    FragmentShader();
};



struct ProgramArg {
    int i;
    GLenum type;
    std::string name;

    ProgramArg(int _i, GLenum _type, char *_name) :
            i(_i), type(_type), name(_name) { }
};

class Program {
public:
    void create();

    void use();
    void unuse();

    bool link();

    void attach_shader(Shader &shader);
    void detach_shader(Shader &shader);
    void detach_all();

    void get_attrs(std::vector<ProgramArg> &out);
    void get_uniforms(std::vector<ProgramArg> &out);

    int get_attr_loc(const std::string &name);
    int get_uniform_loc(const std::string &name);

    void uniform(std::string name, void *value);

    bool is_linked();

private:
    GLuint _program;

    std::map<std::string, int> _attrs;
    std::map<std::string, std::pair<int, GLenum>> _uniforms;
};

}


#endif //F2_DUP_SHADER_H
