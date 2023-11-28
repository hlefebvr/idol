//
// Created by henri on 27.10.23.
//

#ifndef IDOL_EXPORTBRANCHANDBOUNDTREETOCSV_H
#define IDOL_EXPORTBRANCHANDBOUNDTREETOCSV_H


#include "idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"
#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"

namespace idol::Utils {
    class ExportBranchAndBoundTreeToCSV;
}

class idol::Utils::ExportBranchAndBoundTreeToCSV : public BranchAndBoundCallbackFactory<DefaultNodeInfo> {
    const std::string m_filename;
public:
    explicit ExportBranchAndBoundTreeToCSV(std::string t_filename);

    BranchAndBoundCallback<DefaultNodeInfo> *operator()() override;

    BranchAndBoundCallbackFactory<DefaultNodeInfo> *clone() const override;

    class Strategy : public BranchAndBoundCallback<DefaultNodeInfo> {
        const std::string m_filename;
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
        explicit Strategy(std::string t_filename);
    };
};

#endif //IDOL_EXPORTBRANCHANDBOUNDTREETOCSV_H
