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

string languageSetting = "";

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
                if(languageSetting == "English") throw invalid_argument("Deposit amount cannot be negative.");
                else throw invalid_argument("입금 금액은 음수일 수 없습니다.");
            }
            balance += amount;
            
        } catch (const exception& e) {
            if(languageSetting == "English") cout << "Deposit error: " << e.what() << endl;
            else cout << "입금 오류: " << e.what() << endl;
        }
    }

    void deductFunds(double amount) {
        try {
            if (amount < 0) {
                if(languageSetting == "English") throw invalid_argument("Withdrawal amount cannot be negative.");
                else throw invalid_argument("출금 금액은 음수일 수 없습니다.");
            }
            if (balance < amount) {
                if(languageSetting == "English") throw runtime_error("Insufficient balance.");
                else throw runtime_error("잔액 부족.");
            }
            balance -= amount;
            
        } catch (const exception& e) {
            if(languageSetting == "English") cout << "Withdrawal error: " << e.what() << endl;
            else cout << "출금 오류: " << e.what() << endl;
        }
    }

    void recordTransaction(string ID, string cdnumber, string types, double amount, string info){
        if(languageSetting == "English") transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "won " + info);
        else transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "원 " + info);
    }
    
    void recordTransaction(string ID, string cdnumber, string types, double amount, string info, string accountinfo){
        if(languageSetting == "English") transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "won " + info + " " + accountinfo);
        else transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "원 " + info + " " + accountinfo);
    }

    void printTransactionHistory() {
        if(languageSetting == "English") {
            cout << "Transaction History for " << accountNumber << ":" << endl;
            fout << "Transaction History for " << accountNumber << ":" << endl;
        }
        else {
            cout << accountNumber << "의 거래 내역:" << endl;
            fout << accountNumber << "의 거래 내역:" << endl;
        }
        for (const string& transaction : transactionHistory) {
            if(languageSetting == "English") cout << transaction << endl;
            if(languageSetting == "English") fout << transaction << endl;
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
        if(languageSetting == "English") cout << "Account successfully created: " << accountNumber << endl;
        else cout << "계좌가 성공적으로 생성되었습니다: " << accountNumber << endl;
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
                if(languageSetting == "English") throw runtime_error("Account is invalid, cannot verify password.");
                else throw runtime_error("계좌가 유효하지 않아 비밀번호를 확인할 수 없습니다.");
            }
            return account->verifyPassword(password);
        } catch (const exception& e) {
            if(languageSetting == "English") cout << "Password verification error: " << e.what() << endl;
            else cout << "비밀번호 확인 오류: " << e.what() << endl;
            return false;
        }
    }

    // 계좌 간 송금
    void transferBetweenAccounts(const string& sourceAccount, const string& destinationAccount, double amount) {
        try {
            Account* srcAcc = getAccount(sourceAccount);
            Account* destAcc = getAccount(destinationAccount);

            if (!srcAcc || !destAcc) {
                if(languageSetting == "English") throw runtime_error("Source or destination account not found.");
                else throw runtime_error("출발지 또는 목적지 계좌를 찾을 수 없습니다.");
            }

            if (amount <= 0) {
                if(languageSetting == "English") throw invalid_argument("Transfer amount must be greater than 0.");
                else throw invalid_argument("송금 금액은 0보다 커야 합니다.");
            }

            srcAcc->deductFunds(amount);
            destAcc->addFunds(amount);

            if(languageSetting == "English") cout << "송금 완료: " << sourceAccount << "에서 " << destinationAccount << "로 " << amount << "원이 송금되었습니다." << endl;
            else cout << "Transferred " << amount << " won from " << sourceAccount << " to " << destinationAccount << "." << endl;
        } catch (const exception& e) {
            if(languageSetting == "English") cout << "Transfer error: " << e.what() << endl;
            else cout << "송금 중 오류: " << e.what() << endl;
        }
    }

    // 모든 계좌 정보 출력
    void printAllAccounts() const {
        if(languageSetting == "English") cout << "Bank: " << name << " - Account List" << endl;
        else cout << "은행: " << name << " - 계좌 목록" << endl;
        for (const auto& pair : accounts) {
            const Account& account = pair.second;
            if(languageSetting == "English") cout << "Account Number: " << account.getAccountNumber()
                        << ", Owner: " << account.getUserName()
                        << ", Balance: " << account.getBalance() << " 원" << endl;
            else cout << "계좌 번호: " << account.getAccountNumber()
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
            if(languageSetting == "English") cout << "Error: Cash deposit limit of " << depositLimitCash << " bills exceeded." << endl;
            else cout << "현금 입금 한도인 " << depositLimitCash << "장을 초과했습니다." << endl;
            return;
        }

        double totalAmount = denomination * count;
        int fee = transactionFees[calculateFees()];

        if (account->getBalance() >= fee) {
            account->addFunds(totalAmount - fee);
            cashInventory[denomination] += count;
            if(languageSetting == "English") {
                cout << "Deposited " << totalAmount << " won (" << count << " bills of " << denomination << " won)." << endl;
                cout << "Deposit fee of " << fee << " won applied." << endl;
            }
            else {
                cout << totalAmount << "원 입금 (" << denomination << "원 지폐 " << count << "장)." << endl;
                cout << "입금 수수료 " << fee << "원이 적용되었습니다." << endl;
            }
        } else {
            if(languageSetting == "English") cout << "Insufficient funds for deposit fee." << endl;
            else cout << "입금 수수료를 지불할 잔액이 부족합니다." << endl;
        }
    }

    void depositCheck(double amount) {
        if (amount < minimumCheckAmount) {
            if(languageSetting == "English") cout << "Error: Minimum check amount is " << minimumCheckAmount << " won." << endl;
            else cout << "수표 최소 금액은 " << minimumCheckAmount << "원입니다." << endl;
            return;
        }

        int fee = transactionFees["deposit_primary"]; // 수표는 주 은행 수수료만 적용
        if (account->getBalance() >= fee) {
            account->addFunds(amount - fee);
            if(languageSetting == "English") {
                cout << "Deposited check of " << amount << " won to the account." << endl;
                cout << "Deposit fee of " << fee << " won applied." << endl;
            }
            else {
                cout << amount << "원 수표를 계좌에 입금했습니다." << endl;
                cout << "입금 수수료 " << fee << "원이 적용되었습니다." << endl;
            }
        } else {
            if(languageSetting == "English") cout << "Insufficient funds for check deposit fee." << endl;
            else cout << "수표 입금 수수료를 지불할 잔액이 부족합니다." << endl;
        }
    }

    void performTransaction() override {
        // 현재 기능은 입금 유형에 따라 적절한 메서드 호출을 명시적으로 실행해야 함.
        if(languageSetting == "English") cout << "Choose deposit type: \n1. Cash \n2. Check\n";
        else cout << "입금 유형을 선택하세요: \n1. 현금 \n2. 수표\n";
        int choice;
        cin >> choice;

        if (choice == 1) {
            while (1){
                if(languageSetting == "English") cout << "Enter denomination (1000, 5000, 10000, 50000): ";
                else cout << "지폐 단위를 입력하세요 (1000, 5000, 10000, 50000): ";
                cin >> denomination;
                if (denomination != 1000 && denomination != 5000 && denomination != 10000 && denomination != 50000){
                    if(languageSetting == "English") cout << "Invalid denomination. Please enter a valid bill amount.\n";
                    else cout << "잘못된 지폐 단위입니다. 유효한 지폐 금액을 입력하세요.\n";
                    continue;
                }
                if(languageSetting == "English") cout << "Enter number of bills: ";
                else cout << "지폐 수량을 입력하세요: ";
                cin >> count;
                break;
            }
            depositCash(denomination, count);
        } else if (choice == 2) {
            double amount;
            if(languageSetting == "English") cout << "Enter check amount: ";
            else cout << "수표 금액을 입력하세요: ";
            cin >> amount;
            depositCheck(amount);
        } else {
            if(languageSetting == "English") cout << "Invalid deposit type selected.\n";
            else cout << "잘못된 입금 유형이 선택되었습니다.\n";
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
            if(languageSetting == "English") cout << "Error: Maximum withdrawal limit is 500,000 KRW.\n";
            else cout << "최대 출금 한도는 500,000원입니다.\n";
            return;
        }

        int fee = transactionFees[calculateFees()]; // 수수료 계산
        double totalAmount = amount + fee; // 총 출금 금액(수수료 포함)

        if (account->getBalance() < totalAmount) { // 잔액 부족 검사
            if(languageSetting == "English") cout << "Error: Insufficient account balance.\n";
            else cout << "계좌 잔액이 부족합니다.\n";
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
            if(languageSetting == "English") cout << "Error: ATM has insufficient cash to complete the withdrawal.\n";
            else cout << "ATM에 출금에 필요한 현금이 부족합니다.\n";
            return;
        }

        account->deductFunds(totalAmount); // 계좌에서 총 출금 금액 차감
        successful = true; // 출금 성공 여부 설정

        if(languageSetting == "English") cout << "Withdrawal successful! Dispensed: " << amount << " KRW. Fee applied: " << fee << " KRW.\n";
        else cout << "출금 성공! 인출 금액: " << amount << "원. 수수료: " << fee << "원.\n";
        for (const auto& pair : billsToDispense) {
            if (pair.second > 0) {
                if(languageSetting == "English") cout << pair.second << " bills of " << pair.first << " KRW dispensed.\n";
                else cout << pair.second << "장의 " << pair.first << "원 지폐가 배출되었습니다.\n";
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
            if(languageSetting == "English") cout << "Error: Insufficient account balance for transfer.\n";
            else cout << "송금을 위한 계좌 잔액이 부족합니다.\n";
            return;
        }

        account->deductFunds(amount + fee); // 송금 계좌에서 금액 차감
        destinationAccount->addFunds(amount); // 대상 계좌에 금액 추가
        if(languageSetting == "English") cout << "Transfer successful! Amount: " << amount << " KRW. Fee applied: " << fee << " KRW.\n";
        else cout << "송금 성공! 금액: " << amount << "원. 수수료: " << fee << "원.\n";
    }
};


