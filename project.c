#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAX_ACCOUNTS 1000
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[1;36m"
#define GRAY    "\033[1;37m"
#define RESET   "\033[0m"

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char number[100];
    char name[100];
    char email[100];
    float balance;
    char phone[100];
    Date date;
    char status[100];
} Account;

typedef struct {
    char name[100];
    char password[100];
} User;


Account accounts[MAX_ACCOUNTS];
int number_of_accounts;

char *month_names[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};


// -------------------- VALIDATIONS ------------------------

bool validate_account_number(const char *number) {
    if (strlen(number) != 10) {
        return false;
    }
    for (int i = 0; i < strlen(number); i++) {
        if (!isdigit(number[i])) {
            return false;
        }
    }
    return true;
}

bool validate_email(const char *email) {
    if (strlen(email) > 40)
        return false;
    const char *at = strchr(email, '@');
    if (!at)
        return false;
    if (!(strcmp(at, "@gmail.com") == 0 ||
          strcmp(at, "@yahoo.com") == 0 ||
          strcmp(at, "@outlook.com") == 0))
        return false;
    long long length = at - email;
    for (int j = 0; j < length; j++) {
        if (!(isalnum(email[j]) ||
              email[j] == '.')) {
            return false;
        }
    }
    return true;
}

bool validate_phone(const char *phone) {
    if (strlen(phone) != 11) {
        return false;
    }
    for (int i = 0; i < strlen(phone); i++) {
        if (!isdigit(phone[i])) {
            return false;
        }
    }
    return true;
}

bool validate_name(const char *name) {
    if (strlen(name) > 25 || strlen(name) == 0) {
        return false;
    }
    for (int i = 0; i < strlen(name); i++) {
        if (isdigit(name[i])) {
            return false;
        }
    }
    for (int i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ') {
            return false;
        }
    }
    return true;
}

