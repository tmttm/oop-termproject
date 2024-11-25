#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
using namespace std;

class Account {
private:
    string userName; // 계좌 소유자 이름
    string accountNumber; // 계좌 번호
    string password; // 계좌 비밀번호
    double balance; // 계좌 잔액
    vector<string> transactionHistory; // 거래 내역
    string mainbank; // 소속 은행 이름

public:
    Account() : userName(""), accountNumber(""), password(""), balance(0.0), mainbank("") {}

    Account(string name, string number, string pw, double initialFunds, string bankname)
        : userName(name), accountNumber(number), password(pw), balance(initialFunds), mainbank(bankname) {}

    string getUserName() const { return userName; }

    string getBankName() const { return mainbank; }

    string getAccountNumber() const { return accountNumber; }

    double getBalance() const { return balance; }

    bool verifyPassword(const string& pw) const { return password == pw; }

    void addFunds(double amount) {
        try {
            if (amount < 0) {
                throw invalid_argument("Deposit amount cannot be negative. 계좌 입금 금액은 음수일 수 없습니다.");
            }
            balance += amount;
            recordTransaction("Deposit 금액: " + to_string(amount) + " won");
        } catch (const exception& e) {
            cout << "Deposit error 금액 오류: " << e.what() << endl;
        }
    }

    void deductFunds(double amount) {
        try {
            if (amount < 0) {
                throw invalid_argument("Withdrawal amount cannot be negative. 출금 금액은 음수일 수 없습니다.");
            }
            if (balance < amount) {
                throw runtime_error("Insufficient balance. 잔액 부족.");
            }
            balance -= amount;
            recordTransaction("Withdrawal 출금 금액: " + to_string(amount) + " won 원");
        } catch (const exception& e) {
            cout << "Withdrawal error 출금 오류: " << e.what() << endl;
        }
    }

    void recordTransaction(const string& transaction) {
        transactionHistory.push_back(transaction);
    }

    void printTransactionHistory() {
        cout << "Transaction History for " << accountNumber << "의 거래 기록:" << endl;
        for (const string& transaction : transactionHistory) {
            cout << transaction << endl;
        }
    }
};


class Bank {
private:
    string name; // 은행 이름
    unordered_map<string, Account> accounts; // 계좌 정보를 저장하는 해시맵 (key: 계좌 번호)

public:
    Bank(string bankName) : name(bankName) {}

    // 은행 이름 반환
    string getName() const { return name; }

    // 계좌 생성 (createAccount)
    void createAccount(const string& userName, const string& accountNumber, const string& password, double initialBalance) {
        try {
            // 계좌 번호 중복 확인
            if (accounts.find(accountNumber) != accounts.end()) {
                throw runtime_error("This account number already exists. 계좌 번호가 이미 존재합니다.");
            }

            // 계좌 번호 유효성 확인 (12자리)
            if (accountNumber.length() != 12) {
                throw invalid_argument("Account number must be 12 digits long. 계좌 번호는 12자리여야 합니다.");
            }

            // 초기 잔액 음수 확인
            if (initialBalance < 0) {
                throw invalid_argument("Initial balance cannot be negative. 초기 잔액은 음수일 수 없습니다.");
            }

            // 계좌 추가
            accounts[accountNumber] = Account(userName, accountNumber, password, initialBalance, name);
            cout << "Account successfully created : " << accountNumber << "가 성공적으로 생성되었습니다." << endl;
        } catch (const exception& e) {
            cout << "Error creating account 계좌 생성 오류: " << e.what() << endl;
        }
    }

    // 계좌 검색
    Account* getAccount(const string& accountNumber) {
        try {
            auto it = accounts.find(accountNumber);
            if (it == accounts.end()) {
                throw runtime_error("Account not found. 계좌를 찾을 수 없습니다.");
            }
            return &it->second;
        } catch (const exception& e) {
            cout << "Error finding account 계좌 검색 오류: " << e.what() << endl;
            return nullptr;
        }
    }

    // 비밀번호 검증
    bool verifyPassword(const string& accountNumber, const string& password) {
        try {
            Account* account = getAccount(accountNumber);
            if (!account) {
                throw runtime_error("Account is invalid, cannot verify password. 계좌가 유효하지 않아 비밀번호를 확인할 수 없습니다.");
            }
            return account->verifyPassword(password);
        } catch (const exception& e) {
            cout << "Password verification error 비밀번호 확인 오류: " << e.what() << endl;
            return false;
        }
    }

