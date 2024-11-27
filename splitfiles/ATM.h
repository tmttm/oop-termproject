#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include "Bank.h"
#include "Account.h"
#include "Trans.h"
using namespace std;

#ifndef ATM_H
#define ATM_H
class ATM
{
private:
    string serialNumber;                // ATM 시리얼 번호
    string type;                        // ATM 유형 (Single/Multi-Bank)
    bool bilingualSupport;              // 다국어 지원 여부
    unordered_map<int, int> cashInventory; // 현금 보유량
    unordered_map<string, int> transactionFees; // 거래 수수료
    bool sessionActive;                 // 현재 세션 활성화 여부
    Bank* myBank;                       // 주거래 은행
    Account* myAccount;                 // 현재 로그인된 계좌
    static int withdrawalCount;         // 세션당 출금 횟수

public:
    ATM();
    void setupATM();
    void initializeTransactionFees();
    bool isSessionActive() const;
    string getSerialNumber() const;
    Bank* getBank() const;
    Account* getAccount() const;
    void insert_card(const string& accountNumber, const string& password, vector<Bank*>& banks);
    void end_session();
    void deposit();
    void withdraw();
    void transferFunds();
    void displaySnapshot();
    void runATM(vector<Bank*>& banks);
};
#endif