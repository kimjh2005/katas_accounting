# examples of commands used to test manually 
> cleos wallet create --to-console
Creating wallet: default
Save password to use in the future to unlock this wallet.
Without password imported keys will not be retrievable.
"PW5KGtt47QW4LdmTCcXxzgLTzU8SUx6jLr6xJE6UWoT164Mv1jFq5"  

> cleos wallet open
Opened: default

> cleos wallet unlock
password: PW5KGtt47QW4LdmTCcXxzgLTzU8SUx6jLr6xJE6UWoT164Mv1jFq5
Unlocked: default

> cleos wallet list
Wallets:
[
  "default *"
]

> cleos wallet create_key
Created new private key with a public key of: "EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB"  //PUBLIC_KEY


/*
Every new EOSIO chain has a default "system" user called "eosio". This account is used to setup the chain by loading system contracts that dictate the governance and consensus of the EOSIO chain. Every new EOSIO chain comes with a development key, and this key is the same. Load this key to sign transactions on behalf of the system user (eosio)
*/
> cleos wallet import
private key:5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3  //private key for eosio
 imported private key for: EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV

> sh st_nodeos.sh
> cat st_nodeos.sh           
pkill keosd
keosd &
pkill nodeos
nodeos -e -p eosio \
--plugin eosio::producer_plugin \
--plugin eosio::producer_api_plugin \
--plugin eosio::chain_api_plugin \
--plugin eosio::http_plugin \
--plugin eosio::history_plugin \
--plugin eosio::history_api_plugin \
--filter-on="*" \
--access-control-allow-origin='*' \
--contracts-console \
--http-validate-host=false \
--verbose-http-errors \
--delete-all-blocks \
>> nodeos.log 2>&1 &
ps -ef|grep nodeos
     
