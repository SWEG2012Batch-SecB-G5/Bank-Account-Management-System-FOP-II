#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <ctime>
#include <algorithm>
using namespace std;


typedef unsigned int uInt;
enum AccountType
{
    SAVING,
    DEPOSITA
};

enum MainOption
{
    LOGIN = 1,
    ADMIN,
    EXIT_M
};

enum Option
{
    WITHDRAW = 1,
    DEPOSIT,
    BALANCE,
    TRANSFER,
    HISTORY,
    INFO,
    LOGOUT,
    EXIT
};
const uInt WIDTH = 75;
const string ADMIN_PASS = "admin123";
double maxWithdraw = 5000;
uInt currId = 100000;

void printOption();
void printTitle(string title);
void printLine(char fill = '-');

struct Date
{
    time_t ltime; // seconds since epoch
    Date() { ltime = time(0); }

    string print()
    {
        return string(ctime(&ltime)).substr(3);
    }

};

// used to hold history of trasaction
struct Transaction
{
    uInt account;
    Date date; //time of the transaction took place
    double amount;  // amount of transaction
    double remaining; // remaining balance after transaction 
    char type; // for withdraw and transfer and + for deposit

    // prints single transaction
    void print()
    {
        
        cout << account << '\t'
            << type
            << amount << '\t'
            << remaining << '\t'
            << date.print();
    }
};
class Account
{
    string password;
    uInt numOfTransaction = 0;
public:
    string fullName;
    uInt account_number;
    string phone;
    char sex;
    uInt age;
    AccountType type;
    double balance;
    Transaction transactions[10];

    Account() {}

    Account(string name, double initBalance,
        string passw, string phoneNumber,
        char s, uInt agg,
        AccountType t = SAVING)
    {
        fullName = name;
        balance = initBalance;
        type = t;
        account_number = currId;
        password = passw;
        phone = phoneNumber;
        sex = s;
        age = agg;

        // log to transaction
        log(initBalance);

        currId++; // update account number id
    }
    // logs to to transaction history
    void log(double amount, char type = '+')
    {
        Date now;
        Transaction newTrans = { account_number, now, amount, balance, type };
        transactions[numOfTransaction] = newTrans;
        numOfTransaction++;
    }

    // change password
    void setPassword(const char *pass)
    {
        if (strlen(pass) < 4 || strlen(pass) > 15) {
            cout << "[ERROR!] The password must be at least 4 character.\n";
            return;
        }
        password = pass;
    }

    bool checkPassword(string pass)
    {
        if (pass == password) return true;
        cout << "[ERROR!] Incorrect password! Please try again.\n";
        return false;
    }

    void withdraw(double amount)
    {
        if (amount >= maxWithdraw)
            cout << "You exceeded a given maximum withdraw for the day!" << endl;
        else if (amount < balance) {
            balance -= amount;
            // log to transaction
            log(amount, '-');

            cout << "\nSuccess! Your account has been debited with " << amount << ".\n"
                << "Your current balance is " << balance << endl;
        }
        else {
            cout << "\nYou can't withdraw. Check if you have enough balance\n";
        }
    }

    void deposit(double amount)
    {
        balance += amount;
        // log to transaction
        log(amount);
        cout << "\nSuccess! Your account has been credited with " << amount
            << ". \nYour current balance is " << balance << endl;
    }

    void transfer(Account *a, double amount)
    {
        cout << "The account " << a->account_number
            << " belongs to " << a->fullName << ". \nAmount to be transfered is "
            << amount << ". confrim(y/n)? ";
        char choice; cin >> choice;
        if (choice == 'y') {
            if (balance < amount) {
                cout << "You don't have enough balance. " << endl;
                return;
            }

            a->balance += amount;
            balance -= amount;
            // log to transaction
            log(amount, '-');
            a->log(amount, '+');

            cout << "\nSuccess! You sent " << amount << " birr to "
                << a->fullName << "(" << a->account_number << ")"
                << ".\nYour current balance is " << balance << ".\n";
        }
        else
            cout << "Cancelled!.\n";
    }

    void printHistory()
    {
        cout << "\nAccount\tAmount\tRemaining\tDate\n";
        printLine();
        for (uInt i = 0; i < numOfTransaction; i++)
            transactions[i].print();
        cout << endl;
    }
    void printInfo()
    {
        printTitle(fullName + "'s Information");
        cout << "Name\t: " << fullName << endl
            << "ID\t: " << account_number << endl
            << "Phone\t: " << phone << endl
            << "Age\t: " << age << endl
            << "Balance\t: " << balance << endl;
    }
};


// custom dynamic array of accounts
class Users
{
    Account *users;
    uInt size, capcity;


public:
    Users()
    {
        users = new Account[1];
        capcity = 1; size = 0;
    }

    // add user to end of the array
    void push(Account user)
    {
        // this means we need to increase the size of the array
        if (capcity == size) {
            // copying array
            Account *temp = new Account[capcity * 2];
            for (uInt i = 0; i < capcity; i++) {
                temp[i] = users[i];
            }
            delete[] users;
            capcity *= 2;
            users = temp;
        }

        users[size] = user;
        size++;
    }

    // insert in the particular index
    void insert(Account user, uInt index) {}

    // get account from particular index
    Account *get(uInt index)
    {
        if (index < size)
            return &users[index];
        throw "out_of_range";
    }
    // removes from the particular index
    void remove(uInt index)
    {
        if (index == size) {
            cout << "Todo\n";
        }
    }

    uInt getSize() { return size; }