class ATM {
private:
    string serialNumber;                // ATM 시리얼 번호
    string type;                        // ATM 유형 (Single/Multi-Bank)
    unordered_map<int, int> cashInventory; // 현금 보유량
    unordered_map<string, int> transactionFees; // 거래 수수료
    bool sessionActive;                 // 현재 세션 활성화 여부
    Bank* myBank;                       // 주거래 은행
    Account* myAccount;                 // 현재 로그인된 계좌
    static int withdrawalCount;         // 세션당 출금 횟수
    vector<string> transactionHistory;  // 거래 내역. Account의 거래 내역과는 별도로 저장된다. 이건 스냅샷 용.
    string AdminCard = "admin";         // 관리자 카드 번호

public:
    ATM() : sessionActive(false), myBank(nullptr), myAccount(nullptr) {}

    // ATM 초기화
    void setupATM() {
        while (true) {
            if(languageSetting == "English") cout << "Enter 6-digit ATM serial number: ";
            else cout << "6자리 ATM 시리얼 번호를 입력하세요: ";
            cin >> serialNumber;
            if (serialNumber.length() == 6 && all_of(serialNumber.begin(), serialNumber.end(), ::isdigit)) {
                break;
            }
            if(languageSetting == "English") cout << "Invalid serial number. Please enter exactly 6 numeric digits.\n";
            else cout << "잘못된 시리얼 번호입니다. 6자리 숫자를 정확히 입력하세요.\n";
        }

        while (true) {
            if(languageSetting == "English") cout << "Select ATM type (1: Single Bank, 2: Multi-Bank): ";
            else cout << "ATM 유형을 선택하세요 (1: 단일 은행, 2: 다중 은행): ";
            int atmTypeChoice;
            cin >> atmTypeChoice;
            if (atmTypeChoice == 1 || atmTypeChoice == 2) {
                type = (atmTypeChoice == 1) ? "Single Bank" : "Multi-Bank";
                if(languageSetting == "English") cout << "ATM Type set to: " << type << endl;
                else cout << "ATM 유형이 설정되었습니다: " << type << endl;
                break;
            }
            if(languageSetting == "English") cout << "Invalid selection. Please enter 1 or 2.\n";
            else cout << "잘못된 선택입니다. 1 또는 2를 입력하세요.\n";
        }

        while (true) {
            if(languageSetting == "English") cout << "Enter primary bank name: ";
            else cout << "기본 은행 이름을 입력하세요: ";
            string bankName;
            cin >> bankName;
            if (!bankName.empty()) {
                myBank = new Bank(bankName);
                if(languageSetting == "English") cout << "Primary bank set to: " << bankName << endl;
                else cout << "기본 은행이 설정되었습니다: " << bankName << endl;
                break;
            }
            if(languageSetting == "English") cout << "Bank name cannot be empty. Please enter a valid name.\n";
            else cout << "은행 이름은 비워둘 수 없습니다. 유효한 이름을 입력하세요.\n";
        }

        if(languageSetting == "English") cout << "Enter initial cash for each denomination:" << endl;
        else cout << "각 지폐 단위의 초기 현금을 입력하세요:" << endl;
        vector<int> denominations = {1000, 5000, 10000, 50000};
        for (int denom : denominations) {
            while (true) {
                if(languageSetting == "English") cout << denom << " KRW bills: ";
                else cout << denom << "원 지폐: ";
                int count;
                cin >> count;
                if (count >= 0) {
                    cashInventory[denom] = count;
                    break;
                }
                if(languageSetting == "English") cout << "Invalid input. Please enter a non-negative number.\n";
                else cout << "잘못된 입력입니다. 음이 아닌 숫자를 입력하세요.\n";
            }
        }

        initializeTransactionFees();
        if(languageSetting == "English") cout << "ATM setup complete.\n";
        else cout << "ATM 설정이 완료되었습니다.\n";
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
                    if(languageSetting == "English") cout << "Session started on ATM with serial number " << serialNumber << " for Single Bank ATM.\n";
                    else cout << "ATM 시리얼 번호 " << serialNumber << "에 대한 세션이 시작되었습니다. (단일 은행 ATM)\n";
                    if(languageSetting == "English") cout << "Hello, " << myAccount->getUserName() << "!\n";
                    else cout << myAccount->getUserName() << "님 안녕하세요!\n";
                } else {
                    if(languageSetting == "English") cout << "Authentication failed. This ATM only accepts cards from the primary bank (" << myBank->getName() << ").\n";
                    else cout << "인증 실패. 이 ATM은 주 은행 (" << myBank->getName() << ")의 카드만 허용합니다.\n";
                }
            } else {
                if(languageSetting == "English") cout << "Error: This ATM does not have a valid primary bank configuration.\n";
                else cout << "오류: 이 ATM에는 유효한 주 은행 구성이 없습니다.\n";
            }
        } 
        else if (type == "Multi-Bank") {
            // Multi-Bank ATM: 모든 은행의 카드 허용
            for (Bank* bank : banks) {
                if (bank->getAccount(cardNumber) && bank->verifyPassword(cardNumber, password)) {
                    sessionActive = true;
                    myAccount = bank->getAccount(cardNumber);
                    authenticated = true;
                    if(languageSetting == "English") cout << "Session started on ATM with serial number " << serialNumber << " for Multi-Bank ATM.\n";
                    else cout << "ATM 시리얼 번호 " << serialNumber << "에 대한 세션이 시작되었습니다. (다중 은행 ATM)\n";
                    break;
                }
            }
            
            if (!authenticated) {
                if(languageSetting == "English") cout << "Authentication failed. Please check your account number or password.\n";
                else cout << "인증 실패. 계좌 번호 또는 비밀번호를 확인하세요.\n";
            }
        } else {
            if(languageSetting == "English") cout << "Error: Invalid ATM type.\n";
            else cout << "오류: 잘못된 ATM 유형입니다.\n";
        }
    }

    // 세션 종료
    void end_session() {
        if (sessionActive) {
            sessionActive = false;
            if(languageSetting == "English") cout << "Session ended. Please take your card." << endl;
            else cout << "세션이 종료되었습니다. 카드를 가져가세요." << endl;
            myAccount = nullptr;
            withdrawalCount = 0;
        } else {
            if(languageSetting == "English") cout << "No active session to end.\n";
            else cout << "종료할 세션이 없습니다.\n";
        }
    }

    // 입금 함수
    void deposit() {
        int denomination, count;      

        Deposit deposit(myAccount, myBank, transactionFees, cashInventory, denomination, count);
        deposit.performTransaction();
        if(languageSetting == "English") myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "deposit", denomination * count, "deposit");
        else myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "입금", denomination * count, "입금");
        
    }

    // 출금 함수
    void withdraw() {
        if (withdrawalCount >= 3) { // 세션당 출금 횟수 제한 검사
            if(languageSetting == "English") cout << "Error: Maximum of 3 withdrawals allowed per session. End the session to withdraw more.\n";
            else cout << "오류: 세션 당 최대 3회의 출금만 허용됩니다. 더 많이 출금하려면 세션을 종료하세요.\n";
            end_session();
        }
        double amount;
        if(languageSetting == "English") cout << "Enter amount to withdraw: ";
        else cout << "출금할 금액을 입력하세요: ";
        cin >> amount;

        Withdraw withdraw(myAccount, myBank, transactionFees, cashInventory, amount);
        withdraw.performTransaction();
    
        // 출금이 성공적으로 수행된 경우에만 카운트 증가
        if (withdraw.isSuccessful()) {
            withdrawalCount++;
            if(languageSetting == "English") myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "withdraw", amount, "withdraw");
            else myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "출금", amount, "출금");
        }
    }

    // 송금 함수
    void transferFunds(vector<Bank*>& banks) {
        int choice;
        if(languageSetting == "English") cout << "Choose transfer type: \n1. Account Transfer\n2. Cash Transfer\n";
        else cout << "송금 유형을 선택하세요: \n1. 계좌 이체\n2. 현금 이체\n";
        cin >> choice;

        string destAccountNumber;
        if(languageSetting == "English") cout << "Enter destination account number: ";
        else cout << "목적지 계좌 번호를 입력하세요: ";
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
            if(languageSetting == "English") cout << "Destination account not found.\n";
            else cout << "목적지 계좌를 찾을 수 없습니다.\n";
            return;
        }
        if (choice == 1) {
            double amount;
            if(languageSetting == "English") cout << "Enter transfer amount: ";
            else cout << "송금 금액을 입력하세요: ";
            cin >> amount;

            Transfer transfer(myAccount, myBank, transactionFees, destAccount, amount);
            transfer.performTransaction();
            if(languageSetting == "English") {
                myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "transfer", amount, "transfer to", destAccount->getAccountNumber());
                destAccount->recordTransaction(to_string(Account::transactionID), destAccount->getCardNumber(), "transfer", amount, "transfer from", myAccount->getAccountNumber());
            } else {
                myAccount->recordTransaction(to_string(Account::transactionID), myAccount->getCardNumber(), "송금", amount, "송금 에게:", destAccount->getAccountNumber());
                destAccount->recordTransaction(to_string(Account::transactionID), destAccount->getCardNumber(), "송금", amount, "송금 에서:", myAccount->getAccountNumber());
            }
        } else if (choice == 2) {
            int amount50000, amount10000, amount5000, amount1000;
            int amount = 0;
            if(languageSetting == "English"){
                cout << "Insert cash and transition fees" << endl;
                cout << "50000won bills : ";
                cin >> amount50000;
                cout << "10000won bills : ";
                cin >> amount10000;
                cout << "5000won bills : ";
                cin >> amount5000;
                cout << "1000won bills : ";
                cin >> amount1000;
            }
            else{
                cout << "현금과 수수료를 넣어주세요" << endl;
                cout << "50000원 지폐 : ";
                cin >> amount50000;
                cout << "10000원 지폐 : ";
                cin >> amount10000;
                cout << "5000원 지폐 : ";
                cin >> amount5000;
                cout << "1000원 지폐 : ";
                cin >> amount1000;
            }
            amount = amount50000*50000 + amount10000*10000 + amount5000*5000 + amount1000*1000;
            amount -= transactionFees["cash_transfer"];
            if(languageSetting == "English") cout << "You transfered " << amount << "won to " << destAccountNumber << ". Transfer fee" << transactionFees["cash_transfer"] << "was paid." << endl;
            else cout << amount << "원을 " << destAccountNumber << "에게 송금했습니다. 송금 수수료" << transactionFees["cash_transfer"] << "원이 부과되었습니다." << endl;
            destAccount->addFunds(amount);
            cashInventory[50000] += amount50000;
            cashInventory[10000] += amount10000;
            cashInventory[5000] += amount5000;
            cashInventory[1000] += amount1000;
            if(languageSetting == "English") destAccount->recordTransaction(to_string(Account::transactionID), destAccount->getCardNumber(), "transfer", amount, "transfer");
            else destAccount->recordTransaction(to_string(Account::transactionID), destAccount->getCardNumber(), "송금", amount, "송금");
        }
        
    }

    void displaySnapshot(const vector<ATM>& atms, const vector<Bank*>& banks) {
        if(languageSetting == "English") cout << "\n--- All ATMs Snapshot ---" << endl;
        else cout << "\n--- 모든 ATM 스냅샷 ---" << endl;

        // ATM 정보 출력
        for (const auto& atm : atms) {
            if(languageSetting == "English") cout << "ATM [SN: " << atm.getSerialNumber() << "] remaining cash: {";
            else cout << "ATM [SN: " << atm.getSerialNumber() << "] 남은 현금: {";
            bool first = true;
            for (const auto& pair : atm.getCashInventory()) {
                if (!first) cout << ", ";
                if(languageSetting == "English") cout << "KRW " << pair.first << ": " << pair.second;
                else cout << "KRW " << pair.first << ": " << pair.second;
                first = false;
            }
            cout << "}" << endl;
        }

        if(languageSetting == "English") cout << "\n--- All Accounts Snapshot ---" << endl;
        else cout << "\n--- 모든 계좌 스냅샷 ---" << endl;

        // 계좌 정보 출력
        for (const auto& bank : banks) {
            for (const auto& accountPair : bank->getAllAccounts()) {
                const Account& account = accountPair.second;
                if(languageSetting == "English") cout << "Account [Bank: " << bank->getName()
                                                << ", No: " << account.getAccountNumber()
                                                << ", Owner: " << account.getUserName()
                                                << "] balance: " << account.getBalance() << " KRW" << endl;
                else cout << "계좌 [은행: " << bank->getName()
                        << ", 번호: " << account.getAccountNumber()
                        << ", 소유자: " << account.getUserName()
                        << "] 잔액: " << account.getBalance() << " KRW" << endl;
            }
        }
    }

    // ATM 실행 (메뉴 인터페이스)
    void runATM(vector<ATM>& atms, vector<Bank*>& banks) {
        int incorrectPasswordAttempts = 0; // 잘못된 비밀번호 시도 횟수

        while (true) {
            if(languageSetting == "English") cout << "Insert your card (card number): ";
            else cout << "카드를 넣어주세요 (카드 번호): ";
            string cardNumber, password;
            cin >> cardNumber;
            if (cardNumber == AdminCard){
                if(languageSetting == "English") cout << "Admin mode activated.\n";
                else cout << "관리자 모드가 활성화되었습니다.\n";
                while(1){
                    if(languageSetting == "English") {
                        cout << "--- Admin Menu ---\n";
                        cout << "1. Transaction history\n2. Exit\n";
                    }
                    else{
                        cout << "--- 관리자 메뉴 ---\n";
                        cout << "1. 거래 내역\n2. 종료\n";
                    }
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
                        if(languageSetting == "English") cout << "Invalid choice. Try again.\n";
                        else cout << "잘못된 선택입니다. 다시 시도하세요.\n";
                    }
                }
            }
            else {
                if(languageSetting == "English") cout << "Enter your password: ";
                else cout << "비밀번호를 입력하세요: ";
                cin >> password;

                insert_card(cardNumber, password, banks); // 카드 삽입 및 계좌 인증

                if (isSessionActive()) {
                    char action;
                    do {
                        if(languageSetting == "English") {
                            cout << "\n--- ATM Menu ---\n";
                            cout << "1. Deposit\n2. Withdrawal\n3. Transfer\n4. Cancel\n/. Display Snapshot\n";
                            cout << "Select an action: ";
                        }
                        else{
                            cout << "\n--- ATM 메뉴 ---\n";
                            cout << "1. 입금\n2. 출금\n3. 송금\n4. 취소\n/. 스냅샷 출력\n";
                            cout << "원하는 작업을 선택하세요: ";
                        }
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
                                if(languageSetting == "English") cout << "Invalid choice. Try again.\n";
                                else cout << "잘못된 선택입니다. 다시 시도하세요.\n";
                        }
                    } while (action != '4' || isSessionActive());
                }
                else {
                    incorrectPasswordAttempts++;
                    if(languageSetting == "English") cout << "Authentication failed. Attempts remaining: " << (3 - incorrectPasswordAttempts) << endl;
                    else cout << "인증 실패. 남은 시도 횟수: " << (3 - incorrectPasswordAttempts) << endl;

                    if (incorrectPasswordAttempts == 3) {
                        if(languageSetting == "English") cout << "Too many incorrect attempts. Session aborted. Card returned.\n";
                        else cout << "너무 많은 잘못된 시도. 세션이 중단되었습니다. 카드가 반환됩니다.\n";
                        return;
                    }
                }
            }
        }
    }
};
int ATM::withdrawalCount = 0;

