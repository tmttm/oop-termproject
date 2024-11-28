#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <fstream>
using namespace std;


ofstream fout;
fout.open("TransactionLog.txt");

// h2
class Account {
private:
    string userName; // 계좌 소유자 이름
    string accountNumber; // 계좌 번호
    string cardNumber; // 카드 번호
    string password; // 계좌 비밀번호
    double balance; // 계좌 잔액
    vector<string> transactionHistory; // 거래 내역
    string mainbank; // 소속 은행 이름
protected:
    static int transactionID; // 거래 ID

public:
    Account() : userName(""), accountNumber(""), cardNumber(""), password(""), balance(0.0), mainbank("") {}

    Account(string name, string accNumber, string cardNum, string pw, double initialFunds, string bankname)
        : userName(name), accountNumber(accNumber), cardNumber(cardNum), password(pw), balance(initialFunds), mainbank(bankname) {}

    string getUserName() const { return userName; }

    string getBankName() const { return mainbank; }

    string getAccountNumber() const { return accountNumber; }

    string getCardNumber() const { return cardNumber; }

    double getBalance() const { return balance; }

    bool verifyPassword(const string& pw) const { return password == pw; }

    void addFunds(double amount) {
        try {
            if (amount < 0) {
                throw invalid_argument("Deposit amount cannot be negative.");
            }
            balance += amount;
            
        } catch (const exception& e) {
            cout << "Deposit error: " << e.what() << endl;
        }
    }

    void deductFunds(double amount) {
        try {
            if (amount < 0) {
                throw invalid_argument("Withdrawal amount cannot be negative.");
            }
            if (balance < amount) {
                throw runtime_error("Insufficient balance.");
            }
            balance -= amount;
            
        } catch (const exception& e) {
            cout << "Withdrawal error: " << e.what() << endl;
        }
    }

