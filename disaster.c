#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "victims.csv"
#define MAX_NAME 50
#define MAX_LOCATION 50
#define MAX_NEEDS 100

typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
    char gender[10];
    char location[MAX_LOCATION];
    char status[20];
    char needs[MAX_NEEDS];
} Victim;

void addRecord();
void listRecords();
void searchByLocation();
void generateHtmlReport();
int getNextId();
void clearInput();
void ensureHeader();

int main() {
    int choice;
    while (1) {
        printf("\n========================================= Disaster Data Logger ======================================\n");
        printf("1. Add new victim record\n");
        printf("2. List all records\n");
        printf("3. Search by location\n");
        printf("4. Generate HTML report\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            clearInput();
            printf("Invalid input.\n");
            continue;
        }

        switch (choice) {
            case 1: addRecord(); break;
            case 2: listRecords(); break;
            case 3: searchByLocation(); break;
            case 4: generateHtmlReport(); break;
            case 5: printf("Exiting...\n"); return 0;
            default: printf("Invalid choice. Try again.\n");
        }
    }
}

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int getNextId() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) return 1;   // first record

    char line[512];
    int lastId = 0;

    // read first line (might be header)
    if (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "ID,", 3) != 0) {
            int id;
            if (sscanf(line, "%d,", &id) == 1) lastId = id;
        }
    }

    // read remaining lines, keep last ID
    while (fgets(line, sizeof(line), fp)) {
        int id;
        if (sscanf(line, "%d,", &id) == 1) lastId = id;
    }

    fclose(fp);
    return lastId + 1;
}

void ensureHeader() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) {
        fp = fopen(DATA_FILE, "w");
        if (!fp) {
            printf("Error creating data file!\n");
            return;
        }
        fprintf(fp, "ID,Name,Age,Gender,Location,Status,Needs\n");
        fclose(fp);
    } else {
        fclose(fp);
    }
}

void addRecord() {
    ensureHeader();

    Victim v;
    v.id = getNextId();

    printf("Enter name: ");
    clearInput();
    fgets(v.name, MAX_NAME, stdin);
    v.name[strcspn(v.name, "\n")] = 0;

    printf("Enter age: ");
    scanf("%d", &v.age);

    printf("Enter gender (M/F/O): ");
    clearInput();
    fgets(v.gender, sizeof(v.gender), stdin);
    v.gender[strcspn(v.gender, "\n")] = 0;

    printf("Enter location (village/city/camp name): ");
    fgets(v.location, MAX_LOCATION, stdin);
    v.location[strcspn(v.location, "\n")] = 0;

    printf("Enter status (Injured/Missing/Safe/Critical): ");
    fgets(v.status, sizeof(v.status), stdin);
    v.status[strcspn(v.status, "\n")] = 0;

    printf("Enter immediate needs (Food/Water/Shelter/Medical etc): ");
    fgets(v.needs, MAX_NEEDS, stdin);
    v.needs[strcspn(v.needs, "\n")] = 0;

    FILE *fp = fopen(DATA_FILE, "a");
    if (!fp) {
        printf("Error opening data file for writing!\n");
        return;
    }

    fprintf(fp, "%d,%s,%d,%s,%s,%s,%s\n",
            v.id, v.name, v.age, v.gender,
            v.location, v.status, v.needs);
    fclose(fp);

    printf("Record added with ID %d.\n", v.id);
}

void listRecords() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) {
        printf("No data file found. Add some records first.\n");
        return;
    }
    char line[512];
    printf("\n---- All Records (CSV Format) ----\n");
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

void searchByLocation() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) {
        printf("No data file found.\n");
        return;
    }
    char query[MAX_LOCATION];
    printf("Enter location to search: ");
    clearInput();
    fgets(query, MAX_LOCATION, stdin);
    query[strcspn(query, "\n")] = 0;

    char line[512];
    int found = 0;

    // read header
    if (!fgets(line, sizeof(line), fp)) {
        printf("File is empty.\n");
        fclose(fp);
        return;
    }

    printf("\nID | Name | Age | Gender | Location | Status | Needs\n");
    printf("---------------------------------------------------------\n");
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, query)) {
            printf("%s", line);
            found = 1;
        }
    }

    if (!found) {
        printf("No records found for location: %s\n", query);
    }
    fclose(fp);
}

void generateHtmlReport() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) {
        printf("No data file found. Cannot generate report.\n");
        return;
    }

    FILE *html = fopen("report.html", "w");
    if (!html) {
        printf("Could not create HTML file.\n");
        fclose(fp);
        return;
    }

    fprintf(html,
        "<!DOCTYPE html>\n"
        "<html lang='en'>\n"
        "<head>\n"
        "<meta charset='UTF-8'>\n"
        "<title>Disaster Victim Report</title>\n"
        "<style>\n"
        "body { font-family: Arial, sans-serif; margin: 20px; background: #f7f7f7; }\n"
        "h1 { text-align: center; }\n"
        "table { border-collapse: collapse; width: 100%%; background: #fff; }\n"
        "th, td { border: 1px solid #ccc; padding: 8px; text-align: left; }\n"
        "th { background: #333; color: #fff; }\n"
        "tr:nth-child(even) { background: #f2f2f2; }\n"
        ".footer { margin-top: 20px; font-size: 0.9em; color: #555; }\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Disaster Victim Report</h1>\n"
        "<table>\n"
    );

    char line[512];
    int isHeader = 1;
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = 0; // remove newline

        char *token;
        if (isHeader) {
            fprintf(html, "<tr>\n");
            token = strtok(line, ",");
            while (token) {
                fprintf(html, "<th>%s</th>\n", token);
                token = strtok(NULL, ",");
            }
            fprintf(html, "</tr>\n");
            isHeader = 0;
        } else {
            fprintf(html, "<tr>\n");
            token = strtok(line, ",");
            while (token) {
                fprintf(html, "<td>%s</td>\n", token);
                token = strtok(NULL, ",");
            }
            fprintf(html, "</tr>\n");
        }
    }

    fprintf(html,
        "</table>\n"
        "<div class='footer'>Generated by Disaster Data Logger (C Program)</div>\n"
        "</body>\n"
        "</html>\n"
    );

    fclose(fp);
    fclose(html);

    printf("HTML report generated as report.html. Open it in your browser.\n");
}