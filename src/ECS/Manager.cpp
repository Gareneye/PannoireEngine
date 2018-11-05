
#include "PE/ECS/Manager.h"

namespace PE::ECS {

    /**
     * Creating entity
     * @return
     */
    const Entity Manager::createEntity() {
        EntityIndex index;
        EntityVersion version;

        if (m_free_entity_list.empty()) {
            index = m_entity_index_counter++;

            accomodateEntity(index);
            version = m_entity_version[index] = 1;
        } else {
            index = m_free_entity_list.back();
            m_free_entity_list.pop_back();
            version = m_entity_version[index];
        }

        Utils::log(std::to_string(index) + " Entity created.");

        return getEntity(index, version);
    }

    /**
     * Destroying entity
     * @param entity
     */
    void Manager::destroyEntity(Entity entity) {
        const EntityIndex index = getIndex(entity);
        auto mask = m_entity_component_mask[index];

        // remove each component for this entity
        for (ComponentFamily family = 0; family < m_component_pools.size() /* it means for every component */; ++family) {
            if(mask & (ComponentFamily (1) << family))
            {
                removeComponent(entity, family);
            }
        }

        m_entity_component_mask[index] = 0;
        m_entity_version[index]++;
        m_free_entity_list.push_back(index);

        Utils::log(std::to_string(index)+ " Entity destroyed.");
    }

    /**
     * Creating space for Entity
     * @param index
     */
    void Manager::accomodateEntity(EntityIndex index) {
        if (m_entity_component_mask.size() <= index) {
            m_entity_component_mask.resize(index + 1);
            m_entity_version.resize(index + 1);
        }
    }




}