    void recordTransaction(string ID, string cdnumber, string types, int amount, string info){
        transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + " " + info);
    }

    void printTransactionHistory() {
        cout << "Transaction History for " << accountNumber << ":" << endl;
        fout << "Transaction History for " << accountNumber << ":" << endl;
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

    unordered_map<string, Account> getAllAccounts() const { return accounts; }

    // 계좌 생성 (createAccount)
    void createAccount(const string& userName, const string& accountNumber, const string& cardNumber, const string& password, double initialBalance) {
        accounts[cardNumber] = Account(userName, accountNumber, cardNumber, password, initialBalance, name);
        cout << "Account successfully created: " << accountNumber << endl;
    }

    // 계좌 검색
    Account* getAccount(const string& cardNumber) {
        auto it = accounts.find(cardNumber);
        if (it != accounts.end()) {
            return &it->second;
        }
        return nullptr;
    }

    Account* getAccountByNumber(const string& accountNumber) {
        for (auto& pair : accounts) { // 모든 계좌를 순회
            if (pair.second.getAccountNumber() == accountNumber) {
                return &pair.second;
            }
        }
        return nullptr; // 찾지 못한 경우
    }    

    // 비밀번호 검증
    bool verifyPassword(const string& cardNumber, const string& password) {
        try {
            Account* account = getAccount(cardNumber);
            if (!account) {
                throw runtime_error("Account is invalid, cannot verify password.");
            }
            return account->verifyPassword(password);
        } catch (const exception& e) {
            cout << "Password verification error: " << e.what() << endl;
            return false;
        }
    }

    // 계좌 간 송금
    void transferBetweenAccounts(const string& sourceAccount, const string& destinationAccount, double amount) {
        try {
            Account* srcAcc = getAccount(sourceAccount);
            Account* destAcc = getAccount(destinationAccount);

            if (!srcAcc || !destAcc) {
                throw runtime_error("Source or destination account not found.");
            }

            if (amount <= 0) {
                throw invalid_argument("송금 금액은 0보다 커야 합니다.");
            }

            srcAcc->deductFunds(amount);
            destAcc->addFunds(amount);

            cout << "송금 완료: " << sourceAccount << "에서 " << destinationAccount << "로 " << amount << "원이 송금되었습니다." << endl;
        } catch (const exception& e) {
            cout << "송금 중 오류: " << e.what() << endl;
        }
    }

    // 모든 계좌 정보 출력
    void printAllAccounts() const {
        cout << "은행: " << name << " - 계좌 목록" << endl;
        for (const auto& pair : accounts) {
            const Account& account = pair.second;
            cout << "계좌 번호: " << account.getAccountNumber()
                 << ", 소유자: " << account.getUserName()
                 << ", 잔액: " << account.getBalance() << " 원" << endl;
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
    int depositLimitCash = 50; // 현금 입금 한도
    double minimumCheckAmount = 100000; // 수표 입금 최소 금액

public:
    Deposit(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, int denom, int cnt)
        : Trans(acc, bnk, fees), cashInventory(cashInv), denomination(denom), count(cnt) {}

    string calculateFees() {
        return (account->getBankName() == bank->getName()) ? "deposit_primary" : "deposit_non_primary";
    }

    void depositCash(int denomination, int count) {
        if (count > depositLimitCash) {
            cout << "Error: Cash deposit limit of " << depositLimitCash << " bills exceeded." << endl;
            return;
        }

        double totalAmount = denomination * count;
        int fee = transactionFees[calculateFees()];

        if (account->getBalance() >= fee) {
            account->addFunds(totalAmount - fee);
            cashInventory[denomination] += count;
            cout << "Deposited " << totalAmount << " won (" << count << " bills of " << denomination << " won)." << endl;
            cout << "Deposit fee of " << fee << " won applied." << endl;
        } else {
            cout << "Insufficient funds for deposit fee." << endl;
        }
    }

    void depositCheck(double amount) {
        if (amount < minimumCheckAmount) {
            cout << "Error: Minimum check amount is " << minimumCheckAmount << " won." << endl;
            return;
        }

        int fee = transactionFees["deposit_primary"]; // 수표는 주 은행 수수료만 적용
        if (account->getBalance() >= fee) {
            account->addFunds(amount - fee);
            cout << "Deposited check of " << amount << " won to the account." << endl;
            cout << "Deposit fee of " << fee << " won applied." << endl;
        } else {
            cout << "Insufficient funds for check deposit fee." << endl;
        }
    }

    void performTransaction() override {
        // 현재 기능은 입금 유형에 따라 적절한 메서드 호출을 명시적으로 실행해야 함.
        cout << "Choose deposit type: \n1. Cash \n2. Check\n";
        int choice;
        cin >> choice;

        if (choice == 1) {
            while (1){
                cout << "Enter denomination (1000, 5000, 10000, 50000): ";
                cin >> denomination;
                if (denomination != 1000 && denomination != 5000 && denomination != 10000 && denomination != 50000){
                    cout << "Invalid denomination. Please enter a valid bill amount.\n";
                    continue;
                }
                cout << "Enter number of bills: ";
                cin >> count;
                break;
            }
            depositCash(denomination, count);
        } else if (choice == 2) {
            double amount;
            cout << "Enter check amount: ";
            cin >> amount;
            depositCheck(amount);
        } else {
            cout << "Invalid deposit type selected.\n";
        }
    }
};


// 출금 (Withdraw) 클래스
class Withdraw : public Trans {
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    double amount; // 출금 요청 금액
    bool successful; // 출금 성공 여부

public:
    Withdraw(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, double amt)
        : Trans(acc, bnk, fees), cashInventory(cashInv), amount(amt), successful(false) {}

    string calculateFees() {
        return (account->getBankName() == bank->getName()) ? "withdrawal_primary" : "withdrawal_non_primary";
    }

    void performTransaction() override {
        if (amount > 500000) { // 최대 출금 한도 검사
            cout << "Error: Maximum withdrawal limit is 500,000 KRW.\n";
            return;
        }

        int fee = transactionFees[calculateFees()]; // 수수료 계산
        double totalAmount = amount + fee; // 총 출금 금액(수수료 포함)

        if (account->getBalance() < totalAmount) { // 잔액 부족 검사
            cout << "Error: Insufficient account balance.\n";
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
            cout << "Error: ATM has insufficient cash to complete the withdrawal.\n";
            return;
        }

        account->deductFunds(totalAmount); // 계좌에서 총 출금 금액 차감
        successful = true; // 출금 성공 여부 설정

        cout << "Withdrawal successful! Dispensed: " << amount << " KRW. Fee applied: " << fee << " KRW.\n";
        for (const auto& pair : billsToDispense) {
            if (pair.second > 0) {
                cout << pair.second << " bills of " << pair.first << " KRW dispensed.\n";
            }
        }
    }

    bool isSuccessful() const { return successful; }
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
            cout << "Error: Insufficient account balance for transfer.\n";
            return;
        }

        account->deductFunds(amount + fee); // 송금 계좌에서 금액 차감
        destinationAccount->addFunds(amount); // 대상 계좌에 금액 추가
        cout << "Transfer successful! Amount: " << amount << " KRW. Fee applied: " << fee << " KRW.\n";
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
    static int withdrawalCount;         // 세션당 출금 횟수
    string AdminCard = "admin";         // 관리자 카드 번호

public:
    ATM() : sessionActive(false), myBank(nullptr), myAccount(nullptr) {}

    // ATM 초기화
    void setupATM() {
        while (true) {
            cout << "Enter 6-digit ATM serial number: ";
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

        initializeTransactionFees();
        cout << "ATM setup complete.\n";
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

    unordered_map<int, int> getCashInventory() const {
        return cashInventory;
    }

    void insert_card(const string& cardNumber, const string& password, vector<Bank*>& banks) {
        bool authenticated = false;

        if (type == "Single Bank") {
            // Single Bank ATM: 주거래 은행의 카드만 허용
            if (myBank) {
                // 주거래 은행에서 계좌 확인 및 인증
                if (myBank->getAccount(cardNumber) && myBank->verifyPassword(cardNumber, password)) {
                    sessionActive = true;
                    myAccount = myBank->getAccount(cardNumber);
                    authenticated = true;
                    cout << "Session started on ATM with serial number " << serialNumber << " for Single Bank ATM.\n";
                    cout << "Hello, " << myAccount->getUserName() << "!\n";
                } else {
                    cout << "Authentication failed. This ATM only accepts cards from the primary bank (" << myBank->getName() << ").\n";
                }
            } else {
                cout << "Error: This ATM does not have a valid primary bank configuration.\n";
            }
        } 
        else if (type == "Multi-Bank") {
            // Multi-Bank ATM: 모든 은행의 카드 허용
            for (Bank* bank : banks) {
                if (bank->getAccount(cardNumber) && bank->verifyPassword(cardNumber, password)) {
                    sessionActive = true;
                    myAccount = bank->getAccount(cardNumber);
                    authenticated = true;
                    cout << "Session started on ATM with serial number " << serialNumber << " for Multi-Bank ATM.\n";
                    break;
                }
            }
            
            if (!authenticated) {
                cout << "Authentication failed. Please check your account number or password.\n";
            }
        } else {
            cout << "Error: Invalid ATM type.\n";
        }
    }

    // 세션 종료
    void end_session() {
        if (sessionActive) {
            sessionActive = false;
            cout << "Session ended. Please take your card." << endl;
            myAccount = nullptr;
            withdrawalCount = 0;
        } else {
            cout << "No active session to end.\n";
        }
    }

    // 입금 함수
    void deposit() {
        int denomination, count;      

        Deposit deposit(myAccount, myBank, transactionFees, cashInventory, denomination, count);
        deposit.performTransaction();
        myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "deposit", denomination * count, "deposit");
    }

    // 출금 함수
    void withdraw() {
        if (withdrawalCount >= 3) { // 세션당 출금 횟수 제한 검사
            cout << "Error: Maximum of 3 withdrawals allowed per session. End the session to withdraw more.\n";
            end_session();
        }
        double amount;
        cout << "Enter amount to withdraw: ";
        cin >> amount;

        Withdraw withdraw(myAccount, myBank, transactionFees, cashInventory, amount);
        withdraw.performTransaction();
    
        // 출금이 성공적으로 수행된 경우에만 카운트 증가
        if (withdraw.isSuccessful()) {
            withdrawalCount++;
            myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "withdraw", amount, "withdraw");
        }
    }

    // 송금 함수
    void transferFunds(vector<Bank*>& banks) {
        int choice;
        cout << "Choose transfer type: \n1. Account Transfer\n2. Cash Transfer\n";
        cin >> choice;

        string destAccountNumber;
        cout << "Enter destination account number: ";
        cin >> destAccountNumber;

        Account* destAccount = nullptr;
        Bank* destBank = nullptr;

        for (Bank* bank : banks) {
            destAccount = bank->getAccountByNumber(destAccountNumber); // 계좌번호로 검색
            if (destAccount) {
                destBank = bank; // 목적 은행 설정
                break;
            }
        }

        if (!destAccount) {
            cout << "Destination account not found.\n";
            return;
        }
        if (choice == 1) {
            double amount;
            cout << "Enter transfer amount: ";
            cin >> amount;

            Transfer transfer(myAccount, myBank, transactionFees, destAccount, amount);
            transfer.performTransaction();
            myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "transfer", amount, "transfer");
        } else if (choice == 2) {
            int amount50000, amount10000, amount5000, amount1000;
            int amount = 0;
            cout << "Insert cash and transition fees" << endl;
            cout << "50000won bills : ";
            cin >> amount50000;
            cout << "10000won bills : ";
            cin >> amount10000;
            cout << "5000won bills : ";
            cin >> amount5000;
            cout << "1000won bills : ";
            cin >> amount1000;
            amount = amount50000*50000 + amount10000*10000 + amount5000*5000 + amount1000*1000;
            amount -= transactionFees["cash_transfer"];
            cout << "You transfered " << amount << "won to " << destAccountNumber << ". Transfer fee" << transactionFees["cash_transfer"] << "was paid." << endl;
            destAccount->addFunds(amount);
            cashInventory[50000] += amount50000;
            cashInventory[10000] += amount10000;
            cashInventory[5000] += amount5000;
            cashInventory[1000] += amount1000;
            destAccount->recordTransaction(to_string(Account::transactionID), destAccount->getCardNumber(), "transfer", amount, "transfer");
        }
        
    }

    void displaySnapshot(const vector<ATM>& atms, const vector<Bank*>& banks) {
        cout << "\n--- All ATMs Snapshot ---" << endl;

        // ATM 정보 출력
        for (const auto& atm : atms) {
            cout << "ATM [SN: " << atm.getSerialNumber() << "] remaining cash: {";
            bool first = true;
            for (const auto& pair : atm.getCashInventory()) {
                if (!first) cout << ", ";
                cout << "KRW " << pair.first << ": " << pair.second;
                first = false;
            }
            cout << "}" << endl;
        }

        cout << "\n--- All Accounts Snapshot ---" << endl;

        // 계좌 정보 출력
        for (const auto& bank : banks) {
            for (const auto& accountPair : bank->getAllAccounts()) {
                const Account& account = accountPair.second;
                cout << "Account [Bank: " << bank->getName()
                    << ", No: " << account.getAccountNumber()
                    << ", Owner: " << account.getUserName()
                    << "] balance: " << account.getBalance() << " KRW" << endl;
            }
        }
    }

    // ATM 실행 (메뉴 인터페이스)
    void runATM(vector<ATM>& atms, vector<Bank*>& banks) {
        int incorrectPasswordAttempts = 0; // 잘못된 비밀번호 시도 횟수

        while (true) {
            cout << "Insert your card (card number): ";
            string cardNumber, password;
            cin >> cardNumber;
            if (cardNumber == AdminCard){
                cout << "Admin mode activated.\n";
                while(1){
                    cout << "--- Admin Menu ---\n";
                    cout << "1. Transaction history\n2. Exit\n";
                    int adminChoice;
                    cin >> adminChoice;
                    if (adminChoice == 1){
                        for (const auto& pair : banks) {
                            unordered_map<string, Account> accounts = pair->getAllAccounts();
                            for (const auto& pair : accounts) {
                                Account account = pair.second;
                                account.printTransactionHistory();
                            }
                        }
                        break;
                    }
                    else if (adminChoice == 2){
                        break;
                    }
                    else{
                        cout << "Invalid choice. Try again.\n";
                    }
                }
            }
            else {
                cout << "Enter your password: ";
                cin >> password;

                insert_card(cardNumber, password, banks); // 카드 삽입 및 계좌 인증

                if (isSessionActive()) {
                    char action;
                    do {
                        cout << "\n--- ATM Menu ---\n";
                        cout << "1. Deposit\n2. Withdrawal\n3. Transfer\n4. Cancel\n/. Display Snapshot\n";
                        cout << "Select an action: ";
                        cin >> action;

                        switch (action) {
                            case '1': 
                                deposit(); // 입금 함수 호출
                                break;
                            case '2': 
                                withdraw(); // 출금 함수 호출
                                break;
                            case '3': 
                                transferFunds(banks); // 송금 함수 호출
                                break;
                            case '4': 
                                end_session(); // 세션 종료
                                break;
                            case '/': 
                                displaySnapshot(atms, banks); // 스냅샷 출력
                                break;
                            default:
                                cout << "Invalid choice. Try again.\n";
                        }
                    } while (action != '4' || isSessionActive());
                }
                else {
                    incorrectPasswordAttempts++;
                    cout << "Authentication failed. Attempts remaining: " << (3 - incorrectPasswordAttempts) << endl;

                    if (incorrectPasswordAttempts == 3) {
                        cout << "Too many incorrect attempts. Session aborted. Card returned.\n";
                        return;
                    }
                }
            }
        }
    }
};
int ATM::withdrawalCount = 0;


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
        string userName, accountNumber, cardNumber, password;
        double initialBalance;

        cout << "Enter your name: ";
        cin >> userName;

        // 계좌 번호 입력 및 중복 검사
        while (true) {
            cout << "Enter 12-digit account number: ";
            cin >> accountNumber;

            bool isDuplicate = false;
            for (Bank* bank : banks) {
                if (bank->getAccountByNumber(accountNumber)) {
                    isDuplicate = true;
                    break;
                }
            }

            if (accountNumber.length() != 12) {
                cout << "Error: Account number must be exactly 12 numeric digits. Try again.\n";
            }
            else if (isDuplicate) {
                cout << "Error: This account number already exists. Please try a different one.\n";
            }
            else {
                break;
            }
        }

        // 카드 번호 입력 및 중복 검사
        while (true) {
            cout << "Enter the card number: ";
            cin >> cardNumber;

            bool isDuplicate = false;
            for (Bank* bank : banks) {
                if (bank->getAccount(cardNumber)) {
                    isDuplicate = true;
                    break;
                }
            }
            
            if (isDuplicate) {
                cout << "Error: This card number already exists. Please try a different one.\n";
            }
            else {
                break;
            }
        }

        cout << "Enter password: ";
        cin >> password;

        // 초기 잔액 입력 및 유효성 검사
        while (true) {
            cout << "Enter initial balance: ";
            cin >> initialBalance;
            if (initialBalance < 0) {
                cout << "Error: Initial balance cannot be negative. Try again.\n";
            } else {
                break;
            }
        }

        // 선택된 은행에 계좌 생성
        targetBank->createAccount(userName, accountNumber, cardNumber, password, initialBalance);

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
        cin >> atmChoice; // 잘못된 타입의 입력을 받으면 무한루프 돎.

        if (atmChoice == 0) {
            cout << "Exiting the system. Goodbye!\n";
            break;
        }

        if (atmChoice < 1 || atmChoice > atms.size()) {
            cout << "Invalid choice. Try again.\n";
            continue;
        }

        // 선택한 ATM 실행
        atms[atmChoice - 1].runATM(atms, banks); // ATM의 세션 시작
    }

    // 동적 할당된 Bank 메모리 해제
    for (Bank* bank : banks) {
        delete bank;
    }
    fout.close();

    return 0;
}