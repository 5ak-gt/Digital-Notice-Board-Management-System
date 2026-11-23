#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
   IMPORTANT:
   Keep this structure exactly the same as the earlier program.
   This allows the program to read the existing notices.dat file.
*/
#define FILE_NAME "notices.dat"
#define TEMP_FILE "temp.dat"

#define TITLE_LEN 100
#define CAT_LEN 30
#define DATE_LEN 20
#define DESC_LEN 500

/* Terminal colours (work in most modern terminals) */
#define RESET   "\033[0m"
#define PURPLE  "\033[1;35m"
#define CYAN    "\033[1;36m"
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define WHITE   "\033[1;37m"

typedef struct {
    int id;
    char title[TITLE_LEN];
    char category[CAT_LEN];
    char date[DATE_LEN];
    char expiry[DATE_LEN];
    char description[DESC_LEN];
} Notice;

/* Read a complete line from the keyboard */
void readLine(char message[], char text[], int size) {
    int ch;

    printf("%s", message);

    if (fgets(text, size, stdin) == NULL) {
        text[0] = '\0';
        return;
    }

    /* Remove the Enter key character */
    if (strchr(text, '\n') == NULL) {
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Remove extra characters */
        }
    } else {
        text[strcspn(text, "\n")] = '\0';
    }
}

/* Read a menu number */
int readNumber(char message[]) {
    char input[30];
    char extra;
    int number;

    while (1) {
        readLine(message, input, sizeof(input));

        if (sscanf(input, "%d %c", &number, &extra) == 1)
            return number;

        printf(RED "Please enter a valid number.\n" RESET);
    }
}

/* Display a heading */
void showHeading(char title[]) {
    printf("\n" PURPLE);
    printf("============================================================\n");
    printf("                 %s\n", title);
    printf("============================================================\n");
    printf(RESET);
}

/* Generate the next unique notice ID */
int getNextId(void) {
    FILE *fp;
    Notice n;
    int largestId = 0;

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL)
        return 1;

    while (fread(&n, sizeof(Notice), 1, fp) == 1) {
        if (n.id > largestId)
            largestId = n.id;
    }

    fclose(fp);
    return largestId + 1;
}

/*
   Convert DD-MM-YYYY into a date value.
   Returns 1 if the date is valid, otherwise 0.
*/
int makeDate(char text[], struct tm *dateValue) {
    int day, month, year;
    struct tm checkDate;
    time_t result;

    if (sscanf(text, "%d-%d-%d", &day, &month, &year) != 3)
        return 0;

    if (day < 1 || day > 31 || month < 1 || month > 12 || year < 1900)
        return 0;

    memset(&checkDate, 0, sizeof(checkDate));
    checkDate.tm_mday = day;
    checkDate.tm_mon = month - 1;
    checkDate.tm_year = year - 1900;
    checkDate.tm_isdst = -1;

    result = mktime(&checkDate);

    if (result == (time_t)-1)
        return 0;

    /* mktime adjusts invalid dates, so check the values again */
    if (checkDate.tm_mday != day ||
        checkDate.tm_mon != month - 1 ||
        checkDate.tm_year != year - 1900)
        return 0;

    *dateValue = checkDate;
    return 1;
}

/* Return 1 if expired, 0 if active, and -1 if expiry date is invalid */
int getNoticeStatus(char expiry[]) {
    struct tm expiryDate, today;
    time_t currentTime;
    struct tm *currentDate;

    if (!makeDate(expiry, &expiryDate))
        return -1;

    currentTime = time(NULL);
    currentDate = localtime(&currentTime);

    if (currentDate == NULL)
        return -1;

    today = *currentDate;
    today.tm_hour = 0;
    today.tm_min = 0;
    today.tm_sec = 0;
    today.tm_isdst = -1;

    if (mktime(&expiryDate) < mktime(&today))
        return 1;

    return 0;
}

