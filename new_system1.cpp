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

ofstream fout("transaction_history.txt");

// ㅎ2
class Account {
private:
    string userName; // 계좌 소유자 이름
    string accountNumber; // 계좌 번호
    string cardNumber; // 카드 번호
    string password; // 계좌 비밀번호
    double balance; // 계좌 잔액
    vector<string> transactionHistory; // 거래 내역
    string mainbank; // 소속 은행 이름
    static int transactionID; // 거래 ID

public:
    Account() : userName(""), accountNumber(""), cardNumber(""), password(""), balance(0.0), mainbank("") {}

    Account(string name, string accNumber, string cardNum, string pw, double initialFunds, string bankname)
        : userName(name), accountNumber(accNumber), cardNumber(cardNum), password(pw), balance(initialFunds), mainbank(bankname) {}

    string getUserName() const { return userName; }

    string getBankName() const { return mainbank; }

    string getAccountNumber() const { return accountNumber; }

    string getCardNumber() const { return cardNumber; }

    static int getTransactionID() { return transactionID++; }

    double getBalance() const { return balance; }

    bool verifyPassword(const string& pw) const { return password == pw; }

    void addFunds(double amount, string languageSetting) {
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

    void deductFunds(double amount, string languageSetting) {
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

    void recordTransaction(string ID, string cdnumber, string types, double amount, string info, string languageSetting){
        if(languageSetting == "English") transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "won " + info);
        else transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "원 " + info);
    }
    
    void recordTransaction(string ID, string cdnumber, string types, double amount, string info, string accountinfo, string languageSetting){
        if(languageSetting == "English") transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "won " + info + " " + accountinfo);
        else transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "원 " + info + " " + accountinfo);
    }

    void printTransactionHistory(string languageSetting) {
        if(languageSetting == "English") {
            cout << "Transaction History for " << accountNumber << ":" << endl;
            fout << "Transaction History for " << accountNumber << ":" << endl;
        }
        else {
            cout << accountNumber << "의 거래 내역:" << endl;
            fout << accountNumber << "의 거래 내역:" << endl;
        }
        for (const string& transaction : transactionHistory) {
            cout << transaction << endl;
            fout << transaction << endl;
        }
    }
};
int Account::transactionID = 1;

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
    bool verifyPassword(const string& cardNumber, const string& password, string languageSetting) {
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
    void transferBetweenAccounts(const string& sourceAccount, const string& destinationAccount, double amount, string languageSetting) {
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

            srcAcc->deductFunds(amount, languageSetting);
            destAcc->addFunds(amount, languageSetting);

            if(languageSetting == "English") cout << "송금 완료: " << sourceAccount << "에서 " << destinationAccount << "로 " << amount << "원이 송금되었습니다." << endl;
            else cout << "Transferred " << amount << " won from " << sourceAccount << " to " << destinationAccount << "." << endl;
        } catch (const exception& e) {
            if(languageSetting == "English") cout << "Transfer error: " << e.what() << endl;
            else cout << "송금 중 오류: " << e.what() << endl;
        }
    }

    // 모든 계좌 정보 출력
    void printAllAccounts(string languageSetting) const {
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
    string languageSetting;

public:
    Trans(Account* acc, Bank* bnk, unordered_map<string, int>& fees, string language)
        : account(acc), bank(bnk), transactionFees(fees), languageSetting(language) {}

    virtual void performTransaction() = 0; // 순수 가상 함수
    string getLanguage() const { return languageSetting; }
};

// 입금 (Deposit) 클래스
class Deposit : public Trans {
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    int denomination; // 입금할 지폐의 액면가
    int count; // 입금할 지폐의 개수
    int depositLimitCash = 50; // 현금 입금 한도
    double minimumCheckAmount = 100000; // 수표 입금 최소 금액
    bool successful; // 입금 성공 여부
    double record_transaction_amount;
    int fee;

public:
    Deposit(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, int denom, int cnt, string language)
        : Trans(acc, bnk, fees, language), cashInventory(cashInv), denomination(denom), count(cnt), successful(false) {}

