#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <QString>

/*
 * КЛАСС: RequestHandler
 * Назначение: Реализует всю бизнес-логику сервера.
 *
 * Протокол обмена (команды от клиента):
 *   - reg&login&password                     → регистрация
 *   - auth&login&password                    → авторизация
 *   - stat                                   → статистика пользователя
 *   - check&...&x&a&b&c&d&e                 → вычисление функции
 *   - admin_stats                            → общая статистика (только admin)
 *   - admin_reset&login                      → сброс пароля (только admin)
 *   - params&a&b&c&d&e                      → обновление параметров (логируется)
 *
 * Формат ответов:
 *   - "reg+&login" или "reg-"
 *   - "auth+&login&role" или "auth-"
 *   - "stat&solved$total&time"
 *   - "check+" или "check-"
 *   - "admin_stats&users&calcs&active&avg"
 *   - "admin_reset+&password" или "admin_reset-"
 */
class RequestHandler {
public:
    RequestHandler();

    /*
     * Главный метод обработки запроса.
     * @param request   строка запроса от клиента
     * @param userId    ссылка на user_id (меняется при авторизации)
     * @param role      ссылка на роль (меняется при авторизации)
     * @return          строка ответа для клиента
     */
    QString handle(const QString& request, int& userId, QString& role);

private:
    // Обработчики конкретных команд
    QString handleAuth(const QString& login, const QString& password, int& userId, QString& role);
    QString handleReg(const QString& login, const QString& password);           // 2 параметра (без почты)
    QString handleStat(int userId);
    QString handleCheck(int userId, double x, double a, double b, double c, double d, double e);
    QString handleAdminStats();
    QString handleAdminReset(const QString& login);
};

#endif // REQUEST_HANDLER_H
