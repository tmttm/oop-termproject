#include "Trans.h"

Trans::Trans(Account* acc, Bank* bnk, unordered_map<string, int>& fees)
        : account(acc), bank(bnk), transactionFees(fees) {}


Deposit::Deposit(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv)
        : Trans(acc, bnk, fees), cashInventory(cashInv) {}

string Deposit::calculateFees() {
    return (account->getBankName() == bank->getName()) ? "deposit_primary" : "deposit_non_primary";
}

void Deposit::depositCash(int denomination, int count) {
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

void Deposit::depositCheck(double amount) {
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

void Deposit::performTransaction() {
    // 현재 기능은 입금 유형에 따라 적절한 메서드 호출을 명시적으로 실행해야 함.
    cout << "Choose deposit type: \n1. Cash \n2. Check\n";
    int choice;
    cin >> choice;

    if (choice == 1) {
        int denomination, count;
        cout << "Enter denomination and count (e.g., 10000 5): ";
        cin >> denomination >> count;
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


Withdraw::Withdraw(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv, double amt)
    : Trans(acc, bnk, fees), cashInventory(cashInv), amount(amt), successful(false) {}

string Withdraw::calculateFees() {
    return (account->getBankName() == bank->getName()) ? "withdrawal_primary" : "withdrawal_non_primary";
}

void Withdraw::performTransaction() {
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

bool Withdraw::isSuccessful() const { return successful; }


Transfer::Transfer(Account* srcAcc, Bank* bnk, unordered_map<string, int>& fees, Account* destAcc, double amt)
    : Trans(srcAcc, bnk, fees), destinationAccount(destAcc), amount(amt) {}

string Transfer::calculateFees(const string& destBankName) {
    if (account->getBankName() == bank->getName() && destBankName == bank->getName()) {
        return "transfer_primary_to_primary";
    } else if (account->getBankName() == bank->getName() && destBankName != bank->getName()) {
        return "transfer_primary_to_non_primary";
    } else {
        return "transfer_non_primary_to_non_primary";
    }
}

void Transfer::performTransaction() {
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