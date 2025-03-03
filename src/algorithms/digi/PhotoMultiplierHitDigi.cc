// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2022 Chao Peng

/*  General PhotoMultiplier Digitization
 *
 *  Apply the given quantum efficiency for photon detection
 *  Converts the number of detected photons to signal amplitude
 *
 *  Author: Chao Peng (ANL)
 *  Date: 10/02/2020
 */

//Ported by Thomas Britton (JLab)


#include "PhotoMultiplierHitDigi.h"

#include <JANA/JEvent.h>
#include <Evaluator/DD4hepUnits.h>

using namespace dd4hep;


//------------------------
// AlgorithmInit
//------------------------
void PhotoMultiplierHitDigi::AlgorithmInit(std::shared_ptr<spdlog::logger>& logger) 
{
    m_logger=logger;
    m_rngNorm = [&](){
        return m_random.Gaus(0., 1.0);
    };
    m_rngUni = [&](){
        return m_random.Uniform(0., 1.0);
    };
    //auto randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
    auto sc1 = m_rngUni;//m_rngUni.initialize(randSvc, Rndm::Flat(0., 1.));
    auto sc2 = m_rngNorm;//m_rngNorm.initialize(randSvc, Rndm::Gauss(0., 1.));
    //if (!sc1.isSuccess() || !sc2.isSuccess()) {
    if (!sc1 || !sc2) {
        m_logger->error("Cannot initialize random generator!");
        japp->Quit();
    }

    qe_init();
}



//------------------------
// AlgorithmChangeRun
//------------------------
void PhotoMultiplierHitDigi::AlgorithmChangeRun() {
    /// This is automatically run before Process, when a new run number is seen
    /// Usually we update our calibration constants by asking a JService
    /// to give us the latest data for this run number
}

//------------------------
// AlgorithmProcess
//------------------------
void PhotoMultiplierHitDigi::AlgorithmProcess()  {

        // input collection
        //const auto &sim = *m_inputHitCollection.get();
        // Create output collections
        //auto &raw = *m_outputHitCollection.createAndPut();

        struct HitData { int npe; double signal; double time; };
        std::unordered_map<uint64_t, std::vector<HitData>> hit_groups;
        // collect the photon hit in the same cell
        // calculate signal
        for(const auto& ahit : simhits) {
            // quantum efficiency
            if (!qe_pass(ahit->getEDep(), m_rngUni())) {
                continue;
            }
            // cell id, time, signal amplitude
            uint64_t id = ahit->getCellID();
            double time = ahit->getTime();//ahit->getMCParticle().getTime();
            double amp = m_speMean + m_rngNorm()*m_speError;

            // group hits
            auto it = hit_groups.find(id);
            if (it != hit_groups.end()) {
                size_t i = 0;
                for (auto git = it->second.begin(); git != it->second.end(); ++git, ++i) {
                    if (std::abs(time - git->time) <= (m_hitTimeWindow/ns)) {
                        git->npe += 1;
                        git->signal += amp;
                        break;
                    }
                }
                // no hits group found
                if (i >= it->second.size()) {
                    it->second.emplace_back(HitData{1, amp + m_pedMean + m_pedError*m_rngNorm(), time});
                }
            } else {
                hit_groups[id] = {HitData{1, amp + m_pedMean + m_pedError*m_rngNorm(), time}};
            }
        }

        // build hit
        for (auto &it : hit_groups) {
            for (auto &data : it.second) {
                edm4eic::RawPMTHit* hit = new edm4eic::RawPMTHit{
                  it.first,
                  static_cast<uint32_t>(data.signal), 
                  static_cast<uint32_t>(data.time/(m_timeStep/ns))};
                rawhits.push_back(hit);
            }
        }

        
}

void  PhotoMultiplierHitDigi::qe_init()
{
        auto &qeff = u_quantumEfficiency;

        // sort quantum efficiency data first
        std::sort(qeff.begin(), qeff.end(),
            [] (const std::pair<double, double> &v1, const std::pair<double, double> &v2) {
                return v1.first < v2.first;
            });

        // sanity checks
        if (qeff.empty()) {
            qeff = {{2.6*eV, 0.3}, {7.0*eV, 0.3}};
            m_logger->warn("Invalid quantum efficiency data provided, using default values {} {:.2f} {} {:.2f} {} {:.2f} {} {:.2f} {}","{{", qeff.front().first, ",", qeff.front().second, "},{",qeff.back().first,",",qeff.back().second,"}}");
        }
        if (qeff.front().first > 3.0*eV) {
            m_logger->warn("Quantum efficiency data start from {:.2f} {}", qeff.front().first/eV , " eV, maybe you are using wrong units?");
        }
        if (qeff.back().first < 6.0*eV) {
            m_logger->warn("Quantum efficiency data end at {:.2f} {}" , qeff.back().first/eV , " eV, maybe you are using wrong units?");
        }
}


template<class RndmIter, typename T, class Compare> RndmIter  PhotoMultiplierHitDigi::interval_search(RndmIter beg, RndmIter end, const T &val, Compare comp) const
{
        // special cases
        auto dist = std::distance(beg, end);
        if ((dist < 2) || (comp(*beg, val) > 0) || (comp(*std::prev(end), val) < 0)) {
            return end;
        }
        auto mid = std::next(beg, dist / 2);

        while (mid != end) {
            if (comp(*mid, val) == 0) {
                return mid;
            } else if (comp(*mid, val) > 0) {
                end = mid;
            } else {
                beg = std::next(mid);
            }
            mid = std::next(beg, std::distance(beg, end)/2);
        }

        if (mid == end || comp(*mid, val) > 0) {
            return std::prev(mid);
        }
        return mid;
}

bool  PhotoMultiplierHitDigi::qe_pass(double ev, double rand) const
{
        const auto &qeff = u_quantumEfficiency;
        auto it = interval_search(qeff.begin(), qeff.end(), ev,
                    [] (const std::pair<double, double> &vals, double val) {
                        return vals.first - val;
                    });

        if (it == qeff.end()) {
            // info() << ev/eV << " eV is out of QE data range, assuming 0% efficiency" << endmsg;
            return false;
        }

        double prob = it->second;
        auto itn = std::next(it);
        if (itn != qeff.end() && (itn->first - it->first != 0)) {
            prob = (it->second*(itn->first - ev) + itn->second*(ev - it->first)) / (itn->first - it->first);
        }

        // info() << ev/eV << " eV, QE: "  << prob*100. << "%" << endmsg;
        return rand <= prob;
}
