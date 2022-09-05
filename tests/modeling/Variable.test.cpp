//
// Created by henri on 05/09/22.
//

#include <iostream>
#include "../test_utils.h"

TEMPLATE_LIST_TEST_CASE("Variable", "[variables][modeling]", for_each_player) {

    Env env;
    Model<TestType::PlayerT> model(env);

    SECTION("should create a new variable") {

        SECTION("with lb = 0., ub = 1., type = Continuous and default name") {
            auto x = model.add_variable(0., 1., Continuous);
            CHECK(x.lower_bound() == 0._a);
            CHECK(x.upper_bound() == 1._a);
            CHECK(x.type() == Continuous);
            CHECK(x.name() == "Var(" + std::to_string(x.id()) + ")");
            std::stringstream stream;
            stream << x;
            CHECK(stream.str() == x.name());
        }

        SECTION("with lb = 0., ub = 1., type = Integer and default name") {
            auto x = model.add_variable(0., 1., Integer);
            CHECK(x.lower_bound() == 0._a);
            CHECK(x.upper_bound() == 1._a);
            CHECK(x.type() == Integer);
            CHECK(x.name() == "Var(" + std::to_string(x.id()) + ")");
            std::stringstream stream;
            stream << x;
            CHECK(stream.str() == x.name());
        }

        SECTION("with lb = 0., ub = 1., type = Continuous and given name \"x\"") {
            auto x = model.add_variable(0., 1., Continuous, "x");
            CHECK(x.lower_bound() == 0._a);
            CHECK(x.upper_bound() == 1._a);
            CHECK(x.type() == Continuous);
            CHECK(x.name() == "x");
            std::stringstream stream;
            stream << x;
            CHECK(stream.str() == x.name());
        }

    }

    SECTION("should update existing variable") {

        auto x = model.add_variable(-Inf, Inf, Continuous);

        SECTION("should update lower bound to 0.") {
            model.update(x, 0., skip<double>, skip<VariableType>);
            CHECK(x.lower_bound() == 0._a);
            CHECK(x.upper_bound() == Catch::Approx(Inf));
            CHECK(x.type() == Continuous);
        }

        SECTION("should update upper bound to 0.") {
            model.update(x, skip<double>, 0., skip<VariableType>);
            CHECK(x.lower_bound() == Catch::Approx(-Inf));
            CHECK(x.upper_bound() == 0._a);
            CHECK(x.type() == Continuous);
        }

        SECTION("should update type to Integer") {
            model.update(x, skip<double>, skip<double>, Integer);
            CHECK(x.lower_bound() == Catch::Approx(-Inf));
            CHECK(x.upper_bound() == Catch::Approx(Inf));
            CHECK(x.type() == Integer);
        }

    }

}


TEMPLATE_LIST_TEST_CASE("Parameter", "[parameters][modeling]", for_each_player) {

    Env env;
    Model<TestType::PlayerT> model(env);

    SECTION("should create a new parameter") {

        SECTION("with lb = 0., ub = 1., type = Continuous and default name") {
            auto xi = model.add_parameter(0., 1., Continuous);
            CHECK(xi.lower_bound() == 0._a);
            CHECK(xi.upper_bound() == 1._a);
            CHECK(xi.type() == Continuous);
            CHECK(xi.name() == "Var(" + std::to_string(xi.id()) + ")");
            std::stringstream stream;
            stream << xi;
            CHECK(stream.str() == xi.name());
        }

        SECTION("with lb = 0., ub = 1., type = Integer and default name") {
            auto xi = model.add_parameter(0., 1., Integer);
            CHECK(xi.lower_bound() == 0._a);
            CHECK(xi.upper_bound() == 1._a);
            CHECK(xi.type() == Integer);
            CHECK(xi.name() == "Var(" + std::to_string(xi.id()) + ")");
            std::stringstream stream;
            stream << xi;
            CHECK(stream.str() == xi.name());
        }

        SECTION("with lb = 0., ub = 1., type = Continuous and given name \"x\"") {
            auto xi = model.add_parameter(0., 1., Continuous, "x");
            CHECK(xi.lower_bound() == 0._a);
            CHECK(xi.upper_bound() == 1._a);
            CHECK(xi.type() == Continuous);
            CHECK(xi.name() == "x");
            std::stringstream stream;
            stream << xi;
            CHECK(stream.str() == xi.name());
        }

    }

    SECTION("should update existing parameter") {

        auto xi = model.add_parameter(-Inf, Inf, Continuous);

        SECTION("should update lower bound to 0.") {
            model.update(xi, 0., skip<double>, skip<VariableType>);
            CHECK(xi.lower_bound() == 0._a);
            CHECK(xi.upper_bound() == Catch::Approx(Inf));
            CHECK(xi.type() == Continuous);
        }

        SECTION("should update upper bound to 0.") {
            model.update(xi, skip<double>, 0., skip<VariableType>);
            CHECK(xi.lower_bound() == Catch::Approx(-Inf));
            CHECK(xi.upper_bound() == 0._a);
            CHECK(xi.type() == Continuous);
        }

        SECTION("should update type to Integer") {
            model.update(xi, skip<double>, skip<double>, Integer);
            CHECK(xi.lower_bound() == Catch::Approx(-Inf));
            CHECK(xi.upper_bound() == Catch::Approx(Inf));
            CHECK(xi.type() == Integer);
        }

    }

}