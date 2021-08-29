#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <string>
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
    REGISTER,
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
    DELETE,
    MAIN_MENU,
    EXIT
};
const uInt WIDTH = 75;
const string ADMIN_PASS = "admin231";
double maxWithdraw = 3000;

void printOption();
void printTitle(string title);
void printLine(char fill);

struct Account
{
    string fullName;
    uInt account_number;
    string phone;
    char sex;
    uInt age;
    //AccountType type;
    double balance;
    string password;

    void withdraw(double amount)
    {
        if (amount >= maxWithdraw)
        {
            cout << "You exceeded a give maximum withdraw for the day!" << endl;
        }
        else if (amount < balance)
        {
            balance -= amount;
            // log to transactions
            cout << "Success! Your account has been debited with " << amount << ".\n"
                << "Your current balance is " << balance << endl;
        }
        else
        {
            cout << "You can't withdraw. Check if you have enough balance\n";
        }        
    }

    void deposit(double amount)
    {
        balance += amount;
        // log to transaction
        cout << "Success! Your account has been credited with " << amount
            << ". Your current balance is " << balance << endl;
    }

    void transfer(Account& a, double amount)
    {

        cout << "The account " << a.account_number
            << " belongs to " << a.fullName << ". Amount to be transfered is "
            << amount << ". confrim(y/n)? ";
        char choice; cin >> choice;
        if (choice == 'y')
        {
            if (balance > amount)
                cout << "You don't have enough balance. " << endl;
            
            a.balance += amount;
            balance -= amount;
            // log to transactions
            cout << "Success! You sent " << amount << " birr to " << a.fullName << "(" << a.account_number << ')'
                << ". Your current balance is " << balance << ".\n";
        }
        else
        {
            cout << "Cancelled!.\n";
        }
    }
    void printInfo()
    {
        printTitle(fullName + "'s Information");
        cout << "Name\t: " << fullName << endl
            << "Account Number\t: " << account_number << endl
            << "Phone\t: " << phone << endl
            << "Age\t: " << age << endl
            << "Balance\t: " << balance << endl;
    }

};

struct Transaction
{
    string account_number;
    time_t date;
    double amount;
    double remaining;
};

int main()
{
    vector<Account> customers;
    Account currUser;

    do
    {
        printLine('-');
        cout << "What do you wan't to do: \n"
            << "\t1. Login\n"
            << "\t2. Register\n"
            << "\t3. Login as admin\n"
            << "\t4. Exit\n"
            << "? ";
        uInt option;
        cin >> option;
        switch (option)
        {
        case LOGIN: {
            printTitle("Log In");
            uInt acc_number;
            string password;
            cout << "Account Number: "; cin >> acc_number;
            cout << "Password: "; cin >> password;
            uInt id = acc_number % 10000;
            printLine('-');
            try
            {
                currUser = customers.at(id);
                if (currUser.password != password)
                {
                    cout << "Incorrect Password Try again!\n";
                    continue;
                }
            } catch (out_of_range &e)
            {
                cerr << "Can't find user.\n" \
                    "Please retry with correct account number or create a new account.\n";
                continue;
            }
            cout << "Successfully Logged in.\n";

            printTitle("Wellcome, " + currUser.fullName + "!");
            do
            {
                printOption();
                cin >> option;
                switch (option)
                {
                case WITHDRAW: {
                    double amount;
                    cout << "How much? "; cin >> amount;
                    currUser.withdraw(amount);
                    break;
                }
                case DEPOSIT: {
                    double amount;
                    cout << "How much? "; cin >> amount;
                    currUser.deposit(amount);
                    break;
                }
                case BALANCE: {
                    cout << "Your current balance is " << currUser.balance << endl;
                    break;
                }
                case TRANSFER: {
                    uInt acc_number;
                    double amount;
                    cout << "Account: "; cin >> acc_number;
                    cout << "How much do you want to transfer: ";
                    cin >> amount;
                    uInt id = acc_number % 10000;
                    currUser.transfer(customers.at(id), amount);
                    
                    break;
                }
                case INFO: {
                    currUser.printInfo();
                    break;
                }
                             // TODO: case HISTORY
                case DELETE: {
                    cout << "Are you sure you want to delete your account?(y/n) ";
                    char choice; cin >> choice;
                    if (choice == 'y')
                    {
                        uInt id = currUser.account_number % 10000;
                        customers.erase(customers.begin() + id);
                        printTitle("Your account has been deleted!");
                        goto exit_loop2;
                    }
                    else
                    {
                        cout << "cancelled.\n";
                    }
                    break;
                }
                case MAIN_MENU:
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
        exit_loop2:
            ;
            break;
        }
        case REGISTER:
        {
            printTitle("Create New Account");
            cout << "Please Enter Following inforamtions to register:\n";
            Account customer;
            string first, last;
            cout << "Full Name: "; cin >> first >> last;
            customer.fullName = first + ' ' + last;
            cout << "Gender: "; cin >> customer.sex;
            cout << "Intial Balance: "; cin >> customer.balance;
            cout << "Age:2 "; cin >> customer.age;
            cout << "Phone: "; cin >> customer.phone;
            cout << "Password: "; cin >> customer.password;
            customer.account_number = 10000 + customers.size();
            customers.push_back(customer);
            
            cout << endl;
            printLine('_');
            cout << "\nSuccessfully registered! your account number is "
                << customer.account_number << "\n";
            continue;
            break;
        }
        case ADMIN: {
            printTitle("Log In as Admin");
            string password;
            cout << "password : "; cin >> password;
            if (password != ADMIN_PASS)
            {
                cout << "Incorrect password! Please try again.\n";
                continue;
            }

            printTitle("Wellcome, Admin");
            
            // show all accounts
            cout << "Account\tName\t\tAge\tSex\tBalance\n";
            for (Account a : customers)
            {
                cout << a.account_number << '\t'
                    << a.fullName << '\t'
                    << a.age << '\t'
                    << a.sex << '\t'
                    << a.balance << endl;
            }
            
            break;
        }
        case EXIT_M: {
            return 0;
            break;
        }
        default:
            cout << "Invalid Input. Please try again. \n";
            continue;
            break;
        }
    } while (true);

    return 0;
}

void printOption()
{
    printLine('-');
    cout << "What do you wan't to do: \n"
        << "\t1. Withdraw Money\n"
        << "\t2. Deposit\n"
        << "\t3. Check your balance\n"
        << "\t4. Transfer to another account\n"
        << "\t5. Transactions history[TODO]\n"
        << "\t6. Your information\n"
        << "\t7. Delete account\n"
        << "\t8. Go to main menu\n"
        << "\t9. Exit\n"
        << "? ";
}

// prints centered 
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
