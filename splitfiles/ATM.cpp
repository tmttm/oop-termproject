#include "ATM.h"

ATM::ATM() : sessionActive(false), myBank(nullptr), myAccount(nullptr) {}

void ATM::setupATM() {
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
void ATM::initializeTransactionFees() {
    transactionFees["deposit_non_primary"] = 2000;
    transactionFees["deposit_primary"] = 1000;
    transactionFees["withdrawal_primary"] = 1000;
    transactionFees["withdrawal_non_primary"] = 2000;
    transactionFees["transfer_primary_to_primary"] = 2000;
    transactionFees["transfer_primary_to_non_primary"] = 3000;
    transactionFees["transfer_non_primary_to_non_primary"] = 4000;
    transactionFees["cash_transfer"] = 1000;
}

bool ATM::isSessionActive() const { return sessionActive; }
string ATM::getSerialNumber() const { return serialNumber; }
Bank* ATM::getBank() const { return myBank; }
Account* ATM::getAccount() const { return myAccount; }
void ATM::insert_card(const string& accountNumber, const string& password, vector<Bank*>& banks) {
    bool authenticated = false;

    if (type == "Single Bank") {
        // Single Bank ATM: 주거래 은행의 카드만 허용
        if (myBank) {
            // 주거래 은행에서 계좌 확인 및 인증
            if (myBank->getAccount(accountNumber) && myBank->verifyPassword(accountNumber, password)) {
                sessionActive = true;
                myAccount = myBank->getAccount(accountNumber);
                authenticated = true;
                cout << "Session started on ATM with serial number " << serialNumber << " for Single Bank ATM.\n";
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
            if (bank->getAccount(accountNumber) && bank->verifyPassword(accountNumber, password)) {
                sessionActive = true;
                myAccount = bank->getAccount(accountNumber);
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

void ATM::end_session() {
    if (sessionActive) {
        sessionActive = false;
        cout << "Session ended. Please take your card." << endl;
        myAccount = nullptr;
        withdrawalCount = 0;
    } else {
        cout << "No active session to end.\n";
    }
}

void ATM::deposit() {
    int denomination, count;
    
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
    
    Deposit deposit(myAccount, myBank, transactionFees, cashInventory, denomination, count);
    deposit.performTransaction();
}

void ATM::withdraw() {
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
    }
}

void ATM::transferFunds() {
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

void ATM::displaySnapshot() {
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

void ATM::runATM(vector<Bank*>& banks) {
    do {
        cout << "Insert your card (account number): ";
        string accountNumber, password;
        cin >> accountNumber;
        cout << "Enter your password: ";
        cin >> password;

        insert_card(accountNumber, password, banks); // 카드 삽입 및 계좌 인증
        cout << isSessionActive() << endl; // 디버깅
    } while (!isSessionActive());

    if (isSessionActive()) {
        char action;
        do {
            cout << "\n--- ATM Menu ---\n";
            cout << "1. Deposit\n2. Withdrawal\n3. Transfer\n4. Exit\n/. Display Snapshot\n";
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
                    transferFunds(); // 송금 함수 호출
                    break;
                case '4': 
                    end_session(); // 세션 종료
                    break;
                case '/': 
                    displaySnapshot(); // 스냅샷 출력
                    break;
                default:
                    cout << "Invalid choice. Try again.\n";
            }
        } while (action != 4 || isSessionActive());
    } else {
        cout << "Authentication failed. Returning to main menu.\n";
    }
}
int ATM::withdrawalCount = 0;