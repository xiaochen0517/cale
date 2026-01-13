#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

struct Date {
    int year;
    int month;
    int day;
};

void get_current_date(struct Date *date) {
    const time_t now = time(NULL);
    const struct tm *local_time = localtime(&now);

    const int year = local_time->tm_year + 1900;
    const int month = local_time->tm_mon + 1;
    const int day = local_time->tm_mday;

    date->year = year;
    date->month = month;
    date->day = day;
}

int get_weekday_by_first_day(const struct Date *date) {
    struct tm time_info = {0};
    time_info.tm_year = date->year - 1900;
    time_info.tm_mon = date->month - 1;
    time_info.tm_mday = 1;
    time_info.tm_hour = 0;
    time_info.tm_min = 0;
    time_info.tm_sec = 0;

    const time_t timestamp = mktime(&time_info);
    const struct tm *result = localtime(&timestamp);
    const int weekday = result->tm_wday;
    return weekday;
}

const int DAY_IN_MONTH[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

int get_month_day_size(const struct Date *date) {
    if (date->month < 1 || date->month > 12) {
        return -1;
    }
    if (date->month == 2) {
        // 1. 可以被4整除但不可以被100整除
        // 2. 可以被400整除
        if ((date->year % 4 == 0 && date->year % 100 != 0) || (date->year % 400 == 0)) {
            return 29;
        }
    }
    return DAY_IN_MONTH[date->month];
}

const char *WEEKDAY_HEADER[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

void print_headers() {
    for (int index = 0; index < 7; ++index) {
        const char *header_str = WEEKDAY_HEADER[index];
        if (index < 5) {
            printf("%s%s%s\t", RED, header_str, RESET);
        } else {
            printf("%s%s%s\t", YELLOW, header_str, RESET);
        }
    }
    printf("\n");
}

void print_cale(const struct Date *date, const int weekday, const int day_size) {
    int day = 1;
    for (int row_index = 0; row_index < 5; ++row_index) {
        if (day > day_size) {
            break;
        }
        for (int column_index = 0; column_index < 7; ++column_index) {
            if (row_index == 0 && column_index < weekday - 1) {
                printf("\t");
                continue;
            }
            if (day == date->day) {
                printf("%s%d%s\t", GREEN, day, RESET);
            } else if (column_index >= 5) {
                printf("%s%d%s\t", YELLOW, day, RESET);
            } else {
                printf("%d\t", day);
            }
            day++;
            if (day > day_size) {
                break;
            }
        }
        printf("\n");
    }
}

int parse_to_int(const char *str) {
    char *endptr;
    errno = 0;
    const long long_num = strtol(str, &endptr, 10);
    if (errno == ERANGE || long_num > INT_MAX || long_num < INT_MIN) {
        fprintf(stderr, "Parse integer overflow error: %s\n", str);
        exit(1);
    }
    if (endptr == str || (*endptr != '\0' && !isspace(*endptr))) {
        fprintf(stderr, "Parse integer invalid character error: %s\n", str);
        exit(1);
    }
    return (int) long_num;
}

void check_date_info(const struct Date *date) {
    if (date->year < 1900) {
        perror("The year cannot less than 1900.");
        exit(1);
    }
    if (date->month < 1 || date->month > 12) {
        perror("The month must be between 1 and 12.");
        exit(1);
    }
}

int main(const int argc, char *argv[]) {
    if (argc != 1 && argc != 3) {
        perror("Usage: cale <year> <month> or cale.");
        exit(1);
    }

    struct Date date;
    if (argc == 3) {
        date.year = parse_to_int(argv[1]);
        date.month = parse_to_int(argv[2]);
        date.day = 0;
        check_date_info(&date);
    } else {
        get_current_date(&date);
    }

    const int weekday = get_weekday_by_first_day(&date);
    const int day_size = get_month_day_size(&date);

    // 开始打印日历
    print_headers();
    print_cale(&date, weekday, day_size);

    printf("\n");
    return 0;
}
