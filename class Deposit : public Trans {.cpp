class Deposit : public Trans {
private:
    unordered_map<int, int>& cashInventory; // ATM의 현금 보유량
    int depositLimitCash = 50; // 현금 입금 한도
    double minimumCheckAmount = 100000; // 수표 입금 최소 금액

public:
    Deposit(Account* acc, Bank* bnk, unordered_map<string, int>& fees, unordered_map<int, int>& cashInv)
        : Trans(acc, bnk, fees), cashInventory(cashInv) {}

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
};
