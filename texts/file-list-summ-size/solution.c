#include <stdio.h>
#include <sys/stat.h>

int main() {
    char filename[1024];
    struct stat st;
    long long total_size = 0;

    // Считываем имена файлов из стандартного ввода
    while (scanf("%1023s", filename) != EOF) {
        // Проверяем, является ли файл регулярным
        if (lstat(filename, &st) == 0 && S_ISREG(st.st_mode)) {
            total_size += st.st_size; // Добавляем размер файла
        }
    }

    // Выводим общий размер файлов
    printf("%lld\n", total_size);
    return 0;
}
