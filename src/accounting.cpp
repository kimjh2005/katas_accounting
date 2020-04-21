#include "accounting.hpp"
#include <eosio.token/eosio.token.hpp>

using namespace eosio;
using namespace std;

name accounting::name_default{"default"};
symbol accounting::symbol_sys{"SYS", 4};
asset accounting::empty_sys_asset{0, accounting::symbol_sys};

void accounting::createuser(name user_name) {
    require_auth(user_name);

    useraccount_table useraccounts(get_self(), get_self().value);
    const auto user_account_itr = useraccounts.find(user_name.value);
    check(user_account_itr == useraccounts.end(), "duplicate useraccount not allowed");

    /// user will pay
    useraccounts.emplace(user_name, [&](auto &acct) {
        acct.account_name = user_name;
        acct.categories.emplace_back(static_cast<uint64_t>(acct.categories.size() + 1),
                                     name_default, eosio::token::get_balance(eosio::name("eosio.token"), user_name,
                                                                             eosio::symbol_code("SYS")));
    });
}

void accounting::createcateg(name user_name, name category_name) {
    require_auth(user_name);

    /// check valid useraccount
    useraccount_table useraccounts(get_self(), get_self().value);
    const auto user_account_itr = useraccounts.find(user_name.value);
    check(user_account_itr != useraccounts.end(), "useraccount not found");

    /// check human readable category
    check(accounting::isPrintable(category_name.to_string()), "not human readable category");

    symbol sym{"SYS", 4};
    asset empty_asset{0, sym};

    if (find_category(*user_account_itr, category_name) == invalid_index) {
        useraccounts.modify(user_account_itr, user_name, [&category_name](auto &acct) {
            acct.categories.emplace_back(static_cast<uint64_t>(acct.categories.size() + 1), category_name,
                                         empty_sys_asset);
        });

    } else {
        check(false, "duplicate category not allowed: " + category_name.to_string());
    }
}

void accounting::printacct(name user_name) {
    require_auth(user_name);

    /// check valid useraccount
    useraccount_table useraccounts(get_self(), get_self().value);
    const auto user_account_itr = useraccounts.find(user_name.value);
    check(user_account_itr != useraccounts.end(), "useraccount not found");

    string output = "[ accounting: " + user_name.to_string() + " ]";

    for (const auto &categ : user_account_itr->categories) {
        output += ",[" + to_string(categ.id) + "|" + categ.category_name.to_string() + "|" +
                  to_string(categ.balance.amount) + "]";
    }
    print(output);
}

void accounting::transfercateg(name user_name, name from_category, name to_category, asset transferable) {
    require_auth(user_name);

    /// check valid useraccount
    useraccount_table useraccounts(get_self(), get_self().value);
    const auto user_account_itr = useraccounts.find(user_name.value);
    check(user_account_itr != useraccounts.end(), "useraccount not found");

    const auto &categories = user_account_itr->categories;
    int64_t from_categ_idx = find_category(*user_account_itr, from_category);
    int64_t to_categ_idx = find_category(*user_account_itr, to_category);

    check((from_categ_idx != invalid_index), "invalid from-category! Can't change category!");
    check((to_categ_idx != invalid_index), "invalid to-category! Can't change category!");

    check(user_account_itr->categories[from_categ_idx].balance.amount >= transferable.amount,
          "insufficient amount to change category");

    useraccounts.modify(user_account_itr, user_name, [&](auto &acct) {
        acct.categories[from_categ_idx].balance -= transferable;
        acct.categories[to_categ_idx].balance += transferable;
    });
}

[[eosio::on_notify("eosio.token::transfer")]]
void accounting::ontransfer(name from_user, name to_user, asset transferable, string memo) {
    useraccount_table useraccounts(get_self(), get_self().value);

    /// update the account which asset is transferred from
    const auto from_user_account_itr = useraccounts.find(from_user.value);
    check(from_user_account_itr != useraccounts.end(), "receiving useraccount not found");
    int64_t from_categ_idx = find_category(*from_user_account_itr, name_default);
    check(from_user_account_itr->categories[from_categ_idx].balance.amount >= transferable.amount,
          "insufficient amount to change category");

    useraccounts.modify(from_user_account_itr, from_user, [&](auto &acct) {
        acct.categories[from_categ_idx].balance -= transferable;
    });

    /// update the account which asset is transferred to
    const auto to_user_account_itr = useraccounts.find(to_user.value);
    check(from_user_account_itr != useraccounts.end(), "receiving useraccount not found");
    int64_t to_categ_idx = find_category(*to_user_account_itr, name_default);
    check(from_user_account_itr->categories[to_categ_idx].balance.amount >= transferable.amount,
          "insufficient amount to change category");

    useraccounts.modify(to_user_account_itr, to_user, [&](auto &acct) {
        acct.categories[to_categ_idx].balance += transferable;
    });
}


