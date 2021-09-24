#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <ctime>
#include <algorithm>
using namespace std;

typedef unsigned int uInt;

enum class AccountType
{
	SAVING,
	DEPOSIT
};

enum MainOption
{
	LOGIN = 1,
	ADMIN,
	EXIT
};

enum class Option
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

enum class AdminOption
{
	NEW_ACCOUNT = 1,
	ALL_ACCOUNT,
	HISTORY,
	DELETE_ACCOUNT,
	PRINT_ACCOUNTS,
	LOGOUT,
	EXIT
};
const uInt WIDTH = 75;
// max and min length of password and name
const uInt MIN_NAME = 4, MAX_NAME = 25,
		   MIN_PASS = 4, MAX_PASS = 8,
		   MAX_PHONE = 12, MIN_PHONE = 8;
double maxWithdraw = 5000, minDeposit = 25;
const string ADMIN_PASS = "admin123";
uInt currId = 0; // to keep track of the account

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
class Transaction
{
public:
	uInt account;
	Date date;		  //time of the transaction took place
	double amount;	  // amount of transaction
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
public:
	char password[MAX_PASS];
	char fullName[MAX_NAME];
	uInt accountNumber;
	char phone[MAX_PHONE];
	char sex;
	uInt age;
	AccountType type; // type of account saving or deposit
	double balance;
	Transaction transactions[10];
	uInt numOfTransaction = 0;

	Account(){};
	Account(char *name, double initBalance,
			char *passw, char *phoneNumber,
			char s, uInt agg, uInt accNumber,
			AccountType t = AccountType::DEPOSIT);
	// logs to to transaction history
	void log(double amount);
	// change password
	void setPassword(const char *pass);
	bool checkPassword(char pass[]);
	void printInfo();
};

// prototypes
template <typename T>
T &validatedInput(T &val, string msg);
char *validatedInput(char *val, string msg, uInt min, uInt max);
char *validatedInput();
void printError(string msg);
void printHistory(vector<Transaction> &);
void printTitle(string title);
void printLine(char fill = '-');
void printAccounts(ostream &output, fstream &file);
void printTransHeader();
bool compare(Transaction &, Transaction &);
Account getAccount(uInt accNumber, iostream &file);
void newAccount(iostream &file);
void updateBalance(double amount, Account &acc, iostream &file);
void createBlankAccount(fstream& file);
void deleteAccount(fstream &file);
uInt getAccountNumber();
uInt getUserChoice();
uInt getAdminChoice();

