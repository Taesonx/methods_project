#include "request_handler.h"
#include "databasemanager.h"
#include "function29.h"
#include <QDebug>
#include <QStringList>

RequestHandler::RequestHandler() {}

/*
 * handle()
 * Основной диспетчер запросов. Разбирает команду и вызывает соответствующий
 * обработчик.
 *
 * Алгоритм:
 *   1. Разбить запрос на части по символу '&'
 *   2. Первая часть — команда
 *   3. Проверить команду и вызвать нужный метод
 *   4. Некоторые команды доступны без авторизации (reg, auth, params)
 *   5. Команды stat и check требуют user_id != -1
 *   6. Админские команды требуют role == "admin"
 */
QString RequestHandler::handle(const QString& request, int& userId, QString& role)
{
    QStringList parts = request.split('&');
    if (parts.isEmpty()) return "ERROR: empty request";

    QString cmd = parts[0];
    qDebug() << "Command:" << cmd;

    /*
     * ========== КОМАНДЫ БЕЗ АУТЕНТИФИКАЦИИ ==========
     */

    // Регистрация: reg&login&password (2 параметра)
    if (cmd == "reg") {
        if (parts.size() < 3) return "reg-";
        return handleReg(parts[1], parts[2]);   // ← 2 аргумента
    }

    // Авторизация: auth&login&password
    if (cmd == "auth") {
        if (parts.size() < 3) return "auth-";
        return handleAuth(parts[1], parts[2], userId, role);
    }

    // Обновление параметров: params&a&b&c&d&e
    if (cmd == "params") {
        if (parts.size() >= 6) {
            double a = parts[1].toDouble();
            double b = parts[2].toDouble();
            double c = parts[3].toDouble();
            double d = parts[4].toDouble();
            double e = parts[5].toDouble();
            qDebug() << "params updated:" << a << b << c << d << e;
            return "params+";
        }
        return "params-";
    }

    /*
     * ========== КОМАНДЫ, ТРЕБУЮЩИЕ АВТОРИЗАЦИЮ ==========
     */
    if (userId == -1) return "ERROR: not authenticated";

    // Получение статистики: stat
    if (cmd == "stat") {
        return handleStat(userId);
    }

    // Вычисление функции: check&...&x&a&b&c&d&e
    if (cmd == "check") {
        if (parts.size() < 9) return "check-";
        double x = parts[3].toDouble();
        double a = parts[4].toDouble();
        double b = parts[5].toDouble();
        double c = parts[6].toDouble();
        double d = parts[7].toDouble();
        double e = parts[8].toDouble();
        return handleCheck(userId, x, a, b, c, d, e);
    }

    /*
     * ========== АДМИНСКИЕ КОМАНДЫ (требуют role "admin") ==========
     */

    // Общая статистика: admin_stats
    if (cmd == "admin_stats") {
        if (role != "admin") return "ERROR: access denied";
        return handleAdminStats();
    }

    // Админский сброс пароля: admin_reset&login
    if (cmd == "admin_reset") {
        if (role != "admin") return "ERROR: access denied";
        if (parts.size() < 2) return "ERROR: need login";
        return handleAdminReset(parts[1]);
    }

    return "ERROR: unknown command";
}

/*
 * handleAuth()
 * Проверяет логин и пароль через DatabaseManager.
 * При успехе обновляет userId и role (переданные по ссылке),
 * возвращает "auth+&login&role".
 * При ошибке возвращает "auth-".
 */
QString RequestHandler::handleAuth(const QString& login, const QString& password, int& userId, QString& role)
{
    int id = DatabaseManager::getInstance().loginUser(login, password);
    if (id != -1) {
        userId = id;
        role = DatabaseManager::getInstance().getUserRole(id);
        return "auth+&" + login + "&" + role;
    }
    return "auth-";
}

/*
 * handleReg()
 * Регистрирует нового пользователя с указанными логином и паролем.
 * Возвращает "reg+&login" при успехе, "reg-" при ошибке (логин уже занят и т.д.).
 *
 */
QString RequestHandler::handleReg(const QString& login, const QString& password)
{
    bool ok = DatabaseManager::getInstance().registerUser(login, login, password);
    return ok ? "reg+&" + login : "reg-";
}

/*
 * handleStat()
 * Возвращает статистику текущего пользователя.
 * Формат: "stat&solved$total&time"
 */
QString RequestHandler::handleStat(int userId)
{
    QVector<int> stats = DatabaseManager::getInstance().getStats(userId);
    return QString("stat&%1$%2&%3").arg(stats[0]).arg(stats[1]).arg(stats[2]);
}

/*
 * handleCheck()
 * Вычисляет значение функции f29(x, a, b, c, d, e),
 * сохраняет результат в БД и обновляет статистику.
 * Возвращает "check+" при успехе.
 */
QString RequestHandler::handleCheck(int userId, double x, double a, double b, double c, double d, double e)
{
    double result = calculateFunction(x, a, b, c, d, e);
    DatabaseManager::getInstance().save(userId, x, a, b, c, d, e, result);
    DatabaseManager::getInstance().updateStats(userId, true, 1);
    return "check+";
}

/*
 * handleAdminStats()
 * Возвращает общую статистику по системе:
 *   - количество пользователей
 *   - общее количество вычислений
 *   - количество активных пользователей
 *   - среднее количество вычислений на пользователя
 * Формат: "admin_stats&users&calcs&active&avg"
 */
QString RequestHandler::handleAdminStats()
{
    int userCount = DatabaseManager::getInstance().getUserCount();
    int totalCalcs = DatabaseManager::getInstance().getTotalCalculationsCount();
    int activeUsers = DatabaseManager::getInstance().getActiveUserCount();
    double avgCalcs = DatabaseManager::getInstance().getAvgCalculationsPerUser();
    return QString("admin_stats&%1&%2&%3&%4")
        .arg(userCount)
        .arg(totalCalcs)
        .arg(activeUsers)
        .arg(avgCalcs, 0, 'f', 2);
}

/*
 * handleAdminReset()
 * Административный сброс пароля.
 * Возвращает текущий пароль пользователя (из password_plain).
 * Формат: "admin_reset+&password" или "admin_reset-"
 */
QString RequestHandler::handleAdminReset(const QString& login)
{
    QString pass = DatabaseManager::getInstance().adminResetPassword(login);
    return pass.isEmpty() ? "admin_reset-" : "admin_reset+&" + pass;
}
