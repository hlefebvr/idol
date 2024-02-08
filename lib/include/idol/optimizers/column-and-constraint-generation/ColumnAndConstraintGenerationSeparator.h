//
// Created by henri on 08.02.24.
//

#ifndef IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
#define IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H

namespace idol {
    class ColumnAndConstraintGenerationSeparator;
}

class idol::ColumnAndConstraintGenerationSeparator {
public:
    virtual ~ColumnAndConstraintGenerationSeparator() = default;

    [[nodiscard]] virtual ColumnAndConstraintGenerationSeparator* clone() const = 0;
};

#endif //IDOL_COLUMNANDCONSTRAINTGENERATIONSEPARATOR_H
