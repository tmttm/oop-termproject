bool checkPrimaryBank(const std::string& accountNumber, const std::string& primaryBank) {
    // 1. 계좌 정보 불러오기
    if (accountDatabase.find(accountNumber) != accountDatabase.end()) {
        Account account = accountDatabase[accountNumber];
        std::cout << "Account belongs to bank: " << account.bankName << std::endl;

        // 2. 주거래 은행 확인
        if (account.bankName == primaryBank) {
            return true;
        } else {
            return false;
        }
    } else {
        std::cerr << "Account number not found in database!" << std::endl;
        return false;
    }
}