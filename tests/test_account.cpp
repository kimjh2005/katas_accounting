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

	void p()
	{
		static int n = 0;
		std::cout << ++n << std::endl;
	}

	accounting_tester() {
        produce_blocks( 2 );

        /// creates accounts
        create_accounts( { N(apple), N(ibm), N(accounting), N(eosio.token), N(notifyhook) } );
        produce_blocks( 2 );

        set_code( N(eosio.token), contracts::token_wasm() );
        set_abi( N(eosio.token), contracts::token_abi().data() );

        base_tester::push_action( N(eosio.token), N(create), N(eosio.token), mvo()
                ("issuer", "eosio.token")
                ("maximum_supply", SYS_1000000_0000));

        base_tester::push_action( N(eosio.token), N(issue), N(eosio.token), mvo()
                ("to", "eosio.token")
                ("quantity", SYS_1000000_0000)
                ("memo", "issue all tokens to eosio.token"));

        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mvo()
                ("from", "eosio.token")
                ("to", "apple")
                ("quantity", asset::from_string("2000.0000 SYS"))
                ("memo", "transfer 20000 SYS to apple"));

        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mvo()
                ("from", "eosio.token")
                ("to", "ibm")
                ("quantity", asset::from_string("2000.0000 SYS"))
                ("memo", "transfer 2000 SYS to ibm"));

        p();
        set_code( N(accounting), contracts::accounting_wasm() );
        set_abi( N(accounting), contracts::accounting_abi().data() );

        set_code( N(apple), contracts::accounting_wasm() );
        set_abi( N(apple), contracts::accounting_abi().data() );

        set_code( N(ibm), contracts::accounting_wasm() );
        set_abi( N(ibm), contracts::accounting_abi().data() );

p();
        set_code( N(apple), contracts::notifyhook_wasm() );
        set_abi( N(apple), contracts::notifyhook_abi().data() );
p();
        set_code( N(ibm), contracts::notifyhook_wasm() );
        set_abi( N(ibm), contracts::notifyhook_abi().data() );

p();
        produce_blocks();

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(accounting) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
            accounting_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        {
        	const auto& accnt = control->db().get<account_object,by_name>( N(apple) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
            apple_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(ibm) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);

            acct2_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }

        {
            const auto& accnt = control->db().get<account_object,by_name>( N(eosio.token) );
            abi_def abi;

            BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);

            tok_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));
        }


        /*
        add_acct(N(apple));

        add_acct(N(ibm));

        add_acct(N(microsoft));

        add_cat(N(apple), "checking");

        add_cat(N(apple), "savings");

        add_cat(N(ibm), "checking");

        add_cat(N(ibm), "savings");

        transfer_fund(N(apple), "default", "checking", SYS_123_1234);

        base_tester::push_action( N(eosio.token), N(transfer), N(apple), mvo()
                ("from", "apple")
                ("to", "ibm")
                ("quantity", asset::from_string("2000.0000 SYS"))
                ("memo", "tx 20000 to ibm"));
        */
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
                accounting_abi_ser.binary_to_variant("useraccount",
                                          get_row_by_account(N(accounting), N(accounting), N(useraccount), user_name), // code , scope, table, primary key value
                                          abi_serializer::create_yield_function(abi_serializer_max_time));

        const auto& cat_list = var["categories"];

        for(size_t idx=0; idx<cat_list.size(); ++idx)
        {
            if (cat_name == cat_list[idx]["category_name"])
            {
                return asset::from_string(cat_list[idx]["balance"].as_string());
            }
        }
        return INVALID_0_0001;
    }

    void eosio_transfer(const name& from_user, const name &to_user, const asset& amount)
    {
        base_tester::push_action( N(eosio.token), N(transfer), from_user, mvo()
                ("from", from_user)
                ("to", to_user)
                ("quantity", amount)
                ("memo", "token transfer"));
    };

    abi_serializer tok_abi_ser;
    abi_serializer accounting_abi_ser;
    abi_serializer apple_abi_ser;
    abi_serializer acct2_abi_ser;
    abi_serializer acct3_abi_ser;

    const asset SYS_1000000_0000 = asset::from_string("1000000.0000 SYS");
    const asset INVALID_0_0001 = asset::from_string("0.0001 INVALID");

    const std::string CAT_DEFAULT = "default";
    const std::string CAT_PAYROLL = "payroll";
    const std::string CAT_TRAVEL = "travel";


};


