//
// Created by henri on 27/01/23.
//

#ifndef IDOL_CTRVERSION_H
#define IDOL_CTRVERSION_H

#include "TempCtr.h"
#include "modeling/objects/Version.h"

class CtrVersion : public Version, public TempCtr {
public:
    explicit CtrVersion(unsigned int t_index, TempCtr&& t_temp_ctr) : Version(t_index), TempCtr(std::move(t_temp_ctr)) {}
};

#endif //IDOL_CTRVERSION_H
