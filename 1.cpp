#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

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
    unordered_map<string, int> transactionFees;
    bool sessionActive;
    string primaryBank;
    static unordered_set<string> usedSerialNumbers;

public:
    ATM(string serial, string atmType, bool bilingual, string bankName) 
        : serialNumber(serial), type(atmType), bilingualSupport(bilingual), primaryBank(bankName), sessionActive(false) {
        if (usedSerialNumbers.count(serial)) {
            throw invalid_argument("Serial number already in use.");
        }
        usedSerialNumbers.insert(serial);
        initializeTransactionFees();
    }

    void initializeTransactionFees() {
        transactionFees["deposit_non_primary"] = 2000;
        transactionFees["deposit_primary"] = 1000;
        transactionFees["withdrawal_primary"] = 1000;
        transactionFees["withdrawal_non_primary"] = 2000;
        transactionFees["transfer_primary_to_primary"] = 2000;
        transactionFees["transfer_primary_to_non_primary"] = 3000;
        transactionFees["transfer_non_primary_to_non_primary"] = 4000;
        transactionFees["cash_transfer"] = 1000;
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

    void authorizeUser(string cardNumber, string bankName) {
        if (type == "Single Bank ATM" && bankName != primaryBank) {
            cout << "Invalid Card: This ATM only accepts cards from " << primaryBank << "." << endl;
            return;
        }
        cout << "User authorized." << endl;
    }

    int getTransactionFee(const string& transactionType) {
        if (transactionFees.find(transactionType) != transactionFees.end()) {
            return transactionFees[transactionType];
        }
        cout << "Invalid transaction type" << endl;
        return 0;
    }

    void processDeposit(bool isPrimaryBank) {
        string feeType = isPrimaryBank ? "deposit_primary" : "deposit_non_primary";
        int fee = getTransactionFee(feeType);
        cout << "Deposit fee: " << fee << " KRW" << endl;
    }

    void processWithdrawal(bool isPrimaryBank) {
        string feeType = isPrimaryBank ? "withdrawal_primary" : "withdrawal_non_primary";
        int fee = getTransactionFee(feeType);
        cout << "Withdrawal fee: " << fee << " KRW" << endl;
    }

    void processTransfer(bool isPrimaryToPrimary, bool isPrimaryToNonPrimary) {
        string feeType;
        if (isPrimaryToPrimary) {
            feeType = "transfer_primary_to_primary";
        } else if (isPrimaryToNonPrimary) {
            feeType = "transfer_primary_to_non_primary";
        } else {
            feeType = "transfer_non_primary_to_non_primary";
        }
        int fee = getTransactionFee(feeType);
        cout << "Transfer fee: " << fee << " KRW" << endl;
    }

    void processCashTransfer() {
        int fee = getTransactionFee("cash_transfer");
        cout << "Cash transfer fee: " << fee << " KRW" << endl;
    }

    void displayTransactionHistory() {
        cout << "Displaying transaction history..." << endl;
    }

    void handleException(string exceptionType) {
        cout << "Exception: " << exceptionType << endl;
    }
};

unordered_set<string> ATM::usedSerialNumbers;