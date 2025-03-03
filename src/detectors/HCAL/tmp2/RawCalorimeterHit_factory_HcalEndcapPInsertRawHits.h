// Copyright 2022, David Lawrence
// Subject to the terms in the LICENSE file found in the top-level directory.
//

#ifndef _RawCalorimeterHit_factory_HcalEndcapPInsertRawHits_h_
#define _RawCalorimeterHit_factory_HcalEndcapPInsertRawHits_h_

#include <random>

#include <JANA/JEvent.h>
#include <JANA/JFactoryT.h>
#include <services/geometry/dd4hep/JDD4hep_service.h>
#include <algorithms/calorimetry/CalorimeterHitDigi.h>
#include <edm4hep/SimCalorimeterHit.h>
#include <edm4hep/RawCalorimeterHit.h>
#include <Evaluator/DD4hepUnits.h>
#include <services/log/Log_service.h>
#include <extensions/spdlog/SpdlogExtensions.h>

using namespace dd4hep;


class RawCalorimeterHit_factory_HcalEndcapPInsertRawHits : public JFactoryT<edm4hep::RawCalorimeterHit>, CalorimeterHitDigi {

public:

    //------------------------------------------
    // Constructor
    RawCalorimeterHit_factory_HcalEndcapPInsertRawHits() {
        SetTag("HcalEndcapPInsertRawHits");
    }

    //------------------------------------------
    // Init
    void Init() override {
        auto app = GetApplication();

        // Set default values for all config. parameters in CalorimeterHitDigi algorithm
        m_input_tag = "HcalEndcapPInsertHits";
        m_tRes = 0.0 * ns;
        m_tRes = 0.0 * ns;
        m_capADC = 8096;
        m_dyRangeADC = 100 * MeV;
        m_pedMeanADC = 400;
        m_pedSigmaADC = 3.2;
        m_resolutionTDC = 10 * picosecond;
        m_corrMeanScale = 1.0;
        m_geoSvcName = "ActsGeometryProvider";
        m_readout = "";
        m_geoSvc = app->GetService<JDD4hep_service>(); // TODO: implement named geometry service?

        // This is another option for exposing the data members as JANA configuration parameters.
//        app->SetDefaultParameter("ZDC:tag",              m_input_tag);
        app->SetDefaultParameter("ZDC:energyResolutions",u_eRes);
        app->SetDefaultParameter("ZDC:timeResolution",   m_tRes);
        app->SetDefaultParameter("ZDC:capacityADC",      m_capADC);
        app->SetDefaultParameter("ZDC:dynamicRangeADC",  m_dyRangeADC);
        app->SetDefaultParameter("ZDC:pedestalMean",     m_pedMeanADC);
        app->SetDefaultParameter("ZDC:pedestalSigma",    m_pedSigmaADC);
        app->SetDefaultParameter("ZDC:resolutionTDC",    m_resolutionTDC);
        app->SetDefaultParameter("ZDC:scaleResponse",    m_corrMeanScale);
        app->SetDefaultParameter("ZDC:signalSumFields",  u_fields);
        app->SetDefaultParameter("ZDC:fieldRefNumbers",  u_refs);
        app->SetDefaultParameter("ZDC:geoServiceName",   m_geoSvcName);
        app->SetDefaultParameter("ZDC:readoutClass",     m_readout);

        // Call Init for generic algorithm
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
    void ChangeRun(const std::shared_ptr<const JEvent> &event) override {
        AlgorithmChangeRun();
    }

    //------------------------------------------
    // Process
    void Process(const std::shared_ptr<const JEvent> &event) override {
        // Prefill inputs
        simhits = event->Get<edm4hep::SimCalorimeterHit>(m_input_tag);

        // Call Process for generic algorithm
        AlgorithmProcess();

        // Hand owner of algorithm objects over to JANA
        Set(rawhits);
        rawhits.clear(); // not really needed, but better to not leave dangling pointers around
    }

};

#endif // _RawCalorimeterHit_factory_HcalEndcapPInsertRawHits_h_
