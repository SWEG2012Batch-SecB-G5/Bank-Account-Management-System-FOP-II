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

enum AdminOption
{
	NEW_ACCOUNT = 1,
	ALL_ACCOUNT,
	HISTORY_ADMIN,
	DELETE_ACCOUNT,
	PRINT_ACCOUNTS,
	LOGOUT_ADMIN,
	EXIT_ADMIN
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
struct Transaction
{
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

	Account(char name[] = "", double initBalance = 0.0,
			char passw[] = "", char phoneNumber[] = "",
			char s = '\0', uInt agg = 0, uInt accNumber = 0,
			AccountType t = AccountType::DEPOSIT);
	// logs to to transaction history
	void log(double amount);
	// change password
	void setPassword(const char *pass);
	bool checkPassword(char pass[]);
	void transfer(Account *a, double amount);
	void printHistory();
	void printInfo();
};

// prototypes
template <typename T>
T &validatedInput(T &val, string msg);
char *validatedStringInput(char *val, string msg, uInt min, uInt max);
char *validatedNameInput();
void printError(string msg);
void printOption();
void printTitle(string title);
void printLine(char fill = '-');
void printAccounts(ostream &output, fstream &file);
Account getAccount(uInt accNumber, iostream &file);
void newAccount(iostream &file);
void updateAccount(double amount, uInt accNumber, iostream &file);
void createBlankAccount(fstream &file);
void deleteAccount(fstream &file);
uInt getAccountNumber();

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
			 << LOGIN << ". Login\n\t"
			 << ADMIN << ". Login as admin\n\t"
			 << EXIT_M << ". Exit\n";

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
			{
				continue;
			}
			cout << "Successfully Logged in.\n";
			cout << "Wellcome, " << currUser.fullName << "!\n";

