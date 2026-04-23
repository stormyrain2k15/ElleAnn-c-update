/*******************************************************************************
 * WorldModel.cpp — Environmental Awareness & Theory of Mind
 ******************************************************************************/
#include "../../Shared/ElleTypes.h"
#include "../../Shared/ElleServiceBase.h"
#include "../../Shared/ElleLLM.h"
#include "../../Shared/ElleLogger.h"
#include "../../Shared/ElleConfig.h"
#include "../../Shared/ElleSQLConn.h"
#include <vector>
#include <unordered_map>

class WorldModel {
public:
    bool Initialize() {
        /* Hydrate from SQL so the in-memory entity list starts warm, not
         * cold on every process restart. Previously Initialize just
         * logged and returned — so even though entities were persisted
         * via StoreEntity, the next boot forgot all of them.             */
        std::vector<ELLE_WORLD_ENTITY> rows;
        if (ElleDB::GetAllEntities(rows)) {
            m_entities = std::move(rows);
            ELLE_INFO("World model initialized (hydrated %zu entities)", m_entities.size());
        } else {
            ELLE_INFO("World model initialized (cold start, no entities)");
        }
        return true;
    }

    /* Add or update an entity (person, concept, place) */
    void UpdateEntity(const std::string& name, const std::string& type,
                      const std::string& observation) {
        for (auto& e : m_entities) {
            if (std::string(e.name) == name) {
                e.interaction_count++;
                e.last_interaction_ms = ELLE_MS_NOW();
                e.familiarity = std::min(1.0f, e.familiarity + 0.05f);
                
                /* Update mental model with new observation */
                std::string model(e.mental_model);
                model += "\n" + observation;
                if (model.size() > ELLE_MAX_MSG - 1) model = model.substr(model.size() - ELLE_MAX_MSG + 1);
                strncpy_s(e.mental_model, model.c_str(), ELLE_MAX_MSG - 1);

                ElleDB::StoreEntity(e);
                return;
            }
        }

        /* New entity */
        ELLE_WORLD_ENTITY entity = {};
        strncpy_s(entity.name, name.c_str(), ELLE_MAX_NAME - 1);
        strncpy_s(entity.type, type.c_str(), ELLE_MAX_TAG - 1);
        strncpy_s(entity.description, observation.c_str(), ELLE_MAX_MSG - 1);
        entity.familiarity = 0.1f;
        entity.sentiment = 0.0f;
        entity.trust = 0.5f;
        entity.interaction_count = 1;
        entity.last_interaction_ms = ELLE_MS_NOW();
        strncpy_s(entity.mental_model, observation.c_str(), ELLE_MAX_MSG - 1);

        m_entities.push_back(entity);
        ElleDB::StoreEntity(entity);
        ELLE_INFO("New entity: %s (%s)", name.c_str(), type.c_str());
    }

    /* Theory of Mind and world summary helpers (PredictBehavior /
     * GetWorldSummary) used to live here but had no live call sites
     * anywhere in the code base — removed during the second-wave audit.
     * If reintroduced, route them through explicit IPC query opcodes so
     * they actually get exercised.                                       */

    /* Decay familiarity for entities not recently interacted with */
    void Tick() {
        float decay = (float)ElleConfig::Instance().GetFloat("world_model.familiarity_decay_rate", 0.001);
        for (auto& e : m_entities) {
            e.familiarity = std::max(0.0f, e.familiarity - decay);
        }
    }

private:
    std::vector<ELLE_WORLD_ENTITY> m_entities;
};

class ElleWorldModelService : public ElleServiceBase {
public:
    ElleWorldModelService()
        : ElleServiceBase(SVC_WORLD_MODEL, "ElleWorldModel",
                          "Elle-Ann World Model",
                          "Environmental awareness and theory of mind") {}

protected:
    bool OnStart() override {
        m_model.Initialize();
        SetTickInterval(60000); /* Update every minute */
        ELLE_INFO("World model service started");
        return true;
    }

    void OnStop() override { ELLE_INFO("World model service stopped"); }
    void OnTick() override { m_model.Tick(); }

    void OnMessage(const ElleIPCMessage& msg, ELLE_SERVICE_ID sender) override {
        if (msg.header.msg_type == IPC_WORLD_STATE) {
            ELLE_WORLD_ENTITY entity;
            if (msg.GetPayload(entity)) {
                m_model.UpdateEntity(entity.name, entity.type, entity.description);
            }
        }
    }

    std::vector<ELLE_SERVICE_ID> GetDependencies() override {
        return { SVC_HEARTBEAT };
    }

private:
    WorldModel m_model;
};

ELLE_SERVICE_MAIN(ElleWorldModelService)
