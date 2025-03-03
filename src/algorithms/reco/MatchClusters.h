// Original header license: SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2022 Sylvester Joosten, Dmitry Romanov

// Takes a list of particles (presumed to be from tracking), and all available clusters.
// 1. Match clusters to their tracks using the mcID field
// 2. For unmatched clusters create neutrals and add to the particle list

#ifndef EICRECON_MATCHCLUSTERS_H
#define EICRECON_MATCHCLUSTERS_H

#include <algorithm>
#include <cmath>
#include <vector>
#include <map>

#include <spdlog/spdlog.h>


// Event Model related classes
#include <edm4hep/MCParticleCollection.h>
#include <edm4eic/ClusterCollection.h>
#include <edm4eic/MCRecoClusterParticleAssociationCollection.h>
#include <edm4eic/MCRecoParticleAssociationCollection.h>
#include <edm4eic/ReconstructedParticleCollection.h>
#include <edm4eic/TrackParametersCollection.h>
#include <edm4eic/vector_utils.h>

#include "ParticlesWithAssociation.h"

namespace eicrecon {

    class MatchClusters {
    private:

    public:

        void init(std::shared_ptr<spdlog::logger> logger);

        ParticlesWithAssociation *execute(
                std::vector<const edm4hep::MCParticle *> mcparticles,
                std::vector<edm4eic::ReconstructedParticle *> inparts,            // TODO fix const
                std::vector<edm4eic::MCRecoParticleAssociation *> inpartsassoc,   // TODO fix const
                const std::vector<std::vector<const edm4eic::Cluster*>> &cluster_collections,
                const std::vector<std::vector<const edm4eic::MCRecoClusterParticleAssociation*>> &cluster_assoc_collections);

    private:

        std::shared_ptr<spdlog::logger> m_log;


        // get a map of mcID --> cluster
        // input: cluster_collections --> list of handles to all cluster collections
        std::map<int, const edm4eic::Cluster*> indexedClusters(
                const std::vector<std::vector<const edm4eic::Cluster*>> &cluster_collections,
                const std::vector<std::vector<const edm4eic::MCRecoClusterParticleAssociation*>> &associations_collections);


        // reconstruct a neutral cluster
        // (for now assuming the vertex is at (0,0,0))
        edm4eic::ReconstructedParticle
        reconstruct_neutral(const edm4eic::Cluster *cluster, const double mass, const int32_t pdg) const;
    };

} // namespace eicrecon

#endif //EICRECON_MATCHCLUSTERS_H
