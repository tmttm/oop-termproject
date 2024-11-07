#include <iostream>
using namespace std;

class Bank {
public:
    bool authorizeAccount(int accountNumber, int password) {
        return false;
    }
};

class ATM {
    private:
        static int TransactionID; // 거래 식별자
        int serial_num;
        char state;
        bool sessionActive;
        Bank* bank;

    public:
         ATM(int serial_num, Bank* bank) : serial_num(serial_num), state('n'), sessionActive(false), bank(bank) {}

        int get_serial_num() { return serial_num; }
        char get_state() { return state; }

        void insert_card(int accountNumber, int password) {
            state = 'c'; // 카드가 삽입되면 상태를 'c'로 변경

            if (bank -> authorizeAccount(accountNumber, password)) { // Bank 클래스의 계좌번호와 비밀번호 확인 --> 일치하면 세션 시작
                sessionActive = true;
                cout << "Session started on ATM with serial number " << get_serial_num() << endl;
            }

            else {
                cout << "Authentication failed. Please check your account number or password." << endl;
                state = 'n'; // 카드 빼버림ㅋ
            }
        }

        void end_session() {
            if (sessionActive) { // 종료 조건 필요 (취소 버튼, 예외 상황)
                sessionActive = false;
                cout << "Session ended." << endl;
                cout << "계좌 또는 카드 정보, 거래 유형(예금, 송금, 출금 등), 각 거래의 금액" << endl; // get_History 함수 추가
            }
        }
};
int ATM::TransactionID = 1; // 거래할 시, TransactionID ++;


int main() {
    Bank bank;                  // Bank 객체 생성
    ATM atm(123456, &bank);     // ATM 객체 생성 (serial number: 123456)

    int accountNumber, password;

    // 카드 삽입 및 인증 과정
    cout << "Please insert your card (Enter account number): ";
    cin >> accountNumber;
    cout << "Enter password: ";
    cin >> password;

    atm.insert_card(accountNumber, password); // 카드 삽입 및 세션 시작

    // 거래 수행 (거래 기능이 없으므로 간단히 세션 종료)
    if (atm.get_state() == 'c') { // 세션이 성공적으로 시작된 경우에만 세션 종료 가능
        cout << "Performing transactions..." << endl;
        atm.end_session(); // 세션 종료
    } else {
        cout << "Session could not be started due to authentication failure." << endl;
    }

    return 0;
}