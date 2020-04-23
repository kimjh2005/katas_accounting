#Accounting kata
https://github.com/EOSIO/devdocs/wiki/Katas-Accounting-Phase-1

#Setup
1. Built using git@github.com:EOSIO/eos.git (tag v2.0.5)
 

# Build
mkdir build
cd build
cmake ..
make -jN  (N = 1 to # of cpu core)


#Unit test
cd build/tests
./accounting_test

ex)build/tests> ./accounting_test 
Random number generator seeded to 1587660386
Running 2 test cases...
end of category test
end of transfer_notify test

*** No errors detected
