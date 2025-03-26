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
#include "chimaera/api/chimaera_runtime.h"
#include "chimaera/monitor/monitor.h"
#include "chimaera_admin/chimaera_admin_client.h"
#include "coeus_mdm/coeus_mdm_client.h"
#include "common/SQlite.h"

namespace chi::coeus_mdm {

class Server : public Module {
 public:
  std::unique_ptr<SQLiteWrapper> db;
  CLS_CONST LaneGroupId kDefaultGroup = 0;

 public:
  Server() = default;

  CHI_BEGIN(Create)
  /** Construct coeus_mdm */
  void Create(CreateTask *task, RunContext &rctx) {
    // Create a set of lanes for holding tasks
    CreateLaneGroup(kDefaultGroup, 1, QUEUE_LOW_LATENCY);

    // Load the paramset
    CreateTaskParams params = task->GetParams();
    db = std::make_unique<SQLiteWrapper>(params.db_file_);
  }
  void MonitorCreate(MonitorModeId mode, CreateTask *task, RunContext &rctx) {}
  CHI_END(Create)

  /** Route a task to a lane */
  Lane *MapTaskToLane(const Task *task) override {
    // Route tasks to lanes based on their properties
    // E.g., a strongly consistent filesystem could map tasks to a lane
    // by the hash of an absolute filename path.
    return GetLaneByHash(kDefaultGroup, task->prio_, 0);
  }

  CHI_BEGIN(Destroy)
  /** Destroy coeus_mdm */
  void Destroy(DestroyTask *task, RunContext &rctx) {}
  void MonitorDestroy(MonitorModeId mode, DestroyTask *task, RunContext &rctx) {
  }
  CHI_END(Destroy)

  CHI_BEGIN(Mdm_insert)
  /** The Mdm_insert method */
  void Mdm_insert(Mdm_insertTask *task, RunContext &rctx) {
    DbOperation db_op = task->GetDbOp();

    if (db_op.type == OperationType::InsertData) {
      db->InsertVariableMetadata(db_op.step, db_op.rank, db_op.metadata);
      db->InsertBlobLocation(db_op.step, db_op.rank, db_op.name,
                             db_op.blobInfo);

    } else if (db_op.type == OperationType::UpdateSteps) {
      db->UpdateTotalSteps(db_op.uid, db_op.currentStep);

    } else if (db_op.type == OperationType::InsertDerivedData) {
      db->InsertVariableMetadata(db_op.step, db_op.rank, db_op.metadata);
      db->InsertBlobLocation(db_op.step, db_op.rank, db_op.name,
                             db_op.blobInfo);
      db->insertOrUpdateDerivedQuantity(
          db_op.step, db_op.name, "min", db_op.blobInfo.blob_name,
          db_op.blobInfo.bucket_name, db_op.derived_semantics.min_value);
      db->insertOrUpdateDerivedQuantity(
          db_op.step, db_op.name, "max", db_op.blobInfo.blob_name,
          db_op.blobInfo.bucket_name, db_op.derived_semantics.max_value);
    }
    // int step, const std::string& variable,
    //                                      const std::string& operation, const
    //                                      std::string& blob_name, const
    //                                      std::string& bucket_name, float
    //                                      value
  }
  void MonitorMdm_insert(MonitorModeId mode, Mdm_insertTask *task,
                         RunContext &rctx) {
    switch (mode) {
      case MonitorMode::kReplicaAgg: {
        std::vector<FullPtr<Task>> &replicas = *rctx.replicas_;
      }
    }
  }
  CHI_END(Mdm_insert)

  CHI_AUTOGEN_METHODS
 public:
#include "coeus_mdm/coeus_mdm_lib_exec.h"
};

}  // namespace chi::coeus_mdm

CHI_TASK_CC(chi::coeus_mdm::Server, "coeus_mdm");
