#include <eosio/eosio.hpp>
#include <eosio/name.hpp>
#include <eosio/asset.hpp>

/**
 * notification hook
 *
 * @details  used to be notified by eosio.token transfer. Once get called, it will notify acconting
 */
class [[eosio::contract]] notifyhook : public eosio::contract {
public:
    using eosio::contract::contract;

    /// This doesn't do anything, but eosio-cpp requires at least one action.
    [[eosio::action]] void dummy() {}

    [[eosio::on_notify("eosio.token::transfer")]]
    void ontransfer(eosio::name from, eosio::name to, eosio::asset quantity, std::string memo) {
        eosio::print("ontransfer from notifyhook");
        eosio::require_recipient(eosio::name("accounting"));

    }
};

