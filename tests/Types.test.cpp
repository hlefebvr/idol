//
// Created by henri on 02/09/22.
//

#include "test_utils.h"

TEST_CASE("opp_player", "[modeling]") {

    SECTION("opp_player<PlayerT>::value should be the opposite player") {
        CHECK(opp_player<Decision>::value == Parameter);
        CHECK(opp_player<Parameter>::value == Decision);
    }

    SECTION("opp_player_v<PlayerT> should be the opposite player") {
        CHECK(opp_player_v<Decision> == Parameter);
        CHECK(opp_player_v<Parameter> == Decision);
    }

}