/* Display one notice in a formatted box */
void showNotice(Notice n) {
    int status = getNoticeStatus(n.expiry);

    printf(CYAN "\n+----------------------------------------------------------+\n" RESET);
    printf(CYAN "| " WHITE "NOTICE ID   : %-43d" CYAN "|\n" RESET, n.id);
    printf(CYAN "| " WHITE "TITLE       : %-43.43s" CYAN "|\n" RESET, n.title);
    printf(CYAN "| " WHITE "CATEGORY    : %-43.43s" CYAN "|\n" RESET, n.category);
    printf(CYAN "| " WHITE "DATE        : %-43.43s" CYAN "|\n" RESET, n.date);
    printf(CYAN "| " WHITE "EXPIRY DATE : %-43.43s" CYAN "|\n" RESET, n.expiry);

    if (status == 1)
        printf(CYAN "| " RED "STATUS      : %-43s" CYAN "|\n" RESET, "EXPIRED");
    else if (status == 0)
        printf(CYAN "| " GREEN "STATUS      : %-43s" CYAN "|\n" RESET, "ACTIVE");
    else
        printf(CYAN "| " YELLOW "STATUS      : %-43s" CYAN "|\n" RESET, "DATE NOT VALID");

    printf(CYAN "+----------------------------------------------------------+\n" RESET);
    printf(WHITE "DESCRIPTION:\n%s\n" RESET, n.description);
    printf(CYAN "+----------------------------------------------------------+\n" RESET);
}

/* Add a new notice */
void addNotice(void) {
    FILE *fp;
    Notice n;

    n.id = getNextId();

    showHeading("PUBLISH A NEW NOTICE");

    readLine("Notice title: ", n.title, sizeof(n.title));
    readLine("Category (Academic/Event/etc.): ", n.category, sizeof(n.category));
    readLine("Date (DD-MM-YYYY): ", n.date, sizeof(n.date));
    readLine("Expiry date (DD-MM-YYYY): ", n.expiry, sizeof(n.expiry));
    readLine("Description: ", n.description, sizeof(n.description));

    if (n.title[0] == '\0' || n.description[0] == '\0') {
        printf(RED "Title and description cannot be empty.\n" RESET);
        return;
    }

    fp = fopen(FILE_NAME, "ab");

    if (fp == NULL) {
        printf(RED "Could not open the notice file.\n" RESET);
        return;
    }

    if (fwrite(&n, sizeof(Notice), 1, fp) == 1)
        printf(GREEN "\nNotice published successfully! Notice ID: %d\n" RESET, n.id);
    else
        printf(RED "\nError while saving the notice.\n" RESET);

    fclose(fp);
}

/* Display every notice */
void displayNotices(void) {
    FILE *fp;
    Notice n;
    int count = 0;

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL) {
        printf(YELLOW "\nNo notices are available yet.\n" RESET);
        return;
    }

    showHeading("ALL NOTICES");

    while (fread(&n, sizeof(Notice), 1, fp) == 1) {
        showNotice(n);
        count++;
    }

    if (count == 0)
        printf(YELLOW "No notices are available.\n" RESET);

    printf(GREEN "\nTotal notices displayed: %d\n" RESET, count);
    fclose(fp);
}

/* Search notices by ID or keyword */
void searchNotice(void) {
    FILE *fp;
    Notice n;
    int choice, id, found = 0;
    char word[TITLE_LEN];

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL) {
        printf(YELLOW "No notices are available.\n" RESET);
        return;
    }

    showHeading("SEARCH NOTICES");
    printf("1. Search by notice ID\n");
    printf("2. Search by keyword\n");
    choice = readNumber("Choose an option: ");

    if (choice == 1) {
        id = readNumber("Enter notice ID: ");

        while (fread(&n, sizeof(Notice), 1, fp) == 1) {
            if (n.id == id) {
                showNotice(n);
                found = 1;
                break;
            }
        }
    } else if (choice == 2) {
        readLine("Enter keyword: ", word, sizeof(word));

        while (fread(&n, sizeof(Notice), 1, fp) == 1) {
            if (strstr(n.title, word) != NULL ||
                strstr(n.category, word) != NULL ||
                strstr(n.description, word) != NULL) {
                showNotice(n);
                found = 1;
            }
        }
    } else {
        printf(RED "Invalid search option.\n" RESET);
        fclose(fp);
        return;
    }

    if (found == 0)
        printf(YELLOW "\nNo matching notice found.\n" RESET);

    fclose(fp);
}

