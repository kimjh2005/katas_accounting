#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <tuple>

struct [[eosio::table("useraccount"), eosio::contract("accounting")]] useraccount {
    uint64_t id;
    eosio::name user;            // eosio::name of the associated user
    eosio::asset total;           // total assets including uncatergorized and categorized assets

    uint64_t primary_key() const { return id; }

    uint64_t get_user_value() const { return user.value; }
};

using useraccount_table = eosio::multi_index<"useraccount"_n, useraccount,
        eosio::indexed_by<"by.user"_n, eosio::const_mem_fun < useraccount, uint64_t, &useraccount::get_user_value>>>;

struct [[eosio::table("category"), eosio::contract("accounting")]] category {
    uint64_t id;
    eosio::name user;           // associated user
    eosio::name category_name;  // human readable category name
    eosio::asset balance;    // balance of the categorized account

    uint64_t primary_key() const { return id; }

    uint64_t get_user_value() const { return user.value; }

    uint64_t get_category_name_value() const { return category_name.value; }
};

using category_table = eosio::multi_index<"category"_n, category,
        eosio::indexed_by<"by.user"_n, eosio::const_mem_fun < category, uint64_t, &category::get_user_value>>,
eosio::indexed_by<"by.category"_n, eosio::const_mem_fun<category, uint64_t, &category::get_category_name_value>>>;

class accounting : public eosio::contract {
private:

    useraccount_table _useraccount_table;
    category_table _category_table;

    bool has_useraccount(const eosio::name &user_name) const {
        return _useraccount_table.find(user_name.value) != _useraccount_table.end();
    }


    std::tuple<bool, const category *> find_category(const eosio::name &user_name, const eosio::name &category_name) {
        const auto &category_user_index = _category_table.get_index<"by.user"_n>();
        for (auto itr = category_user_index.lower_bound(user_name.value);
             itr != category_user_index.upper_bound(user_name.value); ++itr) {
            if (itr->category_name == category_name) {
                return std::tuple<bool, const category *>(true, &(*itr));
            }
        }

        return std::tuple<bool, const category *>(false, nullptr);
    }


public:
    accounting(eosio::name receiver, eosio::name code, eosio::datastream<const char *> ds) 
        : contract(receiver, code, ds), 
        _useraccount_table(receiver, receiver.value),
        _category_table(receiver, receiver.value) {}

    // Use contract's constructor
    using contract::contract;

    [[eosio::action]]
    void createuser(eosio::name user_name, eosio::asset initial_asset) {
        require_auth(user_name);
        auto sym = initial_asset.symbol;
        eosio::check(sym.is_valid(), "invalid symbol name");
        eosio::check(initial_asset.is_valid(), "invalid asset");
        eosio::check(initial_asset.amount > 0, "asset must be positive");
        eosio::check(!has_useraccount(user_name), "useraccount " + user_name.to_string() + " already exists");

        // user will pay
        _useraccount_table.emplace(user_name, [&](useraccount &acct) {
            acct.id = _useraccount_table.available_primary_key();
            acct.user = user_name;
            acct.total = initial_asset;
        });

        eosio::name default_category{"default"};

        if (auto[exists, ptr] = find_category(user_name, default_category); !exists) {
            // create the category
            _category_table.emplace(user_name, [&](category &cat) {
                cat.id = _category_table.available_primary_key();
                cat.user = user_name;
                cat.category_name = default_category;
                cat.balance = initial_asset;
            });
        } else {
            eosio::check(false,
                         "found an orphan category " + default_category.to_string() + " " + user_name.to_string());
        }
    }

    [[eosio::action]]
    void createcateg(eosio::name user_name, eosio::name category_name) {
        require_auth(user_name);

        eosio::check(eosio::is_account(user_name), "user account " + user_name.to_string() + " not exist");

        eosio::symbol sym{"SYS", 4};
        eosio::asset empty_asset{0, sym};

        if (auto[exists, ptr] = find_category(user_name, category_name); !exists) {
            // create the category
            _category_table.emplace(user_name, [&](category &cat) {
                cat.id = _category_table.available_primary_key();
                cat.user = user_name;
                cat.category_name = category_name;
                cat.balance = empty_asset;
            });
        } else {
            eosio::check(false, "duplicate category " + category_name.to_string());
        }
    }

    [[eosio::action]]
    void print(eosio::name user_name) {
        require_auth(user_name);

        eosio::check(eosio::is_account(user_name), "user account " + user_name.to_string() + " not exist");

        std::string output = "[ accounting: " + user_name.to_string() + " ]";

        const auto &category_user_index = _category_table.get_index<"by.user"_n>();
        for (auto itr = category_user_index.lower_bound(user_name.value);
             itr != category_user_index.upper_bound(user_name.value); itr++) {
            output += ",[" + std::to_string(itr->id) + "|" + itr->category_name.to_string() + "|" +
                      std::to_string(itr->balance.amount) + "]";
        }

        eosio::print(output);
    }

    [[eosio::action]]
    void change(eosio::name user_name, eosio::name from_category, eosio::name to_category, eosio::asset transferable) {
        require_auth(user_name);

        eosio::check(eosio::is_account(user_name), "user account " + user_name.to_string() + " not exist");

        auto[exists_from, ptr_from] = find_category(user_name, from_category);
        auto[exists_to, ptr_to] = find_category(user_name, to_category);

        eosio::check((exists_from && exists_to), "invalid category! Can't change category!");

        eosio::check(ptr_from->balance.amount >= transferable.amount, "insufficient amount to change category");

        // category
        _category_table.modify(*ptr_from, user_name, [&](category &cat) {
            cat.balance -= transferable;
        });

        _category_table.modify(*ptr_to, user_name, [&](category &cat) {
            cat.balance += transferable;
        });
    }

    [[eosio::action]]
    void transfer(eosio::name from_user, eosio::name to_user, eosio::asset transferable, std::string memo) {
        eosio::check(from_user != to_user, "cannot transfer to self");
        require_auth(get_self());

        require_recipient(to_user);

        eosio::check(eosio::is_account(to_user), "receiving user account " + to_user.to_string() + " does not exist!");

        eosio::name default_category{"default"};
        auto[exists_from, ptr_from] = find_category(from_user, default_category);

        eosio::check(exists_from, "invalid category! Can't change category!");

        eosio::check(ptr_from->balance.amount >= transferable.amount, "insufficient amount to transfer");

        // category
        _category_table.modify(*ptr_from, from_user, [&](category &cat) {
            cat.balance -= transferable;
        });

        eosio::print("transfered");
    }

    [[eosio::on_notify("accounting::transfer")]]
    void onTransfer(eosio::name from_user, eosio::name to_user, eosio::asset transferable, std::string memo) {
        if (to_user != _self) return;

        eosio::check(transferable.symbol == eosio::symbol("SYS", 4), "must be SYS token");
        eosio::check(transferable.amount > 0, " amount must be greater than zero");

        eosio::name default_category{"default"};
        auto[exists_to, ptr_to] = find_category(to_user, default_category);

        // category
        _category_table.modify(*ptr_to, to_user, [&](category &cat) {
            cat.balance += transferable;
        });

        eosio::print("onTransfer");
    }
};


