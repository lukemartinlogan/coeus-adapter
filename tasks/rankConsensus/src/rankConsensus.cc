/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "rankConsensus/rankConsensus.h"

#include "chimaera/api/chimaera_runtime.h"
#include "chimaera/monitor/monitor.h"
#include "chimaera_admin/chimaera_admin.h"

namespace chi::rankConsensus {

class Server : public Module {
 public:
  CLS_CONST LaneGroupId kDefaultGroup = 0;
  std::atomic<uint> rank_count;

 public:
  Server() = default;

  /** Construct rankConsensus */
  void Create(CreateTask *task, RunContext &rctx) {
    rank_count = 0;

    // Create a set of lanes for holding tasks
    CreateLaneGroup(kDefaultGroup, 1, QUEUE_LOW_LATENCY);
  }
  void MonitorCreate(MonitorModeId mode, CreateTask *task, RunContext &rctx) {}

  /** Route a task to a lane */
  Lane *MapTaskToLane(const Task *task) override {
    // Route tasks to lanes based on their properties
    // E.g., a strongly consistent filesystem could map tasks to a lane
    // by the hash of an absolute filename path.
    return GetLaneByHash(kDefaultGroup, task->prio_, 0);
  }

  /** Destroy rankConsensus */
  void Destroy(DestroyTask *task, RunContext &rctx) {}
  void MonitorDestroy(MonitorModeId mode, DestroyTask *task, RunContext &rctx) {
  }

  /** The GetRank method */
  void GetRank(GetRankTask *task, RunContext &rctx) { rank_count.fetch_add(1); }
  void MonitorGetRank(MonitorModeId mode, GetRankTask *task, RunContext &rctx) {
    switch (mode) {
      case MonitorMode::kReplicaAgg: {
        std::vector<FullPtr<Task>> &replicas = *rctx.replicas_;
      }
    }
  }

 public:
#include "rankConsensus/rankConsensus_lib_exec.h"
};

}  // namespace chi::rankConsensus

CHI_TASK_CC(chi::rankConsensus::Server, "rankConsensus");
