//
// Created by henri on 31/01/23.
//
#include <catch2/catch_all.hpp>
#include "idol/modeling.h"

using namespace Catch::literals;
using namespace idol;

template<class T>
const T& type() {
    throw Exception("Only intended for type deduction");
}

TEST_CASE("operators type deduction", "[modeling]") {

    SECTION("type deduction: operator+") {

        // Unary plus
        CHECK(std::is_same_v<decltype(+type<Var>()), LinExpr<Var>>);

        // double + X
        CHECK(std::is_same_v<decltype(type<double>() + type<Var>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() + type<LinExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() + type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() + type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() + type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // Var + X
        CHECK(std::is_same_v<decltype(type<Var>() + type<double>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() + type<Var>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() + type<LinExpr<Var>>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() + type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() + type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() + type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // LinExpr + X
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() + type<double>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() + type<Var>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() + type<LinExpr<Var>>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() + type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() + type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() + type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // AffExpr + X
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() + type<double>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() + type<Var>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() + type<LinExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() + type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() + type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() + type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // QuadExpr + X
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() + type<double>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() + type<Var>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() + type<LinExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() + type<AffExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() + type<QuadExpr<Var>>()), QuadExpr<Var>>);
        // GenerationPattern<Ctr> is not supported for QuadExpr

        // GenerationPattern<Ctr> + X
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() + type<double>()), GenerationPattern<Ctr>>);
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() + type<Var>()), GenerationPattern<Ctr>>);
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() + type<LinExpr<Var>>()), GenerationPattern<Ctr>>);
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() + type<AffExpr<Var>>()), GenerationPattern<Ctr>>);
        // QuadExpr is not supported for GenerationPattern<Ctr>
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() + type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

    }

    SECTION("type deduction: operator-") {

        // Unary minus
        CHECK(std::is_same_v<decltype(-type<Var>()), LinExpr<Var>>);

        // double - X
        CHECK(std::is_same_v<decltype(type<double>() - type<Var>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() - type<LinExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() - type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() - type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() - type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // Var - X
        CHECK(std::is_same_v<decltype(type<Var>() - type<double>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() - type<Var>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() - type<LinExpr<Var>>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() - type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() - type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() - type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // LinExpr<Var> - X
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() - type<double>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() - type<Var>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() - type<LinExpr<Var>>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() - type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() - type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() - type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // AffExpr<Var> - X
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() - type<double>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() - type<Var>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() - type<LinExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() - type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() - type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() - type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // QuadExpr<Var> - X
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() - type<double>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() - type<Var>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() - type<LinExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() - type<AffExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() - type<QuadExpr<Var>>()), QuadExpr<Var>>);
        // GenerationPattern<Ctr> is not supported for QuadExpr<Var>

        // GenerationPattern<Ctr> - X
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() - type<double>()), GenerationPattern<Ctr>>);
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() - type<Var>()), GenerationPattern<Ctr>>);
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() - type<LinExpr<Var>>()), GenerationPattern<Ctr>>);
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() - type<AffExpr<Var>>()), GenerationPattern<Ctr>>);
        // QuadExpr<Var> is not supported for GenerationPattern<Ctr>
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() - type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

    }

    SECTION("type deduction: operator*") {

        // double * X
        CHECK(std::is_same_v<decltype(type<double>() * type<Var>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() * type<LinExpr<Var>>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() * type<AffExpr<Var>>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() * type<QuadExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<double>() * type<GenerationPattern<Ctr>>()), GenerationPattern<Ctr>>);

        // Var * X
        CHECK(std::is_same_v<decltype(type<Var>() * type<double>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() * type<Var>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() * type<LinExpr<Var>>()), QuadExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<Var>() * type<AffExpr<Var>>()), QuadExpr<Var>>);
        // QuadExpr and GenerationPattern are not supported for Var

        // LinExpr<Var> * X
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() * type<double>()), LinExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() * type<Var>()), QuadExpr<Var>>);
        //CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() * type<LinExpr<Var>>()), QuadExpr<Var>>);
        //CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() * type<AffExpr<Var>>()), QuadExpr<Var>>);
        // QuadExpr and GenerationPattern are not supported for LinExpr<Var>

        // AffExpr<Var> * X
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() * type<double>()), AffExpr<Var>>);
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() * type<Var>()), QuadExpr<Var>>);
        //CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() * type<LinExpr<Var>>()), QuadExpr<Var>>);
        //CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() * type<AffExpr<Var>>()), QuadExpr<Var>>);
        // QuadExpr and GenerationPattern are not supported for AffExpr<Var>

        // QuadExpr<Var> * X
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() * type<double>()), QuadExpr<Var>>);
        // Var, LinExpr, AffExpr, QuadExpr, GenerationPattern are not supported for QuadExpr<Var>

        // GenerationPattern<Ctr> * X
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() * type<double>()), GenerationPattern<Ctr>>);
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() * type<Var>()), GenerationPattern<Ctr>>);
        //CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() * type<LinExpr<Var>>()), GenerationPattern<Ctr>>);
        // AffExpr, QuadExpr, and GenerationPattern are not supported for GenerationPattern<Ctr>
    }

    SECTION("type deduction: operator/") {

        // double / X
        CHECK(std::is_same_v<decltype(type<double>() / type<double>()), double>);
        // Var, LinExpr, AffExpr, QuadExpr, GenerationPattern are unsupported for double

        // Var / X
        CHECK(std::is_same_v<decltype(type<Var>() / type<double>()), LinExpr<Var>>);
        // Var, LinExpr, AffExpr, QuadExpr, GenerationPattern are unsupported for Var

        // LinExpr<Var> / X
        CHECK(std::is_same_v<decltype(type<LinExpr<Var>>() / type<double>()), LinExpr<Var>>);
        // Var, LinExpr, AffExpr, QuadExpr, GenerationPattern are unsupported for LinExpr<Var>

        // AffExpr<Var> / X
        CHECK(std::is_same_v<decltype(type<AffExpr<Var>>() / type<double>()), AffExpr<Var>>);
        // Var, LinExpr, AffExpr, QuadExpr, GenerationPattern are unsupported for AffExpr<Var>

        // QuadExpr<Var> / X
        CHECK(std::is_same_v<decltype(type<QuadExpr<Var>>() / type<double>()), QuadExpr<Var>>);
        // Var, LinExpr, AffExpr, QuadExpr, GenerationPattern are unsupported for QuadExpr<Var>

        // GenerationPattern<Ctr> / X
        CHECK(std::is_same_v<decltype(type<GenerationPattern<Ctr>>() / type<double>()), GenerationPattern<Ctr>>);
        // Var, LinExpr, AffExpr, QuadExpr, GenerationPattern are unsupported for GenerationPattern<Ctr>
    }

}