    string calculateFees() {
        return (account->getBankName() == bank->getName()) ? "deposit_primary" : "deposit_non_primary";
    }

    double get_RecordTransactionMoney() { return record_transaction_amount; }
    int get_fee() { return fee; }

    void depositCash(int denomination, int count) {
        if (count > depositLimitCash) {
            if(getLanguage() == "English") cout << "Error: Cash deposit limit of " << depositLimitCash << " bills exceeded." << endl;
            else cout << "현금 입금 한도인 " << depositLimitCash << "장을 초과했습니다." << endl;
            return;
        }

        double totalAmount = denomination * count;
        fee = transactionFees[calculateFees()];

        if (account->getBalance() >= fee) {
            account->addFunds(totalAmount - fee, getLanguage());
            cashInventory[denomination] += count;
            successful = true; // 입금 성공 여부 설정
            record_transaction_amount = totalAmount;

            if(getLanguage() == "English") {
                cout << "Deposited " << totalAmount << " won (" << count << " bills of " << denomination << " won)." << endl;
                cout << "Deposit fee of " << fee << " won applied." << endl;
            }
            else {
                cout << totalAmount << "원 입금 (" << denomination << "원 지폐 " << count << "장)." << endl;
                cout << "입금 수수료 " << fee << "원이 적용되었습니다." << endl;
            }
        } else {
            if(getLanguage() == "English") cout << "Insufficient funds for deposit fee." << endl;
            else cout << "입금 수수료를 지불할 잔액이 부족합니다." << endl;
        }
    }

