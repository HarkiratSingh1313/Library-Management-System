#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOOK_FILE "books.txt"
#define STUDENT_FILE "students.txt"
#define ISSUE_FILE "issues.txt"
#define LOG_FILE "logs.txt"

#define ADMIN_ID "admin"
#define ADMIN_PASS "admin123"

typedef struct
{
    int id;
    char title[100];
    char author[100];
    int quantity;
    int available;
} Book;

typedef struct
{
    char id[20];
    char name[100];
    char password[50];
} Student;

typedef struct
{
    int bookID;
    char studentID[20];
    char issueDate[15];
    char dueDate[15];
    int returned;
} Issue;

/* ===========================
   FUNCTION PROTOTYPES
   =========================== */

void logActivity(const char *message);

int bookExists(int id);
int studentExists(const char *id);

void addBook();
void viewBooks();
void searchBookByID();
void searchBookByTitle();
void searchBookByAuthor();
void updateBook();
void deleteBook();

void registerStudent();
int login(char currentUser[]);
void viewStudents();
void changePassword(const char currentUser[]);

void issueBook();
void returnBook();
void viewIssuedBooks();
void viewStudentIssuedBooks(const char currentUser[]);

void adminMenu();
void studentMenu(const char currentUser[]);

void dashboard();

/* ===========================
   UTILITY FUNCTIONS
   =========================== */

void logActivity(const char *message)
{
    FILE *fp = fopen(LOG_FILE, "a");

    if(fp == NULL)
        return;

    fprintf(fp, "%s\n", message);

    fclose(fp);
}

/* ===========================
   BOOK FILE FUNCTIONS
   =========================== */

int readBook(FILE *fp, Book *book)
{
    return fscanf(fp,"%d,%99[^,],%99[^,],%d,%d\n",&book->id,book->title,book->author,&book->quantity,&book->available) == 5;
}

void writeBook(FILE *fp, Book book)
{
    fprintf(fp,"%d,%s,%s,%d,%d\n",book.id,book.title,book.author,book.quantity,book.available);
}

/* ===========================
   STUDENT FILE FUNCTIONS
   =========================== */

int readStudent(FILE *fp, Student *student)
{
    return fscanf(fp,"%19[^,],%99[^,],%49[^\n]\n",student->id,student->name,student->password) == 3;
}

void writeStudent(FILE *fp, Student student)
{
    fprintf(fp,"%s,%s,%s\n",student.id,student.name,student.password);
}

/* ===========================
   ISSUE FILE FUNCTIONS
   =========================== */

int readIssue(FILE *fp, Issue *issue)
{
    return fscanf(fp,"%d,%19[^,],%14[^,],%14[^,],%d\n",&issue->bookID,issue->studentID,issue->issueDate,issue->dueDate,&issue->returned) == 5;
}

void writeIssue(FILE *fp, Issue issue)
{
    fprintf(fp,"%d,%s,%s,%s,%d\n",issue.bookID,issue.studentID,issue.issueDate,issue.dueDate,issue.returned);
}

/* ===========================
   VALIDATION FUNCTIONS
   =========================== */