void setLanguage(){
    while (1) {
        cout << "Select language setting (1: English, 2: Bilingual): ";
        int languageChoice;
        cin >> languageChoice;
        if (languageChoice == 1){
            languageSetting = "English";
            cout << "Language set to English.\n";
            break;
        } else if (languageChoice == 2){
            int BilanguageChoice;
            cout << "Bilingual support enabled.\n";
            cout << "Select language (1: English, 2: Korean): ";
            cin >> BilanguageChoice;
            if (BilanguageChoice == 1){
                languageSetting = "English";
                cout << "Language set to Bilingual English.\n";
                break;
            } else if (BilanguageChoice == 2){
                languageSetting = "Korean";
                cout << "언어가 한국어로 설정되었습니다.\n";
                break;
            } else {
                cout << "Invalid selection. Please enter 1 or 2.\n";
            }

            break;
        }
        cout << "Invalid selection. Please enter 1 or 2.\n";
    }
}

int main() {
    system("chcp 65001"); // UTF-8 인코딩 설정

    vector<ATM> atms;            // 여러 ATM 객체 저장
    vector<Bank*> banks;         // 여러 은행 객체를 포인터로 저장

    // 프로그램 시작 메시지
    cout << "Welcome to the Automated Teller Machine (ATM) System!" << endl;
    cout << "This system allows you to setup ATMs, create accounts, and perform various banking transactions.\n" << endl;
    setLanguage();
    // 1. ATM 생성
    if(languageSetting == "English") cout << "Step 1: Setup ATMs" << endl;
    else cout << "단계 1: ATM 설정" << endl;
    while (true) {
        ATM newATM;
        newATM.setupATM();        // ATM 초기화
        banks.push_back(newATM.getBank()); // ATM의 주거래 은행 포인터를 banks에 추가
        atms.push_back(newATM);   // ATM 추가
        if(languageSetting == "English") cout << "ATM successfully created!\n";
        else cout << "ATM이 성공적으로 생성되었습니다!\n";

        if(languageSetting == "English") cout << "Do you want to create another ATM? (y/n): ";
        else cout << "다른 ATM을 생성하시겠습니까? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'n' || choice == 'N') break;
        else if (choice == 'y' || choice == 'Y') continue;
        else{
            if(languageSetting == "English") cout << "Invalid choice.\n";
            else cout << "잘못된 선택입니다.\n";
            while(1){
                if(languageSetting == "English") cout << "Do you want to create another ATM? (y/n): ";
                else cout << "다른 ATM을 생성하시겠습니까? (y/n): ";
                cin >> choice;
                if (choice == 'n' || choice == 'N') break;
                else if (choice == 'y' || choice == 'Y') break;
                else{
                    if(languageSetting == "English") cout << "Invalid choice.\n";
                    else cout << "잘못된 선택입니다.\n";
                }
            }
            if (choice == 'n' || choice == 'N') break;
            else if (choice == 'y' || choice == 'Y') continue;
        }
    }

    // 2. 계좌 개설
    if(languageSetting == "English") cout << "\nStep 2: Create Bank Accounts" << endl;
    else cout << "\n단계 2: 은행 계좌 생성" << endl;
    while (true) {
        if(languageSetting == "English") cout << "Enter primary bank name: ";
        else cout << "기본 은행 이름을 입력하세요: ";
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
            if(languageSetting == "English") cout << "New bank \"" << bankName << "\" has been created.\n";
            else cout << "새 은행 \"" << bankName << "\"이(가) 생성되었습니다.\n";
        }

        // 계좌 정보 입력
        string userName, accountNumber, cardNumber, password;
        double initialBalance;

        if(languageSetting == "English") cout << "Enter your name: ";
        else cout << "이름을 입력하세요: ";
        cin >> userName;

        // 계좌 번호 입력 및 중복 검사
        while (true) {
            if(languageSetting == "English") cout << "Enter 12-digit account number: ";
            else cout << "12자리 계좌 번호를 입력하세요: ";
            cin >> accountNumber;

            bool isDuplicate = false;
            for (Bank* bank : banks) {
                if (bank->getAccountByNumber(accountNumber)) {
                    isDuplicate = true;
                    break;
                }
            }

            if (accountNumber.length() != 12) {
                if(languageSetting == "English") cout << "Error: Account number must be exactly 12 numeric digits. Try again.\n";
                else cout << "오류: 계좌 번호는 정확히 12자리 숫자여야 합니다. 다시 시도하세요.\n";
            }
            else if (isDuplicate) {
                if(languageSetting == "English") cout << "Error: This account number already exists. Please try a different one.\n";
                else cout << "오류: 이 계좌 번호는 이미 존재합니다. 다른 번호를 입력하세요.\n";
            }
            else {
                break;
            }
        }

        // 카드 번호 입력 및 중복 검사
        while (true) {
            if(languageSetting == "English") cout << "Enter the card number: ";
            else cout << "카드 번호를 입력하세요: ";
            cin >> cardNumber;

            bool isDuplicate = false;
            for (Bank* bank : banks) {
                if (bank->getAccount(cardNumber)) {
                    isDuplicate = true;
                    break;
                }
            }
            
            if (isDuplicate) {
                if(languageSetting == "English") cout << "Error: This card number already exists. Please try a different one.\n";
                else cout << "오류: 이 카드 번호는 이미 존재합니다. 다른 번호를 입력하세요.\n";
            }
            else {
                break;
            }
        }

        if(languageSetting == "English") cout << "Enter password: ";
        else cout << "비밀번호를 입력하세요: ";
        cin >> password;

        // 초기 잔액 입력 및 유효성 검사
        while (true) {
            if(languageSetting == "English") cout << "Enter initial balance: ";
            else cout << "초기 잔액을 입력하세요: ";
            cin >> initialBalance;
            if (initialBalance < 0) {
                if(languageSetting == "English") cout << "Error: Initial balance cannot be negative. Try again.\n";
                else: cout << "오류: 초기 잔액은 음수일 수 없습니다. 다시 시도하세요.\n";
            } else {
                break;
            }
        }

        // 선택된 은행에 계좌 생성
        targetBank->createAccount(userName, accountNumber, cardNumber, password, initialBalance);

        if(languageSetting == "English") cout << "Do you want to create another account? (y/n): ";
        else cout << "다른 계좌를 생성하시겠습니까? (y/n): ";
        char choice;
        cin >> choice;
        if (choice == 'n' || choice == 'N') break;
        else if (choice == 'y' || choice == 'Y') continue;
        else{
            if(languageSetting == "English") cout << "Invalid choice.\n";
            else cout << "잘못된 선택입니다.\n";
            while(1){
                if(languageSetting == "English") cout << "Do you want to create another account? (y/n): ";
                else cout << "다른 계좌를 생성하시겠습니까? (y/n): ";
                cin >> choice;
                if (choice == 'n' || choice == 'N') break;
                else if (choice == 'y' || choice == 'Y') break;
                else{
                    if(languageSetting == "English") cout << "Invalid choice.\n";
                    else cout << "잘못된 선택입니다.\n";
                }
            }
            if (choice == 'n' || choice == 'N') break;
            else if (choice == 'y' || choice == 'Y') continue;
        }
    }

    // 초기 세팅 완료 메시지
    if(languageSetting == "English") {
        cout << "\n--- Initial Setup Complete! ---\n";
        cout << "You can now use the ATMs to perform various transactions.\n";
    }
    else {
        cout << "\n--- 초기 설정 완료! ---\n";
        cout << "이제 ATM을 사용하여 다양한 거래를 수행할 수 있습니다.\n";
    }

    // 3. ATM 세션 시작
    while (true) {
        if(languageSetting == "English") cout << "\nSelect an ATM to start a session:" << endl;
        else cout << "\n세션을 시작할 ATM을 선택하세요:" << endl;
        for (size_t i = 0; i < atms.size(); ++i) {
            if(languageSetting == "English") cout << i + 1 << ". ATM [Serial Number: " << atms[i].getSerialNumber() << "]\n";
            else cout << i + 1 << ". ATM [시리얼 번호: " << atms[i].getSerialNumber() << "]\n";
        }

        if(languageSetting == "English") cout << "Enter ATM number (1 to " << atms.size() << ") or 0 to exit: ";
        else cout << "ATM 번호를 입력하세요 (1에서 " << atms.size() << "까지) 또는 0을 입력하여 종료: ";
        int atmChoice;
        cin >> atmChoice; // 잘못된 타입의 입력을 받으면 무한루프 돎.

        if (atmChoice == 0) {
            if(languageSetting == "English") cout << "Exiting the system. Goodbye!\n";
            else cout << "시스템을 종료합니다. 안녕히 가세요!\n";
            break;
        }

        if (atmChoice < 1 || atmChoice > atms.size()) {
            if(languageSetting == "English") cout << "Invalid choice. Try again.\n";
            else cout << "잘못된 선택입니다. 다시 시도하세요.\n";
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