			do
			{
				printOption();
				validatedInput(option, "your choice");

				switch (option)
				{
				case WITHDRAW:
				{
					double amount;
					amount = validatedInput(amount, "Amount");
					updateAccount(-amount, currUser.accountNumber, file);
					break;
				}
				case DEPOSIT:
				{
					double amount;
					amount = validatedInput(amount, "Amount");
					updateAccount(amount, currUser.accountNumber, file);
					break;
				}
				case BALANCE:
				{
					cout << "Your current balance is " << currUser.balance << endl;
					break;
				}
				case TRANSFER:
				{
					uInt accNumber = getAccountNumber();
					double amount;
					validatedInput(amount, "Transfer amount");

					updateAccount(-amount, currUser.accountNumber, file); // withdraw from current user
					updateAccount(amount, accNumber, file);				  // deposit to another account
					break;
				}
				case INFO:
				{
					currUser.printInfo();
					break;
				}
				case HISTORY:
				{
					printTitle("Transactions");
					currUser.printHistory();
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
					cout << "\nChoose:\n  "
						 << NEW_ACCOUNT 	<<". Create new account\n  "
						 << ALL_ACCOUNT		<<". Get info about all accounts\n  "
						 << HISTORY_ADMIN	<<". Recent Transacations\n  "
						 << DELETE_ACCOUNT	<<". Delete Account\n  "
						 << PRINT_ACCOUNTS 	<<". Print to file `account.txt`\n  "
						 << LOGOUT_ADMIN	<<". Logout\n  "
						 << EXIT_ADMIN	 	<<". Exit\n";
					uInt option;
					validatedInput(option, "your choice");
					switch (option)
					{
					case NEW_ACCOUNT:
					{
						uInt numOfAccount;
						validatedInput(numOfAccount, "number of accounts to be created");

						for (uInt i = 0; i < numOfAccount; i++)
						{
							cout << "Enter following info for user " << i + 1 << ": \n";
							newAccount(file);
						}
						break;
					}
					case ALL_ACCOUNT:
						printTitle("All accounts");
						printAccounts(cout, file);
						break;
					case HISTORY_ADMIN:
						printTitle("Recent Transacations");

						cout << endl
							 << setw(7) << "Account"
							 << setw(12) << "Remaining"
							 << setw(12) << "Amount"
							 << setw(22) << "Date" << endl;
						printLine();
						for (uInt i = 0; i < 10; i++)
						{
							Account user = getAccount(i + 1, file);
							for (uInt j = 0; j < 10; j++)
							{
								user.transactions[j].print();
							}
						}

						break;
					case DELETE_ACCOUNT:
					{
						printTitle("Delete Account");
						deleteAccount(file);
						break;
					}
					case PRINT_ACCOUNTS:
					{
						ofstream oAccount("account.txt");
						if (!oAccount)
							cout << "Can't create `account.txt`. Please try again." << endl;

						printAccounts(oAccount, file);
						cout << "Created account.txt. Check it in your current working directory\n";
						break;
					}
					case LOGOUT_ADMIN:
						goto exit_loop2;
						break;
					case EXIT_ADMIN:
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

	} while (true);

	return 0;
}

// prints option
void printOption()
{
	printLine();
	cout << "What do you want to do: \n\t"
		 << WITHDRAW << ". Withdraw Money\n\t"
		 << DEPOSIT  << ". Deposit\n\t"
		 <<	BALANCE  << ". Check your balance\n\t"
		 << TRANSFER << ". Transfer to another account\n\t"
		 << HISTORY  << ". Transactions history\n\t"
		 << INFO	 << ". Your information\n\t"
		 << LOGOUT 	 << ". Logout\n\t"
		 << EXIT	 << ". Exit\n";
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
			printError("Enter a valid "+ msg + " value!");
			cin.clear();
			cin.ignore(10000, '\n');
		}
	}

	return val;
}

// validate string
char *validatedStringInput(char *val, string msg, uInt min, uInt max)
{
	while (true)
	{
		cout << "Enter " << msg << ": ";
		cin >> val;
		if (strlen(val) >= min && strlen(val) <= max)
			break;
		else
		{
			printError("Invalid input. The input should be " + to_string(min) +
					   " to " + to_string(max) + " character long.");
		}
	}
	return val;
}
// validate string
char *validatedNameInput(char *name)
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
	output << "\n\n"
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

Account getAccount(uInt accNumber, iostream &file)
{
	Account acc;
	file.seekg((accNumber - 1) * sizeof(Account));
	file.read((char *)&acc, sizeof(Account));

	return acc;
}

void newAccount(iostream &file)
{
	Account acc, a;

	acc.accountNumber = getAccountNumber();
	file.seekg((acc.accountNumber - 1) * sizeof(Account));
	file.read((char *)&a, sizeof(Account));

	// accepts account informations
	validatedNameInput(acc.fullName);
	validatedInput(acc.sex, "Sex");
	validatedInput(acc.balance, "intial balance");	
	validatedInput(acc.age, "age");
	validatedStringInput(acc.phone, "Phone", 10, 12);
	validatedStringInput(acc.password, "Password", MIN_PASS, MAX_PASS);

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

void updateAccount(double amount, uInt accNumber, iostream &file)
{
	Account acc;
	file.seekg((accNumber - 1) * sizeof(Account));
	file.read((char *)&acc, sizeof(Account));

	if (acc.accountNumber == 0)
		printError("account #" + to_string(acc.accountNumber) + " doesn't exist.");
	else if (amount < 0 || amount >= maxWithdraw)
	{
		printError("You exceeded a given maximum withdraw for the day.");
		return;
		cin.clear();
		cin.ignore(10000, '\n');
	}

	acc.balance += amount;
	// log to transaction
	acc.log(amount);

	// replace the old with new one
	file.seekg((accNumber - 1) * sizeof(Account));
	file.write((char *)&acc, sizeof(Account));

	cout << "\nSuccess! Your account has been " << (amount > 0 ? "debited" : "credited")
		 << " with " << amount << ".\n"
		 << "Your current balance is " << acc.balance << endl;
}

void createBlankAccount(fstream &file)
{
	Account blank;

	for (size_t i = 0; i < 100; i++)
	{
		file.seekg((i - 1) * sizeof(Account));
		file.write((char *)&blank, sizeof(Account));
	}
}

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
	{
		cout << "Cancelled\n";
	}
}

Account::Account(char name[], double initBalance,
				 char passw[], char phoneNumber[],
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

bool Account::checkPassword(char pass[])
{
	if (strcmp(pass, password) == 0)
		return true;
	printError("Incorrect password! Please try again.");
	return false;
}

void Account::printHistory()
{
	cout << endl
		 << setw(7) << "Account"
		 << setw(12) << "Remaining"
		 << setw(12) << "Amount"
		 << setw(22) << "Date" << endl;
	printLine();
	for (uInt i = 0; i < 10; i++)
	{
		if (transactions[i].account != 0)
			transactions[i].print();
	}
	cout << endl;
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