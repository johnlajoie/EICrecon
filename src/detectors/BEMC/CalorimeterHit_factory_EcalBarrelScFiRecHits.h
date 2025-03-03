
#ifndef CalorimeterHit_factory_EcalBarrelScFiRecHits_h_
#define CalorimeterHit_factory_EcalBarrelScFiRecHits_h_

#include <JANA/JFactoryT.h>

#include <algorithms/calorimetry/CalorimeterHitReco.h>
#include <services/log/Log_service.h>
#include <extensions/spdlog/SpdlogExtensions.h>

class CalorimeterHit_factory_EcalBarrelScFiRecHits : public JFactoryT<edm4eic::CalorimeterHit>, CalorimeterHitReco {

public:
    //------------------------------------------
    // Constructor
    CalorimeterHit_factory_EcalBarrelScFiRecHits(){
        SetTag("EcalBarrelScFiRecHits");
    }

    //------------------------------------------
    // Init
    void Init() override{
        auto app = GetApplication();

        m_input_tag = "EcalBarrelScFiRawHits";

        // digitization settings, must be consistent with digi class
        m_capADC=8096;//{this, "capacityADC", 8096};
        m_dyRangeADC=100. * MeV;//{this, "dynamicRangeADC", 100. * MeV};
        m_pedMeanADC=400;//{this, "pedestalMean", 400};
        m_pedSigmaADC=3.2;//{this, "pedestalSigma", 3.2};
        m_resolutionTDC=10 * dd4hep::picosecond;//{this, "resolutionTDC", 10 * ps};

        // zero suppression values
        m_thresholdFactor=5.0;// from ATHENA's reconstruction.py
        m_thresholdValue=0.0;//{this, "thresholdValue", 0.0};

        // energy correction with sampling fraction
        m_sampFrac=0.10262666247845109;// from ${DETECTOR_PATH}/calibrations/emcal_barrel_calibration.json

        // geometry service to get ids, ignored if no names provided
        m_geoSvcName="geoServiceName";
        m_readout="EcalBarrelScFiHits";  // from ATHENA's reconstruction.py
        m_layerField="layer";            // from ATHENA's reconstruction.py (i.e. not defined there)
        m_sectorField="module";          // from ATHENA's reconstruction.py

        m_localDetElement="";         // from ATHENA's reconstruction.py (i.e. not defined there)
        u_localDetFields={"system", "module"};          // from ATHENA's reconstruction.py (i.e. not defined there)

//        app->SetDefaultParameter("BEMC:tag",              m_input_tag);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:input_tag",        m_input_tag, "Name of input collection to use");
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:capacityADC",      m_capADC);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:dynamicRangeADC",  m_dyRangeADC);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:pedestalMean",     m_pedMeanADC);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:pedestalSigma",    m_pedSigmaADC);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:resolutionTDC",    m_resolutionTDC);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:thresholdFactor",  m_thresholdFactor);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:thresholdValue",   m_thresholdValue);
        app->SetDefaultParameter("BEMC:EcalBarrelScFiRecHits:samplingFraction", m_sampFrac);
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

#endif // CalorimeterHit_factory_EcalBarrelScFiRecHits_h_
