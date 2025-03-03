
#ifndef CalorimeterHit_factory_HcalBarrelRecHits_h_
#define CalorimeterHit_factory_HcalBarrelRecHits_h_

#include <JANA/JFactoryT.h>

#include <algorithms/calorimetry/CalorimeterHitReco.h>
#include <services/log/Log_service.h>
#include <extensions/spdlog/SpdlogExtensions.h>

class CalorimeterHit_factory_HcalBarrelRecHits : public JFactoryT<edm4eic::CalorimeterHit>, CalorimeterHitReco {

public:
    //------------------------------------------
    // Constructor
    CalorimeterHit_factory_HcalBarrelRecHits(){
        SetTag("HcalBarrelRecHits");
    }

    //------------------------------------------
    // Init
    void Init() override{
        auto app = GetApplication();

        m_input_tag = "HcalBarrelRawHits";

        // digitization settings, must be consistent with digi class
        m_capADC=8096; // best guess
        m_dyRangeADC=50. * GeV; // best guess
        m_pedMeanADC=10; // best guess
        m_pedSigmaADC=2; // best guess
        m_resolutionTDC=1 * dd4hep::nanosecond; // best guess

        // zero suppression values
        m_thresholdFactor=5.0;// from ATHENA's reconstruction.py
        m_thresholdValue=0.0;//{this, "thresholdValue", 0.0};

        // energy correction with sampling fraction
        m_sampFrac=0.033;  // average, from sPHENIX simulations

        // geometry service to get ids, ignored if no names provided
        m_geoSvcName="geoServiceName";
        m_readout="HcalBarrelHits";  
        m_layerField="tower";      
        m_sectorField="sector";      

        m_localDetElement="";         
        u_localDetFields={};          

//        app->SetDefaultParameter("HCAL:tag",              m_input_tag);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:capacityADC",      m_capADC);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:dynamicRangeADC",  m_dyRangeADC);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:pedestalMean",     m_pedMeanADC);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:pedestalSigma",    m_pedSigmaADC);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:resolutionTDC",    m_resolutionTDC);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:thresholdFactor",  m_thresholdFactor);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:thresholdValue",   m_thresholdValue);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:samplingFraction", m_sampFrac);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:geoServiceName",   m_geoSvcName);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:readout",          m_readout);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:layerField",       m_layerField);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:sectorField",      m_sectorField);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:localDetElement",  m_localDetElement);
        app->SetDefaultParameter("HCAL:HcalBarrelRecHits:localDetFields",   u_localDetFields);
        m_geoSvc = app->template GetService<JDD4hep_service>(); // TODO: implement named geometry service?

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
        rawhits = event->Get<edm4hep::RawCalorimeterHit>(m_input_tag);

        // Call Process for generic algorithm
        AlgorithmProcess();

        // Hand owner of algorithm objects over to JANA
        Set(hits);
        hits.clear(); // not really needed, but better to not leave dangling pointers around
    }

};

#endif // CalorimeterHit_factory_HcalBarrelRecHits_h_
