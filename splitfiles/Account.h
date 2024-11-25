#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
using namespace std;

#ifndef ACCOUNT_H
#define ACCOUNT_H
class Account
{
private:
    string userName; // 계좌 소유자 이름
    string accountNumber; // 계좌 번호
    string password; // 계좌 비밀번호
    double balance; // 계좌 잔액
    vector<string> transactionHistory; // 거래 내역
    string mainbank; // 소속 은행 이름
public:
    Account();
    Account(string name, string number, string pw, double initialFunds, string bankname);

    string getUserName() const;
    string getBankName() const;
    string getAccountNumber() const;
    double getBalance() const;
    bool verifyPassword(const string& pw) const;
    void addFunds(double amount);
    void deductFunds(double amount);
    void recordTransaction(const string& transaction);
    void recordTransaction(string ID, string cdnumber, string types, int amount, string info);
    void printTransactionHistory();
};
#endif