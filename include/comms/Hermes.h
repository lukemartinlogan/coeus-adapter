/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Coeus-adapter. The full Coeus-adapter copyright      *
 * notice, including terms governing use, modification, and redistribution,  *
 * is contained in the COPYING file, which can be found at the top directory.*
 * If you do not have access to the file, you may request a copy             *
 * from scslab@iit.edu.                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef COEUS_INCLUDE_COMMS_HERMES_H_
#define COEUS_INCLUDE_COMMS_HERMES_H_

#include "Bucket.h"
#include "common/Tracer.h"
#include "interfaces/IHermes.h"

namespace coeus {
class Hermes : public IHermes {
 public:
  float promote_weight = 0.25;
  float demote_weight = -0.25;

  Hermes() = default;

  bool connect() override {
    std::cout << "Entering connect" << std::endl;
    std::cout << "HERMES_CONF: " << getenv("HERMES_CONF") << std::endl;

    TRANSPARENT_HERMES();
    hermes = HERMES;
    CHI_ADMIN->RegisterModule(HSHM_MCTX, chi::DomainQuery::GetGlobalBcast(),
                              "coeus_coeus_mdm");
    CHI_ADMIN->RegisterModule(HSHM_MCTX, chi::DomainQuery::GetGlobalBcast(),
                              "coeus_rankConsensus");
    std::cout << "Registered task" << std::endl;
    return hermes->IsInitialized();
  };

  bool GetBucket(const std::string &bucket_name) override {
    bkt = (IBucket *)new coeus::Bucket(bucket_name, this);
    return true;
  }

  bool Demote(const std::string &bucket_name,
              const std::string &blob_name) override {
    hapi::Context ctx;
    hermes::Bucket bkt(bucket_name);

    hermes::BlobId blob_id = bkt.GetBlobId(blob_name);
    float blob_score = bkt.GetBlobScore(blob_id);

    bkt.ReorganizeBlob(blob_id, blob_score + demote_weight, blob_score, ctx);
    return true;
  }

  bool Prefetch(const std::string &bucket_name,
                const std::string &blob_name) override {
    hapi::Context ctx;
    hermes::Bucket bkt(bucket_name);

    hermes::BlobId blob_id = bkt.GetBlobId(blob_name);
    float blob_score = bkt.GetBlobScore(blob_id);

    bkt.ReorganizeBlob(blob_id, 1, blob_score + promote_weight, ctx);
    return true;
  }
};

}  // namespace coeus
#endif  // COEUS_INCLUDE_COMMS_HERMES_H_
