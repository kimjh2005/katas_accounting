#include <boost/test/unit_test.hpp>
#include <iostream>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/resource_limits.hpp>
#include "contracts.hpp"


using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;


struct accounting_tester: public tester {
	accounting_tester() {
        produce_blocks( 2 );

        /// creates accounts
        create_accounts( { N(apple), N(ibm), N(accounting), N(eosio.token), N(notifyhook) } );
        produce_blocks( 2 );

        set_code( N(eosio.token), contracts::token_wasm() );
        set_abi( N(eosio.token), contracts::token_abi().data() );

        base_tester::push_action( N(eosio.token), N(create), N(eosio.token), mutable_variant_object()
                ("issuer", "eosio.token")
                ("maximum_supply", SYS_1000000_0000));

        base_tester::push_action( N(eosio.token), N(issue), N(eosio.token), mutable_variant_object()
                ("to", "eosio.token")
                ("quantity", SYS_1000000_0000)
                ("memo", "issue all tokens to eosio.token"));

        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mutable_variant_object()
                ("from", "eosio.token")
                ("to", "apple")
                ("quantity", asset::from_string("2000.0000 SYS"))
                ("memo", "transfer 20000 SYS to apple"));

        base_tester::push_action( N(eosio.token), N(transfer), N(eosio.token), mutable_variant_object()
                ("from", "eosio.token")
                ("to", "ibm")
                ("quantity", asset::from_string("2000.0000 SYS"))
                ("memo", "transfer 2000 SYS to ibm"));

        set_code( N(accounting), contracts::accounting_wasm() );
        set_abi( N(accounting), contracts::accounting_abi().data() );

        set_code( N(apple), contracts::accounting_wasm() );
        set_abi( N(apple), contracts::accounting_abi().data() );

        set_code( N(ibm), contracts::accounting_wasm() );
        set_abi( N(ibm), contracts::accounting_abi().data() );

        set_code( N(apple), contracts::notifyhook_wasm() );
        set_abi( N(apple), contracts::notifyhook_abi().data() );
        set_code( N(ibm), contracts::notifyhook_wasm() );
        set_abi( N(ibm), contracts::notifyhook_abi().data() );

        produce_blocks();

		const auto& accnt = control->db().get<account_object,by_name>( N(accounting) );
		abi_def abi;

		BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
		accounting_abi_ser.set_abi(abi, abi_serializer::create_yield_function(abi_serializer_max_time));

        produce_blocks();
    }

    void add_acct( const name& user_name ) {
        base_tester::push_action( N(accounting), N(createuser), user_name, mutable_variant_object()
                ("user_name", user_name));
    }

    void add_cat( const name& user_name, const std::string& cat_name ) {
        base_tester::push_action( N(accounting), N(createcateg), user_name, mutable_variant_object()
                ("user_name", user_name)
                ("category_name", cat_name));
    }

    void transfer_fund( const name& user, const std::string& cat_from, const std::string& cat_to, const asset& amount) {

        base_tester::push_action( N(accounting), N(transfercateg), user, mutable_variant_object()
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
        base_tester::push_action( N(eosio.token), N(transfer), from_user, mutable_variant_object()
                ("from", from_user)
                ("to", to_user)
                ("quantity", amount)
                ("memo", "token transfer"));
    };


    void setupAccounts() {
    	BOOST_REQUIRE_NO_THROW( add_acct(N(apple)) );

    	/// duplicate account
    	BOOST_REQUIRE_THROW( add_acct(N(apple)), transaction_exception );
		BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));

		BOOST_REQUIRE_NO_THROW( add_acct(N(ibm)) );
		BOOST_REQUIRE_THROW( add_acct(N(ibm)), transaction_exception );
		BOOST_CHECK_EQUAL( get_fund_balance(N(ibm), CAT_DEFAULT), asset::from_string("2000.0000 SYS"));

		BOOST_REQUIRE_NO_THROW( add_cat(N(apple), CAT_PAYROLL) );
		BOOST_REQUIRE_THROW( add_cat(N(apple), CAT_PAYROLL), transaction_exception);
		BOOST_REQUIRE_NO_THROW( add_cat(N(apple), CAT_TRAVEL) );

		BOOST_REQUIRE_NO_THROW( add_cat(N(ibm), CAT_PAYROLL) );
		BOOST_REQUIRE_THROW( add_cat(N(ibm), CAT_PAYROLL), transaction_exception);
		BOOST_REQUIRE_NO_THROW( add_cat(N(ibm), CAT_TRAVEL) );
    }


    abi_serializer accounting_abi_ser;
    const asset SYS_1000000_0000 = asset::from_string("1000000.0000 SYS");
    const asset INVALID_0_0001 = asset::from_string("0.0001 INVALID");

    const std::string CAT_DEFAULT = "default";
    const std::string CAT_PAYROLL = "payroll";
    const std::string CAT_TRAVEL = "travel";
};


BOOST_FIXTURE_TEST_CASE( category_test, accounting_tester )try {
	setupAccounts();

	/// empty balance
	BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_TRAVEL, CAT_PAYROLL, asset::from_string("1.0000 SYS")), eosio_assert_message_exception );
	/// too much transfer
	BOOST_REQUIRE_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, asset::from_string("100000.0000 SYS")), eosio_assert_message_exception );

	// working
	BOOST_REQUIRE_NO_THROW( transfer_fund( N(apple), CAT_DEFAULT, CAT_PAYROLL, asset::from_string("1.0000 SYS")) );
	BOOST_REQUIRE_NO_THROW( transfer_fund( N(apple), CAT_PAYROLL, CAT_TRAVEL, asset::from_string("1.0000 SYS")));

	BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_TRAVEL), asset::from_string("1.0000 SYS"));
	BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("1999.0000 SYS"));

	std::cout << "end of category test" << std::endl;

} FC_LOG_AND_RETHROW()


BOOST_FIXTURE_TEST_CASE( transfer_notify_test, accounting_tester )try {
	setupAccounts();

	/// wrong sender
    BOOST_REQUIRE_THROW(eosio_transfer(N(fakeapple), N(ibm), asset::from_string("100.0000 SYS")), transaction_exception);

    /// wrong receiver
    BOOST_REQUIRE_THROW(eosio_transfer(N(apple), N(fakeibm), asset::from_string("100.0000 SYS")), eosio_assert_message_exception);

    /// too much asset
    BOOST_REQUIRE_THROW(eosio_transfer(N(apple), N(ibm), asset::from_string("1000000.0000 SYS")), eosio_assert_message_exception);

    BOOST_REQUIRE_NO_THROW(eosio_transfer(N(apple), N(ibm), asset::from_string("100.0000 SYS")));
    BOOST_CHECK_EQUAL( get_fund_balance(N(apple), CAT_DEFAULT), asset::from_string("1900.0000 SYS"));
    BOOST_CHECK_EQUAL( get_fund_balance(N(ibm), CAT_DEFAULT), asset::from_string("2100.0000 SYS"));

	std::cout << "end of transfer_notify test" << std::endl;

} FC_LOG_AND_RETHROW()
