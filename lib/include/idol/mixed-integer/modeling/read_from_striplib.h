#ifndef IDOL_READ_FROM_STRIPLIB_H
#define IDOL_READ_FROM_STRIPLIB_H

#include <string>
#include <utility>

#include "idol/mixed-integer/modeling/annotations/Annotation.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol::DantzigWolfe {

    /**
     * Reads an unpresolved GCG `.dec` file and returns the corresponding idol decomposition annotation.
     *
     * The model is not modified. The annotation is attached to its variables and constraints.
     * @param t_model Model named consistently with the decomposition file.
     * @param t_decomposition_file Path to an extracted `.dec` file.
     * @return Dantzig-Wolfe decomposition annotation.
     */
    Annotation<unsigned int> read_decomposition(Model& t_model, const std::string& t_decomposition_file);

    /**
     * Imports a strIPlib model and its unpresolved GCG decomposition.
     *
     * The model format must be supported by an available wrapper used by `Model::read_from_file`.
     * @param t_env Environment receiving the model and annotation.
     * @param t_model_file Path to the extracted model file.
     * @param t_decomposition_file Path to the extracted `.dec` file.
     * @return Imported model and its Dantzig-Wolfe decomposition annotation.
     */
    std::pair<Model, Annotation<unsigned int>> read_from_striplib(Env& t_env,
                                                                  const std::string& t_model_file,
                                                                  const std::string& t_decomposition_file);

}

#endif // IDOL_READ_FROM_STRIPLIB_H
