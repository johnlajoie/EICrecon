// Copyright 2022, David Lawrence
// Subject to the terms in the LICENSE file found in the top-level directory.
//

#pragma once

#include <random>

#include <JANA/JFactoryT.h>
#include <services/geometry/dd4hep/JDD4hep_service.h>
#include <algorithms/calorimetry/ImagingTopoCluster.h>
#include <services/log/Log_service.h>
#include <extensions/spdlog/SpdlogExtensions.h>

class ProtoCluster_factory_EcalBarrelImagingProtoClusters : public JFactoryT<edm4eic::ProtoCluster>, ImagingTopoCluster {

public:

    std::string m_input_tag;

    //------------------------------------------
    // Constructor
    ProtoCluster_factory_EcalBarrelImagingProtoClusters(){
        SetTag("EcalBarrelImagingProtoClusters");
    }

    //------------------------------------------
    // Init
    void Init() override{
        auto app = GetApplication();

        m_input_tag = "EcalBarrelImagingRecHits";

        // from https://eicweb.phy.anl.gov/EIC/benchmarks/physics_benchmarks/-/blob/master/options/reconstruction.py#L593
        u_localDistXY          = {2.0 * mm, 2 * mm};     //  # same layer
        u_layerDistEtaPhi      = {10 * mrad, 10 * mrad}; //  # adjacent layer
        m_neighbourLayersRange = 2.0;                    //  # id diff for adjacent layer
        m_sectorDist           = 3.0 * cm;

        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters:input_tag", m_input_tag, "Name of input collection to use");
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::neighbourLayersRange",    m_neighbourLayersRange);
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::localDistXY",    u_localDistXY);
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::layerDistEtaPhi",    u_layerDistEtaPhi);
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::sectorDist",    m_sectorDist);
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::minClusterHitEdep",    m_minClusterHitEdep);
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::minClusterCenterEdep",    m_minClusterCenterEdep);
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::minClusterEdep",    m_minClusterEdep);
        app->SetDefaultParameter("BEMC:EcalBarrelImagingProtoClusters::neighbourLayersRange",    m_minClusterNhits);

        std::string tag=this->GetTag();
        m_log = app->GetService<Log_service>()->logger(tag);

        initialize();
    }

    //------------------------------------------
    // Process
    void Process(const std::shared_ptr<const JEvent> &event) override{
        // Prefill inputs
        m_inputHits = event->Get<edm4eic::CalorimeterHit>(m_input_tag);

        // Call Process for generic algorithm
        execute();

        // Hand owner of algorithm objects over to JANA
        Set(m_outputProtoClusters);
        m_outputProtoClusters.clear(); // not really needed, but better to not leave dangling pointers around
    }
};

