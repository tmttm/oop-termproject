#include "Account.h"
Account::Account() : userName(""), accountNumber(""), password(""), balance(0.0), mainbank("") {}

Account::Account(string name, string number, string pw, double initialFunds, string bankname)
    : userName(name), accountNumber(number), password(pw), balance(initialFunds), mainbank(bankname) {}

string Account::getUserName() const {return userName;}
string Account::getBankName() const {return mainbank;}
string Account::getAccountNumber() const {return accountNumber;}
double Account::getBalance() const {return balance;}
bool Account::verifyPassword(const string& pw) const {return password == pw;}

void Account::addFunds(double amount) {
    try {
        if (amount < 0) {
            throw invalid_argument("Deposit amount cannot be negative.");
        }
        balance += amount;
        recordTransaction("Deposit: " + to_string(amount) + " won");
    } catch (const exception& e) {
        cout << "Deposit error: " << e.what() << endl;
    }
}

void Account::deductFunds(double amount) {
    try {
        if (amount < 0) {
            throw invalid_argument("Withdrawal amount cannot be negative.");
        }
        if (balance < amount) {
            throw runtime_error("Insufficient balance.");
        }
        balance -= amount;
        recordTransaction("Withdrawal: " + to_string(amount) + " won");
    } catch (const exception& e) {
        cout << "Withdrawal error: " << e.what() << endl;
    }
}

void Account::recordTransaction(const string& transaction) {
    transactionHistory.push_back(transaction);
}
void Account::recordTransaction(string ID, string cdnumber, string types, int amount, string info) {
    transactionHistory.push_back(ID + " " + cdnumber + " " + types + " " + to_string(amount) + " " + info);
}
void Account::printTransactionHistory() {
    cout << "Transaction History for " << accountNumber << ":" << endl;
    for (const string& transaction : transactionHistory) {
        cout << transaction << endl;
    }
}