    void depositCheck(double amount) {
        if (amount < minimumCheckAmount) {
            if(languageSetting == "English") cout << "Error: Minimum check amount is " << minimumCheckAmount << " won." << endl;
            else cout << "수표 최소 금액은 " << minimumCheckAmount << "원입니다." << endl;
            return;
        }

        fee = transactionFees["deposit_primary"]; // 수표는 주 은행 수수료만 적용
        if (account->getBalance() >= fee) {
            account->addFunds(amount - fee, getLanguage());
            successful = true; // 입금 성공 여부 설정
            record_transaction_amount = amount;

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
                if (count <= 0){
                    if(languageSetting == "English") cout << "Invalid count. Please enter a positive number.\n";
                    else cout << "잘못된 수량입니다. 양수를 입력하세요.\n";
                    continue;
                }
                if (count > 50){
                    if(languageSetting == "English") cout << "Error: Cash deposit limit of 50 bills exceeded.\n";
                    else cout << "오류: 현금 입금 한도인 50장을 초과했습니다.\n";
                    break;
                }
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

    bool isSuccessful() const { return successful; }
};

// 출금 (Withdraw) 클래스
class Withdraw : public Trans {
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    double amount; // 출금 요청 금액
    bool successful; // 출금 성공 여부
    int fee;

public:
    Withdraw(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, double amt, string language)
        : Trans(acc, bnk, fees, language), cashInventory(cashInv), amount(amt), successful(false) {}

    string calculateFees() {
        return (account->getBankName() == bank->getName()) ? "withdrawal_primary" : "withdrawal_non_primary";
    }

    int get_fee() { return fee; }

    void performTransaction() override {
        if (amount > 500000) { // 최대 출금 한도 검사
            if(languageSetting == "English") cout << "Error: Maximum withdrawal limit is 500,000 KRW.\n";
            else cout << "최대 출금 한도는 500,000원입니다.\n";
            return;
        }

        fee = transactionFees[calculateFees()]; // 수수료 계산
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

        account->deductFunds(totalAmount, getLanguage()); // 계좌에서 총 출금 금액 차감
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
    bool successful; // 송금 성공 여부
    int fee;

public:
    Transfer(Account* srcAcc, Bank* bnk, unordered_map<string, int>& fees, Account* destAcc, double amt, string language)
        : Trans(srcAcc, bnk, fees, language), destinationAccount(destAcc), amount(amt), successful(false) {}

    string calculateFees(const string& destBankName) {
        if (account->getBankName() == bank->getName() && destBankName == bank->getName()) {
            return "transfer_primary_to_primary";
        } else if (account->getBankName() == bank->getName() && destBankName != bank->getName()) {
            return "transfer_primary_to_non_primary";
        } else {
            return "transfer_non_primary_to_non_primary";
        }
    }

    int get_fee() { return fee; }

    void performTransaction() override {
        string feeType = calculateFees(destinationAccount->getBankName());
        fee = transactionFees[feeType]; // 수수료 계산

        if (account->getBalance() < amount + fee) { // 잔액 부족 검사
            if(languageSetting == "English") cout << "Error: Insufficient account balance for transfer.\n";
            else cout << "송금을 위한 계좌 잔액이 부족합니다.\n";
            return;
        }

        account->deductFunds(amount + fee, getLanguage()); // 송금 계좌에서 금액 차감
        destinationAccount->addFunds(amount, getLanguage()); // 대상 계좌에 금액 추가
        successful = true; // 송금 성공 여부 설정
        if(languageSetting == "English") cout << "Transfer successful! Amount: " << amount << " KRW. Fee applied: " << fee << " KRW.\n";
        else cout << "송금 성공! 금액: " << amount << "원. 수수료: " << fee << "원.\n";
    }

    bool isSuccessful() const { return successful; }
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
    int incorrectPasswordAttempts;
    string languageSetting;

public:
    ATM() : sessionActive(false), myBank(nullptr), myAccount(nullptr) {}

    // ATM 초기화
    void setupATM() {
        setLanguage();
        while (true) {
            if(getLanguage() == "English") cout << "Enter 6-digit ATM serial number: ";
            else cout << "6자리 ATM 시리얼 번호를 입력하세요: ";
            cin >> serialNumber;
            if (serialNumber.length() == 6 && all_of(serialNumber.begin(), serialNumber.end(), ::isdigit)) {
                break;
            }
            if(getLanguage() == "English") cout << "Invalid serial number. Please enter exactly 6 numeric digits.\n";
            else cout << "잘못된 시리얼 번호입니다. 6자리 숫자를 정확히 입력하세요.\n";
        }

        while (true) {
            if(getLanguage() == "English") cout << "Select ATM type (1: Single Bank, 2: Multi-Bank): ";
            else cout << "ATM 유형을 선택하세요 (1: 단일 은행, 2: 다중 은행): ";
            int atmTypeChoice;
            cin >> atmTypeChoice;
            if (atmTypeChoice == 1 || atmTypeChoice == 2) {
                if (getLanguage() == "English"){
                    type = (atmTypeChoice == 1) ? "Single Bank" : "Multi-Bank";
                    if(getLanguage() == "English") cout << "ATM Type set to: " << type << endl;
                }
                else {
                    type = (atmTypeChoice == 1) ? "단일 은행" : "다중 은행";
                    cout << "ATM 유형이 설정되었습니다: " << type << endl;
                }
                break;
            }
            if(getLanguage() == "English") cout << "Invalid selection. Please enter 1 or 2.\n";
            else cout << "잘못된 선택입니다. 1 또는 2를 입력하세요.\n";
        }

        while (true) {
            if(getLanguage() == "English") cout << "Enter primary bank name: ";
            else cout << "기본 은행 이름을 입력하세요: ";
            string bankName;
            cin >> bankName;
            if (!bankName.empty()) {
                myBank = new Bank(bankName);
                if(getLanguage() == "English") cout << "Primary bank set to: " << bankName << endl;
                else cout << "기본 은행이 설정되었습니다: " << bankName << endl;
                break;
            }
            if(getLanguage() == "English") cout << "Bank name cannot be empty. Please enter a valid name.\n";
            else cout << "은행 이름은 비워둘 수 없습니다. 유효한 이름을 입력하세요.\n";
        }

        if(getLanguage() == "English") cout << "Enter initial cash for each denomination:" << endl;
        else cout << "각 지폐 단위의 초기 현금을 입력하세요:" << endl;
        vector<int> denominations = {1000, 5000, 10000, 50000};
        for (int denom : denominations) {
            while (true) {
                if(getLanguage() == "English") cout << denom << " KRW bills: ";
                else cout << denom << "원 지폐: ";
                int count;
                cin >> count;
                if (count >= 0) {
                    cashInventory[denom] = count;
                    break;
                }
                if(getLanguage() == "English") cout << "Invalid input. Please enter a non-negative number.\n";
                else cout << "잘못된 입력입니다. 음이 아닌 숫자를 입력하세요.\n";
            }
        }

        initializeTransactionFees();
        if(getLanguage() == "English") cout << "ATM setup complete.\n";
        else cout << "ATM 설정이 완료되었습니다.\n";
    }
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

    string getLanguage() const {
        return languageSetting;
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

    void recordTransaction(string ID, string cdnumber, string types, double amount, string info, string languageSetting){
        if(languageSetting == "English") transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "won " + info);
        else transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "원 " + info);
    }
    
    void recordTransaction(string ID, string cdnumber, string types, double amount, string info, string accountinfo, string languageSetting){
        if(languageSetting == "English") transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "won " + info + " " + accountinfo);
        else transactionHistory.push_back(ID + " : " + cdnumber + " " + types + " " + to_string(amount) + "원 " + info + " " + accountinfo);
    }

    void insert_card(const string& cardNumber, const string& password, vector<Bank*>& banks) {
        bool authenticated = false;

        if (type == "Single Bank" || type == "단일 은행") {
            if (myBank) {
                if (!myBank->getAccount(cardNumber)) {
                    // 잘못된 카드 번호
                    if (getLanguage() == "English") cout << "Authentication failed. Invalid card number. This ATM only accepts cards from the primary bank (" << myBank->getName() << ").\n";
                    else cout << "인증 실패. 잘못된 카드 번호입니다. 이 ATM은 주 은행 (" << myBank->getName() << ")의 카드만 허용합니다.\n";
                } else if (!myBank->verifyPassword(cardNumber, password, getLanguage())) {
                    // 잘못된 비밀번호
                    incorrectPasswordAttempts++;
                    if (getLanguage() == "English") cout << "Authentication failed. Incorrect password. Attempts remaining: " << (3 - incorrectPasswordAttempts) << endl;
                    else cout << "인증 실패. 비밀번호가 틀렸습니다. 남은 시도 횟수: " << (3 - incorrectPasswordAttempts) << endl;
                } else {
                    // 인증 성공
                    sessionActive = true;
                    myAccount = myBank->getAccount(cardNumber);
                    authenticated = true;
                    if (getLanguage() == "English") cout << "Session started on ATM with serial number " << serialNumber << " for Single Bank ATM.\n";
                    else cout << "ATM 시리얼 번호 " << serialNumber << "에 대한 세션이 시작되었습니다. (단일 은행 ATM)\n";
                    if (getLanguage() == "English") cout << "Hello, " << myAccount->getUserName() << "!\n";
                    else cout << myAccount->getUserName() << "님 안녕하세요!\n";
                    incorrectPasswordAttempts = 0; // 인증 성공 시 시도 횟수 초기화
                }
            } else {
                if (getLanguage() == "English") cout << "Error: This ATM does not have a valid primary bank configuration.\n";
                else cout << "오류: 이 ATM에는 유효한 주 은행 구성이 없습니다.\n";
            }
        } else if (type == "Multi-Bank" || type == "다중 은행") {
            for (Bank* bank : banks) {
                if (!bank->getAccount(cardNumber)) {
                    // 잘못된 카드 번호
                    continue;
                } else if (!bank->verifyPassword(cardNumber, password, getLanguage())) {
                    // 잘못된 비밀번호
                    incorrectPasswordAttempts++;
                    if (getLanguage() == "English") cout << "Authentication failed. Incorrect password. Attempts remaining: " << (3 - incorrectPasswordAttempts) << endl;
                    else cout << "인증 실패. 비밀번호가 틀렸습니다. 남은 시도 횟수: " << (3 - incorrectPasswordAttempts) << endl;
                    return; // 잘못된 비밀번호면 더 이상 탐색하지 않음
                } else {
                    // 인증 성공
                    sessionActive = true;
                    myAccount = bank->getAccount(cardNumber);
                    authenticated = true;
                    if (getLanguage() == "English") cout << "Session started on ATM with serial number " << serialNumber << " for Multi-Bank ATM.\n";
                    else cout << "ATM 시리얼 번호 " << serialNumber << "에 대한 세션이 시작되었습니다. (다중 은행 ATM)\n";
                    incorrectPasswordAttempts = 0; // 인증 성공 시 시도 횟수 초기화
                    break;
                }
            }

            if (!authenticated) {
                if (getLanguage() == "English") cout << "Authentication failed. Invalid card number. Please try again.\n";
                else cout << "인증 실패. 잘못된 카드 번호입니다. 다시 시도하세요.\n";
            }
        } else {
            if (getLanguage() == "English") cout << "Error: Invalid ATM type.\n";
            else cout << "오류: 잘못된 ATM 유형입니다.\n";
        }
    }

    // 세션 종료
    void end_session() {
        if (sessionActive) {
            sessionActive = false; // 세션 비활성화
            cout << "\n--- Session Summary ---\n";

            if (myAccount) {
                // 계좌 정보 출력
                if (getLanguage() == "English") {
                    cout << "Card Number: " << myAccount->getCardNumber() << "\n";
                    cout << "Account Number: " << myAccount->getAccountNumber() << "\n";
                    cout << "User Name: " << myAccount->getUserName() << "\n\n";
                    cout << "Transactions:\n";
                } else {
                    cout << "카드 번호: " << myAccount->getCardNumber() << "\n";
                    cout << "계좌 번호: " << myAccount->getAccountNumber() << "\n";
                    cout << "사용자 이름: " << myAccount->getUserName() << "\n\n";
                    cout << "거래 내역:\n";
                }

                // 세션별 거래 내역 출력
                if (transactionHistory.empty()) {
                    if (getLanguage() == "English") {
                        cout << "No transactions were completed during this session.\n";
                    } else {
                        cout << "세션 동안 완료된 거래가 없습니다.\n";
                    }
                }
                
                else {
                    for (const string& transaction : transactionHistory) {
                        cout << transaction << "\n";
                    }

                    if (getLanguage() == "English") {
                        cout << "\nTotal Transactions: " << transactionHistory.size() << "\n";
                    } else {
                        cout << "\n총 거래 수: " << transactionHistory.size() << "\n";
                    }
                }
            }

            // 세션 종료 메시지
            if (getLanguage() == "English") {
                cout << "\nThank you for using the ATM. Take your card.\n";
            } else {
                cout << "\nATM을 이용해 주셔서 감사합니다.당신의 카드를 받으세요.\n";
            }

            transactionHistory.clear();
            withdrawalCount = 0;
            myAccount = nullptr;
        }
        
        else {
            // 세션이 활성 상태가 아닌 경우
            if (getLanguage() == "English") {
                cout << "No active session to end.\n";
            } else {
                cout << "종료할 세션이 없습니다.\n";
            }
        }
    }

    // 입금 함수
    void deposit() {
        int denomination, count;

        Deposit deposit(myAccount, myBank, transactionFees, cashInventory, denomination, count, getLanguage());
        deposit.performTransaction();
        
        // 거래 성공 시
        if (deposit.isSuccessful()) {
            double amount = denomination * count;
            int ID = myAccount->getTransactionID();

            if (getLanguage() == "English") {
                myAccount->recordTransaction(to_string(ID), myAccount->getCardNumber(), "deposit", amount, "deposit", getLanguage());
                transactionHistory.push_back("Deposit: " + to_string(deposit.get_RecordTransactionMoney()) + " won" + ", Fee: " + to_string(deposit.get_fee()) + " won");
            } else {
                myAccount->recordTransaction(to_string(ID), myAccount->getCardNumber(), "입금", amount, "입금", getLanguage());
                transactionHistory.push_back("입금: " + to_string(deposit.get_RecordTransactionMoney()) + " 원" + ", 수수료: " + to_string(deposit.get_fee()) + " 원");
            }
        }
    }

    // 출금 함수
    void withdraw() {
        double amount;
        if(getLanguage() == "English") cout << "Enter amount to withdraw: ";
        else cout << "출금할 금액을 입력하세요: ";
        cin >> amount;

        Withdraw withdraw(myAccount, myBank, transactionFees, cashInventory, amount, getLanguage());
        withdraw.performTransaction();

        // 거래 성공 시
        if (withdraw.isSuccessful()) {
            withdrawalCount++;
            int ID = myAccount->getTransactionID();

            // 출금 제한 메시지 출력
            if (getLanguage() == "English") {
                cout << "You can withdraw " << (3 - withdrawalCount) << " more time(s) during this session." << endl;
            } else {
                cout << "이 세션에서 " << (3 - withdrawalCount) << "번 더 출금할 수 있습니다." << endl;
            }

            if(getLanguage() == "English") myAccount->recordTransaction(to_string(ID), myAccount->getCardNumber(), "withdraw", amount, "withdraw", getLanguage());
            else myAccount->recordTransaction(to_string(ID), myAccount->getCardNumber(), "출금", amount, "출금", getLanguage());

            // 세션별 거래 기록
            if(getLanguage() == "English") transactionHistory.push_back("Withdraw: " + to_string(amount) + " won" + ", Fee: " + to_string(withdraw.get_fee()) + " won");
            else transactionHistory.push_back("출금: " + to_string(amount) + " 원" + ", 수수료: " + to_string(withdraw.get_fee()) + " 원");
        }
    }

    // 송금 함수
    void transferFunds(vector<Bank*>& banks) {
        int choice;
        if(getLanguage() == "English") cout << "Choose transfer type: \n1. Account Transfer\n2. Cash Transfer\n";
        else cout << "송금 유형을 선택하세요: \n1. 계좌 이체\n2. 현금 이체\n";
        cin >> choice;

        string destAccountNumber;
        if(getLanguage() == "English") cout << "Enter destination account number: ";
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
            if(getLanguage() == "English") cout << "Destination account not found.\n";
            else cout << "목적지 계좌를 찾을 수 없습니다.\n";
            return;
        }
        if (myAccount->getAccountNumber() == destAccountNumber) {
            if(getLanguage() == "English") cout << "Error: Cannot transfer to the same account.\n";
            else cout << "오류: 동일한 계좌로 송금할 수 없습니다.\n";
            return;
        }

        if (choice == 1) {
            double amount;
            if(getLanguage() == "English") cout << "Enter transfer amount: ";
            else cout << "송금 금액을 입력하세요: ";
            cin >> amount;

            Transfer transfer(myAccount, myBank, transactionFees, destAccount, amount, getLanguage());
            transfer.performTransaction();

            // 거래 성공 시
            if (transfer.isSuccessful()) {
                int ID = myAccount->getTransactionID();
                if(getLanguage() == "English") {
                    myAccount->recordTransaction(to_string(ID), myAccount->getCardNumber(), "transfer", amount, "transfer to", destAccount->getAccountNumber(), getLanguage());
                    destAccount->recordTransaction(to_string(ID), destAccount->getCardNumber(), "transfer", amount, "transfer from", myAccount->getAccountNumber(), getLanguage());
                    transactionHistory.push_back("Account Transfer: " + to_string(amount) + " won to " + destAccountNumber + ", Fee: " + to_string(transfer.get_fee()) + " won");
                } else {
                    myAccount->recordTransaction(to_string(ID), myAccount->getCardNumber(), "송금", amount, "송금 에게:", destAccount->getAccountNumber(), getLanguage());
                    destAccount->recordTransaction(to_string(ID), destAccount->getCardNumber(), "송금", amount, "송금 에서:", myAccount->getAccountNumber(), getLanguage());
                    transactionHistory.push_back("계좌 송금: " + to_string(amount) + " 원 -> " + destAccountNumber + ", 수수료: " + to_string(transfer.get_fee()) + " 원");
                }
            }
        } else if (choice == 2) {
            int amount50000, amount10000, amount5000, amount1000;
            int amount = 0;
            if(getLanguage() == "English"){
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

            if (amount50000 < 0 || amount10000 < 0 || amount5000 < 0 || amount1000 < 0) {
                if(getLanguage() == "English") cout << "Invalid input. Bills cannot be negative.\n";
                else cout << "잘못된 입력입니다. 지폐 수량은 음수일 수 없습니다.\n";
            }
            else if (amount50000+amount10000+amount5000+amount1000 == 0) {
                if(getLanguage() == "English") cout << "Error: No cash inserted.\n";
                else cout << "오류: 현금이 넣어지지 않았습니다.\n";
            }
            else if (amount50000+amount10000+amount5000+amount1000 > 50) {
                if(getLanguage() == "English") cout << "Error: Maximum 50 bills can be inserted.\n";
                else cout << "오류: 최대 50장까지 넣을 수 있습니다.\n";
            }
            else if (amount > 0) {
                int ID = myAccount->getTransactionID();
                if(getLanguage() == "English") {
                    cout << "You transferred " << amount << " won to " << destAccountNumber << ". Transfer fee " << transactionFees["cash_transfer"] << " was paid." << endl;
                    transactionHistory.push_back("Cash Transfer: " + to_string(amount) + " won to " + destAccountNumber + ", Fee: " + to_string(transactionFees["cash_transfer"]) + " won");
                } else {
                    cout << amount << "원을 " << destAccountNumber << "에게 송금했습니다. 송금 수수료 " << transactionFees["cash_transfer"] << "원이 부과되었습니다." << endl;
                    transactionHistory.push_back("현금 송금: " + to_string(amount) + " 원 -> " + destAccountNumber + ", 수수료: " + to_string(transactionFees["cash_transfer"]) + " 원");
                }
                destAccount->addFunds(amount, getLanguage());
                cashInventory[50000] += amount50000;
                cashInventory[10000] += amount10000;
                cashInventory[5000] += amount5000;
                cashInventory[1000] += amount1000;
                if(getLanguage() == "English") destAccount->recordTransaction(to_string(ID), destAccount->getCardNumber(), "transfer", amount, "transfer", getLanguage());
                else destAccount->recordTransaction(to_string(ID), destAccount->getCardNumber(), "송금", amount, "송금", getLanguage());
            } else {
                if(getLanguage() == "English") cout << "Error: Insufficient amount after fees for cash transfer.\n";
                else cout << "오류: 수수료를 제외하고 남은 금액이 부족합니다.\n";
            }
        }
    }

    void displaySnapshot(const vector<ATM>& atms, const vector<Bank*>& banks) {
        if(getLanguage() == "English") cout << "\n--- All ATMs Snapshot ---" << endl;
        else cout << "\n--- 모든 ATM 스냅샷 ---" << endl;

        // ATM 정보 출력
        for (const auto& atm : atms) {
            if(getLanguage() == "English") cout << "ATM [SN: " << atm.getSerialNumber() << "] remaining cash: {";
            else cout << "ATM [SN: " << atm.getSerialNumber() << "] 남은 현금: {";
            bool first = true;
            for (const auto& pair : atm.getCashInventory()) {
                if (!first) cout << ", ";
                if(getLanguage() == "English") cout << "KRW " << pair.first << ": " << pair.second;
                else cout << "KRW " << pair.first << ": " << pair.second;
                first = false;
            }
            cout << "}" << endl;
        }

        if(getLanguage() == "English") cout << "\n--- All Accounts Snapshot ---" << endl;
        else cout << "\n--- 모든 계좌 스냅샷 ---" << endl;

        // 계좌 정보 출력
        for (const auto& bank : banks) {
            for (const auto& accountPair : bank->getAllAccounts()) {
                const Account& account = accountPair.second;
                if(getLanguage() == "English") cout << "Account [Bank: " << bank->getName()
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

    void runATM(vector<ATM>& atms, vector<Bank*>& banks) {
        incorrectPasswordAttempts = 0; // 잘못된 비밀번호 시도 횟수

        while (true) {
            if (getLanguage() == "English") cout << "Insert your card (card number): ";
            else cout << "카드를 넣어주세요 (카드 번호): ";
            string cardNumber, password;
            cin >> cardNumber;

            if (cardNumber == AdminCard) {
                if (getLanguage() == "English") cout << "Admin mode activated.\n";
                else cout << "관리자 모드가 활성화되었습니다.\n";

                while (true) {
                    if (getLanguage() == "English") {
                        cout << "--- Admin Menu ---\n";
                        cout << "1. Transaction history\n2. Exit\n";
                    } else {
                        cout << "--- 관리자 메뉴 ---\n";
                        cout << "1. 거래 내역\n2. 종료\n";
                    }

                    int adminChoice;
                    cin >> adminChoice;
                    if (adminChoice == 1) {
                        for (const auto& pair : banks) {
                            unordered_map<string, Account> accounts = pair->getAllAccounts();
                            for (const auto& pair : accounts) {
                                Account account = pair.second;
                                account.printTransactionHistory(getLanguage());
                            }
                        }
                    } else if (adminChoice == 2) {
                        return;
                        break;
                    } else {
                        if (getLanguage() == "English") cout << "Invalid choice. Try again.\n";
                        else cout << "잘못된 선택입니다. 다시 시도하세요.\n";
                    }
                }
            } else {
                if (getLanguage() == "English") cout << "Enter your password: ";
                else cout << "비밀번호를 입력하세요: ";
                cin >> password;

                insert_card(cardNumber, password, banks);

                if (incorrectPasswordAttempts >= 3) {
                    if (getLanguage() == "English") cout << "Too many incorrect attempts. Session aborted. Card returned.\n";
                    else cout << "너무 많은 잘못된 시도. 세션이 중단되었습니다. 카드가 반환됩니다.\n";
                    return; // 프로그램 종료
                }

                if (isSessionActive()) {
                    char action;
                    do {
                        if (getLanguage() == "English") {
                            cout << "\n--- ATM Menu ---\n";
                            cout << "1. Deposit\n2. Withdrawal\n3. Transfer\n4. Cancel\n/. Display Snapshot\n";
                            cout << "Select an action: ";
                        } else {
                            cout << "\n--- ATM 메뉴 ---\n";
                            cout << "1. 입금\n2. 출금\n3. 송금\n4. 취소\n/. 스냅샷 출력\n";
                            cout << "원하는 작업을 선택하세요: ";
                        }
                        cin >> action;

                        switch (action) {
                            case '1':
                                deposit();
                                break;
                            case '2':
                                if (withdrawalCount >= 3) {
                                    if (getLanguage() == "English") cout << "Error: Maximum of 3 withdrawals allowed per session. End the session to withdraw more.\n";
                                    else cout << "오류: 세션 당 최대 3회의 출금만 허용됩니다. 더 많이 출금하려면 세션을 종료하세요.\n";
                                    end_session();
                                    return;
                                } else {
                                    withdraw();
                                }
                                break;
                            case '3':
                                transferFunds(banks);
                                break;
                            case '4':
                                end_session();
                                return;
                                break;
                            case '/':
                                displaySnapshot(atms, banks);
                                break;
                            default:
                                if (getLanguage() == "English") cout << "Invalid choice. Try again.\n";
                                else cout << "잘못된 선택입니다. 다시 시도하세요.\n";
                        }
                    } while (action != '4' && isSessionActive());
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
        else if (choice == 'y' || choice == 'Y') continue;
        else{
            cout << "Invalid choice.\n";
            while(1){
                cout << "Do you want to create another ATM? (y/n): ";
                cin >> choice;
                if (choice == 'n' || choice == 'N') break;
                else if (choice == 'y' || choice == 'Y') break;
                else cout << "Invalid choice.\n";
            }
            if (choice == 'n' || choice == 'N') break;
            else if (choice == 'y' || choice == 'Y') continue;
        }
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
        else if (choice == 'y' || choice == 'Y') continue;
        else{
            cout << "Invalid choice.\n";
            while(1){
                cout << "Do you want to create another account? (y/n): ";
                cin >> choice;
                if (choice == 'n' || choice == 'N') break;
                else if (choice == 'y' || choice == 'Y') break;
                else cout << "Invalid choice.\n";
            }
            if (choice == 'n' || choice == 'N') break;
            else if (choice == 'y' || choice == 'Y') continue;
        }
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