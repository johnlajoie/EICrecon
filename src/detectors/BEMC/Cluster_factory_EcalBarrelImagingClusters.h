// Copyright 2022, Thomas Britton
// Subject to the terms in the LICENSE file found in the top-level directory.
//

#ifndef _Cluster_factory_EcalBarrelImagingClusters_h_
#define _Cluster_factory_EcalBarrelImagingClusters_h_

#include <random>

#include <JANA/JFactoryT.h>
#include <services/geometry/dd4hep/JDD4hep_service.h>
#include <algorithms/calorimetry/ImagingClusterReco.h>
#include <services/log/Log_service.h>
#include <extensions/spdlog/SpdlogExtensions.h>



class Cluster_factory_EcalBarrelImagingClusters : public JFactoryT<edm4eic::Cluster>, ImagingClusterReco {

public:

    std::string m_input_simhit_tag;
    std::string m_input_protoclust_tag;

    //------------------------------------------
    // Constructor
    Cluster_factory_EcalBarrelImagingClusters(){
        SetTag("EcalBarrelImagingClusters");
    }

    //------------------------------------------
    // Init
    void Init() override{
        auto app = GetApplication();
        //-------- Configuration Parameters ------------
        m_input_simhit_tag="EcalBarrelHits";
        m_input_protoclust_tag="EcalBarrelImagingProtoClusters";

        app->SetDefaultParameter("BEMC:EcalBarrelImagingClusters:input_protoclust_tag",        m_input_protoclust_tag, "Name of input collection to use");
        app->SetDefaultParameter("BEMC:EcalBarrelImagingClusters:trackStopLayer",  m_trackStopLayer);


        std::string tag=this->GetTag();
        m_log = app->GetService<Log_service>()->logger(tag);

        initialize();
    }

    //------------------------------------------
    // Process
    void Process(const std::shared_ptr<const JEvent> &event) override{

        
        // Prefill inputs
        m_mcHits=event->Get<edm4hep::SimCalorimeterHit>(m_input_simhit_tag);
        m_inputProtoClusters=event->Get<edm4eic::ProtoCluster>(m_input_protoclust_tag);

        // Call Process for generic algorithm
        execute();

        // Hand owner of algorithm objects over to JANA
        Set(m_outputClusters);
        event->Insert(m_outputAssociations, "EcalBarrelImagingClusterAssociations");
        event->Insert(m_outputLayers, "EcalBarrelImagingLayers");
        m_outputClusters.clear(); // not really needed, but better to not leave dangling pointers around
        m_outputAssociations.clear();
        m_outputLayers.clear();
    }
};

#endif // _Cluster_factory_EcalBarrelImagingClusters_h_