/* Update a notice */
void updateNotice(void) {
    FILE *fp;
    Notice n;
    int id, found = 0;

    fp = fopen(FILE_NAME, "rb+");

    if (fp == NULL) {
        printf(YELLOW "No notices are available.\n" RESET);
        return;
    }

    showHeading("UPDATE NOTICE");
    id = readNumber("Enter notice ID to update: ");

    while (fread(&n, sizeof(Notice), 1, fp) == 1) {
        if (n.id == id) {
            found = 1;
            printf("\nCurrent notice details:\n");
            showNotice(n);

            printf("\nEnter the replacement details:\n");
            readLine("New title: ", n.title, sizeof(n.title));
            readLine("New category: ", n.category, sizeof(n.category));
            readLine("New date (DD-MM-YYYY): ", n.date, sizeof(n.date));
            readLine("New expiry date (DD-MM-YYYY): ", n.expiry, sizeof(n.expiry));
            readLine("New description: ", n.description, sizeof(n.description));

            if (n.title[0] == '\0' || n.description[0] == '\0') {
                printf(RED "Title and description cannot be empty.\n" RESET);
                fclose(fp);
                return;
            }

            /* Move back to the record and replace it */
            if (fseek(fp, -(long)sizeof(Notice), SEEK_CUR) == 0 &&
                fwrite(&n, sizeof(Notice), 1, fp) == 1) {
                printf(GREEN "Notice updated successfully.\n" RESET);
            } else {
                printf(RED "Could not update the notice.\n" RESET);
            }

            break;
        }
    }

    if (found == 0)
        printf(YELLOW "Notice ID not found.\n" RESET);

    fclose(fp);
}

