#ifndef PE_RENDER_TEXTURE_H
#define PE_RENDER_TEXTURE_H

#include <cstdint>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gsl/gsl>

#include "Shader.h"
#include "PE/Resource/Resource.h"
#include "PE/Utils/Utils.h"

namespace PE::Render {

    class Texture : public Resource::IResource<Texture> {
    public:
        Texture(uint32_t id, int32_t width, int32_t height, int32_t components) : m_id(id), m_width(width), m_height(height), m_components(components) {};
        explicit Texture() = default;
        virtual ~Texture();

        void load(const std::string & path);

        inline void bindTexture() const {
            glBindTexture(GL_TEXTURE_2D, m_id);
        }

        inline void bindTexture(uint8_t index) const {
            glActiveTexture(GL_TEXTURE0 + index);
            glBindTexture(GL_TEXTURE_2D, m_id);
        }

        inline void bindTexture(const Shader &shader, const std::string& name, uint8_t index) const {
            glActiveTexture(GL_TEXTURE0 + index);
            shader.set(name, static_cast<int>(index));
            glBindTexture(GL_TEXTURE_2D, m_id);
        }

        inline bool isTrasparent() const {
            return (m_components == 4);
        }

        static void placeholdersInit();
        static void placeholdersDestroy();
        static Texture *white, *black, *transparent;

    private:
        void loadImageFromFile(const std::string& path);

        uint32_t m_id{0}; //GLuint is 32bit
        int32_t m_width{0};
        int32_t m_height{0};
        int32_t m_components{3};
    };

}

#endif //PE_RENDER_TEXTURE_H
