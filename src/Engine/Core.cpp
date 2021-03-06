#include "PE/Engine/Core.h"


namespace PE::Engine {

    Core::Core()
            : m_ecs(ECS::MakeECS()),
              m_res_manager(Resource::MakeManager()),
              m_scripting(Scripting::MakeScriptEngine()) {}

    void Core::init() {
        // UTILS
        Utils::Locator::provide(new Utils::ConsoleLogger());
        Utils::log("Core initializing");

        // Resources
        m_res_manager->registerResource<Resource::Properties>();
        m_res_manager->registerResource<Render::Mesh>();
        m_res_manager->registerResource<Render::Texture>();
        m_res_manager->registerResource<Render::Material>();
        m_res_manager->registerResource<Render::Model>(m_res_manager);
        m_res_manager->registerResource<Render::Shader>(m_res_manager);
        m_res_manager->registerResource<Render::VertexShader>();
        m_res_manager->registerResource<Render::FragmentShader>();
        m_res_manager->registerResource<Scripting::Script>(m_scripting);
        m_res_manager->registerResource<Engine::Scene>(m_res_manager, m_ecs);


        // APP CONFIG
        auto config = m_res_manager->load<Resource::Properties>("config.yml");

        // RENDER CONTEXT
        m_context = Render::createContext(
                config->get<std::string>("title"),
                config->get<uint32_t>("width"),
                config->get<uint32_t>("height")
        );
        Render::Texture::placeholdersInit();

        // SCRIPTING
        m_scripting->init();
        m_api = std::make_shared<Engine::API>(m_scripting, m_ecs);

        // MAIN SCENE
        m_camera = std::make_shared<Render::Camera>(
                config->get<uint32_t>("width"),
                config->get<uint32_t>("height")
        );

        m_context->setInputCallback([&](uint32_t key, uint32_t action) {

            if (key == 68) // d
                m_camera->move(Render::Camera_Movement::RIGHT);

            if (key == 65) // a
                m_camera->move(Render::Camera_Movement::LEFT);

            if (key == 87) // w
                m_camera->move(Render::Camera_Movement::FORWARD);

            if (key == 83) // s
                m_camera->move(Render::Camera_Movement::BACKWARD);
        });


        m_shader = m_res_manager->load<Render::Shader>("shader.yml");
        m_scene = m_res_manager->load<Engine::Scene>(config->get<std::string>("main_scene"));


        // RENDERER
        m_renderer = std::make_shared<Render::Renderer>(m_camera);

        m_ecs->view<Component::Transform, Component::Camera>()
                .each([&](const ECS::Entity entity, Component::Transform &t, Component::Camera &c) {
                    m_camera->setPos(t.xPos, t.yPos, t.zPos);
                    m_camera->rotate(t.xAngle, t.yAngle);
                    m_camera->zoom(c.zoom);
                });


        // Monitor
        auto monitor = std::make_unique<Resource::WindowsFileMonitor>();

        monitor->watchDirectory("res", [&](const std::string &filename) {
            Utils::log(filename + " has been changed!");
            m_res_manager->addMsg<Resource::ResourceEvents::FILE_CHANGED>(Resource::FileInfo{filename});
        });



        initLoop();
    }

    /**
     * Fixed update
     */
    void Core::fixedUpdate() {
        m_context->processInput();

        m_renderer->reset();

        m_res_manager->dispatch();

        m_ecs->view<Component::Script>()
                .each([&](const ECS::Entity entity, Component::Script &s) {
                    auto script = m_res_manager->get<Scripting::Script>(s.resIndex)->getBehaviour(ECS::getIndex(entity));
                    script.fixedUpdate();
                });

        m_ecs->view<Component::Transform, Component::Light>()
                .each([&](const ECS::Entity entity, Component::Transform &t, Component::Light &l) {
                    auto light = Render::Light();
                    light.x = t.xPos;
                    light.y = t.yPos;
                    light.z = t.zPos;

                    light.r = l.colorR;
                    light.g = l.colorG;
                    light.b = l.colorB;

                    m_renderer->setLight(light);
                });

        m_ecs->view<Component::Transform, Component::Model>()
                .each([&](const ECS::Entity entity, Component::Transform &t, Component::Model &r) {

                    for (auto &element : m_res_manager->get<Render::Model>(r.resIndex)->getObject()) {
                        m_renderer->add(
                                element.first.get(), element.second.get(),
                                t.xPos, t.yPos, t.zPos,
                                t.xAngle, t.yAngle, t.zAngle,
                                t.xScale, t.yScale, t.zScale
                        );
                    }

                });

        m_renderer->sort();
    }

    /**
     * Free update
     */
    void Core::update(double alpha) {
        m_context->pollEvents();

        m_ecs->view<Component::Script>()
                .each([&](const ECS::Entity entity, Component::Script &s) {
                    auto script = m_res_manager->get<Scripting::Script>(s.resIndex)->getBehaviour(ECS::getIndex(entity));
                    script.update(alpha);
                });

        auto bg = m_scene->getBg();

        m_context->render([&]() {
            m_renderer->render(*m_shader, alpha);
        }, {bg.r, bg.g, bg.b});
    }

    /**
     * Fixed timestamp loop
     */
    void Core::initLoop() {
        Utils::log("Main loop initializing");

        /**
         * LOOP
         */

        double t = 0.0;
        const double dt = 0.01;

        double currentTime = m_context->getTime();
        double accumulator = 0.0;

        while (m_context->isRunning()) {
            double newTime = m_context->getTime();
            double frameTime = newTime - currentTime;

            if (frameTime > 0.25)
                frameTime = 0.25;

            currentTime = newTime;
            accumulator += frameTime;

            while (accumulator >= dt) {
                // logic here
                fixedUpdate();

                t += dt;
                accumulator -= dt;
            }

            const double alpha = accumulator / dt;

            // render here
            update(alpha);
        }

    }

    Core::~Core() {
        Render::Texture::placeholdersDestroy();

        Utils::log("Engine turned off");
    }

}