/* Delete a notice after asking for confirmation */
void deleteNotice(void) {
    FILE *fp, *temp;
    Notice n;
    int id, found = 0;
    char answer[10];

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL) {
        printf(YELLOW "No notices are available.\n" RESET);
        return;
    }

    showHeading("DELETE NOTICE");
    id = readNumber("Enter notice ID to delete: ");

    /* First check whether the notice exists */
    while (fread(&n, sizeof(Notice), 1, fp) == 1) {
        if (n.id == id) {
            showNotice(n);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (found == 0) {
        printf(YELLOW "Notice ID not found.\n" RESET);
        return;
    }

    readLine(RED "Are you sure you want to delete this notice? (Y/N): " RESET,
             answer, sizeof(answer));

    if (answer[0] != 'Y' && answer[0] != 'y') {
        printf(YELLOW "Deletion cancelled.\n" RESET);
        return;
    }

    fp = fopen(FILE_NAME, "rb");
    temp = fopen(TEMP_FILE, "wb");

    if (fp == NULL || temp == NULL) {
        printf(RED "Could not open the required files.\n" RESET);
        if (fp != NULL) fclose(fp);
        if (temp != NULL) fclose(temp);
        return;
    }

    while (fread(&n, sizeof(Notice), 1, fp) == 1) {
        if (n.id != id)
            fwrite(&n, sizeof(Notice), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    /* Replace the old file with the temporary file */
    if (remove(FILE_NAME) != 0) {
        printf(RED "Could not replace the notice file.\n" RESET);
        remove(TEMP_FILE);
        return;
    }

    if (rename(TEMP_FILE, FILE_NAME) != 0) {
        printf(RED "Could not rename the temporary file.\n" RESET);
        return;
    }

    printf(GREEN "Notice deleted successfully.\n" RESET);
}

/* Count total, active, expired and category-wise notices */
void showStatistics(void) {
    FILE *fp;
    Notice n;
    char categories[50][CAT_LEN];
    int categoryCounts[50] = {0};
    int categoryTotal = 0;
    int total = 0, active = 0, expired = 0, i, found;
    int status;

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL) {
        printf(YELLOW "\nNo notice data found.\n" RESET);
        return;
    }

    while (fread(&n, sizeof(Notice), 1, fp) == 1) {
        total++;

        status = getNoticeStatus(n.expiry);
        if (status == 1)
            expired++;
        else
            active++;

        found = 0;
        for (i = 0; i < categoryTotal; i++) {
            if (strcmp(categories[i], n.category) == 0) {
                categoryCounts[i]++;
                found = 1;
                break;
            }
        }

        if (found == 0 && categoryTotal < 50) {
            strcpy(categories[categoryTotal], n.category);
            categoryCounts[categoryTotal] = 1;
            categoryTotal++;
        }
    }

    fclose(fp);

    showHeading("NOTICE STATISTICS");
    printf(WHITE "Total notices   : " CYAN "%d\n" RESET, total);
    printf(WHITE "Active notices  : " GREEN "%d\n" RESET, active);
    printf(WHITE "Expired notices : " RED "%d\n" RESET, expired);

    printf("\n" PURPLE "CATEGORY-WISE NOTICE COUNT\n" RESET);
    printf("----------------------------------------\n");

    if (categoryTotal == 0)
        printf("No categories available.\n");

    for (i = 0; i < categoryTotal; i++)
        printf("%-25s : %d\n", categories[i], categoryCounts[i]);

    printf("----------------------------------------\n");
}

/* Display only active or expired notices */
void displayByStatus(int wantedStatus) {
    FILE *fp;
    Notice n;
    int count = 0;

    fp = fopen(FILE_NAME, "rb");

    if (fp == NULL) {
        printf(YELLOW "No notices are available.\n" RESET);
        return;
    }

    if (wantedStatus == 0)
        showHeading("ACTIVE NOTICES");
    else
        showHeading("EXPIRED NOTICES");

    while (fread(&n, sizeof(Notice), 1, fp) == 1) {
        if (getNoticeStatus(n.expiry) == wantedStatus) {
            showNotice(n);
            count++;
        }
    }

    if (count == 0)
        printf(YELLOW "No matching notices found.\n" RESET);

    printf(GREEN "\nNotices found: %d\n" RESET, count);
    fclose(fp);
}


/* Pause so the user can read the result before the menu returns */
void pauseScreen(void) {
    char temp[10];

    printf("\n" YELLOW "Press Enter to continue..." RESET);
    fgets(temp, sizeof(temp), stdin);
}

/* Administrator dashboard */
void adminMenu(void) {
    int choice;

    do {
        showHeading("ADMINISTRATOR DASHBOARD");
        showStatistics();

        printf("\n" CYAN "ADMIN MENU\n" RESET);
        printf("1. Add New Notice\n");
        printf("2. Display All Notices\n");
        printf("3. Search Notice\n");
        printf("4. Update Notice\n");
        printf("5. Delete Notice\n");
        printf("6. View Active Notices\n");
        printf("7. View Expired Notices\n");
        printf("8. View Notice Statistics\n");
        printf("0. Logout\n");

        choice = readNumber("\nEnter your choice: ");

        switch (choice) {
            case 1: addNotice(); break;
            case 2: displayNotices(); break;
            case 3: searchNotice(); break;
            case 4: updateNotice(); break;
            case 5: deleteNotice(); break;
            case 6: displayByStatus(0); break;
            case 7: displayByStatus(1); break;
            case 8: showStatistics(); break;
            case 0: printf(GREEN "Administrator logged out.\n" RESET); break;
            default: printf(RED "Invalid choice. Please try again.\n" RESET);
        }

        /* Keep the result visible before showing the dashboard again */
        if (choice != 0) {
            pauseScreen();
        }
    } while (choice != 0);
}

/* Student menu */
void studentMenu(void) {
    int choice;

    do {
        showHeading("STUDENT NOTICE BOARD");

        printf(CYAN "STUDENT MENU\n" RESET);
        printf("1. View All Notices\n");
        printf("2. Search Notice by ID or Keyword\n");
        printf("0. Back to Main Menu\n");

        choice = readNumber("\nEnter your choice: ");

        switch (choice) {
            case 1: displayNotices(); break;
            case 2: searchNotice(); break;
            case 0: printf(GREEN "Returning to main menu.\n" RESET); break;
            default: printf(RED "Invalid choice. Please try again.\n" RESET);
        }

        /* Keep the result visible before showing the student menu again */
        if (choice != 0) {
            pauseScreen();
        }
    } while (choice != 0);
}

/* Main function: program starts here */
int main(void) {
    int choice;
    char password[50];

    showHeading("DIGITAL NOTICE BOARD MANAGEMENT SYSTEM");
    printf(CYAN "Welcome! Please select how you want to continue.\n" RESET);

    do {
        printf("\n" PURPLE "============== MAIN MENU ==============\n" RESET);
        printf("1. Administrator Login\n");
        printf("2. Student Access\n");
        printf("0. Exit Program\n");

        choice = readNumber("\nEnter your choice: ");

        switch (choice) {
            case 1:
                readLine("Enter administrator password: ",
                         password, sizeof(password));

                /* Demonstration password */
                if (strcmp(password, "ACP123") == 0) {
                    printf(GREEN "\nLogin successful!\n" RESET);
                    adminMenu();
                } else {
                    printf(RED "\nIncorrect password. Access denied.\n" RESET);
                }
                break;

            case 2:
                studentMenu();
                break;

            case 0:
                printf(GREEN "\nThank you for using Digital Notice Board!\n" RESET);
                break;

            default:
                printf(RED "Invalid choice. Please try again.\n" RESET);
        }
    } while (choice != 0);

    return 0;
}
