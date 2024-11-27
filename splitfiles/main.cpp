#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include "Bank.h"
#include "Account.h"
#include "Trans.h"
#include "ATM.h"
using namespace std;

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
        atms[atmChoice - 1].runATM(banks); // ATM의 세션 시작
    }

    // 동적 할당된 Bank 메모리 해제
    for (Bank* bank : banks) {
        delete bank;
    }

    return 0;
}