    // 계좌 간 송금
    void transferBetweenAccounts(const string& sourceAccount, const string& destinationAccount, double amount) {
        try {
            Account* srcAcc = getAccount(sourceAccount);
            Account* destAcc = getAccount(destinationAccount);

            if (!srcAcc || !destAcc) {
                throw runtime_error("Source or destination account not found. 소스 또는 대상 계좌를 찾을 수 없습니다.");
            }

            if (amount <= 0) {
                throw invalid_argument("Transfer amount should be bigger than 0. 송금 금액은 0보다 커야 합니다.");
            }

            srcAcc->deductFunds(amount);
            destAcc->addFunds(amount);

            cout << "송금 완료: " << sourceAccount << "에서 " << destinationAccount << "로 " << amount << "원이 송금되었습니다." << endl;
            cout << "Transfer complete: from " << sourceAccount << " to " << destinationAccount << ", " << amount << "won is transferred." << endl;

        } catch (const exception& e) {
            cout << "Transfer error 송금 중 오류: " << e.what() << endl;
        }
    }

    // 모든 계좌 정보 출력
    void printAllAccounts() const {
        cout << "Bank 은행: " << name << " - accounts list 계좌 목록" << endl;
        for (const auto& pair : accounts) {
            const Account& account = pair.second;
            cout << "Account number 계좌 번호: " << account.getAccountNumber()
                 << ", owner 소유자: " << account.getUserName()
                 << ", remain 잔액: " << account.getBalance() << " 원" << endl;
        }
    }
};

// 부모 클래스
class Trans {
protected:
    Account* account; // 계좌 객체
    Bank* bank; // 관련된 은행 객체
    unordered_map<string, int>& transactionFees; // 수수료 맵

public:
    Trans(Account* acc, Bank* bnk, unordered_map<string, int>& fees)
        : account(acc), bank(bnk), transactionFees(fees) {}

    virtual void performTransaction() = 0; // 순수 가상 함수
};

// 입금 (Deposit) 클래스
class Deposit : public Trans {
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    int denomination; // 입금할 지폐의 액면가
    int count; // 입금할 지폐의 개수

public:
    Deposit(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, int denom, int cnt)
        : Trans(acc, bnk, fees), cashInventory(cashInv), denomination(denom), count(cnt) {}

    string calculateFees() {
        return (account->getBankName() == bank->getName()) ? "deposit_primary" : "deposit_non_primary";
    }

    void performTransaction() override {
        if (count > 50) { // 최대 입금 한도 검사
            cout << "Error: Maximum limit of 50 bills exceeded for deposit. 최대 50장의 지폐만 삽입할 수 있습니다." << endl;
            return;
        }
        double totalAmount = denomination * count; // 입금 금액 계산
        int fee = transactionFees[calculateFees()]; // 수수료 계산

        account->addFunds(totalAmount - fee); // 계좌에 입금 금액 추가
        cashInventory[denomination] += count; // ATM 현금 보유량 갱신
        cout << "Deposited " << count << " bills of " << denomination << " KRW. Total credited: "
             << totalAmount - fee << " KRW. Fee applied: " << fee << " KRW.\n";
        cout << "입금 완료: " << denomination << "원 지폐 " << count << "장을 삽입했습니다. 총 입금액: "
             << totalAmount - fee << "원. 수수료: " << fee << "원." << endl;
    }
};

// 출금 (Withdraw) 클래스
class Withdraw : public Trans {
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    double amount; // 출금 요청 금액

public:
    Withdraw(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, double amt)
        : Trans(acc, bnk, fees), cashInventory(cashInv), amount(amt) {}

    string calculateFees() {
        return (account->getBankName() == bank->getName()) ? "withdrawal_primary" : "withdrawal_non_primary";
    }

