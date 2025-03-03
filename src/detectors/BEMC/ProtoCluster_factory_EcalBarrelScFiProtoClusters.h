// Copyright 2022, David Lawrence
// Subject to the terms in the LICENSE file found in the top-level directory.
//

#ifndef _ProtoCLuster_factory_EcalBarrelScFiIslandProtoClusters_h_
#define _ProtoCLuster_factory_EcalBarrelScFiIslandProtoClusters_h_

#include <random>

#include <JANA/JFactoryT.h>
#include <services/geometry/dd4hep/JDD4hep_service.h>
#include <algorithms/calorimetry/CalorimeterIslandCluster.h>
#include <services/log/Log_service.h>
#include <extensions/spdlog/SpdlogExtensions.h>

class ProtoCluster_factory_EcalBarrelScFiProtoClusters : public JFactoryT<edm4eic::ProtoCluster>, CalorimeterIslandCluster {

public:
    //------------------------------------------
    // Constructor
    ProtoCluster_factory_EcalBarrelScFiProtoClusters(){
        SetTag("EcalBarrelScFiProtoClusters");
    }

    //------------------------------------------
    // Init
    void Init() override{
        auto app = GetApplication();
        m_input_tag = "EcalBarrelScFiMergedHits";

        m_splitCluster=false;               // from ATHENA reconstruction.py
        m_minClusterHitEdep=1.0 * MeV;    // from ATHENA reconstruction.py
        m_minClusterCenterEdep=10.0 * MeV; // from ATHENA reconstruction.py

        // neighbour checking distances
        m_sectorDist=5.0 * cm;             // ?
        u_localDistXY={};     //{this, "localDistXY", {}};
        u_localDistXZ={};     //{this, "localDistXZ", {}};
        u_localDistYZ={};     //{this, "localDistYZ", {}};
        u_globalDistRPhi={};  //{this, "globalDistRPhi", {}};
        u_globalDistEtaPhi={};//{this, "globalDistEtaPhi", {}};
        u_dimScaledLocalDistXY={30 * dd4hep::mm, 30 * dd4hep::mm};// from ATHENA reconstruction.py

        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:input_tag", m_input_tag, "Name of input collection to use");
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:splitCluster",             m_splitCluster);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:minClusterHitEdep",  m_minClusterHitEdep);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:minClusterCenterEdep",     m_minClusterCenterEdep);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:sectorDist",   m_sectorDist);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:localDistXY",   u_localDistXY);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:localDistXZ",   u_localDistXZ);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:localDistYZ",  u_localDistYZ);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:globalDistRPhi",    u_globalDistRPhi);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:globalDistEtaPhi",    u_globalDistEtaPhi);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:dimScaledLocalDistXY",    u_dimScaledLocalDistXY);
        m_geoSvc = app->template GetService<JDD4hep_service>();

        std::string tag=this->GetTag();
        std::shared_ptr<spdlog::logger> m_log = app->GetService<Log_service>()->logger(tag);

        // Get log level from user parameter or default
        std::string log_level_str = "info";
        auto pm = app->GetJParameterManager();
        pm->SetDefaultParameter(tag + ":LogLevel", log_level_str, "verbosity: trace, debug, info, warn, err, critical, off");
        m_log->set_level(eicrecon::ParseLogLevel(log_level_str));
        AlgorithmInit(m_log);
    }

    //------------------------------------------
    // ChangeRun
    void ChangeRun(const std::shared_ptr<const JEvent> &event) override{
        AlgorithmChangeRun();
    }

    //------------------------------------------
    // Process
    void Process(const std::shared_ptr<const JEvent> &event) override{
        // Prefill inputs
        hits = event->Get<edm4eic::CalorimeterHit>(m_input_tag);

        // Call Process for generic algorithm
        AlgorithmProcess();

        // Hand owner of algorithm objects over to JANA
        Set(protoClusters);
        protoClusters.clear(); // not really needed, but better to not leave dangling pointers around
    }
};

#endif // _ProtoCLuster_factory_EcalBarrelScFiIslandProtoClusters_h_
