#include <boost/test/unit_test.hpp>
//#include "eosio.system_tester.hpp"
#include <iostream>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/resource_limits.hpp>
#include "contracts.hpp"
#include "test_symbol.hpp"



using namespace eosio::chain;
using namespace eosio::testing;
//using namespace eosio_system;
using namespace fc;

using mvo = fc::mutable_variant_object;


struct accounting_tester: public tester {
    accounting_tester() {
        produce_blocks( 2 );

        // NOTE: account names have to be 12 chars on real chain


        create_accounts( { N(accnt1), N(accnt2), N(accnt3), N(accounting), N(eosio.token) } );
        produce_blocks( 2 );


        std::cout << "contracts::token_wasm() " << std::endl;
        set_code( N(eosio.token), contracts::token_wasm() );
        set_abi( N(eosio.token), contracts::token_abi().data() );

        std::cout << "contracts::accounting_wasm() " << std::endl;
        set_code( N(accounting), contracts::accounting_wasm() );
        set_abi( N(accounting), contracts::accounting_abi().data() );

        std::cout << "contracts::notifyhook_wasm() 1" << std::endl;
        set_code( N(accnt1), contracts::notifyhook_wasm() );
        set_abi( N(accnt1), contracts::notifyhook_abi().data() );

        std::cout << "contracts::notifyhook_wasm() 2" << std::endl;
        set_code( N(accnt2), contracts::notifyhook_wasm() );
        set_abi( N(accnt2), contracts::notifyhook_abi().data() );

        std::cout << "contracts::notifyhook_wasm() 3" << std::endl;
        set_code( N(accnt3), contracts::notifyhook_wasm() );
        set_abi( N(accnt3), contracts::notifyhook_abi().data() );


        produce_blocks();

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(accounting) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);

            accounting_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(accnt1) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);

            acct1_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(accnt2) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);

            acct2_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(accnt3) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);

            acct3_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(eosio.token) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);

            tok_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        base_tester::push_action( N(eosio.token), N(create), N(eosio.token), mvo()
                ("issuer", "eosio.token")
                ("maximum_supply", SYS_1000000_0000));

        base_tester::push_action( N(eosio.token), N(issue), N(eosio.token), mvo()
                ("to", "eosio.token")
                ("quantity", SYS_1000000_0000)
                ("memo", "issue all tokens to token accnt"));

        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mvo()
                ("from", "eosio.token")
                ("to", "accnt1")
                ("quantity", SYS_2000_0000)
                ("memo", "issue 20000 to accnt1"));

        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mvo()
                ("from", "eosio.token")
                ("to", "accnt2")
                ("quantity", SYS_2000_0000)
                ("memo", "issue 2000 to accnt2"));

        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mvo()
                ("from", "eosio.token")
                ("to", "accnt3")
                ("quantity", SYS_2000_0000)
                ("memo", "issue 2000 to accnt2"));

        // assign "eosio.token" to accounting account for assign
        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mvo()
                ("from", "eosio.token")
                ("to", "accounting")
                ("quantity", SYS_2000_0000)
                ("memo", "issue 2000 to accounting"));

        produce_blocks();
    }

    void add_acct( const name& user_name ) {
        base_tester::push_action( N(accounting), N(createuser), user_name, mvo()
                ("user_name", user_name));
    }

    void add_cat( const name& user_name, const std::string& cat_name ) {
        base_tester::push_action( N(accounting), N(createcateg), user_name, mvo()
                ("user_name", user_name)
                ("category_name", cat_name));
    }

    void transfer_fund( const name& user, const std::string& cat_from, const std::string& cat_to, const asset& amount) {

        base_tester::push_action( N(accounting), N(transfercateg), user, mvo()
                ("user_name", user)
                ("from_category", cat_from)
                ("to_category", cat_to)
                ("transferable", amount));
    }

    // Get balance for given account and category
    asset get_fund_balance( const name& user_name, const std::string& cat_name) {
        const auto& var =
                accounting_abi_ser.binary_to_variant("acct",
                                          get_row_by_account(N(accounting), N(accounting), N(account), user_name), // code , scope, table, primary key value
                                          abi_serializer::create_yield_function(abi_serializer_max_time));
//        wdump((var));
        const auto& cat_list = var["cat_list"];

        for(size_t idx=0; idx<cat_list.size(); ++idx)
        {
            if (cat_name == cat_list[idx]["cat_name"])
            {
                return asset::from_string(cat_list[idx]["cat_balance"].as_string());
            }
        }
        return INVALID_0_0001;
    }

    void eosio_transfer(const name& acct, const asset& amount)
    {
            base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mvo()
                    ("from", "eosio.token")
                    ("to", acct)
                    ("quantity", amount.to_string())
                    ("memo", "eosio_transfer 123"));
    };
    abi_serializer tok_abi_ser;
    abi_serializer accounting_abi_ser;
    abi_serializer acct1_abi_ser;
    abi_serializer acct2_abi_ser;
    abi_serializer acct3_abi_ser;

    const asset SYS_1000000_0000 = asset::from_string("1000000.0000 SYS");
    const asset SYS_2000_0000 = asset::from_string("2000.0000 SYS");
    const asset SYS_0_0000 = asset::from_string("0.0000 SYS");
    const asset SYS_123_1234 = asset::from_string("123.1234 SYS");
    const asset SYS_234_3456 = asset::from_string("234.3456 SYS");
    const asset SYS_3456_1234 = asset::from_string("3456.1234 SYS");
    const asset SYS_1_2345 = asset::from_string("1.2345 SYS");
    const asset SYS_2_3456 = asset::from_string("2.3456 SYS");
    const asset SYS_40_3456 = asset::from_string("40.3456 SYS");
    const asset INVALID_0_0001 = asset::from_string("0.0001 INVALID");
    const asset SYS_NEG_23_5678 = asset::from_string("-23.5678 SYS");
    const asset SYS_NEG_5678_1234 = asset::from_string("-5678.1234 SYS");

    const std::string CAT_DEFAULT = "default";
    const std::string CAT_RENT = "rent";
    const std::string CAT_VACATION = "vacation";
};

