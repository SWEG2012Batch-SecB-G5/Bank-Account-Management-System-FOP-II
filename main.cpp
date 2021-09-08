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
// max and min length of password and name
const uInt MIN_NAME = 4, MAX_NAME = 25, MIN_PASS = 4, MAX_PASS = 8;
double maxWithdraw = 5000, minDeposit = 25;
uInt currId = 100000; // to keep track of the account 

// prototypes
uInt &validatedInput(uInt &val, string msg);
double &validatedInput(double &val, string msg);
string &validatedStringInput(string &val, string msg, uInt min, uInt max);
string &validatedNameInput(string &val, uInt min, uInt max);
char &validatedInput(char &val, string msg);
void printError(string msg);
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

// used to hold history of transaction
struct Transaction
{
    uInt account;
    Date date; //time of the transaction took place
    double amount;  // amount of transaction
    double remaining; // remaining balance after transaction 

    // prints single transaction
    void print()
    {
        cout << setw(7) << account
            << setw(12) << remaining
            << setw(12) << showpos << amount
            << noshowpos << setw(25) << date.print();
    }
};
class Account
{
    string password;
public:
    string fullName;
    uInt account_number;
    string phone;
    char sex;
    uInt age;
    AccountType type; // type of account saving or deposit
    double balance;
    Transaction transactions[10];
    uInt numOfTransaction = 0;

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
    void log(double amount)
    {
        Date now;
        Transaction newTrans = { account_number, now, amount, balance };
        transactions[numOfTransaction] = newTrans;
        numOfTransaction++;
    }

    // change password
    void setPassword(const char *pass)
    {
        if (strlen(pass) < MAX_PASS || strlen(pass) > MIN_PASS) {
            printError("The password must be at least 4 character.");
            return;
        }
        password = pass;
    }

    bool checkPassword(string pass)
    {
        if (pass == password) return true;
        printError("Incorrect password! Please try again.");
        return false;
    }

    void withdraw(double amount)
    {
        if (amount >= maxWithdraw)
            printError("You exceeded a given maximum withdraw for the day.");
        else if (amount < balance) {
            balance -= amount;
            // log to transaction
            log(-amount);

            cout << "\nSuccess! Your account has been debited with " << amount << ".\n"
                << "Your current balance is " << balance << endl;
        }
        else {
            printError("You can't withdraw. Check if you have enough balance");
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
                printError("You don't have enough balance. ");
                return;
            }

            a->balance += amount;
            balance -= amount;
            // log to transaction
            log(-amount);
            a->log(amount);

            cout << "\nSuccess! You sent " << amount << " birr to "
                << a->fullName << "(" << a->account_number << ")"
                << ".\nYour current balance is " << balance << ".\n";
        }
        else
            cout << "Cancelled!.\n";
    }

    void printHistory()
    {
        cout << endl << setw(7) << "Account"
            << setw(12) << "Remaining"
            << setw(12) << "Amount"
            << setw(22) << "Date" << endl;
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
        // increase the size of the array
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
    }

    // get account using account number
    Account *getByAccount(uInt accNumber)
    {
        int index = getIndex(accNumber);
        if (index < 0)
            return nullptr;
        return get(getIndex(accNumber));
    }

    int getIndex(uInt accNumber)
    {
        uInt index = -1;
        for (uInt i = 0; i < size; i++) {
            if (users[i].account_number == accNumber) {
                index = i;
                break;
            }
        }
        if (index != -1)
            printError("Not found");
        
        return index;
    }

    // removes from the particular index
    void remove(uInt index)
    {
        if (index == size) { // if the last elements
            size--;
        }
        else { // if the element in the middle swapping after the element
            for (uInt i = index; i < size; i++)
                users[i] = users[i + 1];
            size--;
        }
    }

    uInt getSize() { return size; }

    void print()
    {
        cout << "\n\n" << left << setw(10) << "Account"
            << setw(17) << "Name"
            << setw(5) << "Age"
            << setw(5) << "Sex"
            << setw(10) << "Type"
            << right << setw(12) << "Balance"
            << setw(15) << "Phone\n";
        printLine();
        for (uInt i = 0; i < size; i++) {
            cout << left << setw(10) << users[i].account_number
                << setw(17) << users[i].fullName
                << setw(5) << users[i].age
                << setw(5) << users[i].sex
                << setw(10) << (users[i].type == DEPOSIT ? "Deposit" : "Transfer")
                << right << setw(12) << users[i].balance
                << setw(15) << users[i].phone << '\n';
        }
        cout << endl;
    }
};


