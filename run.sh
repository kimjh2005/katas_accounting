eosio-cpp accounting.cpp 
cleos create account eosio accounting EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos set code accounting accounting.wasm
cleos set abi accounting accounting.abi
cleos create account eosio bob EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos create account eosio jane EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
cleos push action accounting createuser '[bob, "10000.0000 SYS"]' -p bob
cleos push action accounting createuser '[jane, "30000.0000 SYS"]' -p jane
cleos push action accounting createcateg '[bob, savings]' -p bob
cleos push action accounting createcateg '[bob, checking]' -p bob
cleos push action accounting createcateg '[bob, stock]' -p bob
cleos push action accounting createcateg '[jane, savings]' -p jane1
cleos push action accounting createcateg '[jane, checking]' -p jane
cleos get table accounting accounting useraccount
cleos get table accounting accounting category
cleos push action accounting change '[bob, default, savings, "10.0000 SYS"]' -p bob
cleos push action accounting change '[bob, savings, checking, "10.0000 SYS"]' -p bob
cleos push action accounting change '[bob, checking, stock,   "10.0000 SYS"]' -p bob
cleos push action accounting transfer '[jane, bob, "1.0000 SYS", "memo"]' -p jane
cleos push action accounting print '[bob]' -p bob
cleos get table accounting accounting useraccount
cleos get table accounting accounting category

