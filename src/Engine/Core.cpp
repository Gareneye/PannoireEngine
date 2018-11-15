#include "PE/Engine/Core.h"


namespace PE::Engine {

    Resource::ResourceHandle<Render::Model> model;
    Resource::ResourceHandle<Render::Shader> shader;
    Render::Camera *camera;

    Core::Core()
            : m_ecs(ECS::MakeECS()),
              m_res_manager(Resource::MakeManager()) {}

    void Core::init() {
        // UTILS
        Utils::Locator::provide(new Utils::ConsoleLogger());
        Utils::log("Core initializing");

        // APP CONFIG
        auto config = m_res_manager->load<Resource::Properties>("config.yml");


        // RENDERER SYSTEM
        Render::init();
        m_context = Render::createContext(
                config->get<std::string>("title"),
                config->get<uint32_t>("width"),
                config->get<uint32_t>("height")
        );

        // RESOURCE MANAGER
        auto texture = m_res_manager->load<Render::Texture>("container.jpg");

        // todo one shader manifest, two shader classes
        shader = m_res_manager->load<Render::Shader>("shader.yml", m_res_manager);

        // MAIN SCENE
        auto scene = m_res_manager->load<Engine::Scene>(config->get<std::string>("main_scene"), m_res_manager, m_ecs);

        // Model
        model = m_res_manager->load<Render::Model>("res/ForestScene.obj", m_res_manager);
        camera = new Render::Camera({0.5f, 0.5f, 15.0f});

        initLoop();
    }

    /**
     * Fixed update
     */
    void Core::fixedUpdate() {
        m_context->processInput();
    }

    /**
     * Free update
     */
    void Core::update(double alpha) {
        m_context->pollEvents();

        // For alpha I need to create transform old and new...
        //m_ecs->updateSystem<Render::RenderSystem>();

        m_context->render([&]() {
            m_context->configCamera(*shader, camera);
            model->draw(*shader);
        });
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
        double dt = 0.01;

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
        Utils::log("Engine turned off");

        model.destroy();
        shader.destroy();
        delete camera;
    }

}