int bookExists(int id)
{
    FILE *fp = fopen(BOOK_FILE, "r");

    if(fp == NULL)
        return 0;

    Book book;

    while(readBook(fp, &book))
    {
        if(book.id == id)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int studentExists(const char *id)
{
    FILE *fp = fopen(STUDENT_FILE, "r");

    if(fp == NULL)
        return 0;

    Student student;

    while(readStudent(fp, &student))
    {
        if(strcmp(student.id, id) == 0)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

/* ===========================
   STUDENT REGISTRATION
   =========================== */

void registerStudent()
{
    Student student;
    char confirmPassword[50];

    printf("\n===== STUDENT REGISTRATION =====\n");

    printf("Enter Student ID: ");
    scanf("%19s", student.id);

    if(studentExists(student.id))
    {
        printf("Student ID already exists.\n");
        return;
    }

    getchar();

    printf("Enter Student Name: ");
    fgets(student.name, sizeof(student.name), stdin);
    student.name[strcspn(student.name, "\n")] = '\0';

    printf("Create Password: ");
    scanf("%49s", student.password);

    printf("Confirm Password: ");
    scanf("%49s", confirmPassword);

    if(strcmp(student.password, confirmPassword) != 0)
    {
        printf("Passwords do not match.\n");
        return;
    }

    FILE *fp = fopen(STUDENT_FILE, "a");

    if(fp == NULL)
    {
        printf("Unable to open file.\n");
        return;
    }

    writeStudent(fp, student);

    fclose(fp);

    logActivity("New student registered.");

    printf("Student registered successfully.\n");
}

/* ===========================
   LOGIN SYSTEM
   =========================== */

int login(char currentUser[])
{
    char id[20];
    char password[50];

    printf("\n===== LOGIN =====\n");

    printf("Enter User ID: ");
    scanf("%19s", id);

    printf("Enter Password: ");
    scanf("%49s", password);

    if(strcmp(id, ADMIN_ID) == 0 &&
       strcmp(password, ADMIN_PASS) == 0)
    {
        strcpy(currentUser, ADMIN_ID);

        printf("\nAdmin Login Successful.\n");

        return 2;
    }

    FILE *fp = fopen(STUDENT_FILE, "r");

    if(fp == NULL)
    {
        printf("Student database not found.\n");
        return 0;
    }

    Student student;

    while(readStudent(fp, &student))
    {
        if(strcmp(student.id, id) == 0 &&
           strcmp(student.password, password) == 0)
        {
            strcpy(currentUser, student.id);

            fclose(fp);

            printf("\nStudent Login Successful.\n");

            return 1;
        }
    }

    fclose(fp);

    printf("Invalid credentials.\n");

    return 0;
}

/* ===========================
   CHANGE PASSWORD
   =========================== */

void changePassword(const char currentUser[])
{
    FILE *fp = fopen(STUDENT_FILE, "r");
    FILE *temp = fopen("temp_students.txt", "w");

    if(fp == NULL || temp == NULL)
    {
        printf("File error.\n");
        return;
    }

    Student student;

    char oldPassword[50];
    char newPassword[50];
    char confirmPassword[50];

    int found = 0;

    printf("Enter Current Password: ");
    scanf("%49s", oldPassword);

    while(readStudent(fp, &student))
    {
        if(strcmp(student.id, currentUser) == 0)
        {
            found = 1;

            if(strcmp(student.password, oldPassword) != 0)
            {
                printf("Incorrect password.\n");

                fclose(fp);
                fclose(temp);

                remove("temp_students.txt");

                return;
            }

            printf("Enter New Password: ");
            scanf("%49s", newPassword);

            printf("Confirm New Password: ");
            scanf("%49s", confirmPassword);

            if(strcmp(newPassword, confirmPassword) != 0)
            {
                printf("Passwords do not match.\n");

                fclose(fp);
                fclose(temp);

                remove("temp_students.txt");

                return;
            }

            strcpy(student.password, newPassword);
        }

        writeStudent(temp, student);
    }

    fclose(fp);
    fclose(temp);

    remove(STUDENT_FILE);
    rename("temp_students.txt", STUDENT_FILE);

    if(found)
    {
        logActivity("Password changed.");

        printf("Password updated successfully.\n");
    }
}

/* =====================================
   ADD BOOK
   ===================================== */

void addBook()
{
    Book book;

    printf("\n===== ADD BOOK =====\n");

    printf("Enter Book ID: ");
    scanf("%d", &book.id);

    if(bookExists(book.id))
    {
        printf("Book ID already exists.\n");
        return;
    }

    getchar();

    printf("Enter Title: ");
    fgets(book.title, sizeof(book.title), stdin);
    book.title[strcspn(book.title, "\n")] = '\0';

    printf("Enter Author: ");
    fgets(book.author, sizeof(book.author), stdin);
    book.author[strcspn(book.author, "\n")] = '\0';

    printf("Enter Quantity: ");
    scanf("%d", &book.quantity);

    book.available = book.quantity;

    FILE *fp = fopen(BOOK_FILE, "a");

    if(fp == NULL)
    {
        printf("Unable to open books file.\n");
        return;
    }

    writeBook(fp, book);

    fclose(fp);

    logActivity("Book added.");

    printf("Book added successfully.\n");
}

/* =====================================
   VIEW BOOKS
   ===================================== */

void viewBooks()
{
    FILE *fp = fopen(BOOK_FILE, "r");

    if(fp == NULL)
    {
        printf("No books available.\n");
        return;
    }

    Book book;

    printf("\n");
    printf("----------------------------------------------------------------------------------------------------\n");
    printf("%-10s %-35s %-25s %-10s %-10s\n",
           "ID",
           "Title",
           "Author",
           "Qty",
           "Available");
    printf("----------------------------------------------------------------------------------------------------\n");

    while(readBook(fp, &book))
    {
        printf("%-10d %-35s %-25s %-10d %-10d\n",
               book.id,
               book.title,
               book.author,
               book.quantity,
               book.available);
    }

    fclose(fp);
}

/* =====================================
   SEARCH BY ID
   ===================================== */

void searchBookByID()
{
    int id;
    int found = 0;

    printf("\nEnter Book ID: ");
    scanf("%d", &id);

    FILE *fp = fopen(BOOK_FILE, "r");

    if(fp == NULL)
    {
        printf("No books available.\n");
        return;
    }

    Book book;

    while(readBook(fp, &book))
    {
        if(book.id == id)
        {
            printf("\nBook Found\n");
            printf("------------------------\n");
            printf("ID        : %d\n", book.id);
            printf("Title     : %s\n", book.title);
            printf("Author    : %s\n", book.author);
            printf("Quantity  : %d\n", book.quantity);
            printf("Available : %d\n", book.available);

            found = 1;
            break;
        }
    }

    fclose(fp);

    if(!found)
        printf("Book not found.\n");
}

/* =====================================
   SEARCH BY TITLE
   ===================================== */

void searchBookByTitle()
{
    char keyword[100];
    int found = 0;

    getchar();

    printf("Enter Title Keyword: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = '\0';

    FILE *fp = fopen(BOOK_FILE, "r");

    if(fp == NULL)
    {
        printf("No books available.\n");
        return;
    }

    Book book;

    while(readBook(fp, &book))
    {
        if(strstr(book.title, keyword))
        {
            printf("%d | %s | %s\n",
                   book.id,
                   book.title,
                   book.author);

            found = 1;
        }
    }

    fclose(fp);

    if(!found)
        printf("No matching books found.\n");
}

/* =====================================
   SEARCH BY AUTHOR
   ===================================== */

void searchBookByAuthor()
{
    char keyword[100];
    int found = 0;

    getchar();

    printf("Enter Author Name: ");
    fgets(keyword, sizeof(keyword), stdin);
    keyword[strcspn(keyword, "\n")] = '\0';

    FILE *fp = fopen(BOOK_FILE, "r");

    if(fp == NULL)
    {
        printf("No books available.\n");
        return;
    }

    Book book;

    while(readBook(fp, &book))
    {
        if(strstr(book.author, keyword))
        {
            printf("%d | %s | %s\n",
                   book.id,
                   book.title,
                   book.author);

            found = 1;
        }
    }

    fclose(fp);

    if(!found)
        printf("No matching books found.\n");
}

/* =====================================
   UPDATE BOOK
   ===================================== */

void updateBook()
{
    int id;
    int found = 0;

    printf("\nEnter Book ID to update: ");
    scanf("%d", &id);

    FILE *fp = fopen(BOOK_FILE, "r");
    FILE *temp = fopen("temp_books.txt", "w");

    if(fp == NULL || temp == NULL)
    {
        printf("File error.\n");
        return;
    }

    Book book;

    while(readBook(fp, &book))
    {
        if(book.id == id)
        {
            found = 1;

            getchar();

            printf("New Title: ");
            fgets(book.title, sizeof(book.title), stdin);
            book.title[strcspn(book.title, "\n")] = '\0';

            printf("New Author: ");
            fgets(book.author, sizeof(book.author), stdin);
            book.author[strcspn(book.author, "\n")] = '\0';

            printf("New Quantity: ");
            scanf("%d", &book.quantity);

            if(book.available > book.quantity)
                book.available = book.quantity;
        }

        writeBook(temp, book);
    }

    fclose(fp);
    fclose(temp);

    remove(BOOK_FILE);
    rename("temp_books.txt", BOOK_FILE);

    if(found)
    {
        logActivity("Book updated.");
        printf("Book updated successfully.\n");
    }
    else
    {
        printf("Book not found.\n");
    }
}

/* =====================================
   DELETE BOOK
   ===================================== */

void deleteBook()
{
    int id;
    int found = 0;

    printf("\nEnter Book ID to delete: ");
    scanf("%d", &id);

    FILE *fp = fopen(BOOK_FILE, "r");
    FILE *temp = fopen("temp_books.txt", "w");

    if(fp == NULL || temp == NULL)
    {
        printf("File error.\n");
        return;
    }

    Book book;

    while(readBook(fp, &book))
    {
        if(book.id == id)
        {
            found = 1;
            continue;
        }

        writeBook(temp, book);
    }

    fclose(fp);
    fclose(temp);

    remove(BOOK_FILE);
    rename("temp_books.txt", BOOK_FILE);

    if(found)
    {
        logActivity("Book deleted.");
        printf("Book deleted successfully.\n");
    }
    else
    {
        printf("Book not found.\n");
    }
}

/* =====================================
   VIEW STUDENTS
   ===================================== */

void viewStudents()
{
    FILE *fp = fopen(STUDENT_FILE, "r");

    if(fp == NULL)
    {
        printf("No students registered.\n");
        return;
    }

    Student student;

    printf("\n");
    printf("--------------------------------------------------------------\n");
    printf("%-20s %-30s\n",
           "Student ID",
           "Student Name");
    printf("--------------------------------------------------------------\n");

    while(readStudent(fp, &student))
    {
        printf("%-20s %-30s\n",
               student.id,
               student.name);
    }

    fclose(fp);
}

/* =====================================
   DASHBOARD HELPERS
   ===================================== */

int totalBooks()
{
    FILE *fp = fopen(BOOK_FILE, "r");

    if(fp == NULL)
        return 0;

    Book book;
    int count = 0;

    while(readBook(fp, &book))
        count++;

    fclose(fp);

    return count;
}

int totalStudents()
{
    FILE *fp = fopen(STUDENT_FILE, "r");

    if(fp == NULL)
        return 0;

    Student student;
    int count = 0;

    while(readStudent(fp, &student))
        count++;

    fclose(fp);

    return count;
}

int totalIssuedBooks()
{
    FILE *fp = fopen(ISSUE_FILE, "r");

    if(fp == NULL)
        return 0;

    Issue issue;
    int count = 0;

    while(readIssue(fp, &issue))
    {
        if(issue.returned == 0)
            count++;
    }

    fclose(fp);

    return count;
}

/* =====================================
   DASHBOARD
   ===================================== */

void dashboard()
{
    printf("\n");
    printf("=====================================\n");
    printf("         LIBRARY DASHBOARD\n");
    printf("=====================================\n");

    printf("Total Books      : %d\n",
           totalBooks());

    printf("Total Students   : %d\n",
           totalStudents());

    printf("Books Issued     : %d\n",
           totalIssuedBooks());

    printf("=====================================\n");
}

/* =====================================
   FINE CALCULATION
   ===================================== */

int calculateFine()
{
    int overdueDays;

    printf("Enter number of overdue days: ");
    scanf("%d", &overdueDays);

    if(overdueDays <= 0)
        return 0;

    return overdueDays * 5;   /* Rs. 5 per day */
}


/* =====================================
   VALIDATE DATES
   ===================================== */

int isLeapYear(int year);
int isValidDate(const char *date);


int isLeapYear(int year)
{
    if(year % 400 == 0)
        return 1;

    if(year % 100 == 0)
        return 0;

    if(year % 4 == 0)
        return 1;

    return 0;
}

int isValidDate(const char *date)
{
    int day, month, year;

    if(sscanf(date, "%d-%d-%d", &day, &month, &year) != 3)
        return 0;

    if(year < 2000 || year > 2100)
        return 0;

    if(month < 1 || month > 12)
        return 0;

    int daysInMonth[] =
    {
        31,28,31,30,31,30,
        31,31,30,31,30,31
    };

    if(isLeapYear(year))
        daysInMonth[1] = 29;

    if(day < 1 || day > daysInMonth[month - 1])
        return 0;

    return 1;
}

/* =====================================
   ISSUE BOOK
   ===================================== */

void issueBook()
{
    int bookID;
    char studentID[20];

    printf("\n===== ISSUE BOOK =====\n");

    printf("Enter Book ID: ");
    scanf("%d", &bookID);

    printf("Enter Student ID: ");
    scanf("%19s", studentID);

    if(!studentExists(studentID))
    {
        printf("Student does not exist.\n");
        return;
    }

    FILE *fp = fopen(BOOK_FILE, "r");
    FILE *temp = fopen("temp_books.txt", "w");

    if(fp == NULL || temp == NULL)
    {
        printf("Book file error.\n");
        return;
    }

    Book book;
    int found = 0;

    while(readBook(fp, &book))
    {
        if(book.id == bookID)
        {
            found = 1;

            if(book.available <= 0)
            {
                printf("Book currently unavailable.\n");

                fclose(fp);
                fclose(temp);

                remove("temp_books.txt");

                return;
            }

            book.available--;
        }

        writeBook(temp, book);
    }

    fclose(fp);
    fclose(temp);

    if(!found)
    {
        remove("temp_books.txt");

        printf("Book ID not found.\n");
        return;
    }

    remove(BOOK_FILE);
    rename("temp_books.txt", BOOK_FILE);

    Issue issue;

    issue.bookID = bookID;

    strcpy(issue.studentID, studentID);

    while(1)
    {
        printf("Enter Issue Date (DD-MM-YYYY): ");
        scanf("%14s", issue.issueDate);

        if(isValidDate(issue.issueDate))
            break;

        printf("Invalid date. Try again.\n");
    }

    while(1)
    {
        printf("Enter Due Date (DD-MM-YYYY): ");
        scanf("%14s", issue.dueDate);

        if(isValidDate(issue.dueDate))
            break;

        printf("Invalid date. Try again.\n");
    }

    issue.returned = 0;

    FILE *fi = fopen(ISSUE_FILE, "a");

    if(fi == NULL)
    {
        printf("Issue file error.\n");
        return;
    }

    writeIssue(fi, issue);

    fclose(fi);

    logActivity("Book issued.");

    printf("Book issued successfully.\n");
}

/* =====================================
   RETURN BOOK
   ===================================== */

void returnBook()
{
    int bookID;
    char studentID[20];

    printf("\n===== RETURN BOOK =====\n");

    printf("Enter Book ID: ");
    scanf("%d", &bookID);

    printf("Enter Student ID: ");
    scanf("%19s", studentID);

    FILE *fi = fopen(ISSUE_FILE, "r");
    FILE *tempIssue = fopen("temp_issues.txt", "w");

    if(fi == NULL || tempIssue == NULL)
    {
        printf("Issue file error.\n");
        return;
    }

    Issue issue;

    int found = 0;

    while(readIssue(fi, &issue))
    {
        if(issue.bookID == bookID &&
           strcmp(issue.studentID, studentID) == 0 &&
           issue.returned == 0)
        {
            issue.returned = 1;
            found = 1;
        }

        writeIssue(tempIssue, issue);
    }

    fclose(fi);
    fclose(tempIssue);

    remove(ISSUE_FILE);
    rename("temp_issues.txt", ISSUE_FILE);

    if(!found)
    {
        printf("Issue record not found.\n");
        return;
    }

    FILE *fp = fopen(BOOK_FILE, "r");
    FILE *tempBook = fopen("temp_books.txt", "w");

    if(fp == NULL || tempBook == NULL)
    {
        printf("Book file error.\n");
        return;
    }

    Book book;

    while(readBook(fp, &book))
    {
        if(book.id == bookID)
        {
            book.available++;
        }

        writeBook(tempBook, book);
    }

    fclose(fp);
    fclose(tempBook);

    remove(BOOK_FILE);
    rename("temp_books.txt", BOOK_FILE);

    printf("\nFine Calculation\n");

    if(calculateFine() > 0)
    {
        int fine = calculateFine();

        printf("Fine Amount: Rs. %d\n", fine);
    }
    else
    {
        printf("No Fine.\n");
    }

    logActivity("Book returned.");

    printf("Book returned successfully.\n");
}

/* =====================================
   VIEW ALL ISSUED BOOKS
   ===================================== */

void viewIssuedBooks()
{
    FILE *fp = fopen(ISSUE_FILE, "r");

    if(fp == NULL)
    {
        printf("No issued books found.\n");
        return;
    }

    Issue issue;

    printf("\n");
    printf("--------------------------------------------------------------------------------------\n");
    printf("%-10s %-15s %-15s %-15s %-12s\n",
           "BookID",
           "StudentID",
           "IssueDate",
           "DueDate",
           "Status");
    printf("--------------------------------------------------------------------------------------\n");

    while(readIssue(fp, &issue))
    {
        printf("%-10d %-15s %-15s %-15s %-12s\n",
               issue.bookID,
               issue.studentID,
               issue.issueDate,
               issue.dueDate,
               issue.returned ? "Returned" : "Issued");
    }

    fclose(fp);
}

/* =====================================
   VIEW STUDENT ISSUED BOOKS
   ===================================== */

void viewStudentIssuedBooks(const char currentUser[])
{
    FILE *fp = fopen(ISSUE_FILE, "r");

    if(fp == NULL)
    {
        printf("No issued books found.\n");
        return;
    }

    Issue issue;
    int found = 0;

    printf("\n");
    printf("--------------------------------------------------------------\n");
    printf("%-10s %-15s %-15s %-12s\n",
           "BookID",
           "IssueDate",
           "DueDate",
           "Status");
    printf("--------------------------------------------------------------\n");

    while(readIssue(fp, &issue))
    {
        if(strcmp(issue.studentID, currentUser) == 0)
        {
            printf("%-10d %-15s %-15s %-12s\n",
                   issue.bookID,
                   issue.issueDate,
                   issue.dueDate,
                   issue.returned ? "Returned" : "Issued");

            found = 1;
        }
    }

    fclose(fp);

    if(!found)
    {
        printf("No books issued to this student.\n");
    }
}

/* =====================================
   SEARCH MENU
   ===================================== */

void searchBookMenu()
{
    int choice;

    printf("\n===== SEARCH BOOK =====\n");
    printf("1. Search By ID\n");
    printf("2. Search By Title\n");
    printf("3. Search By Author\n");
    printf("Enter Choice: ");
    scanf("%d", &choice);

    switch(choice)
    {
        case 1:
            searchBookByID();
            break;

        case 2:
            searchBookByTitle();
            break;

        case 3:
            searchBookByAuthor();
            break;

        default:
            printf("Invalid choice.\n");
    }
}

/* =====================================
   ADMIN MENU
   ===================================== */

void adminMenu()
{
    int choice;

    while(1)
    {
        printf("\n");
        printf("=========================================\n");
        printf("         ADMIN CONTROL PANEL\n");
        printf("=========================================\n");
        printf("1. Dashboard\n");
        printf("2. Add Book\n");
        printf("3. View Books\n");
        printf("4. Search Book\n");
        printf("5. Update Book\n");
        printf("6. Delete Book\n");
        printf("7. Issue Book\n");
        printf("8. Return Book\n");
        printf("9. View Issued Books\n");
        printf("10. View Students\n");
        printf("0. Logout\n");
        printf("=========================================\n");

        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                dashboard();
                break;

            case 2:
                addBook();
                break;

            case 3:
                viewBooks();
                break;

            case 4:
                searchBookMenu();
                break;

            case 5:
                updateBook();
                break;

            case 6:
                deleteBook();
                break;

            case 7:
                issueBook();
                break;

            case 8:
                returnBook();
                break;

            case 9:
                viewIssuedBooks();
                break;

            case 10:
                viewStudents();
                break;

            case 0:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}

/* =====================================
   STUDENT MENU
   ===================================== */

void studentMenu(const char currentUser[])
{
    int choice;

    while(1)
    {
        printf("\n");
        printf("=========================================\n");
        printf("            STUDENT PANEL\n");
        printf("=========================================\n");
        printf("1. View Books\n");
        printf("2. Search Book\n");
        printf("3. My Issued Books\n");
        printf("4. Change Password\n");
        printf("0. Logout\n");
        printf("=========================================\n");

        printf("Enter Choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                viewBooks();
                break;

            case 2:
                searchBookMenu();
                break;

            case 3:
                viewStudentIssuedBooks(currentUser);
                break;

            case 4:
                changePassword(currentUser);
                break;

            case 0:
                return;

            default:
                printf("Invalid choice.\n");
        }
    }
}

/* =====================================
   STARTUP SCREEN
   ===================================== */

void welcomeScreen()
{
    printf("\n");
    printf("=====================================================\n");
    printf("          LIBRARY MANAGEMENT SYSTEM\n");
    printf("=====================================================\n");
    printf("      File Handling + Structures + CSV Storage\n");
    printf("=====================================================\n");
}

/* =====================================
   MAIN FUNCTION
   ===================================== */

int main()
{
    int choice;
    int role;

    char currentUser[20];

    while(1)
    {
        welcomeScreen();

        printf("\n");
        printf("1. Register Student\n");
        printf("2. Login\n");
        printf("0. Exit\n");

        printf("\nEnter Choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                registerStudent();
                break;

            case 2:

                role = login(currentUser);

                if(role == 2)
                {
                    adminMenu();
                }
                else if(role == 1)
                {
                    studentMenu(currentUser);
                }

                break;

            case 0:

                printf("\nThank you for using the system.\n");

                exit(0);

            default:

                printf("Invalid choice.\n");
        }
    }

    return 0;
}