/*

BOOST_FIXTURE_TEST_CASE( on_notify_test, accounting_tester )try {

    BOOST_REQUIRE_NO_THROW( add_acct(N(apple)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // transfer eosio token to apple
    BOOST_REQUIRE_NO_THROW(eosio_transfer(N(apple), SYS_234_3456));
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), SYS_234_3456 + asset::from_string("2000.0000 SYS"));
} FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE( on_notify_fail_test, accounting_tester )try {
    BOOST_REQUIRE_NO_THROW( add_acct(N(apple)) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // negative assign
    BOOST_REQUIRE_THROW(eosio_transfer(N(apple), SYS_NEG_23_5678), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // different currency
    BOOST_REQUIRE_THROW(eosio_transfer(N(apple), INVALID_0_0001), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // overdraw
    BOOST_REQUIRE_THROW(eosio_transfer(N(apple), SYS_1000000_0000), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
} FC_LOG_AND_RETHROW()
*/

BOOST_FIXTURE_TEST_CASE( accounting_test, accounting_tester )try {
	BOOST_REQUIRE_NO_THROW( add_acct(N(apple)) );
	BOOST_REQUIRE_THROW( add_acct(N(apple)), transaction_exception );
	BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));

	BOOST_REQUIRE_NO_THROW( add_acct(N(ibm)) );
	BOOST_REQUIRE_THROW( add_acct(N(ibm)), transaction_exception );
	BOOST_CHECK_EQUAL( get_fund_balance(N(ibm), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));

	BOOST_REQUIRE_NO_THROW( add_cat(N(apple), CAT_PAYROLL) );
	BOOST_REQUIRE_THROW( add_cat(N(apple), CAT_PAYROLL), transaction_exception);

	BOOST_REQUIRE_NO_THROW( add_cat(N(ibm), CAT_PAYROLL) );
	BOOST_REQUIRE_THROW( add_cat(N(ibm), CAT_PAYROLL), transaction_exception);

	BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_TRAVEL, CAT_PAYROLL, asset::from_string("1.0000 SYS")), eosio_assert_message_exception );
	BOOST_REQUIRE_NO_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, asset::from_string("1.0000 SYS")) );
	BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_PAYROLL), asset::from_string("1.0000 SYS"));
	BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("1999.0000 SYS"));


    eosio_transfer(N(apple), N(ibm), asset::from_string("100.0000 SYS"));
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("1899.0000 SYS"));
    BOOST_CHECK_EQUAL( get_fund_balance(N(ibm), CAT_DEFAULT), asset::from_string("2100.0000 SYS"));



	std::cout << "end of accounting test" << std::endl;

} FC_LOG_AND_RETHROW()

/*
BOOST_FIXTURE_TEST_CASE( transfer_fund_fail_test, accounting_tester )try {
    // create an account
    BOOST_REQUIRE_NO_THROW ( add_acct(N(apple)));

    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));

    // transfer to non-existing category, nothing change
    BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, SYS_1_2345), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS") );

    // transfer from non-existing category, nothing change
    BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_PAYROLL, CAT_DEFAULT, SYS_1_2345), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS") );

    // same category transfer, nothing change
    BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_DEFAULT, SYS_1_2345), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS") );

    // transfer from not existing account, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(ibm), CAT_DEFAULT, CAT_PAYROLL, SYS_40_3456), eosio_assert_message_exception);

    // add category
    BOOST_REQUIRE_NO_THROW( add_cat(N(apple), CAT_PAYROLL) );
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_PAYROLL), SYS_0_0000);

    // over draw, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, SYS_3456_1234), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_PAYROLL), SYS_0_0000);

    // transfer 0 fund
    BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, SYS_0_0000), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_PAYROLL), SYS_0_0000);

    // transfer negative fund, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, SYS_NEG_23_5678), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_PAYROLL), SYS_0_0000);

    // transfer different currency, no change
    BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, INVALID_0_0001), eosio_assert_message_exception);
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));
    // check fund
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_PAYROLL), SYS_0_0000);

} FC_LOG_AND_RETHROW()
*/
