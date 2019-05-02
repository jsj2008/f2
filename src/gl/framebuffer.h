//
// Created by Dillon Yao on 4/25/19.
//

#ifndef F2_DUP_FRAMEBUFFER_H
#define F2_DUP_FRAMEBUFFER_H


namespace gl {

class Framebuffer {
public:
  explicit Framebuffer(bool init=false);
  ~Framebuffer();
  
  void bind();
  void unbind();

private:
  unsigned _fbo;
};

}

#endif //F2_DUP_FRAMEBUFFER_H
