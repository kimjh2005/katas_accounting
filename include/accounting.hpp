#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <tuple>
#include <vector>
#include <locale>

/**
 * Kata accounting
 *
 * @details Implementation of a personal accounting contract
 */

class accounting : public eosio::contract {
public:
    accounting(eosio::name receiver, eosio::name code, eosio::datastream<const char *> ds)
            : contract(receiver, code, ds) {}

    // Use contract's constructor
    using contract::contract;

    /**
     * createuser action
     *
     * @details create a new user's account creating default category. check a duplicate account
     * @param user_name - user's account name
     */
    [[eosio::action]]
    void createuser(eosio::name user_name);

    /**
     * createcate action
     *
     * @details create a category for the user account
     *
     * @param user_name - user's account name
     * @param category_name
     */
    [[eosio::action]]
    void createcateg(eosio::name user_name, eosio::name category_name);

    /**
     * printacct ction
     *
     * @details This action prints account details.
     *
     * @param user_name - user's account name
     */
    [[eosio::action]]
    void printacct(eosio::name user_name);

    /**
     * transfercateg action
     *
     * @details This action transfers asset between the users's categories.
     * @param user_name - user's account name
     * @param from_category - category which asset is transferred from
     * @param to_category - cateogry which asset is transferred to
     * @param transferable - asset transferred
     */
    [[eosio::action]]
    void
    transfercateg(eosio::name user_name, eosio::name from_category, eosio::name to_category, eosio::asset transferable);

    /**
     * notification
     *
     * @details notified by eosio.token transfer action
     *
     * @param from_user - user's account name who sends asset
     * @param to_user  - user's account name who receives asset
     * @param transferable - asset that is transferred
     * @param memo - note
     */
    [[eosio::on_notify("eosio.token::transfer")]]
    void ontransfer(eosio::name from_user, eosio::name to_user, eosio::asset transferable, std::string memo);

private:

    static eosio::name name_default;
    static eosio::symbol symbol_sys;
    static eosio::asset empty_sys_asset;

    static constexpr int64_t invalid_index = -1;

    struct category {
        uint64_t id;
        eosio::name category_name;  // human readable category name
        eosio::asset balance;    // balance of the categorized account

        category() = default;

        category(uint64_t index, const eosio::name &catname, const eosio::asset &catbal)
                : id(index), category_name(catname), balance(catbal) {
        }

        std::string to_string() const {
            return category_name.to_string() + "|" + std::to_string(id) + "|" + balance.to_string();
        }

        EOSLIB_SERIALIZE(category, (id)(category_name)(balance)
        )
    };


    struct [[eosio::table("useraccount"), eosio::contract("accounting")]] useraccount {
        eosio::name account_name;            // eosio::name of the associated user
        std::vector<category> categories;

        uint64_t primary_key() const { return account_name.value; }

        EOSLIB_SERIALIZE(useraccount, (account_name)(categories)
        )
    };

    typedef eosio::multi_index<"useraccount"_n, useraccount> useraccount_table;


    int64_t find_category(const useraccount &ua, const eosio::name &category_name) {
        for (int i = 0; i < ua.categories.size(); ++i) {
            if (ua.categories[i].category_name == category_name) return i;
        }
        return invalid_index;
    }

    static bool isPrintable(const std::string &value) {
        for (auto c : value) {
            if (!std::isprint(c)) return false;
        }
        return true;
    }
};