    void performTransaction() override {
        if (amount > 500000) { // 최대 출금 한도 검사
            cout << "Error: Maximum withdrawal limit is 500,000 KRW. 최대 출금 한도는 500,000원 입니다." << endl;
            return;
        }

        int fee = transactionFees[calculateFees()]; // 수수료 계산
        double totalAmount = amount + fee; // 총 출금 금액(수수료 포함)

        if (account->getBalance() < totalAmount) { // 잔액 부족 검사
            cout << "Error: Insufficient account balance. 잔액 부족." << endl;
            return;
        }

        int billTypes[] = {50000, 10000, 5000, 1000}; // 지폐 단위 배열
        unordered_map<int, int> billsToDispense; // 인출할 지폐 구성
        double remainingAmount = amount; // 출금 금액

        // 지폐 배출 계산
        for (int bill : billTypes) {
            while (remainingAmount >= bill && cashInventory[bill] > 0) {
                remainingAmount -= bill;
                billsToDispense[bill]++;
                cashInventory[bill]--;
            }
        }

        if (remainingAmount > 0) { // ATM 잔액 부족 확인
            cout << "Error: ATM has insufficient cash to complete the withdrawal. 출금할 현금이 부족합니다." << endl;
            return;
        }

        account->deductFunds(totalAmount); // 계좌에서 총 출금 금액 차감
        cout << "Withdrawal successful! 출금 완료! Dispensed 배출됨: " << amount << " KRW. Fee applied 거래 수수료: " << fee << " KRW.\n";
        for (const auto& pair : billsToDispense) {
            if (pair.second > 0) {
                cout << pair.second << " bills of " << pair.first << " KRW dispensed." << endl;
                cout << pair.second << "장의 " << pair.first << "원 지폐가 배출되었습니다." << endl;
            }
        }
    }
};

// 송금 (Transfer) 클래스
class Transfer : public Trans {
private:
    Account* destinationAccount; // 송금 대상 계좌
    double amount; // 송금 금액

public:
    Transfer(Account* srcAcc, Bank* bnk, unordered_map<string, int>& fees, Account* destAcc, double amt)
        : Trans(srcAcc, bnk, fees), destinationAccount(destAcc), amount(amt) {}

    string calculateFees(const string& destBankName) {
        if (account->getBankName() == bank->getName() && destBankName == bank->getName()) {
            return "transfer_primary_to_primary";
        } else if (account->getBankName() == bank->getName() && destBankName != bank->getName()) {
            return "transfer_primary_to_non_primary";
        } else {
            return "transfer_non_primary_to_non_primary";
        }
    }

    void performTransaction() override {
        string feeType = calculateFees(destinationAccount->getBankName());
        int fee = transactionFees[feeType]; // 수수료 계산

        if (account->getBalance() < amount + fee) { // 잔액 부족 검사
            cout << "Error: Insufficient account balance for transfer. 송금 계좌의 잔액이 부족합니다." << endl;
            return;
        }

        account->deductFunds(amount + fee); // 송금 계좌에서 금액 차감
        destinationAccount->addFunds(amount); // 대상 계좌에 금액 추가
        cout << "Transfer successful! 거래 완료! Amount 금액 : " << amount << " KRW. Fee applied 거래 수수료 : " << fee << " KRW.\n";

    }
};


class ATM {
private:
    string serialNumber;                // ATM 시리얼 번호
    string type;                        // ATM 유형 (Single/Multi-Bank)
    bool bilingualSupport;              // 다국어 지원 여부
    unordered_map<int, int> cashInventory; // 현금 보유량
    unordered_map<string, int> transactionFees; // 거래 수수료
    bool sessionActive;                 // 현재 세션 활성화 여부
    Bank* myBank;                       // 주거래 은행
    Account* myAccount;                 // 현재 로그인된 계좌

public:
    ATM() : sessionActive(false), myBank(nullptr), myAccount(nullptr) {}

    // ATM 초기화
    void setupATM() {
        while (true) {
            cout << "Enter 6-digit ATM serial number 6자리의 ATM 시리얼 번호를 입력하세요: ";
            cin >> serialNumber;
            if (serialNumber.length() == 6 && all_of(serialNumber.begin(), serialNumber.end(), ::isdigit)) {
                break;
            }
            cout << "Invalid serial number. Please enter exactly 6 numeric digits.\n";
        }

        while (true) {
            cout << "Select ATM type (1: Single Bank, 2: Multi-Bank): ";
            int atmTypeChoice;
            cin >> atmTypeChoice;
            if (atmTypeChoice == 1 || atmTypeChoice == 2) {
                type = (atmTypeChoice == 1) ? "Single Bank" : "Multi-Bank";
                cout << "ATM Type set to: " << type << endl;
                break;
            }
            cout << "Invalid selection. Please enter 1 or 2.\n";
        }

        while (true) {
            cout << "Select language setting (1: English, 2: Bilingual): ";
            int languageChoice;
            cin >> languageChoice;
            if (languageChoice == 1 || languageChoice == 2) {
                bilingualSupport = (languageChoice == 2);
                cout << (bilingualSupport ? "Bilingual support enabled.\n" : "Language set to English only.\n");
                break;
            }
            cout << "Invalid selection. Please enter 1 or 2.\n";
        }

        while (true) {
            cout << "Enter primary bank name: ";
            string bankName;
            cin >> bankName;
            if (!bankName.empty()) {
                myBank = new Bank(bankName);
                cout << "Primary bank set to: " << bankName << endl;
                break;
            }
            cout << "Bank name cannot be empty. Please enter a valid name.\n";
        }

        cout << "Enter initial cash for each denomination:" << endl;
        vector<int> denominations = {1000, 5000, 10000, 50000};
        for (int denom : denominations) {
            while (true) {
                cout << denom << " KRW bills: ";
                int count;
                cin >> count;
                if (count >= 0) {
                    cashInventory[denom] = count;
                    break;
                }
                cout << "Invalid input. Please enter a non-negative number.\n";
            }
        }

        cout << "ATM setup complete.\n";
    }

