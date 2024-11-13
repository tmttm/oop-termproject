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
    string password;
    double balance;
    vector<string> transactionHistory;

public:
    Account() : userName(""), accountNumber(""), password(""), balance(0.0) {}

    Account(string name, string number, string pw, double initialFunds) 
        : userName(name), accountNumber(number), password(pw), balance(initialFunds) {}

    // 은행명 필요
    static Account setupAccount() {
        string userName, accountNumber, password, bankName;
        double initialBalance;

        cout << "Enter user name: ";
        cin >> userName;
        cout << "Enter 12-digit account number: ";
        cin >> accountNumber;
        cout << "Enter password: ";
        cin >> password;
        cout << "Enter initial balance: ";
        cin >> initialBalance;

        return Account(userName, accountNumber, password, initialBalance);
    }

    string getAccountNumber() const {return accountNumber;}

    double getBalance() const {return balance;}

    void addFunds(double amount) {
        balance += amount;
        recordTransaction("Deposit: " + to_string(amount) + " won");
    }

    void deductFunds(double amount) {
        if (amount <= balance) {
            balance -= amount;
            recordTransaction("Withdrawal: " + to_string(amount) + " won");
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

    bool verifyPassword(const string& pw) const {return password == pw;}
};



class Bank {
private:
    string name;
    unordered_map<string, Account> accounts;

public:
    Bank(string bankName) : name(bankName) {}

    void addAccount(const Account& account) {
        accounts[account.getAccountNumber()] = account;
    }

    bool verifyPassword(const string& accountNumber, const string& password) {
        if (accounts.find(accountNumber) != accounts.end() && accounts[accountNumber].verifyPassword(password)) {
            cout << "Password verified." << endl;
            return true;
        } else {
            cout << "Password verification failed." << endl;
            return false;
        }
    }

    string getName() const {return name;}

    Account* getAccount(const string& accountNumber) {
        auto it = accounts.find(accountNumber);
        return (it != accounts.end()) ? &(it->second) : nullptr;
    }

    void transferBetweenAccounts(const string& sourceAccount, const string& destinationAccount, double amount) {
        Account* srcAcc = getAccount(sourceAccount);
        Account* destAcc = getAccount(destinationAccount);
        if (srcAcc && destAcc) {
            srcAcc->deductFunds(amount);
            destAcc->addFunds(amount);
        } else {
            cout << "One or both accounts not found." << endl;
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
    Bank* myBank;
    Account* myAccount;
    const int depositLimitCash = 50;
    const int depositLimitCheck = 30;

public:
    ATM() : sessionActive(false), bilingualSupport(false), myAccount(nullptr), myBank(nullptr) {};

    void setupATM() {
        re_serial:
        cout << "Enter 6-digit ATM serial number: ";
        cin >> serialNumber;
        if (serialNumber.length() != 6) {goto re_serial;}

        cout << "Select ATM type (1: Single Bank, 2: Multi-Bank): ";
        int atmTypeChoice;
        cin >> atmTypeChoice;
        type = (atmTypeChoice == 1) ? "Single Bank" : "Multi-Bank";

        cout << "Select language setting (1: English, 2: Bilingual): ";
        int languageChoice;
        cin >> languageChoice;
        bilingualSupport = (languageChoice == 2);

        cout << "Enter primary bank name: ";
        string bankName;
        cin >> bankName;
        myBank = new Bank(bankName);

        cout << "Enter initial cash for each denomination:" << endl;
        cout << "1000 KRW bills: ";
        cin >> cashInventory[1000];
        cout << "5000 KRW bills: ";
        cin >> cashInventory[5000];
        cout << "10000 KRW bills: ";
        cin >> cashInventory[10000];
        cout << "50000 KRW bills: ";
        cin >> cashInventory[50000];

        initializeTransactionFees();

        cout << "ATM setup complete." << endl;
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

    bool isSessionActive() const {return sessionActive;}

    Bank* getBank() const {return myBank;}

    Account* getAccount() const {return myAccount;}

    void insert_card(const string& accountNumber, const string& password) {
        if (myBank && myBank->verifyPassword(accountNumber, password)) {
            sessionActive = true;
            myAccount = myBank->getAccount(accountNumber);
            cout << "Session started on ATM with serial number " << serialNumber << endl;
        } else {
            cout << "Authentication failed. Please check your account number or password." << endl;
        }
    }

    void end_session() {
        if (sessionActive) {
            sessionActive = false;
            cout << "Session ended. Please take your card." << endl;
            if (myAccount) {
                myAccount->printTransactionHistory();
            }
            myAccount = nullptr;
        }
    }

    //4
    void depositCash(int denomination, int count) {
        if (count > depositLimitCash) {
            cout << "Error: Cash deposit limit of " << depositLimitCash << " bills exceeded." << endl;
            return;
        }

        double totalAmount = denomination * count;
        int fee = (type == "Single Bank") ? transactionFees["deposit_primary"] : transactionFees["deposit_non_primary"];

        if (myAccount->getBalance() >= fee) {
            myAccount->addFunds(totalAmount - fee);
            cashInventory[denomination] += count;
            cout << "Deposited " << totalAmount << " won (" << count << " bills of " << denomination << " won)." << endl;
            cout << "Deposit fee of " << fee << " won applied." << endl;
        } else {
            cout << "Insufficient funds for deposit fee." << endl;
        }
    }

    void depositCheck(double amount) {
        if (amount < 100000) {
            cout << "Error: Minimum check amount is 100,000 won." << endl;
            return;
        }

        int fee = transactionFees["deposit_primary"]; // 수표는 주 은행 수수료만 적용
        if (myAccount->getBalance() >= fee) {
            myAccount->addFunds(amount - fee);
            cout << "Deposited check of " << amount << " won to the account." << endl;
            cout << "Deposit fee of " << fee << " won applied." << endl;
        } else {
            cout << "Insufficient funds for check deposit fee." << endl;
        }
    }

    void handleDeposit() {
        int depositType;
        cout << "Select deposit type (1: Cash, 2: Check): ";
        cin >> depositType;

        if (depositType == 1) {
            int denomination, count;
            cout << "Enter bill denomination (1000, 5000, 10000, 50000): ";
            cin >> denomination;
            cout << "Enter number of bills: ";
            cin >> count;
            depositCash(denomination, count);
        } else if (depositType == 2) {
            double checkAmount;
            cout << "Enter check amount: ";
            cin >> checkAmount;
            depositCheck(checkAmount);
        } else {
            cout << "Invalid deposit type selected." << endl;
        }
    }    

    void withdraw(double withdrawAmount) {
        static int withdrawalsThisSession = 0;

        // 세션당 최대 인출 횟수 확인
        if (withdrawalsThisSession >= 3) {
            cout << "Error: Maximum number of withdrawals per session is 3. Please end the current session and start a new one." << endl;
            end_session();
            return;
        }

        // 출금 가능한 최대 금액 확인
        if (withdrawAmount > 500000) {
            cout << "Error: Maximum withdrawal limit per transaction is 500,000 KRW." << endl;
            return;
        }

        // 수수료 부과 --> 아직 안 함
        double fee = transactionFees["withdrawal_primary"];
        double totalAmount = withdrawAmount + fee;

        // 계좌 잔액이 부족한 경우, 오류 메시지 출력
        if (totalAmount > myAccount->getBalance()) {
            cout << "Error: Insufficient funds in your account to cover the withdrawal and fee." << endl;
            return;
        }

        // 지폐 단위 설정 (고액부터 내림차순)
        int bill_types[] = {50000, 10000, 5000, 1000};
        unordered_map<int, int> billsToDispense;

        // 남은 금액이 0이 될 때까지 반복하면서 지폐를 인출
        double remainingAmount = withdrawAmount;
        for (int bill_type : bill_types) {
            while (remainingAmount >= bill_type && cashInventory[bill_type] > 0) {
                remainingAmount -= bill_type;
                billsToDispense[bill_type]++;
                cashInventory[bill_type]--; // ATM 현금 보유량 감소
            }
        }

        // ATM 현금 잔액이 부족한 경우, 오류 메시지 출력
        if (remainingAmount > 0) {
            // 남은 지폐 수 디버깅 코드
            for (int bill_type : bill_types) {
                cout << cashInventory[bill_type] << endl;
            }            
            cout << "Error: Insufficient cash in ATM for this transaction." << endl;
            return;
        }

        // 출금에 성공했을 경우 계좌 잔액 차감 및 거래 기록 업데이트
        myAccount->deductFunds(totalAmount);
        withdrawalsThisSession++;

        // 지폐 배출 결과 출력
        cout << "ATM dispenses:" << endl;
        for (const auto& pair : billsToDispense) {
            if (pair.second > 0) {
                cout << pair.second << " bill(s) of " << pair.first << " KRW" << endl;
            }
        }

        cout << "Withdrawal successful. Fee applied: " << fee << " KRW" << endl;
        cout << "You have " << withdrawalsThisSession << "withdrawal opportunities left in this session." << endl;
    }
    
    void transferFunds(){
        int choice;
        cout << "Press 1 to transfer cash, 2 to transfer account funds." << endl;
        cin >> choice;
        cout << "press destination account number" << endl;
        string destinationAccount;
        cin >> destinationAccount;
        string sourceAccount;
        double amount;
        if (choice == 1) {
            cout << "insert cash and transition fees" << endl;
            // insert cash and transition fees
            // 금액 확인 및 전송 확인
            // 목표 계좌에 돈 입금
            // ATM 현금 보유량 증가
        }
        else if (choice == 2) {
            cout << "press the source account number" << endl;
            cin >> sourceAccount;
            cout << "press the amount of fund to transfer" << endl;
            cin >> amount;
            // 금액 확인 및 전송 확인
            // 소스 계좌에 돈 출금
            // 목표 계좌에 돈 입금
        }
        cout << "Transferred " << amount << " won from " << sourceAccount << " to " << destinationAccount << "." << endl;
    }
};

int main() {
    ATM myATM;
    myATM.setupATM();
    Bank* myBank = myATM.getBank();

    int numAccounts;
    cout << "\nEnter the number of accounts to create: ";
    cin >> numAccounts;
    for (int i = 0; i < numAccounts; ++i) {
        cout << "\nCreating Account #" << (i + 1) << endl;
        Account newAccount = Account::setupAccount();
        myBank->addAccount(newAccount);
    }
    cout << "Complete to create account!" << endl;

    retry:
    string accountNumber, password;
    cout << "Insert card (Enter account number): ";
    cin >> accountNumber;
    cout << "Enter password: ";
    cin >> password;

    myATM.insert_card(accountNumber, password);

    if (!myATM.isSessionActive()) {goto retry;}

    int choice;
    do {
        if (!myATM.isSessionActive()) {goto retry;}

        cout << "\n--- ATM Menu ---" << endl;
        cout << "1. Check Balance\n2. Deposit\n3. Withdraw\n4. Transfer\n5. Transaction History\n6. Exit\n";
        cout << "Select an option: ";
        cin >> choice;

        if (choice == 1) {
            if (myATM.getAccount()) {
                cout << "Balance: " << myATM.getAccount()->getBalance() << " won" << endl;
            } else {
                cout << "No account selected." << endl;
            }
        }
        else if (choice == 2) {
            double depositAmount;
            cout << "Enter deposit amount: ";
            cin >> depositAmount;
            myATM.getAccount()->addFunds(depositAmount);
            cout << "Deposit successful." << endl;
        }
        else if (choice == 3) {
            double withdrawAmount;

            cout << "Enter withdraw amount: ";
            cin >> withdrawAmount;

            myATM.withdraw(withdrawAmount);
        }
        else if (choice == 4) {
            string destinationAccount;
            double transferAmount;
            cout << "Enter destination account number: ";
            cin >> destinationAccount;
            cout << "Enter transfer amount: ";
            cin >> transferAmount;

            myBank->transferBetweenAccounts(accountNumber, destinationAccount, transferAmount);
            cout << "Transfer successful." << endl;
        }
        else if (choice == 5) {
            if (myATM.getAccount()) {
                myATM.getAccount()->printTransactionHistory();
            } else {
                cout << "No account selected." << endl;
            }
        }
        else if (choice == 6) {
            myATM.end_session();
            cout << "Thank you for using the ATM. Goodbye!" << endl;
        }
        else {
            cout << "Invalid option. Please try again." << endl;
        }

    } while (choice != 6);

    return 0;
}