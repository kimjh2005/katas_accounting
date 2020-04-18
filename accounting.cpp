#include "accounting.hpp"

using namespace eosio;
using namespace std;

name accounting::name_default{"default"};
symbol accounting::symbol_sys{"SYS", 4};
asset accounting::empty_sys_asset{0, accounting::symbol_sys};

void accounting::createuser(name user_name, asset initial_deposit) {
	require_auth(user_name);

	useraccount_table useraccounts(get_self(), get_self().value);

	// find using sorted index
	const auto &user_index = useraccounts.get_index<"by.name"_n>();
    const auto user_account_itr = user_index.find(user_name.value);
	check(user_account_itr == user_index.end(), "duplicate useraccount not allowed");

	auto sym = initial_deposit.symbol;
	check(sym.is_valid(), "invalid symbol name");
	check(initial_deposit.is_valid(), "invalid asset");
	check(initial_deposit.amount > 0, "asset must be positive");

	// user will pay
	useraccounts.emplace(user_name, [&](auto &acct) {
		acct.id = useraccounts.available_primary_key();
		acct.account_name = user_name;
		acct.categories.emplace_back(static_cast<uint64_t>(acct.categories.size() + 1), name_default, initial_deposit);
	});
}

void accounting::createcateg(name user_name, name category_name) {
	require_auth(user_name);

	// check valid useraccount
	useraccount_table useraccounts(get_self(), get_self().value);
    const auto &user_index = useraccounts.get_index<"by.name"_n>();
    const auto user_account_itr = user_index.find(user_name.value);
	check(user_account_itr != user_index.end(), "useraccount not found");
    auto found_by_id_itr = useraccounts.find(user_account_itr->id);

	// check human readable category
	check(accounting::isPrintable(category_name.to_string()), "not human readable category");

	symbol sym{"SYS", 4};
	asset empty_asset{0, sym};


	if (find_category(*found_by_id_itr, category_name) == invalid_index) {
		useraccounts.modify(found_by_id_itr, user_name, [&category_name](auto &acct) {
			acct.categories.emplace_back(static_cast<uint64_t>(acct.categories.size() + 1), category_name, empty_sys_asset);
		});

	} else {
		check(false, "duplicate category not allowed: " + category_name.to_string());
	}
}

void accounting::printacct(name user_name) {
	require_auth(user_name);

	// check valid useraccount
	useraccount_table useraccounts(get_self(), get_self().value);
    const auto &user_index = useraccounts.get_index<"by.name"_n>();
    const auto user_account_itr = user_index.find(user_name.value);
	check(user_account_itr != user_index.end(), "useraccount not found");

	string output = "[ accounting: " + user_name.to_string() + " ]";

    for (const auto &categ : user_account_itr->categories) {
		output += ",[" + to_string(categ.id) + "|" + categ.category_name.to_string() + "|" +
				  to_string(categ.balance.amount) + "]";
	}
	print(output);
}

void accounting::changecateg(name user_name, name from_category, name to_category, asset transferable) {
	require_auth(user_name);
	// check valid useraccount
	useraccount_table useraccounts(get_self(), get_self().value);
    const auto &user_index = useraccounts.get_index<"by.name"_n>();
    const auto user_account_itr = user_index.find(user_name.value);
	check(user_account_itr != user_index.end(), "useraccount not found");
    auto found_by_id_itr = useraccounts.find(user_account_itr->id);

	const auto &categories = found_by_id_itr->categories;
	int64_t from_categ_idx = find_category(*found_by_id_itr, from_category);
	int64_t to_categ_idx = find_category(*found_by_id_itr, to_category);

	check((from_categ_idx != invalid_index), "invalid from-category! Can't change category!");
	check((to_categ_idx != invalid_index), "invalid to-category! Can't change category!");

	check(found_by_id_itr->categories[from_categ_idx].balance.amount >= transferable.amount, "insufficient amount to change category");

	useraccounts.modify(found_by_id_itr, user_name, [&](auto &acct) {
		acct.categories[from_categ_idx].balance -= transferable;
		acct.categories[to_categ_idx].balance += transferable;
	});
}

void accounting::move(name from_user, name to_user, asset transferable, string memo) {
	check(from_user != to_user, "cannot transfer to self");
	require_auth(from_user);

	require_recipient(to_user);

	useraccount_table useraccounts(get_self(), get_self().value);
    const auto &user_index = useraccounts.get_index<"by.name"_n>();
    const auto user_account_itr = user_index.find(from_user.value);
	check(user_account_itr != user_index.end(), "receiving useraccount not found");
    auto found_by_id_itr = useraccounts.find(user_account_itr->id);

	int64_t from_categ_idx = find_category(*found_by_id_itr, name_default);

	check(found_by_id_itr->categories[from_categ_idx].balance.amount >= transferable.amount, "insufficient amount to change category");

	useraccounts.modify(found_by_id_itr, from_user, [&](auto &acct) {
		acct.categories[from_categ_idx].balance -= transferable;
	});

	print("moved");
}

void accounting::onMove(name from_user, name to_user, asset transferable, string memo) {
	if (to_user != _self) return;
	require_auth(to_user);

	useraccount_table useraccounts(get_self(), get_self().value);
    const auto &user_index = useraccounts.get_index<"by.name"_n>();
    const auto user_account_itr = user_index.find(to_user.value);
	check(user_account_itr != user_index.end(), "receiving useraccount not found");
    auto found_by_id_itr = useraccounts.find(user_account_itr->id);

	int64_t to_categ_idx = find_category(*found_by_id_itr, name_default);

	check(found_by_id_itr->categories[to_categ_idx].balance.amount >= transferable.amount, "insufficient amount to change category");

	useraccounts.modify(found_by_id_itr, to_user, [&](auto &acct) {
		acct.categories[to_categ_idx].balance += transferable;
	});



	print("onMove");
}