int main()
{
	Account currUser;
	fstream file("./data/account.dat", ios::out | ios::in | ios::binary);

	if (!file)
	{
		printError("unable to open file. Make sure file 'account.dat' exists");
		exit(EXIT_FAILURE);
	}

	do
	{
		printLine('-');
		cout << "  What do you wan't to do: \n\t"
			 << "1. Login\n\t"
			 << "2. Login as admin\n\t"
			 << "3. Exit\n";

		uInt option;
		validatedInput(option, "your choice");
		switch (option)
		{
		case LOGIN:
		{
			printTitle("Log In");

			uInt accNumber = getAccountNumber();
			char password[MAX_PASS];
			cout << "password : ";
			cin >> password;

			cout << endl;

			// check if user exists
			currUser = getAccount(accNumber, file);
			if (!(currUser.checkPassword(password)))
				continue;
			cout << "Successfully Logged in.\n";
			cout << "Wellcome, " << currUser.fullName << "!\n";

			do
			{
				switch ((Option)getUserChoice())
				{
				case Option::WITHDRAW:
				{
					double amount;
					amount = validatedInput(amount, "Amount");
					if (amount >= maxWithdraw)
					{
						printError("You exceeded a given maximum withdraw for the day.");
						continue;
					}
					updateBalance(-amount, currUser, file);
					cout << "\nYour account has been credited with $" << amount
						 << ".\nYour current balance is " << currUser.balance << endl;

					break;
				}
				case Option::DEPOSIT:
				{
					double amount;
					amount = validatedInput(amount, "Amount");
					updateBalance(amount, currUser, file);

					cout << "\nYour account has been debited with $" << amount
						 << ".\nYour current balance is " << currUser.balance << endl;
					break;
				}
				case Option::BALANCE:
					cout << "Your current balance is " << currUser.balance << endl;
					break;
				case Option::TRANSFER:
				{
					Account acc = getAccount(getAccountNumber(), file);
					double amount;
					validatedInput(amount, "Transfer amount");
					if (amount >= (currUser.balance - minDeposit ))
					{
						printError("Your remaining balance should be atlease 25.");
						continue;
					}

					if (acc.accountNumber == 0)  {
						printError("Account Doesn't exits");
						continue;
					}

					// prompting for confirmition
					cout << "Account #" << acc.accountNumber << " Belongs to `"
						 << acc.fullName << "`. Do you want to continue? ";
					char ch;
					validatedInput(ch, "your choice(y/n)");

					// if cancelled
					if (ch != 'y')
						continue;

					updateBalance(-amount, currUser, file); // withdraw from current user
					updateBalance(amount, acc, file);		// deposit to another account

					cout << "\nYou have transferred $" << amount
						 << " to " << acc.fullName << "\n"
						 << "Your current balance is " << acc.balance << endl;
					break;
				}
				case Option::INFO:
					currUser.printInfo();
					break;
				case Option::HISTORY:
				{
					printTitle("Transactions");
					printTransHeader();
					vector<Transaction> history; // transaction history;
					for (uInt i = 0; i < 10; i++)
					{
						// if account exists then add to be sorted
						Transaction trans = currUser.transactions[i];
						if (trans.account != 0 && trans.account <= 100)
							history.push_back(trans);
					}
					printHistory(history);
					break;
				}
				case Option::LOGOUT:
					goto exit_loop2;
					break;
				case Option::EXIT:
					return 0;
					break;
				default:
					cout << "Invalid Input. Please try again. \n";
					continue;
				}
			} while (true);
			break;
		}
		case ADMIN:
		{
			printTitle("Log In as Admin");
			string password;
			cout << "password : ";
			cin >> password;

			// checks if the password is correct
			if (password != ADMIN_PASS)
			{
				printError("incorrect password! Please try again.");
				continue;
			}
			else
			{
				do
				{
					switch ((AdminOption)getAdminChoice())
					{
					case AdminOption::NEW_ACCOUNT:
					{
						uInt numOfAccount;
						validatedInput(numOfAccount, "number of accounts to be created");

						for (uInt i = 0; i < numOfAccount; i++)
						{
							cout << "\nEnter following info for user " << i + 1 << ": \n";
							newAccount(file);
						}
						break;
					}
					case AdminOption::ALL_ACCOUNT:
						printTitle("All accounts");
						printAccounts(cout, file);
						break;
					case AdminOption::HISTORY:
					{
						printTitle("Recent Transacations");
						printTransHeader();
						vector<Transaction> history; // transaction history;
						for (uInt i = 0; i < 100; i++)
						{
							Account user;
							file.seekg(i * sizeof(user));
							file.read((char *)&user, sizeof(user));
							for (uInt j = 0; j < 10 && user.accountNumber != 0; j++)
							{
								Transaction trans = user.transactions[j];
								if (trans.account == user.accountNumber)
									history.push_back(trans);
							}
						}
						printHistory(history);
					}
					break;
					case AdminOption::DELETE_ACCOUNT:
						printTitle("Delete Account");
						deleteAccount(file);
						break;
					case AdminOption::PRINT_ACCOUNTS:
					{
						ofstream oAccount("account.txt");
						if (!oAccount)
							cout << "Can't create `account.txt`. Please try again." << endl;

						printAccounts(oAccount, file);
						cout << "Created account.txt. Check it in your current working directory\n";
						break;
					}
					case AdminOption::LOGOUT:
						goto exit_loop2;
						break;
					case AdminOption::EXIT:
						return 0;
					default:
						printError("Invalid input. Please try again.");
					}
				} while (true);
			}
			break;
		}
		case EXIT:
			return 0;
			break;
		default:
			printError("Invalid input. Please try again.");
		}
	exit_loop2:;
		file.clear(); // reset eof indicator
	} while (true);

	return 0;
}

