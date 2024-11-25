#include "Bank.h"

Bank::Bank(string bankName) : name(bankName) {}
string Bank::getName() const { return name; }
void Bank::createAccount(const string& userName, const string& accountNumber, const string& password, double initialBalance) {
    try {
        // 계좌 번호 중복 확인
        if (accounts.find(accountNumber) != accounts.end()) {
            throw runtime_error("This account number already exists.");
        }

        // 계좌 번호 유효성 확인 (12자리)
        if (accountNumber.length() != 12) {
            throw invalid_argument("Account number must be 12 digits long.");
        }

        // 초기 잔액 음수 확인
        if (initialBalance < 0) {
            throw invalid_argument("Initial balance cannot be negative.");
        }

        // 계좌 추가
        accounts[accountNumber] = Account(userName, accountNumber, password, initialBalance, name);
        cout << "Account successfully created: " << accountNumber << endl;
    } catch (const exception& e) {
        cout << "Error creating account: " << e.what() << endl;
    }
}
Account* Bank::getAccount(const string& accountNumber) {
    auto it = accounts.find(accountNumber);
    if (it != accounts.end()) {
        return &it->second;
    }
    return nullptr;
}
bool Bank::verifyPassword(const string& accountNumber, const string& password) {
    try {
        Account* account = getAccount(accountNumber);
        if (!account) {
            throw runtime_error("Account is invalid, cannot verify password.");
        }
        return account->verifyPassword(password);
    } catch (const exception& e) {
        cout << "Password verification error: " << e.what() << endl;
        return false;
    }
}
void Bank::transferBetweenAccounts(const string& sourceAccount, const string& destinationAccount, double amount) {
    try {
        Account* srcAcc = getAccount(sourceAccount);
        Account* destAcc = getAccount(destinationAccount);

        if (!srcAcc || !destAcc) {
            throw runtime_error("Source or destination account not found.");
        }

        if (amount <= 0) {
            throw invalid_argument("Transfer amount should be bigger than 0.");
        }

        srcAcc->deductFunds(amount);
        destAcc->addFunds(amount);

        cout << "Transfer complete: from " << sourceAccount << " to " << destinationAccount << ", " << amount << "won is transferred." << endl;

    } catch (const exception& e) {
        cout << "Transfer error: " << e.what() << endl;
    }
}
void Bank::printAllAccounts() const {
    cout << "Bank: " << name << " - accounts list" << endl;
    for (const auto& pair : accounts) {
        const Account& account = pair.second;
        cout << "Account number: " << account.getAccountNumber()
             << ", owner: " << account.getUserName()
             << ", remain: " << account.getBalance() << " won" << endl;
    }
}