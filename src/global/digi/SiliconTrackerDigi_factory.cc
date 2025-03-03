// Created by Dmitry Romanov
// Subject to the terms in the LICENSE file found in the top-level directory.
//

#include <services/log/Log_service.h>
#include <extensions/spdlog/SpdlogExtensions.h>
#include <extensions/string/StringHelpers.h>
#include "SiliconTrackerDigi_factory.h"


void eicrecon::SiliconTrackerDigi_factory::Init() {
    using namespace eicrecon::str;

    auto app = GetApplication();
    auto pm = app->GetJParameterManager();

    std::string plugin_name = ReplaceAll(GetPluginName(), ".so", "");

    // We will use plugin name to get parameters for correct factory
    // So if we use <plugin name>:parameter whichever plugin uses this template. eg:
    //    "BTRK:parameter" or "FarForward:parameter"
    // That has limitations but the convenient in the most of the cases
    std::string param_prefix = plugin_name + ":" + GetTag();   // Will be something like SiTrkDigi_BarrelTrackerRawHit

    // Set input tags
    InitDataTags(param_prefix);

    // Logger. Get plugin level sub-log
    InitLogger(param_prefix, "info");

    // Setup digitization algorithm
    auto cfg = GetDefaultConfig();
    pm->SetDefaultParameter(param_prefix + ":Threshold", cfg.threshold, "EDep threshold for hits to pass through, [GeV]");
    pm->SetDefaultParameter(param_prefix + ":TimeResolution", cfg.timeResolution, "Time resolution gauss smearing [ns]");

    // Initialize digitization algorithm
    m_digi_algo.applyConfig(cfg);
    m_digi_algo.init(m_log);
}

void eicrecon::SiliconTrackerDigi_factory::ChangeRun(const std::shared_ptr<const JEvent> &event) {

}

void eicrecon::SiliconTrackerDigi_factory::Process(const std::shared_ptr<const JEvent> &event) {

    // Collect all hits from different tags
    std::vector<const edm4hep::SimTrackerHit*> total_sim_hits;
    for(const auto &input_tag: GetInputTags()) {
        auto sim_hits = event->Get<edm4hep::SimTrackerHit>(input_tag);
        for (const auto hit : sim_hits) {
            total_sim_hits.push_back(hit);                     /// TODO a better way to concatenate arrays
        }
    }

    // RUN algorithm
    auto digitised_hits = m_digi_algo.produce(total_sim_hits);  // Digitize hits
    this->Set(digitised_hits);                                                       // Add data as a factory output

    // >oO debug
    m_log->trace("SiliconTrackerDigi_factoryT<>::Process(...) end\n");
}