// prints option
uInt getUserChoice()
{
	printLine();
	uInt choice;

	cout << "What do you want to do: \n\t"
		 << "1. Withdraw Money\n\t"
		 << "2. Deposit\n\t"
		 << "3. Check your balance\n\t"
		 << "4. Transfer to another account\n\t"
		 << "5. Transactions history\n\t"
		 << "6. Your information\n\t"
		 << "7. Logout\n\t"
		 << "8. Exit\n";
	validatedInput(choice, "your choice");
	return choice;
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
template <typename T>
T &validatedInput(T &val, string msg)
{
	while (true)
	{
		cout << "Enter " << msg << ": ";
		if (cin >> val)
			break;
		else
		{
			printError("Enter a valid " + msg + " value!");
			cin.clear();
			cin.ignore(10000, '\n');
		}
	}

	return val;
}

// validate string
char *validatedInput(char *val, string msg, uInt min, uInt max)
{
	while (true)
	{
		cout << "Enter " << msg << ": ";
		cin >> val;
		if (strlen(val) >= min && strlen(val) <= max)
			break;
		else
			printError("Invalid input. The input should be " + to_string(min) +
					   " to " + to_string(max) + " character long.");
	}
	return val;
}
// validate name string
char *validatedInput(char *name)
{
	while (true)
	{
		cin.clear();
		cin.ignore(10000, '\n');

		cout << "Enter full name: ";
		cin.getline(name, MAX_NAME);

		if (strlen(name) >= MIN_NAME && strlen(name) <= MAX_NAME)
			break;
		else
			printError("Invalid input. The Name should be between " + to_string(MIN_NAME) + " to " + to_string(MAX_NAME) + " Long.");
	}
	return name;
}

void printError(string msg)
{
	cout << "\n[Error!] " << msg << endl;
}

void printAccounts(ostream &output, fstream &file)
{
	output << "\n"
		   << left << setw(10) << "Account"
		   << setw(17) << "Name"
		   << setw(5) << "Age"
		   << setw(5) << "Sex"
		   << setw(10) << "Type"
		   << right << setw(12) << "Balance"
		   << setw(15) << "Phone\n"
		   << string(WIDTH, '-') << endl;

	Account acc;

	for (uInt i = 0; i < 100; i++)
	{
		file.seekg(i * sizeof(Account));
		file.read((char *)&acc, sizeof(Account));

		if (acc.accountNumber != 0)
			output << left << setw(10) << acc.accountNumber
				   << setw(17) << acc.fullName
				   << setw(5) << acc.age
				   << setw(5) << acc.sex
				   << setw(10) << (acc.type == AccountType::DEPOSIT ? "Deposit" : "Transfer")
				   << right << setw(12) << setprecision(2) << fixed << acc.balance
				   << setw(15) << acc.phone << '\n';
	}
	cout << endl;
}

// get account for particular account number
Account getAccount(uInt accNumber, iostream &file)
{
	Account acc, blank;
	file.seekg((accNumber - 1) * sizeof(Account));
	file.read((char *)&acc, sizeof(Account));

	if (acc.accountNumber != 0)
		return acc;
	else
		return blank;
}

// creates new accounts and saves it to the file
void newAccount(iostream &file)
{
	Account acc, a;

	acc.accountNumber = getAccountNumber();
	file.seekg((acc.accountNumber - 1) * sizeof(Account));
	file.read((char *)&a, sizeof(Account));

	// accepts account informations
	validatedInput(acc.fullName);
	validatedInput(acc.sex, "Sex");
	validatedInput(acc.balance, "intial balance");
	validatedInput(acc.age, "age");
	validatedInput(acc.phone, "Phone", 10, 12);
	validatedInput(acc.password, "Password", MIN_PASS, MAX_PASS);

	acc.log(acc.balance);
	if (a.accountNumber == 0)
	{
		file.seekg((acc.accountNumber - 1) * sizeof(Account));
		file.write((char *)&acc, sizeof(Account));
		cout << "Account #" << acc.accountNumber << " is created successfully.\n";
	}
	else
		printError("account #" + to_string(acc.accountNumber) + " already exists.");
}

// update balance
void updateBalance(double amount, Account &acc, iostream &file)
{
	// update balance
	acc.balance += amount;
	// log to transaction
	acc.log(amount);

	// replace the old acc with updated
	file.seekg((acc.accountNumber - 1) * sizeof(Account));
	file.write((char *)&acc, sizeof(Account));
}

// create blank account file
void createBlankAccount(fstream &file)
{
	Account blank;

	for (size_t i = 0; i < 100; i++)
	{
		file.seekg((i - 1) * sizeof(Account));
		file.write((char *)&blank, sizeof(Account));
	}
}

// get valid account number
uInt getAccountNumber()
{
	uInt accNumber;
	do
	{
		validatedInput(accNumber, "account number");
		if (accNumber > 100 || accNumber < 1)
			printError("account number must be in range of 1 to 100. Try again.");
		else
			break;
	} while (true);

	return accNumber;
}

// delete accout
void deleteAccount(fstream &file)
{
	uInt accNumber = getAccountNumber();
	char choice;
	Account blank;

	cout << "Are you sure you wanna delete account #"
		 << accNumber << "(y/n)? ";
	cin >> choice;
	if (choice == 'y')
	{
		file.seekg((accNumber - 1) * sizeof(Account));
		file.write((char *)&blank, sizeof(Account));
		cout << "\nAccount #" << accNumber << " succesfully deleted\n";
	}
	else
		cout << "Cancelled\n";
}

Account::Account(char *name, double initBalance,
				 char *passw, char *phoneNumber,
				 char s, uInt agg, uInt accNumber,
				 AccountType t)
{
	strcpy(fullName, name);
	balance = initBalance;
	type = t;
	accountNumber = accNumber;
	strcpy(password, passw);
	strcpy(phone, phoneNumber);
	sex = s;
	age = agg;
	numOfTransaction = 0;

	for (uInt i = 0; i < 10; i++)
	{
		Transaction t;
		transactions[i] = t;
	}

	// log to transaction
	log(initBalance);

	currId++; // update account number id
}
// logs to to transaction history
void Account::log(double amount)
{
	Date now;
	Transaction newTrans = {accountNumber, now, amount, balance};
	transactions[numOfTransaction % 10] = newTrans;
	numOfTransaction++;
}

// change password
void Account::setPassword(const char *pass)
{
	if (strlen(pass) < MAX_PASS || strlen(pass) > MIN_PASS)
	{
		printError("The password must be at least 4 character.");
		return;
	}
	strcpy(password, pass);
}

// checks if the password is correct
bool Account::checkPassword(char pass[])
{
	if (strcmp(pass, password) == 0)
		return true;
	printError("Incorrect password! Please try again.");
	return false;
}

void Account::printInfo()
{
	printTitle("Your Information");
	cout << "Name\t: " << fullName << endl
		 << "ID\t: " << accountNumber << endl
		 << "Phone\t: " << phone << endl
		 << "Age\t: " << age << endl
		 << "Balance\t: " << balance << endl;
}

void printTransHeader()
{
	// prints table header
	cout << endl
		 << setw(7) << "Account"
		 << setw(12) << "Remaining"
		 << setw(12) << "Amount"
		 << setw(22) << "Date" << endl;
	printLine();
}

uInt getAdminChoice()
{
	cout << "\nChoose:\n  "
		 << "1. Create new account\n  "
		 << "2. Get info about all accounts\n  "
		 << "3. Recent Transacations\n  "
		 << "4. Delete Account\n  "
		 << "5. Print to file `account.txt`\n  "
		 << "6. Logout\n  "
		 << "7. Exit\n";
	uInt option;
	validatedInput(option, "your choice");

	return option;
}
// prints transaction history
void printHistory(vector<Transaction> &history)
{
	// sort based on date newest first
	sort(history.begin(), history.end(), compare);

	// prints each transaction
	for (auto t : history)
	{
		if (t.account != 0)
			t.print();
	}

	cout << endl;
}

// compares two transaction using their date (the most recent)
bool compare(Transaction &t1, Transaction &t2)
{
	return t1.date.ltime > t2.date.ltime;
}