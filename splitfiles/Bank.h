#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include "Account.h"
using namespace std;

#ifndef BANK_H
#define BANK_H
Class Bank
{
private:
    string name;
    unordered_map<string, Account> accounts;
public:
    Bank(string bankName);
    string getName() const;
    void createAccount(const string& userName, const string& accountNumber, const string& password, double initialBalance);v cb n
    Account* getAccount(const string& accountNumber);
    bool verifyPassword(const string& accountNumber, const string& password);
    void transferBetweenAccounts(const string& sourceAccount, const string& destinationAccount, double amount);
    void printAllAccounts() const;
}
#endif