bool validate_money(float money) {
    if (money <= 0) {
        return false;
    }
    return true;
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool validate_date( int month, int year) {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;

    return true;
}


// ----------------- Helper functions --------------------

void print_account(int index_receiver) {
    printf(GRAY "Account number: " RESET "%s\n", accounts[index_receiver].number);
    printf(GRAY "Name: " RESET "%s\n", accounts[index_receiver].name);
    printf(GRAY "Email: " RESET "%s\n", accounts[index_receiver].email);
    printf(GRAY "Balance: " RESET "%.2f$\n", accounts[index_receiver].balance);
    printf(GRAY "Mobile: " RESET "%s\n", accounts[index_receiver].phone);
    printf(GRAY "Date Opened: " RESET "%s %d\n", month_names[accounts[index_receiver].date.month - 1],
           accounts[index_receiver].date.year);
    printf(GRAY "Status: " RESET "%s\n", accounts[index_receiver].status);
}

int find_account_index(char *account_num) {
    for (int i = 0; i < number_of_accounts; i++) {
        if (strcmp(account_num, accounts[i].number) == 0) {
            return i;
        }
    }
    return -1; // means account is non-existent
}

bool save_to_file() {
    FILE *fptr = fopen("accounts1.txt", "w");
    if (fptr == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return false;
    }
    for (int i = 0; i < number_of_accounts; ++i) {
        fprintf(fptr, "%s,%s,%s,%.1f,%s,%d-%d,%s\n", accounts[i].number, accounts[i].name,
                accounts[i].email, accounts[i].balance,
                accounts[i].phone, accounts[i].date.month, accounts[i].date.year, accounts[i].status);
    }
    fclose(fptr);
    return true;
}

void log_transaction(char *transaction, char *account_number, float amount) {
    char filename[150];
    snprintf(filename, sizeof(filename), "%s.txt", account_number);
    FILE *fptr = fopen(filename, "a");
    if (!fptr) {
        printf(RED "Error opening transaction file!\n" RESET);
        return;
    }
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(fptr, "%s %.2f %d-%d-%d\n", transaction, amount, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    fclose(fptr);
}

float daily_limit(char *acc_number) {
    char filename[150];
    snprintf(filename, sizeof(filename), "%s.txt", acc_number);
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL) {
        return 0;
    }
    char transaction[15];
    float withdrawn_sum = 0;
    float amount = 0;
    char line[1000];
    time_t t = time(&t);
    struct tm tm = *localtime(&t);
    while (fgets(line, 1000, fptr) != NULL) {
        strcpy(transaction, strtok(line, " "));
        char *token = strtok(NULL, " ");
        amount = atof(token);
        token = strtok(NULL, "-");
        int transaction_day = atoi(token);

        if (strcmp(transaction, "withdraw") == 0 && transaction_day == tm.tm_mday) {
            withdrawn_sum += amount;
        }
    }
    fclose(fptr);
    return withdrawn_sum;
}

int cmp_name(const void *a, const void *b) {
    const Account *first = (const Account *) a;
    const Account *second = (const Account *) b;
    return strcmp(first->name, second->name);
}

int cmp_balance(const void *a, const void *b) {
    const Account *first = (const Account *) a;
    const Account *second = (const Account *) b;
    return first->balance > second->balance ? 1 : -1;
}

int cmp_date(const void *a, const void *b) {
    const Account *first = (const Account *) a;
    const Account *second = (const Account *) b;
    if (first->date.year == second->date.year) {
        return first->date.month - second->date.month;
    }
    return first->date.year - second->date.year;
}

void to_lower(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

// ---------------- Features ------------------

bool Login() {
    User user;
    printf(BLUE "Enter your username: " RESET);
    fgets(user.name, 20, stdin); // /n
    user.name[strlen(user.name) - 1] = '\0';
    printf(BLUE "Enter your password: " RESET);
    fgets(user.password, 20, stdin);
    user.password[strlen(user.password) - 1] = '\0';
    FILE *fptr = fopen("users.txt", "r");
    if (fptr == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return 0;
    }
    char FileUser[100];
    char FilePass[100];
    while (fscanf(fptr, "%s %s", FileUser, FilePass) != EOF) {
        if (strcmp(user.name, FileUser) == 0 && strcmp(user.password, FilePass) == 0) {
            return true;
        }
    }
    fclose(fptr);
    return false;
}

int Load() {
    FILE *fptr = fopen("accounts.txt", "r");
    if (fptr == NULL) {
        printf(RED "Error opening file!\n" RESET);
        return 0;
    }
    char line[1000];
    int num = 0;
    while (fgets(line, 1000, fptr) != NULL) {
        // missing validation.
        strcpy(accounts[num].number, strtok(line, ","));
        strcpy(accounts[num].name, strtok(NULL, ","));
        strcpy(accounts[num].email, strtok(NULL, ","));
        char *token = strtok(NULL, ",");
        accounts[num].balance = strtof(token, NULL);
        strcpy(accounts[num].phone, strtok(NULL, ","));
        token = strtok(NULL, "-");
        accounts[num].date.month = atoi(token);
        token = strtok(NULL, ",");
        accounts[num].date.year = atoi(token);
        strcpy(accounts[num].status, strtok(NULL, "\r"));
        num++;
    }
    fclose(fptr);
    return num;
}

void Search() {
    printf(BLUE "Enter account number: " RESET);
    char acc_number_receiver[100];
    scanf("%s", acc_number_receiver);
    int index_receiver = find_account_index(acc_number_receiver);
    if (index_receiver != -1) {
        print_account(index_receiver);
    } else {
        printf(YELLOW "Account not found.\n" RESET);
    }
}

void Advanced_Search() {
    char keyword[100];
    printf(BLUE "Enter the keyword: " RESET);
    scanf("%s", keyword);
    bool found = false;
    printf(CYAN "Search Results:\n\n" RESET);
    for (int i = 0; i < number_of_accounts; i++) {
        if (strstr(accounts[i].name, keyword) != NULL) {
            print_account(i);
            printf("\n");
            found = true;
        }
    }
    if (!found) {
        printf(YELLOW "No matches are found.\n" RESET);
    }
}

void Add_Account() {
    Account temp;
    temp.balance = 0.0;
    strcpy(temp.status, "active");
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
   temp.date.day = tm.tm_mday;
 temp.date.month = tm.tm_mon + 1;
    temp.date.year = tm.tm_year + 1900;
    while (true) {
        printf(BLUE "Enter account number: " RESET);
        fgets(temp.number, 100, stdin);
        temp.number[strlen(temp.number) - 1] = '\0';
        if (!validate_account_number(temp.number)) {
            printf(YELLOW "Account number is not valid.\n" RESET);
            continue;
        }
        bool found = (find_account_index(temp.number) != -1);
        if (found) {
            printf(YELLOW "Account number already exists, try different one.\n" RESET);
        } else {
            break;
        }
    }
    while (true) {
        printf(BLUE "Enter account name: " RESET);
        fgets(temp.name, 25, stdin);
        temp.name[strlen(temp.name) - 1] = '\0';
        if (!(validate_name(temp.name))) {
            printf(YELLOW "Account name is invalid, try again.\n" RESET);
        } else
            break;
    }
    while (true) {
        printf(BLUE "Enter account email: " RESET);
        scanf("%s", temp.email);
        if (!(validate_email(temp.email))) {
            printf(YELLOW "Account email is invalid, try again.\n" RESET);
        } else
            break;
    }
    while (true) {
        printf(BLUE "Enter account phone: " RESET);
        scanf("%s", temp.phone);
        if (!validate_phone(temp.phone)) {
            printf(YELLOW "Account phone is invalid, try again.\n" RESET);
        } else
            break;
    }
    printf(BLUE "Enter yes to confirm the changes or anything else to discard: " RESET);
    char confirm[100];
    scanf("%s", confirm);
    to_lower(confirm);
    if (strcmp(confirm, "yes") == 0) {
        accounts[number_of_accounts] = temp;
        number_of_accounts++;
        if (save_to_file()) {
            printf(CYAN "Account successfully added.\n" RESET);
        } else {
            number_of_accounts--;
            printf(YELLOW "Failed to save the changes.\n" RESET);
        }
    }
}

void delete_account() {
    char number[100];
    int i;
    while (true) {
        printf(BLUE "Enter account number: " RESET);
        scanf("%s", number);
        i = find_account_index(number);
        if (i == -1) {
            printf(YELLOW "Account number is not found, try again.\n" RESET);
            continue;
        } else
            break;
    }
    print_account(i);
    printf(BLUE "Enter yes to confirm the changes or anything else to discard: " RESET);
    char confirm[100];
    scanf("%s", confirm);
    to_lower(confirm);
    if (strcmp(confirm, "yes") == 0) {
        if (accounts[i].balance == 0) {
            accounts[i] = accounts[number_of_accounts - 1];
            number_of_accounts--;
            save_to_file();
            printf(CYAN "Account successfully deleted.\n" RESET);
        } else {
            printf(RED "Unable to delete account, balance greater than zero.\n" RESET);
        }
    }
}

void modify_account() {
    char temp[100];
    int index_receiver;
    while (true) {
        printf(BLUE "Enter account number: " RESET);
        char acc_number_receiver[20];
        fgets(acc_number_receiver, 50, stdin);
        acc_number_receiver[strlen(acc_number_receiver) - 1] = '\0';
        index_receiver = find_account_index(acc_number_receiver);
        if (index_receiver == -1) {
            printf(YELLOW "Account number is not found, try again.\n" RESET);
            continue;
        } else
            break;
    }
    while (true) {
        printf(BLUE "Choose one of the following to modify: " RESET);
        printf("[ Name - Mobile - Email ]\n");
        char modify[100];
        fgets(modify, 100, stdin);
        modify[strlen(modify) - 1] = '\0';
        if (strcmp(modify, "Name") == 0 || strcmp(modify, "name") == 0) {
            while (true) {
                printf(BLUE "Enter the new account name: " RESET);
                fgets(temp, 50, stdin);
                temp[strlen(temp) - 1] = '\0';
                if (!validate_name(temp)) {
                    printf(YELLOW "Account name is invalid, try again.\n" RESET);
                    continue;
                }
                strcpy(accounts[index_receiver].name, temp);
                if (save_to_file()) {
                    printf(CYAN "Account name successfully modified.\n" RESET);
                    return;
                } else {
                    printf(YELLOW "Unable to modify account.\n" RESET);
                    return;
                }
            }
        } else if (strcmp(modify, "Mobile") == 0 || strcmp(modify, "mobile") == 0) {
            while (true) {
                printf(BLUE "Enter the new account mobile: " RESET);
                fgets(temp, 100, stdin);
                temp[strlen(temp) - 1] = '\0';
                if (!validate_phone(temp)) {
                    printf(YELLOW "Account number is invalid, try again.\n" RESET);
                    continue;
                }
                strcpy(accounts[index_receiver].phone, temp);
                if (save_to_file()) {
                    printf(CYAN "Account number successfully modified.\n" RESET);
                    return;
                } else {
                    printf(YELLOW "Unable to modify account.\n" RESET);
                    return;
                }
            }
        } else if (strcmp(modify, "Email") == 0 || strcmp(modify, "email") == 0) {
            while (true) {
                printf(BLUE "Enter the new account email: " RESET);
                fgets(temp, 100, stdin);
                temp[strlen(temp) - 1] = '\0';
                if (!validate_email(temp)) {
                    printf(YELLOW "Account email is invalid, try again.\n" RESET);
                    continue;
                }
                strcpy(accounts[index_receiver].email, temp);
                if (save_to_file()) {
                    printf(CYAN "Account email successfully modified.\n" RESET);
                    return;
                } else {
                    printf(YELLOW "Unable to modify account.\n" RESET);
                    return;
                }
            }
            break;
        } else {
            printf(YELLOW "Invalid choice, try again\n" RESET);
            continue;
        }
    }
}

bool change_account_status() {
    char acc_number_receiver[100];
    int index_receiver;
    while (true) {
        printf(BLUE "Enter account number: " RESET);
        fgets(acc_number_receiver, 100, stdin);
        acc_number_receiver[strlen(acc_number_receiver) - 1] = '\0';
        index_receiver = find_account_index(acc_number_receiver);
        if (index_receiver == -1) {
            printf(YELLOW "Account number is not found, try again\n" RESET);
            continue;
        } else
            break;
    }
    printf(CYAN "The current account status is %s.\n" RESET, accounts[index_receiver].status);
    char temp_status[100];
    while (true) {
        printf(BLUE "Choose the new status [ active - inactive ]: " RESET);
        fgets(temp_status, 100, stdin);
        temp_status[strlen(temp_status) - 1] = '\0';
        if (strcmp(temp_status, "active") == 0 || strcmp(temp_status, "inactive") == 0) {
            break;
        } else {
            printf(YELLOW "Invalid choice, try again.\n" RESET);
            continue;
        }
    }
    printf(RED "Attention! "BLUE"Enter yes to confirm changes or anything else to discard: " RESET);
    char confirm[100];
    fgets(confirm, 100, stdin);
    confirm[strlen(confirm) - 1] = '\0';
    to_lower(confirm);
    if (strcmp(confirm, "yes") == 0) {
        strcpy(accounts[index_receiver].status, temp_status);
        if (save_to_file()) {
            printf(CYAN "Successfully changed account status to %s.\n" RESET, temp_status);
        } else
            printf(YELLOW "Unable to change account status.\n" RESET);
    } else {
        printf(YELLOW "Discarded changes." RESET);
    }
    if (strcmp(accounts[index_receiver].status, "active") == 0) {
        return true;
    } else {
        return false;
    }
}

void withdraw_money() {
    char acc_number[100];
    int index;
    while (true) {
        printf(BLUE "Enter account number: " RESET);
        fgets(acc_number, 100, stdin);
        acc_number[strlen(acc_number) - 1] = '\0';
        if (!(validate_account_number(acc_number))) {
            printf(YELLOW "Account number is invalid, try again.\n" RESET);
            continue;
        }
        index = find_account_index(acc_number);
        if (index == -1) {
            printf(YELLOW "Account number is not found, try again.\n" RESET);
            continue;
        }
        if (strcmp(accounts[index].status, "inactive") == 0) {
            printf(
                RED "Warning! " YELLOW "Unable to complete process, account status is currently inactive.\n" RESET);
            return;
        } else
            break;
    }
    float withdrawn = daily_limit(acc_number);
    float amount;
    while (true) {
        printf(BLUE "Enter amount to be withdrawn: " RESET);
        scanf("%f", &amount);
        if (!(validate_money(amount))) {
            printf(YELLOW "Amount is invalid, try again.\n" RESET);
            continue;
        }
        if (accounts[index].balance < amount) {
            printf(RED "Warning!" YELLOW "Balance is not eligible to withdraw %.2f from, try again.\n" RESET,
                   amount);
            continue;
        }
        if (amount > 10000) {
            printf(
                RED"Warning! " YELLOW "Maximum withdraw limit for each transaction is 10,000$, try again.\n" RESET);
            continue;
        }
        if ((amount + withdrawn) > 50000) {
            printf(RED"Warning! " YELLOW "Maximum daily withdraw limit is 50,000$, try again later.\n" RESET);
            return;
        } else
            break;
    }
    accounts[index].balance -= amount;
    if (save_to_file()) {
        printf(CYAN"Successfully withdrew %.2f from %s.\n" RESET, amount, accounts[index].name);
        log_transaction("withdraw", acc_number, amount);
    } else {
        printf(YELLOW "Unable to withdraw %.2f.\n", amount);
    }
}

void deposit_money() {
    char acc_number[100];
    int index;
    while (true) {
        printf(BLUE "Enter account number: " RESET);
        fgets(acc_number, 100, stdin);
        acc_number[strlen(acc_number) - 1] = '\0';
        if (!(validate_account_number(acc_number))) {
            printf(YELLOW "Account number is invalid, try again.\n" RESET);
            continue;
        }
        index = find_account_index(acc_number);
        if (index == -1) {
            printf(YELLOW "Account number is not found, try again.\n" RESET);
            continue;
        }
        if (strcmp(accounts[index].status, "inactive") == 0) {
            printf(
                RED "Warning! " YELLOW "Unable to complete process, account status is currently inactive.\n" RESET);
            return;
        } else
            break;
    }
    float amount;
    while (true) {
        printf(BLUE "Enter amount to deposit: " RESET);
        scanf("%f", &amount);
        if (!(validate_money(amount))) {
            printf(YELLOW "Invalid amount, try again.\n" RESET);
            continue;
        }
        if (amount > 10000) {
            printf(
                RED"Warning! " YELLOW "Maximum deposit limit for each transaction is 10,000$, try again.\n" RESET);
            continue;
        } else {
            accounts[index].balance += amount;
            if (save_to_file()) {
                printf(CYAN "%.2f $ successfully deposited to %s.\n" RESET, amount, accounts[index].name);
                log_transaction("deposit", acc_number, amount);
                return;
            } else {
                printf(YELLOW "Unable to deposit %0.2f.\n" RESET, amount);
                return;
            }
        }
    }
}

void transfer_money() {
    char acc_number_sender[100];
    int index_sender;
    while (true) {
        printf(BLUE "Enter your account number: " RESET);
        fgets(acc_number_sender, 100, stdin);
        acc_number_sender[strlen(acc_number_sender) - 1] = '\0';
        if (!(validate_account_number(acc_number_sender))) {
            printf(YELLOW "Account number is not valid, try again.\n" RESET);
            continue;
        }
        index_sender = find_account_index(acc_number_sender);
        if (strcmp(accounts[index_sender].status, "inactive") == 0) {
            printf(RED "Warning! " YELLOW "Sender account is currently inactive, transfer process cannot be completed.\n" RESET);
            return;
        } else {
            break;
        }
    }
    char acc_number_receiver[100];
    int index_receiver;

    while (true) {
        printf(BLUE "Enter the account number you want to transfer money to: " RESET);
        fgets(acc_number_receiver, 100, stdin);
        acc_number_receiver[strlen(acc_number_receiver) - 1] = '\0';
        if (!(validate_account_number(acc_number_receiver))) {
            printf(YELLOW "Account number is invalid, try again.\n" RESET);
            continue;
        }
        index_receiver = find_account_index(acc_number_receiver);
        if (index_receiver == -1) {
            printf(YELLOW "Account number is not found, try again.\n" RESET);
            continue;
        }
        if (strcmp(accounts[index_receiver].status, "inactive") == 0) {
            printf(
                RED "Warning! " YELLOW
                "Receiver account is currently inactive, transfer process cannot be completed.\n" RESET);
            return;
        } else
            break;
    }
    float amount;
    while (true) {
        printf(BLUE "Enter the amount of money you want to transfer: " RESET);
        scanf("%f", &amount);
        if (!(validate_money(amount))) {
            printf(YELLOW "Amount of money entered is invalid, try again.\n" RESET);
            continue;
        }
        if (amount <= accounts[index_sender].balance) {
            accounts[index_sender].balance -= amount;
            accounts[index_receiver].balance += amount;
            if (save_to_file()) {
                printf(CYAN "Successfully transferred %.2f to %s.\n" RESET, amount, accounts[index_receiver].name);
                log_transaction("transfer", acc_number_sender, amount);
                log_transaction("receiver", acc_number_receiver, amount);
                return;
            } else {
                printf(YELLOW "Unable to transfer %.2f to %s.\n" RESET, amount, accounts[index_receiver].name);
                return;
            }
        } else {
            printf(YELLOW "Balance is not eligible to transfer entered amount, try again.\n" RESET);
            continue;
        }
    }
}

void report() {
    char acc_number[100];
    while (true) {
        printf(BLUE "Enter your account number: " RESET);
        scanf("%s", acc_number);
        if (!(validate_account_number(acc_number))) {
            printf(YELLOW "Account number is not valid, try again.\n" RESET);
            continue;
        } else
            break;
    }
    char filename[150];
    snprintf(filename, sizeof(filename), "%s.txt", acc_number);
    FILE *fptr = fopen(filename, "r");
    if (fptr == NULL) {
        printf(YELLOW "Account has no recent transactions.\n" RESET);
    } else {
        char line[100];
        int count = 0;
        while (fgets(line, 100, fptr) != NULL) {
            ++count;
        }
        int num_to_skip = count <= 5 ? 0 : count - 5;
        rewind(fptr);
        while (num_to_skip > 0) {
            fgets(line, 100, fptr);
            --num_to_skip;
        }
        while (fgets(line, 100, fptr) != NULL) {
            printf(GRAY "%s" RESET, line);
        }
    }
}

void print_sorted() {
    int choice;
    char removenewline;
    printf(BLUE"Choose how you would like the printed accounts to be sorted:\n" RESET);
    printf(CYAN "1. " RESET "By name\n");
    printf(CYAN "2. " RESET "By balance\n");
    printf(CYAN "3. " RESET "By date opened\n");
    scanf(" %d", &choice);
    removenewline = getchar();
    switch (choice) {
        case 1:
            qsort(accounts, number_of_accounts, sizeof(Account), cmp_name);
            break;
        case 2:
            qsort(accounts, number_of_accounts, sizeof(Account), cmp_balance);
            break;
        case 3:
            qsort(accounts, number_of_accounts, sizeof(Account), cmp_date);
            break;
        default:
            printf(YELLOW "Invalid choice. Please try again.\n" RESET);
            sleep(1);
            return print_sorted();
    }
    for (int i = 0; i < number_of_accounts; i++) {
        print_account(i);
        printf("\n");
    }
}

void delete_multiple() {
    char run_again[100];
    do {
        int choice;
        printf(BLUE"How would you like to delete the accounts?\n" RESET);
        printf(CYAN "1. "RESET"Delete accounts by date\n");
        printf(CYAN "2. "RESET"Delete accounts by inactivity\n");
        printf(BLUE"Enter your choice: " RESET);
        scanf("%d", &choice);

        if (choice == 1) {
            int year, month;
            while (true) {
                printf(BLUE"Enter the date "YELLOW"(year-month): " RESET);
                while (getchar() != '\n');
                scanf("%d-%d", &year, &month);
                if ( !(validate_date( month, year)) ) {
                    printf(RED"Invalid date entered! Please try again.\n" RESET);
                    return;
                }
                else
                    break;
            }
            int deleted = 0;
            for (int i = 0; i < number_of_accounts;) {
                if (accounts[i].date.year == year && accounts[i].date.month == month ) {
                    accounts[i] = accounts[number_of_accounts - 1];
                    number_of_accounts--;
                    deleted++;
                } else {
                    i++;
                }
            }
            if (deleted == 0)
                printf(YELLOW"No accounts found on this date\n" RESET);
            else {
                save_to_file();
                printf(GREEN"%d accounts deleted successfully\n" RESET, deleted);
            }
        } else if (choice == 2) {
            time_t now = time(NULL);
            int deleted = 0;
            for (int i = 0; i < number_of_accounts;) {
                struct tm acc_date = {0};
                acc_date.tm_year = accounts[i].date.year - 1900;
                acc_date.tm_mon = accounts[i].date.month - 1;
                time_t acc_time = mktime(&acc_date);
                int diff_days = (int) (difftime(now, acc_time) / 86400);
                if (strcmp(accounts[i].status, "inactive") == 0 && accounts[i].balance == 0 && diff_days > 90) {
                    accounts[i] = accounts[number_of_accounts - 1];
                    number_of_accounts--;
                    deleted++;
                } else {
                    i++;
                }
            }
            if (deleted == 0)
                printf(RED "Warning! " YELLOW "No inactive zero-balance accounts older than 90 days.\n" RESET);
            else {
                save_to_file();
                printf(GREEN"%d accounts deleted successfully\n" RESET, deleted);
            }
        } else {
            printf(YELLOW "Invalid choice! Try again\n" RESET);
        }

        printf(BLUE"Do you want to DELETE MULTIPLE again? " RESET);
        scanf("%s", run_again);
        to_lower(run_again);
    } while (strcmp(run_again, "yes") == 0);
}

void menu() {
    sleep(1);
    int choice;
    char removenewline;
    printf(BLUE "Welcome to the System Menu!\n");
    printf("How can we assist you?\n" RESET);
    printf(CYAN "1. " RESET "Add Account\n");
    printf(CYAN "2. " RESET "Delete Account\n");
    printf(CYAN "3. " RESET "Modify Account\n");
    printf(CYAN "4. " RESET "Search Account\n");
    printf(CYAN "5. " RESET "Advanced Search\n");
    printf(CYAN "6. " RESET "Change Status\n");
    printf(CYAN "7. " RESET "Withdraw\n");
    printf(CYAN "8. " RESET "Deposit\n");
    printf(CYAN "9. " RESET "Transfer  \n");
    printf(CYAN "10. " RESET "Report \n");
    printf(CYAN "11. " RESET "Print Sorted \n");
    printf(CYAN "12. " RESET "Quit \n");
    printf(CYAN "13. " RESET "Other\n");
    printf(BLUE "Enter your choice: " RESET);
    if (scanf("%d", &choice) != 1) {
        printf(YELLOW "Invalid input. Please enter a number.\n" RESET);
        while (getchar() != '\n');
        return menu();
    }
    removenewline = getchar();
    switch (choice) {
        case 1:
            Add_Account();
            return menu();
        case 2:
            delete_account();
            return menu();
        case 3:
            modify_account();
            return menu();
        case 4:
            Search();
            return menu();
        case 5:
            Advanced_Search();
            return menu();
        case 6:
            change_account_status();
            return menu();
        case 7:
            withdraw_money();
            return menu();
        case 8:
            deposit_money();
            return menu();
        case 9:
            transfer_money();
            return menu();
        case 10:
            report();
            return menu();
        case 11:
            print_sorted();
            return menu();
        case 12:
            printf(RED "Exiting the system... " GREEN "Thank you for using our banking system!\n" RESET);
            sleep(1);
            return;
        case 13:
            delete_multiple();
            return menu();
        default:
            printf(YELLOW "Invalid choice, try again.\n" RESET);
            return menu();
    }
}

int main(void) {
    while (!Login()) {
        printf(RED "Invalid username or password, try again.\n" RESET);
    }
    printf(GREEN "Login Successful!\n" RESET);
    number_of_accounts = Load();
    menu();

    return 0;
}
