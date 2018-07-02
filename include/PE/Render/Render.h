#ifndef PANNOIREENGINE_RENDER_H
#define PANNOIREENGINE_RENDER_H

#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gsl/gsl>

namespace PE::Render {

    class Render {
    public:
        Render();
        void render(std::string text);
    };

}



#endif //PANNOIREENGINE_RENDER_H