/*

BOOST_FIXTURE_TEST_CASE( on_notify_test, accounting_tester )try {

    BOOST_REQUIRE_NO_THROW( add_acct(N(accnt1)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // transfer eosio token to accnt1
    BOOST_REQUIRE_NO_THROW(eosio_transfer(N(accnt1), SYS_234_3456));
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_234_3456 + SYS_2000_0000);
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( on_notify_fail_test, accounting_tester )try {
    BOOST_REQUIRE_NO_THROW( add_acct(N(accnt1)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // negative assign
    BOOST_REQUIRE_THROW(eosio_transfer(N(accnt1), SYS_NEG_23_5678), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // different currency
    BOOST_REQUIRE_THROW(eosio_transfer(N(accnt1), INVALID_0_0001), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // overdraw
    BOOST_REQUIRE_THROW(eosio_transfer(N(accnt1), SYS_1000000_0000), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
} FC_LOG_AND_RETHROW()
*/

BOOST_FIXTURE_TEST_CASE( create_acct_test, accounting_tester )try {
    BOOST_REQUIRE_NO_THROW( add_acct(N(accnt1)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
} FC_LOG_AND_RETHROW()

/*
BOOST_FIXTURE_TEST_CASE( create_acct_fail_test, accounting_tester )try {
    // create an invalid account
    BOOST_REQUIRE_THROW( add_acct(N(accntinvalid)), transaction_exception );
    // create dup account
    BOOST_REQUIRE_NO_THROW( add_acct(N(accnt1)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    BOOST_REQUIRE_THROW( add_acct(N(accnt1)), transaction_exception);
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( create_cat_test, accounting_tester )try {
    // create an account
    BOOST_REQUIRE_NO_THROW( add_acct(N(accnt1)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // create the category
    BOOST_REQUIRE_NO_THROW( add_cat(N(accnt1), CAT_RENT) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_0_0000);
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( create_cat_fail_test, accounting_tester )try {
    // create an account
    BOOST_REQUIRE_NO_THROW( add_acct(N(accnt1)) );
    // create the category
    BOOST_REQUIRE_NO_THROW( add_cat(N(accnt1), CAT_RENT) );
    // create duplicated category
    BOOST_REQUIRE_THROW( add_cat(N(accnt1), CAT_RENT), transaction_exception  );
    // create valid category to non-exist account
    BOOST_REQUIRE_THROW( add_cat(N(accntinvalid), CAT_VACATION), transaction_exception );
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( transfer_fund_test, accounting_tester )try {
    // create an account
    BOOST_REQUIRE_NO_THROW( add_acct(N(accnt1)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // create the category
    BOOST_REQUIRE_NO_THROW( add_cat(N(accnt1), CAT_RENT) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_0_0000);

    // transfer same account different categories
    BOOST_REQUIRE_NO_THROW( transfer_fund( N(accnt1), CAT_DEFAULT, CAT_RENT, SYS_123_1234) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), (SYS_2000_0000 - SYS_123_1234) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_123_1234 );

} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( transfer_fund_fail_test, accounting_tester )try {
    // create an account
    BOOST_REQUIRE_NO_THROW ( add_acct(N(accnt1)));

    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);

    // transfer to non-existing category, nothing change
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt1), CAT_DEFAULT, CAT_RENT, SYS_1_2345), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000 );

    // transfer from non-existing category, nothing change
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt1), CAT_RENT, CAT_DEFAULT, SYS_1_2345), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000 );

    // same category transfer, nothing change
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt1), CAT_DEFAULT, CAT_DEFAULT, SYS_1_2345), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000 );

    // transfer from not existing account, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt2), CAT_DEFAULT, CAT_RENT, SYS_40_3456), eosio_assert_message_exception);

    // add category
    BOOST_REQUIRE_NO_THROW( add_cat(N(accnt1), CAT_RENT) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_0_0000);

    // over draw, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt1), CAT_DEFAULT, CAT_RENT, SYS_3456_1234), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_0_0000);

    // transfer 0 fund
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt1), CAT_DEFAULT, CAT_RENT, SYS_0_0000), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_0_0000);

    // transfer negative fund, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt1), CAT_DEFAULT, CAT_RENT, SYS_NEG_23_5678), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_0_0000);

    // transfer different currency, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(accnt1), CAT_DEFAULT, CAT_RENT, INVALID_0_0001), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_DEFAULT), SYS_2000_0000);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(accnt1), CAT_RENT), SYS_0_0000);

} FC_LOG_AND_RETHROW()
*/
