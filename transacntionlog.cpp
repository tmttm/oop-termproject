
//거래 기록용
struct Transaction {
    string transaction_type; // 거래 유형 (예: "입금", "출금", "이체")
    double amount;           // 거래 금액
    string source;           // 출발 계좌
    string destination;      // 도착 계좌
};

// 거래 기록을 저장할 벡터
vector<Transaction> transaction_log;


// 거래 기록용 함수
void recordTransaction(const string& transaction_type, double amount, const string& source, const string& destination) {
    Transaction transaction = {
        transaction_type,
        amount,
        source,
        destination,
    };
    transaction_log.push_back(transaction);

    // 로그 출력
    cout << "Transaction log: " 
         << transaction.transaction_type << " | " 
         << transaction.amount << "won | " 
         << transaction.source << " -> " 
         << transaction.destination << " | " 
         << transaction.timestamp << endl;
}

// 거래 기록을 출력하는 함수
void printTransactionLog() {
    cout << "\nOverall Transaction log:" << endl;
    for (const auto& transaction : transaction_log) {
        cout << transaction.transaction_type << " | "
             << transaction.amount << "won | "
             << transaction.source << " -> "
             << transaction.destination << " | "
             << transaction.timestamp << endl;
    }
}