    // 세션 활성 여부 확인
    bool isSessionActive() const {
        return sessionActive;
    }

    string getSerialNumber() const {
        return serialNumber;
    }

    // 현재 ATM과 연결된 주거래 은행 반환
    Bank* getBank() const {
        return myBank;
    }

    // 현재 로그인된 계좌 반환
    Account* getAccount() const {
        return myAccount;
    }

    // 카드 삽입 및 인증
    void insert_card(const string& accountNumber, const string& password, vector<Bank*>& banks) {
        bool authenticated = false;
        for (Bank* bank : banks) { // 포인터를 사용하여 Bank 객체를 참조
            if (bank->getAccount(accountNumber) && bank->verifyPassword(accountNumber, password)) {
                sessionActive = true;
                myAccount = bank->getAccount(accountNumber);
                authenticated = true;
                cout << "Session started on ATM with serial number " << serialNumber << endl;
                break;
            }
        }

        if (!authenticated) {
            cout << "Authentication failed. Please check your account number or password." << endl;
        }
    }

    // 세션 종료
    void end_session() {
        if (sessionActive) {
            sessionActive = false;
            cout << "Session ended. Please take your card." << endl;
            myAccount = nullptr;
        } else {
            cout << "No active session to end.\n";
        }
    }

    // 입금 함수
    void deposit() {
        int denomination, count;
        cout << "Enter denomination (1000, 5000, 10000, 50000): ";
        cin >> denomination;
        cout << "Enter number of bills: ";
        cin >> count;

        Deposit deposit(myAccount, myBank, transactionFees, cashInventory, denomination, count);
        deposit.performTransaction();
    }

    // 출금 함수
    void withdraw() {
        double amount;
        cout << "Enter amount to withdraw: ";
        cin >> amount;

        Withdraw withdraw(myAccount, myBank, transactionFees, cashInventory, amount);
        withdraw.performTransaction();
    }

    // 송금 함수
    void transferFunds() {
        string destAccountNumber;
        cout << "Enter destination account number: ";
        cin >> destAccountNumber;

        Account* destAccount = myBank->getAccount(destAccountNumber);
        if (!destAccount) {
            cout << "Destination account not found.\n";
            return;
        }

        double amount;
        cout << "Enter transfer amount: ";
        cin >> amount;

        Transfer transfer(myAccount, myBank, transactionFees, destAccount, amount);
        transfer.performTransaction();
    }

    // ATM 상태와 계좌 상태 출력 (스냅샷 기능)
    void displaySnapshot() {
        cout << "\n--- ATM Snapshot ---" << endl;
        cout << "Serial Number: " << serialNumber << endl;
        cout << "Type: " << type << endl;
        cout << "Cash Inventory:\n";
        for (const auto& pair : cashInventory) {
            cout << pair.first << " KRW bills: " << pair.second << endl;
        }

        if (sessionActive && myAccount) {
            cout << "\n--- Account Snapshot ---" << endl;
            cout << "Account Number: " << myAccount->getAccountNumber() << endl;
            cout << "Owner: " << myAccount->getUserName() << endl;
            cout << "Balance: " << myAccount->getBalance() << " KRW\n";
        } else {
            cout << "No active account session.\n";
        }
    }

