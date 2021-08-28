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
    DELETE,
    MAIN_MENU,
    EXIT
};
double maxWithdraw = 3000;
struct Account
{
    string fullName;
    string account_number;
    string phone;
    char sex;
    uInt age;
    //AccountType type;
    double balance;
    string password;
};

struct Transaction
{
    string account_number;
    time_t date;
    double amount;
    double remaining;
};

void printOption();
int main()
{
    vector<Account> customers;
    Account currUser;

    do
    {
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
            cout << "Log In" << endl;
            char acc_number[10];
            string password;
            cout << "Account Number: "; cin >> acc_number;
            cout << "Password: "; cin >> password;
            uInt id = stoi(acc_number) % 10000;
            try
            {
                currUser = customers.at(id);
            } catch (out_of_range &e)
            {
                cerr << "Can't find user.\n" \
                    "Please retry with correct account number and " \
                    "password or create a new account.\n";
                continue;
            }
            cout << "Successfully Logged in.\n";
            cout << "Wellcome, " << currUser.fullName << "!\n";
            do
            {
                printOption();
                cin >> option;
                switch (option)
                {
                case WITHDRAW: {
                    double amount;
                    cout << "How much? "; cin >> amount;
                    if (amount < maxWithdraw && amount < currUser.balance)
                    {
                        currUser.balance -= amount;
                        // log to transactions
                        cout << "Success! Your account has been debited with " << amount << ".\n"
                            << "Your current balance is " << currUser.balance << endl;
                    } else
                    {
                        cout << "You can't withdraw. Check if you have enough balance\n";
                    }

                    break;
                }
                case DEPOSIT: {
                    double amount;
                    cout << "How much? "; cin >> amount;
                    currUser.balance += amount;
                    // log to transaction
                    cout << "Success! Your account has been credited with " << amount
                        << ". Your current balance is " << currUser.balance << endl;
                    break;
                }
                case BALANCE: {
                    cout << "Your current balance is " << currUser.balance << endl;
                    break;
                }
                case TRANSFER: {
                    string acc_number;
                    double amount;
                    cout << "Account: "; cin >> acc_number;
                    cout << "How much do you want to transfer: ";
                    cin >> amount;
                    uInt id = stoi(acc_number) % 10000;
                    Account acc = customers.at(id);

                    cout << "The account " << acc_number 
                        << " belongs to " << acc.fullName << ". Amount to be transfered is "
                        << amount << ". confrim(y/n)? ";
                    char choice; cin >> choice;
                    if (choice == 'y' && currUser.balance > amount)
                    {
                        acc.balance += amount;
                        currUser.balance -= amount;
                        // log to transactions
                        cout << "Success! You sent " << amount << " birr to " << acc.fullName << "(" << acc.account_number << ')'
                            << ". Your current balance is " << currUser.balance << ".\n";
                    } else if (choice == 'n')
                    {
                        cout << "Cancelled!.\n";
                    }                     else
                    {
                        cout << "You don't have enough balance. \n";
                    }
                    break;
                }
                             // TODO: case HISTORY
                case DELETE: {
                    cout << "Are you sure you want to delete your account?(y/n) ";
                    char choice;
                    cin >> choice;
                    if (choice == 'y')
                    {
                        uInt id = stoi(currUser.account_number) % 10000;
                        customers.erase(customers.begin() + id);
                        cout << "Your account has been deleted!\n";
                        goto exit_loop2;
                    } else
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
            cout << "Please Enter Following inforamtions to register:\n";
            Account customer;
            string first, last;
            cout << "Full Name: "; cin >> first >> last;
            customer.fullName = first + ' ' + last;
            cout << "Gender: "; cin >> customer.sex;
            cout << "Intial Balance: "; cin >> customer.balance;
            cout << "Age: "; cin >> customer.age;
            cout << "Phone: "; cin >> customer.phone;
            cout << "Password: "; cin >> customer.password;
            customer.account_number = to_string(10000 + (int)customers.size());

            customers.push_back(customer);
            cout << "\nSuccessfully registered! your account number is "
                << customer.account_number << "\n";
            continue;
            break;
        }
        case ADMIN: {
            cout << " ";
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
    cout << endl << string(100, '-') << endl;
        << "What do you wan't to do: \n"
        << "\t1. Withdraw Money\n"
        << "\t2. Deposit\n"
        << "\t3. Check your balance\n"
        << "\t4. Transfer to another account\n"
        << "\t5. Transactions history[TODO]\n"
        << "\t6. Delete account\n"
        << "\t7. Go to main menu\n"
        << "\t8. Exit\n"
        << "? ";
}