> tail -f nodeos.log     
> cleos create account eosio bob EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
executed transaction: 0c585e5683c0abb4d2c02923840edd443cf1d055f4fce7c6a1ed95bff045e1ee  200 bytes  164 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"bob","owner":{"threshold":1,"keys":[{"key":"EOS6anZ5PCEDTgBRQdyFCUY92N8cf...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
> cleos create account eosio alice EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
executed transaction: 1d878423279ddebc4a3efbfa8628644c68c1b76c6cde258d55dd90a04d389aa7  200 bytes  186 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"alice","owner":{"threshold":1,"keys":[{"key":"EOS6anZ5PCEDTgBRQdyFCUY92N8...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
> cleos create account eosio jane EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
executed transaction: 50fcd5e6a69abc3c0d0a64153fc71ff96f2d717e8534f754790ade49ebc739ec  200 bytes  157 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"jane","owner":{"threshold":1,"keys":[{"key":"EOS6anZ5PCEDTgBRQdyFCUY92N8c...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
> cleos create account eosio tim EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
executed transaction: 7a9257aa6c4ab9889f32f0dbbd11b1cf116b600fbc9287f4efec383e0bae88ee  200 bytes  138 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"tim","owner":{"threshold":1,"keys":[{"key":"EOS6anZ5PCEDTgBRQdyFCUY92N8cf...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 


> cleos get account eosio
created: 2018-06-01T12:00:00.000
privileged: true
permissions: 
     owner     1:    1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
        active     1:    1 EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
memory: 
     quota:       unlimited  used:      2.66 KiB  

net bandwidth: 
     used:               unlimited
     available:          unlimited
     limit:              unlimited

cpu bandwidth:
     used:               unlimited
     available:          unlimited
     limit:              unlimited


> cleos get account alice
created: 2020-04-20T17:22:55.000
permissions: 
     owner     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
        active     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
memory: 
     quota:       unlimited  used:      2.66 KiB  

net bandwidth: 
     used:               unlimited
     available:          unlimited
     limit:              unlimited

cpu bandwidth:
     used:               unlimited
     available:          unlimited
     limit:              unlimited


> cleos get account bob  
created: 2020-04-20T17:22:41.000
permissions: 
     owner     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
        active     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
memory: 
     quota:       unlimited  used:      2.66 KiB  

net bandwidth: 
     used:               unlimited
     available:          unlimited
     limit:              unlimited

cpu bandwidth:
     used:               unlimited
     available:          unlimited
     limit:              unlimited


> cleos get account jane
created: 2020-04-20T17:23:02.000
permissions: 
     owner     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
        active     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
memory: 
     quota:       unlimited  used:      2.66 KiB  

net bandwidth: 
     used:               unlimited
     available:          unlimited
     limit:              unlimited

cpu bandwidth:
     used:               unlimited
     available:          unlimited
     limit:              unlimited

> cleos get account tim 
created: 2020-04-20T17:23:18.000
permissions: 
     owner     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
        active     1:    1 EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
memory: 
     quota:       unlimited  used:      2.66 KiB  

net bandwidth: 
     used:               unlimited
     available:          unlimited
     limit:              unlimited

cpu bandwidth:
     used:               unlimited
     available:          unlimited
     limit:              unlimited


> cd eosio.contracts 
eosio.contracts> ls
CMakeLists.txt	CONTRIBUTING.md  LICENSE  README.md  build.sh  contracts  docker  docs	pipeline.jsonc	scripts  tests
j/eosio.contracts> cd contracts 
eosio.contracts/contracts> ls
CMakeLists.txt	eosio.bios  eosio.msig	eosio.system  eosio.token  eosio.wrap  icons

// Create the eosio.token account
eosio.contracts/contracts> cleos create account eosio eosio.token EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
executed transaction: de693109ca968e9cc7a11394cc07dc89512e01dedd9e7cd56b66d720ca429336  200 bytes  215 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"eosio.token","owner":{"threshold":1,"keys":[{"key":"EOS6MRyAjQq8ud7hVNYcf...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

eosio.contracts/contracts/eosio.token> ls src
eosio.token.cpp
eosio.contracts/contracts/eosio.token> eosio-cpp -I include -o eosio.token.wasm src/eosio.token.cpp --abigen
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <create> does not have a ricardian contract
Warning, action <issue> does not have a ricardian contract
Warning, action <retire> does not have a ricardian contract
Warning, action <transfer> does not have a ricardian contract
Warning, action <open> does not have a ricardian contract
Warning, action <close> does not have a ricardian contract
Warning, action <create> does not have a ricardian contract
Warning, action <issue> does not have a ricardian contract
Warning, action <retire> does not have a ricardian contract
Warning, action <transfer> does not have a ricardian contract
Warning, action <open> does not have a ricardian contract
Warning, action <close> does not have a ricardian contract
eosio.contracts/contracts/eosio.token> 

eosio.contracts/contracts/eosio.token> cleos set contract eosio.token /Users/jongho.kim/contracts/eosio.contracts/contracts/eosio.token --abi eosio.token.abi -p eosio.token@active
Reading WASM from /Users/jongho.kim/contracts/eosio.contracts/contracts/eosio.token/eosio.token.wasm...
Publishing contract...
executed transaction: 345d68eec3661f3b7af7c2089c7b1ed6c8c5450fe64917ff630c539dac2b8a63  6984 bytes  1794 us
#         eosio <= eosio::setcode               {"account":"eosio.token","vmtype":0,"vmversion":0,"code":"0061736d0100000001a0011b60000060017e006002...
#         eosio <= eosio::setabi                {"account":"eosio.token","abi":"0e656f73696f3a3a6162692f312e310008076163636f756e7400010762616c616e63...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

/**Create the token
The command above created a new token SYS with a precision of 4 decimals and a maximum supply of 1000000000.0000 SYS. It also designates alice as the issuer. To create this token, the contract requires the permission of the eosio.token account
*/
eosio.contracts/contracts/eosio.token/src> cleos push action eosio.token create '["bob", "100000000.0000 SYS"]' -p eosio.token@active
executed transaction: 769eb82421797eb2dddf70e10f97e7dca039bc3d8a88480c0fa4350884d3a47e  120 bytes  191 us
#   eosio.token <= eosio.token::create          {"issuer":"bob","maximum_supply":"100000000.0000 SYS"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

eosio.contracts/contracts/eosio.token/src> cleos push action eosio.token issue '["bob", "11000.0000 SYS", "issuing SYS"]' -p bob@active
executed transaction: 436bbc0069fbd5e390bf268060f22e6b9e58da46435da480a059fc795dd7f494  128 bytes  203 us
#   eosio.token <= eosio.token::issue           {"to":"bob","quantity":"11000.0000 SYS","memo":"issuing SYS"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

eosio.contracts/contracts/eosio.token> cleos get currency balance eosio.token bob SYS  
11000.0000 SYS

eosio.contracts/contracts/eosio.token/src> cleos push action eosio.token transfer '["bob", "alice", "10.0000 SYS", "give 25 sys"]' -p bob@active
executed transaction: 78549906ceff396be770ab350be910e89dbd57a3ffc496a587ceff20cb7c3eec  136 bytes  390 us
#   eosio.token <= eosio.token::transfer        {"from":"bob","to":"alice","quantity":"10.0000 SYS","memo":"give 25 sys"}
#           bob <= eosio.token::transfer        {"from":"bob","to":"alice","quantity":"10.0000 SYS","memo":"give 25 sys"}
#         alice <= eosio.token::transfer        {"from":"bob","to":"alice","quantity":"10.0000 SYS","memo":"give 25 sys"}
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

eosio.contracts/contracts/eosio.token/src> cleos get currency balance eosio.token bob SYS
10990.0000 SYS
eosio.contracts/contracts/eosio.token/src> cleos get currency balance eosio.token alice SYS
10.0000 SYS
eosio.contracts/contracts/eosio.token/src> 

kata/accounting> eosio-cpp -o accounting.wasm accounting.cpp notify_hook.cpp --abigen
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <createuser> does not have a ricardian contract
Warning, action <createcateg> does not have a ricardian contract
Warning, action <printacct> does not have a ricardian contract
Warning, action <changecateg> does not have a ricardian contract
Warning, action <createuser> does not have a ricardian contract
Warning, action <createcateg> does not have a ricardian contract
Warning, action <printacct> does not have a ricardian contract
Warning, action <changecateg> does not have a ricardian contract
Warning, empty ricardian clause file
Warning, empty ricardian clause file
Warning, action <createuser> does not have a ricardian contract
Warning, action <createcateg> does not have a ricardian contract
Warning, action <printacct> does not have a ricardian contract
Warning, action <changecateg> does not have a ricardian contract
Warning, action <createuser> does not have a ricardian contract
Warning, action <createcateg> does not have a ricardian contract
Warning, action <printacct> does not have a ricardian contract
Warning, action <changecateg> does not have a ricardian contract
kata/accounting> ls
CMakeLists.txt	accounting.abi	accounting.hpp	 build	include		 run.sh  tests
README.txt	accounting.cpp	accounting.wasm  cw.sh	notify_hook.cpp  src

// Doesn’t work yet
kata/accounting> cleos set contract accounting /Users/jongho.kim/contracts/kata/accounting

Failed to get existing code hash, continue without duplicate check...
Reading WASM from /Users/jongho.kim/contracts/kata/accounting/accounting.wasm...
Publishing contract...
Error 3090003: Provided keys, permissions, and delays do not satisfy declared authorizations
Ensure that you have the related private keys inside your wallet and your wallet is unlocked.
Error Details:
transaction declares authority '{"actor":"accounting","permission":"active"}', but does not have signatures for it.

// Need to create account of accounting
kata/accounting> cleos create account eosio accounting EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
executed transaction: 29c826d6cbc2134d83e58048b49708a3e4852ee99d1f0b777714d0033dc458b9  200 bytes  167 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"accounting","owner":{"threshold":1,"keys":[{"key":"EOS6anZ5PCEDTgBRQdyFCU...
warning: transaction executed locally, but may not be confirmed by the network yet         ]


// Now it works
kata/accounting> cleos set contract accounting /Users/jongho.kim/contracts/kata/accounting                  

Reading WASM from /Users/jongho.kim/contracts/kata/accounting/accounting.wasm...
Publishing contract...
executed transaction: 2ed02b36f1f6ff1f0a298a09af3b7acf32d3ef0de20959f4c8efee3cffacbd55  10624 bytes  1183 us
#         eosio <= eosio::setcode               {"account":"accounting","vmtype":0,"vmversion":0,"code":"0061736d0100000001dd012160000060047f7f7f7f0...
#         eosio <= eosio::setabi                {"account":"accounting","abi":"0e656f73696f3a3a6162692f312e3100060863617465676f727900030269640675696...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

eosio.contracts/contracts/eosio.token/src> cleos get currency balance eosio.token bob SYS  
10950.0000 SYS

kata/accounting> cleos create account eosio notifyhook EOS6anZ5PCEDTgBRQdyFCUY92N8cfy7m5xj3a5C6eSS8Vdsnt2GrB
executed transaction: cadef9aa8aacd0e6cc7a94bb874d67a4a863743bf0bf23d52be36125b1b110df  200 bytes  191 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"notifyhook","owner":{"threshold":1,"keys":[{"key":"EOS6anZ5PCEDTgBRQdyFCU...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
kata/accounting> cleos set contract notifyhook /Users/jongho.kim/contracts/kata/accounting                  
Reading WASM from /Users/jongho.kim/contracts/kata/accounting/accounting.wasm...
Publishing contract...
executed transaction: 36ce3f1379be6bec5a766af7053d59c10a7a23a706eccfe291ddc72f142e6e52  10608 bytes  1178 us
#         eosio <= eosio::setcode               {"account":"notifyhook","vmtype":0,"vmversion":0,"code":"0061736d0100000001dd012160000060047f7f7f7f0...
#         eosio <= eosio::setabi                {"account":"notifyhook","abi":"0e656f73696f3a3a6162692f312e3100060863617465676f727900030269640675696...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

kata/accounting> cleos set code bob notifyhook.wasm
Reading WASM from /Users/jongho.kim/contracts/kata/accounting/notifyhook.wasm...
Setting Code...
executed transaction: 5785c04f34aca8388db0a67e0c7d3ef172e0d19f9b7d91ab3eb1de581ea80cdb  2576 bytes  794 us
#         eosio <= eosio::setcode               {"account":"bob","vmtype":0,"vmversion":0,"code":"0061736d010000000163126000006000017f60027f7f006003...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
kata/accounting> cleos set abi bob notifyhook.abi
Setting ABI...
executed transaction: f44f9278680448cd289c69b2bcc4147f0cfaa010423267fa9749d4d91c4a2c8a  128 bytes  139 us
#         eosio <= eosio::setabi                {"account":"bob","abi":"0e656f73696f3a3a6162692f312e3100010564756d6d7900000100000000002fa54e0564756d...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
kata/accounting> cleos set abi jane notifyhook.abi
Setting ABI...
executed transaction: 6bbadf0c5ccae291219ea0d6d534bffb225996621411cf88b81261cfecd340b6  128 bytes  155 us
#         eosio <= eosio::setabi                {"account":"jane","abi":"0e656f73696f3a3a6162692f312e3100010564756d6d7900000100000000002fa54e0564756...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 
kata/accounting> cleos set code jane notifyhook.wasm
Reading WASM from /Users/jongho.kim/contracts/kata/accounting/notifyhook.wasm...
Setting Code...
executed transaction: 4c6e3703e96346c480872ff77df15ce6918eedd3125865bb0c8f53cef9fdfdfe  2576 bytes  454 us
#         eosio <= eosio::setcode               {"account":"jane","vmtype":0,"vmversion":0,"code":"0061736d010000000163126000006000017f60027f7f00600...
warning: transaction executed locally, but may not be confirmed by the network yet         ]

eosio.contracts/contracts/eosio.token/src> cleos push action eosio.token transfer '["bob", "tim", "1.2000 SYS", "give 1 sys"]' -p bob@active
executed transaction: 6bbbd84ad796f6b919f0373e98548f3839f2b94654c8f16de6eb24f06387cf21  136 bytes  5119 us
#   eosio.token <= eosio.token::transfer        {"from":"bob","to":"tim","quantity":"1.2000 SYS","memo":"give 1 sys"}
#           bob <= eosio.token::transfer        {"from":"bob","to":"tim","quantity":"1.2000 SYS","memo":"give 1 sys"}
>> ontransfer from notifyhook
#           tim <= eosio.token::transfer        {"from":"bob","to":"tim","quantity":"1.2000 SYS","memo":"give 1 sys"}
>> ontransfer from notifyhook
#    accounting <= eosio.token::transfer        {"from":"bob","to":"tim","quantity":"1.2000 SYS","memo":"give 1 sys"}
>> *******  accounting::ontransfer bob tim give 1 sys^^^^^ pass authontransfer
warning: transaction executed locally, but may not be confirmed by the network yet         ]

j/kata/katas_accounting/build/accounting> cleos set contract accounting `pwd`                                      
Reading WASM from /Users/jongho.kim/contracts/kata/katas_accounting/build/accounting/accounting.wasm...
Publishing contract...
executed transaction: 7332df17bc9d7d9cfa76d63fc484943106f1995db60b467894c03c536a3d3fce  11288 bytes  1065 us
#         eosio <= eosio::setcode               {"account":"accounting","vmtype":0,"vmversion":0,"code":"0061736d0100000001d7012060000060047f7f7f7f0...
#         eosio <= eosio::setabi                {"account":"accounting","abi":"0e656f73696f3a3a6162692f312e3100060863617465676f727900030269640675696...
warning: transaction executed locally, but may not be confirmed by the network yet         ] 

cleos push action accounting printacct '[bob]' -p bob
cleos get table accounting accounting useraccount