int main()
{
    // list of accounts
    Users users;
    Account *currUser = 0;

    // sample accounts for demonistration purpose only 
    Account user("Biniam Tifna", 23.45, "1234", "0985020616", 'M', 22);
    Account user1("Abebe Jared", 234.45, "1234", "0904353460", 'M', 18);
    Account user2("Isuelt Faire", 234.45, "1234", "0945353460", 'F', 18);
    users.push(user);
    users.push(user2);
    users.push(user1);

    do {
        printLine('-');
        cout << "  What do you wan't to do: \n"
            << "\t1. Login\n"
            << "\t2. Login as admin\n"
            << "\t3. Exit\n";

        uInt option;
        validatedInput(option, "your choice");
        switch (option) {
        case LOGIN: {
            printTitle("Log In");

            uInt accNumber;
            string password;

            validatedInput(accNumber, "Account Number");
            cout << "Password: "; cin >> password;
            cout << endl;

            // check if user exists
            currUser = users.getByAccount(accNumber);
            if (currUser == 0) {
                cout << "[ERROR!] Can't find user.\n" \
                    "Please make sure you have account " << accNumber << ".\n"
                    "If you don't have create a new account.\n";
                continue;
            }
            if (!(currUser->checkPassword(password))) {
                cout << "[Error!] Incorrect Password Try again!\n";
                continue;
            }
            cout << "Successfully Logged in.\n";

            cout << "Wellcome, " << currUser->fullName << "!\n";

            do {
                printOption();
                validatedInput(option, "your choice");

                switch (option) {
                case WITHDRAW: {
                    double amount;
                    amount = validatedInput(amount, "Amount");
                    currUser->withdraw(amount);
                    break;
                }
                case DEPOSIT: {
                    double amount;
                    amount = validatedInput(amount, "Amount");
                    currUser->deposit(amount);
                    break;
                }
                case BALANCE: {
                    cout << "Your current balance is " << currUser->balance << endl;
                    break;
                }
                case TRANSFER: {
                    uInt accNumber;
                    double amount;
                    validatedInput(accNumber, "Account number");
                    validatedInput(amount, "Transfer amount");
                    currUser->transfer(users.getByAccount(accNumber), amount);

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
                }
            } while (true);

            break;
        }
        case ADMIN: {
            printTitle("Log In as Admin");
            string password;
            cout << "password : "; cin >> password;

            // checks if the password is correct
            if (password != ADMIN_PASS) {
                cout << "[ERROR!] Incorrect password! Please try again.\n";
                continue;
            }
            else {
                do {
                    cout << "\nChoose:\n"
                        << "  1. Create new account\n"
                        << "  2. Get info about all accounts\n"
                        << "  3. Recent Transacations\n"
                        << "  4. Delete Account\n"
                        << "  5. Logout\n";
                    uInt option;
                    validatedInput(option, "your choice");
                    switch (option) {
                    case 1: {

                        uInt nums;
                        double initBalance;
                        char sex;
                        uInt age;
                        string fullName, password, phone;

                        validatedInput(nums, "number of accounts to be created");

                        cin.clear(); cin.ignore(10000, '\n');

                        for (uInt i = 0; i < nums; i++) {
                            cout << "Enter following info for user " << i + 1 << ": \n";
                            // accepts validated input
                            validatedNameInput(fullName, MIN_NAME, MAX_NAME);
                            validatedInput(sex, "Sex");
                            validatedInput(initBalance, "intial balance");
                            validatedInput(age, "age");
                            validatedStringInput(phone, "Phone", 10, 12);
                            validatedStringInput(password, "Password", MIN_PASS, MAX_PASS);

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
                        printTitle("Recent Transacations");

                        cout << endl << setw(7) << "Account"
                            << setw(12) << "Remaining"
                            << setw(12) << "Amount"
                            << setw(22) << "Date" << endl;
                        printLine();
                        for (uInt i = 0; i < users.getSize(); i++) {
                            Account *user = users.get(i);
                            for (uInt j = 0; j < user->numOfTransaction; j++) {
                                user->transactions[j].print();
                            }
                        }

                        break;
                    case 4: {
                        printTitle("Delete Account");

                        uInt accNumber;
                        char choice;

                        validatedInput(accNumber, "account number");
                        cout << "Are you sure you wanna delete account number "
                            << accNumber << "(y/n)? ";
                        cin >> choice;
                        if (choice == 'y') {
                            users.remove(users.getIndex(accNumber));
                        }
                        else {
                            cout << "Cancelled\n";
                        }
                        break;

                    }
                    case 5:
                        goto exit_loop2;
                        break;
                    default:
                        printError("Invalid input. Please try again.");
                    }
                } while (true);
            }
            break;
        }
        case EXIT_M:
            return 0;
            break;
        default:
            printError("Invalid input. Please try again.");
        }
    exit_loop2:
        ;

    } while (true);

    return 0;
}

// prints option
void printOption()
{
    printLine();
    cout << "What do you want to do: \n"
        << "\t1. Withdraw Money\n"
        << "\t2. Deposit\n"
        << "\t3. Check your balance\n"
        << "\t4. Transfer to another account\n"
        << "\t5. Transactions history\n"
        << "\t6. Your information\n"
        << "\t7. Logout\n"
        << "\t8. Exit\n";
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

// validates input
uInt &validatedInput(uInt &val, string msg)
{
    while (true) {
        cout << "Enter " << msg << ": ";
        if (cin >> val)
            break;
        else {
            printError(" Enter a valid integer value!");
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    return val;
}

// validates input
double &validatedInput(double &val, string msg)
{
    while (true) {
        cout << "Enter " << msg << ": ";
        if (cin >> val)
            break;
        else {
            printError("Enter a valid real value!");
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    return val;
}



// validate string 
string &validatedStringInput(string &val, string msg, uInt min, uInt max)
{
    while (true) {
        cout << "Enter " << msg << ": ";
        cin >> val;
        if (val.length() >= min && val.length() <= max)
            break;
        else {
            printError("Invalid input. The input should be "
                + to_string(min) + " to " + to_string(max) + " Long.");
        }
    }
    return val;
}
// validate string 
string &validatedNameInput(string &val, uInt min, uInt max)
{
    while (true) {
        cout << "Enter full name: ";
        getline(cin, val);
        if (val.length() >= min && val.length() <= max)
            break;
        else {
            printError("Invalid input. The Name should be between "
                + to_string(min) + " to " + to_string(max) + " Long.");
        }
    }
    return val;
}
 
char &validatedInput(char &val, string msg)
{
    while (true) {
        cout << "Enter " << msg << ": ";
        if (cin >> val) {
            if (val == 'M' || val == 'F')
                break;
            else
                printError("The sex should be either 'M' or 'F'");
        }
        else {
            printError("Enter a valid char value!");
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    return val;
}

void printError(string msg)
{
    cout << "\n[Error!] " << msg << endl;
}