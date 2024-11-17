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

    // deposit 수수료 계산 함수
    string depositFees() {
        if (myAccount->getBankName() == myBank->getName()) {
            return "deposit_primary";
        } else {
            return "deposit_non_primary";
        }
    }

    // withdraw 수수료 계산 함수
    string withdrawFees() {
        if (myAccount->getBankName() == myBank->getName()) {
            return "withdrawal_primary";
        } else {
            return "withdrawal_non_primary";
        }
    }

    // trasfer 수수료 계산 함수
    string transferFees(const string& destinationBankName) {
        if (myAccount->getBankName() == myBank->getName() && destinationBankName == myBank->getName()) {
            return "transfer_primary_to_primary";
        } else if (myAccount->getBankName() == myBank->getName() && destinationBankName != myBank->getName()) {
            return "transfer_primary_to_non_primary";
        } else {
            return "transfer_non_primary_to_non_primary";
        }
    }

    // cashTransfer 수수료 계산 함수
    string cashTransferFees() {
        return "cash_transfer";
    }

    //4
    void depositCash(int denomination, int count) {
        if (count > depositLimitCash) {
            cout << "Error: Cash deposit limit of " << depositLimitCash << "bills exceeded." << endl;
            return;
        }

        int fee = transactionFees[depositFees()]; // 수표는 주 은행 수수료만 적용
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


    // 5
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
        double fee = transactionFees[withdrawFees()];
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
        if (choice == 1) {
            cout << "insert cash and transition fees" << endl;
            // insert cash and transition fees
            // 금액 확인 및 전송 확인
            // 목표 계좌에 돈 입금
            // ATM 현금 보유량 증가
        }
        else if (choice == 2) {
            cout << "press the source account number" << endl;
            string sourceAccount;
            cin >> sourceAccount;
            cout << "press the amount of fund to transfer" << endl;
            double amount;
            cin >> amount;
            // 금액 확인 및 전송 확인
            // 소스 계좌에 돈 출금
            // 목표 계좌에 돈 입금
        }
        cout << "Transferred " << amount << " won from " << sourceAccount << " to " << destinationAccount << "." << endl;
    }