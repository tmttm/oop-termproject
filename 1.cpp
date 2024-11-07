#include <iostream>
using namespace std;

class Account {
private:
    string userName;
    string accountNumber;
    double availableFunds;
    vector<string> transactionHistory;

public:
    Account(string name, string number, double initialFunds) 
        : userName(name), accountNumber(number), availableFunds(initialFunds) {}

    double getBalance() {
        return availableFunds;
    }

    void addFunds(double amount) {
        availableFunds += amount;
    }

    void deductFunds(double amount) {
        if (amount <= availableFunds) {
            availableFunds -= amount;
        } else {
            cout << "Insufficient funds." << endl;
        }
    }

    void recordTransaction(const string& transaction) {
        transactionHistory.push_back(transaction);
    }

    void printTransactionHistory() {
        cout << "Transaction History for " << accountNumber << ":" << endl;
        for (const string& transaction : transactionHistory) {
            cout << transaction << endl;
        }
    }
};


class Bank {
private:
    string name;
    unordered_map<string, Account> accounts;
    string adminCard;

public:
    Bank(string bankName) : name(bankName) {}

    void createAccount(string userName, string accountNumber, double initialFunds) {
        accounts[accountNumber] = Account(userName, accountNumber, initialFunds);
    }

    bool verifyPassword(string cardNumber, string password) {
        return true;
    }

    void depositToAccount(string accountNumber, double amount) {
        if (accounts.find(accountNumber) != accounts.end()) {
            accounts[accountNumber].addFunds(amount);
        } else {
            cout << "Account not found." << endl;
        }
    }

    void withdrawFromAccount(string accountNumber, double amount) {
        if (accounts.find(accountNumber) != accounts.end()) {
            accounts[accountNumber].deductFunds(amount);
        } else {
            cout << "Account not found." << endl;
        }
    }

    void transferBetweenAccounts(string sourceAccount, string destinationAccount, double amount) {
        if (accounts.find(sourceAccount) != accounts.end() && accounts.find(destinationAccount) != accounts.end()) {
            accounts[sourceAccount].deductFunds(amount);
            accounts[destinationAccount].addFunds(amount);
        } else {
            cout << "One or both accounts not found." << endl;
        }
    }

    void displayAllAccounts() {
        for (const auto& entry : accounts) {
            cout << "Account [Bank: " << name << ", No: " << entry.first 
                 << ", Owner: " << entry.second.getBalance() << " won]" << endl;
        }
    }
};

class ATM {
private:
    string serialNumber;
    string type;
    bool bilingualSupport;
    unordered_map<int, int> cashInventory;
    double transactionFees;
    bool sessionActive;

public:
    ATM(string serial, string atmType, bool bilingual) 
        : serialNumber(serial), type(atmType), bilingualSupport(bilingual), sessionActive(false) {}

    void initializeATM() {
        cout << "ATM initialized with serial number: " << serialNumber << endl;
    }

    void setLanguage() {
        if (bilingualSupport) {
            cout << "Select language: 1. English 2. Korean" << endl;
        } else {
            cout << "Language set to English." << endl;
        }
    }

    void startSession() {
        sessionActive = true;
        cout << "Session started." << endl;
    }

    void endSession() {
        sessionActive = false;
        cout << "Session ended." << endl;
    }

    void authorizeUser(string cardNumber, string password) {
        cout << "User authorized." << endl;
    }

    void depositCash(int denomination, int count) {
        cashInventory[denomination] += count;
        cout << "Deposited " << count << " bills of " << denomination << " won." << endl;
    }

    void withdrawCash(double amount) {
        cout << "Withdrew " << amount << " won." << endl;
    }

    void transferFunds(string sourceAccount, string destinationAccount, double amount) {
        cout << "Transferred " << amount << " won from " << sourceAccount << " to " << destinationAccount << "." << endl;
    }

    void displayTransactionHistory() {
        cout << "Displaying transaction history..." << endl;
    }

    void handleException(string exceptionType) {
        cout << "Exception: " << exceptionType << endl;
    }
};
