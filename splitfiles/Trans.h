#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include "Account.h"
#include "Bank.h"
using namespace std;

#ifndef TRANS_H
#define TRANS_H
class Trans
{
protected:
    Account* account; // 계좌 객체
    Bank* bank; // 관련된 은행 객체
    unordered_map<string, int>& transactionFees; // 수수료 맵

public:
    Trans(Account* acc, Bank* bnk, unordered_map<string, int>& fees);
    virtual void performTransaction() = 0;
};

class Deposit : public Trans
{
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    int depositLimitCash = 50; // 현금 입금 한도
    double minimumCheckAmount = 100000; // 수표 입금 최소 금액

public:
    Deposit(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv);
    string calculateFees();
    void depositCash(int denomination, int count);
    void depositCheck(double amount);
    void performTransaction() override;
};
class Withdraw : public Trans
{
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    double amount; // 출금 요청 금액
    bool successful; // 출금 성공 여부

public:
    Withdraw(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, double amt);
    string calculateFees();
    void performTransaction() override;
    bool isSuccessful() const;
};
class Transfer : public Trans
{
protected:
    Account* destinationAccount; // 송금 대상 계좌
    double amount; // 송금 금액

public:
    Transfer(Account* srcAcc, Bank* bnk, unordered_map<string, int>& fees, Account* destAcc, double amt);
    string calculateFees(const string& destBankName);
    void performTransaction() override;
};
#endif