    void print()
    {
        cout << "\n\nAccount\tName\tAge\tSex\tBalance\t\tPhone\n";
        printLine();
        for (uInt i = 0; i < size; i++) {
            cout << users[i].account_number << '\t'
                << users[i].fullName << '\t'
                << users[i].age << '\t'
                << users[i].sex << '\t'
                // << (users[i].type == DEPOSIT ? "Deposit\t" : "Transfer\t")
                << users[i].balance << "\t\t"
                << users[i].phone << '\n';
        }
        cout << endl;
    }
};


int main()
{
    // list of accounts
    Users users;
    Account *currUser = 0;

    // sample accounts for demonistration only 
    Account user("Biniam", 23.45, "1234", "0972586160", 'M', 22);
    Account user1("Abebe", 234.45, "1234", "0945353460", 'M', 18);
    Account user2("Mesert", 234.45, "1234", "0945353460", 'F', 18);
    users.push(user);
    users.push(user2);
    users.push(user1);

    do {
        printLine('-');
        cout << "  What do you wan't to do: \n"
            << "\t1. Login\n"
            << "\t2. Login as admin\n"
            << "\t3. Exit\n"
            << "? ";

        uInt option;
        cin >> option;
        switch (option) {
        case LOGIN: {
            printTitle("Log In");
            uInt acc_number;
            string password;
            cout << "Account Number: "; cin >> acc_number;
            cout << "Password: "; cin >> password;
            printLine();
            try {
                currUser = users.get(acc_number % 100'000);
                if (!(currUser->checkPassword(password))) {
                    cout << "Incorrect Password Try again!\n";
                    continue;
                }
            } catch (out_of_range &e) {
                cerr << "Can't find user.\n" \
                    "Please retry with correct account number or create a new account.\n";
                continue;
            }
            cout << "Successfully Logged in.\n";

            printTitle("Wellcome, " + currUser->fullName + "!");
            do {
                printOption();
                cin >> option;
                switch (option) {
                case WITHDRAW: {
                    double amount;
                    cout << "How much? "; cin >> amount;
                    currUser->withdraw(amount);
                    break;
                }
                case DEPOSIT: {
                    double amount;
                    cout << "How much? "; cin >> amount;
                    currUser->deposit(amount);
                    break;
                }
                case BALANCE: {
                    cout << "Your current balance is " << currUser->balance << endl;
                    break;
                }
                case TRANSFER: {
                    uInt acc_number;
                    double amount;
                    cout << "Account: "; cin >> acc_number;
                    cout << "How much do you want to transfer: ";
                    cin >> amount;
                    uInt id = acc_number % 100'000;
                    currUser->transfer(users.get(id), amount);

                    break;
                }
                case INFO: {
                    currUser->printInfo();
                    break;
                }
                case HISTORY: {
                    printTitle("Transactions");
                    currUser->printHistory();
                    break;
                }
                case LOGOUT:
                    goto exit_loop2;
                    break;
                case EXIT:
                    return 0;
                    break;
                default:
                    cout << "Invalid Input. Please try again. \n";
                    continue;
                    break;
                }
            } while (true);

            break;
        }
        case ADMIN: {
            printTitle("Log In as Admin");
            string password;
            cout << "password : "; cin >> password;
            if (password != ADMIN_PASS) {
                cout << "[ERROR!] Incorrect password! Please try again.\n";
                continue;
            }
            else {
                do {
                    cout << "Choose:\n"
                        << "1. Create new account\n"
                        << "2. Get info about all accounts\n"
                        << "3. Recent Transacations\n"
                        << "4. Deactivate Account\n"
                        << "5. Logout\n? ";
                    uInt option; cin >> option;
                    switch (option) {
                    case 1: {
                        cout << "How many accounts do you wanna create: ";
                        uInt nums; cin >> nums;
                        double initBalance;
                        char sex;
                        uInt age;
                        string fullName, password, phone;
                        for (uInt i = 0; i < nums; i++) {
                            cout << "Enter following info for user " << i + 1 << ": \n";
                            cout << "Full Name: "; cin >> fullName;
                            cout << "Gender: "; cin >> sex;
                            cout << "Intial Balance: "; cin >> initBalance;
                            cout << "Age: "; cin >> age;
                            cout << "Phone: "; cin >> phone;
                            cout << "Password: "; cin >> password;
                            Account newAccount(fullName, initBalance, password, phone, sex, age);
                            users.push(newAccount);
                        }
                        break;
                    }
                    case 2:
                        printTitle("All accounts");
                        users.print();
                        break;
                    case 3:
                        cout << "Recent Transacations\n";
                        break;
                    case 4:
                        printTitle("Deactivate Account");
                        break;
                    case 5:
                        goto exit_loop2;
                        break;
                    default:
                        cout << "[ERROR!] Invalid input. Please try again.\n";
                        break;
                    }
                } while (true);
            }
            break;
        }
        case EXIT_M:
            return 0;
            break;
        default:
            cout << "[ERROR!] Invalid input. Please try again.\n";
            break;
        }

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "\n[ERROR!] You've entered wrong input. Please try again.!" << endl;
        }
        else
            break;
    exit_loop2:
        ;
    } while (true);

    return 0;
}

// prints option
void printOption()
{
    printLine();
    cout << "What do you wan't to do: \n"
        << "\t1. Withdraw Money\n"
        << "\t2. Deposit\n"
        << "\t3. Check your balance\n"
        << "\t4. Transfer to another account\n"
        << "\t5. Transactions history[TODO]\n"
        << "\t6. Your information\n"
        << "\t7. Logout\n"
        << "\t8. Exit\n"
        << "? ";
}

// prints centered title
void printTitle(string title)
{
    printLine('~');
    cout << string((WIDTH - title.length()) / 2, ' ') << title << endl;
    printLine('~');
}

// print n character long line with specfied fill character
void printLine(char fill)
{
    cout << string(WIDTH, fill) << endl;
}
