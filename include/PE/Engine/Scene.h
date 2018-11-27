#ifndef PANNOIREENGINE_SCENE_H
#define PANNOIREENGINE_SCENE_H

#include <string>
#include <vector>
#include <memory>

#include <yaml-cpp/yaml.h>

#include "PE/ECS/ECS.h"
#include "PE/Utils/Utils.h"
#include "PE/Resource/Resource.h"
#include "PE/Resource/Properties.h"
#include "PE/Render/Shader.h"
#include "PE/Render/Model.h"
#include "PE/Render/Camera.h"

#include "Component.h"

namespace PE::Engine {

    class Scene : public Resource::IResource<Scene> {
        using PropertiesHandle = Resource::ResourceHandle<Resource::Properties>;
        using ModelHandle = Resource::ResourceHandle<Render::Model>;

        using ResManagerPtr = std::shared_ptr<Resource::ResourceManager>;
        using ECSPtr = std::shared_ptr<ECS::Manager>;

    public:
        explicit Scene(ResManagerPtr manager, ECSPtr ecs)
                : m_ecs(std::move(ecs)),
                  m_manager(std::move(manager))
        {}

        explicit Scene() = delete;
        virtual ~Scene();

        void load(const std::string& path);

    private:
        void makeEntity(const YAML::Node&);

        std::vector<ECS::Entity> m_entities;

        std::vector<ModelHandle> m_models; // todo make Resource bag <id, pool_index>

        ECSPtr m_ecs;
        ResManagerPtr m_manager;
    };


}

#endif //PANNOIREENGINE_SCENE_H