    // ATM 실행 (메뉴 인터페이스)
    void runATM(vector<Bank*>& banks) {
        cout << "Insert your card (account number): ";
        string accountNumber, password;
        cin >> accountNumber;
        cout << "Enter your password: ";
        cin >> password;

        insert_card(accountNumber, password, banks); // 카드 삽입 및 계좌 인증

        if (isSessionActive()) {
            int action;
            do {
                cout << "\n--- ATM Menu ---\n";
                cout << "1. Deposit\n2. Withdrawal\n3. Transfer\n4. Display Snapshot\n5. Exit\n";
                cout << "Select an action: ";
                cin >> action;

                switch (action) {
                    case 1: 
                        deposit(); // 입금 함수 호출
                        break;
                    case 2: 
                        withdraw(); // 출금 함수 호출
                        break;
                    case 3: 
                        transferFunds(); // 송금 함수 호출
                        break;
                    case 4: 
                        displaySnapshot(); // 스냅샷 출력
                        break;
                    case 5: 
                        end_session(); // 세션 종료
                        break;
                    default:
                        cout << "Invalid choice. Try again.\n";
                        break;
                }
            } while (action != 5);
        } else {
            cout << "Authentication failed. Returning to main menu.\n";
        }
    }
};


int main() {
    system("chcp 65001"); // UTF-8 인코딩 설정

    vector<ATM> atms;            // 여러 ATM 객체 저장
    vector<Bank*> banks;         // 여러 은행 객체를 포인터로 저장

    // 프로그램 시작 메시지
    cout << "Welcome to the Automated Teller Machine (ATM) System!" << endl;
    cout << "This system allows you to setup ATMs, create accounts, and perform various banking transactions.\n" << endl;

    // 1. ATM 생성
    cout << "Step 1: Setup ATMs" << endl;
    while (true) {
        ATM newATM;
        newATM.setupATM();        // ATM 초기화
        banks.push_back(newATM.getBank()); // ATM의 주거래 은행 포인터를 banks에 추가
        atms.push_back(newATM);   // ATM 추가
        cout << "ATM successfully created!\n";

        cout << "Do you want to create another ATM? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'n' || choice == 'N') break;
    }

    // 2. 계좌 개설
    cout << "\nStep 2: Create Bank Accounts" << endl;
    while (true) {
        cout << "Enter primary bank name: ";
        string bankName;
        cin >> bankName;

        // 입력한 은행 이름으로 Bank 포인터 검색
        Bank* targetBank = nullptr;
        auto it = find_if(banks.begin(), banks.end(), [&](Bank* bank) {
            return bank->getName() == bankName;
        });

        if (it != banks.end()) {
            targetBank = *it; // 이미 존재하는 은행
        } else {
            // 새 은행 생성 및 banks 벡터에 추가
            targetBank = new Bank(bankName);
            banks.push_back(targetBank);
            cout << "New bank \"" << bankName << "\" has been created.\n";
        }

        // 계좌 정보 입력
        string userName, accountNumber, password;
        double initialBalance;

        cout << "Enter your name: ";
        cin >> userName;
        cout << "Enter 12-digit account number: ";
        cin >> accountNumber;
        cout << "Enter password: ";
        cin >> password;
        cout << "Enter initial balance: ";
        cin >> initialBalance;

        // 선택된 은행에 계좌 생성
        targetBank->createAccount(userName, accountNumber, password, initialBalance);

        cout << "Do you want to create another account? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'n' || choice == 'N') break;
    }

    // 초기 세팅 완료 메시지
    cout << "\n--- Initial Setup Complete! ---\n";
    cout << "You can now use the ATMs to perform various transactions.\n";

    // 3. ATM 세션 시작
    while (true) {
        cout << "\nSelect an ATM to start a session:" << endl;
        for (size_t i = 0; i < atms.size(); ++i) {
            cout << i + 1 << ". ATM [Serial Number: " << atms[i].getSerialNumber() << "]\n";
        }

        cout << "Enter ATM number (1 to " << atms.size() << ") or 0 to exit: ";
        int atmChoice;
        cin >> atmChoice;

        if (atmChoice == 0) {
            cout << "Exiting the system. Goodbye!\n";
            break;
        }

        if (atmChoice < 1 || atmChoice > atms.size()) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        // 선택한 ATM 실행
        atms[atmChoice - 1].runATM(banks); // ATM의 세션 시작
    }

    // 동적 할당된 Bank 메모리 해제
    for (Bank* bank : banks) {
        delete bank;
    }

    return 0;
}