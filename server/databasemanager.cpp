#include "databasemanager.h"
#include <QCoreApplication>
#include <QRandomGenerator>

/*
 * КОНСТРУКТОР: DatabaseManager
 * Создаёт объект SQLite и указывает путь к файлу базы данных.
 * Файл database.db будет создан в папке с исполняемым файлом.
 */
DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/database.db");
}

DatabaseManager::~DatabaseManager() {
    close();  // При разрушении объекта закрываем соединение
}

/*
 * getInstance()
 * Реализация Singleton. Объект создаётся при первом вызове этого метода
 * и живёт до конца программы. Потокобезопасность гарантируется стандартом C++11.
 */
DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

/*
 * connect()
 * Открывает соединение с БД и создаёт все необходимые таблицы.
 * Если таблиц нет — создаёт. Если есть — использует существующие.
 * Также создаёт трёх тестовых пользователей для демонстрации.
 */
bool DatabaseManager::connect() {
    if (!db.open()) {
        qDebug() << "Database connection error:" << db.lastError().text();
        return false;
    }
    qDebug() << "Connected to SQLite!";

    QSqlQuery query;

    /*
     * Таблица users: хранит информацию о пользователях.
     * password_plain хранит пароль в открытом виде — сделано для упрощения
     * демонстрации сброса пароля. В боевом проекте так нельзя.
     */
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT, "
                    "login TEXT UNIQUE, "
                    "password TEXT, "
                    "password_plain TEXT, "
                    "role TEXT DEFAULT 'user', "
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        qDebug() << "Error creating users table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Users table ready";

    /*
     * Таблица calculations: история всех вычислений.
     * Каждая запись привязана к user_id (внешний ключ к users.id).
     */
    if (!query.exec("CREATE TABLE IF NOT EXISTS calculations ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "x REAL, a REAL, b REAL, c REAL, d REAL, e REAL, result REAL, "
                    "user_id INTEGER, "
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        qDebug() << "Error creating calculations table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Calculations table ready";

    /*
     * Таблица stats: накопленная статистика по каждому пользователю.
     * solved — количество успешно решённых задач
     * total — общее количество попыток
     * total_time — суммарное затраченное время (условно, в демо всегда 1)
     */
    if (!query.exec("CREATE TABLE IF NOT EXISTS stats ("
                    "user_id INTEGER PRIMARY KEY, "
                    "solved INTEGER DEFAULT 0, "
                    "total INTEGER DEFAULT 0, "
                    "total_time INTEGER DEFAULT 0)")) {
        qDebug() << "Error creating stats table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Stats table ready";

    /*
     * ==================== ТЕСТОВЫЕ ПОЛЬЗОВАТЕЛИ ====================
     * Создаются только если ещё не существуют.
     */

    // 1. Администратор: login = "admin", password = "admin123"
    QSqlQuery checkAdmin;
    checkAdmin.exec("SELECT COUNT(*) FROM users WHERE login = 'admin'");
    if (checkAdmin.next() && checkAdmin.value(0).toInt() == 0) {
        QSqlQuery ins;
        ins.prepare("INSERT INTO users (name, login, password, password_plain, role) VALUES (?, ?, ?, ?, ?)");
        ins.addBindValue("Administrator");
        ins.addBindValue("admin");
        ins.addBindValue(hashPassword("admin123"));
        ins.addBindValue("admin123");
        ins.addBindValue("admin");
        if (ins.exec()) {
            int uid = ins.lastInsertId().toInt();
            QSqlQuery statIns;
            statIns.prepare("INSERT INTO stats (user_id) VALUES (?)");
            statIns.addBindValue(uid);
            statIns.exec();
            qDebug() << "✅ Admin created: admin / admin123";
        }
    }

    // 2. Обычный пользователь: login = "user", password = "123"
    QSqlQuery checkUser;
    checkUser.exec("SELECT COUNT(*) FROM users WHERE login = 'user'");
    if (checkUser.next() && checkUser.value(0).toInt() == 0) {
        QSqlQuery ins;
        ins.prepare("INSERT INTO users (name, login, password, password_plain, role) VALUES (?, ?, ?, ?, ?)");
        ins.addBindValue("Test User");
        ins.addBindValue("user");
        ins.addBindValue(hashPassword("123"));
        ins.addBindValue("123");
        ins.addBindValue("user");
        if (ins.exec()) {
            int uid = ins.lastInsertId().toInt();
            QSqlQuery statIns;
            statIns.prepare("INSERT INTO stats (user_id) VALUES (?)");
            statIns.addBindValue(uid);
            statIns.exec();
            qDebug() << "✅ User created: user / 123";
        }
    }

    // 3. Тестовый пользователь: login = "test", password = "test"
    QSqlQuery checkTest;
    checkTest.exec("SELECT COUNT(*) FROM users WHERE login = 'test'");
    if (checkTest.next() && checkTest.value(0).toInt() == 0) {
        QSqlQuery ins;
        ins.prepare("INSERT INTO users (name, login, password, password_plain, role) VALUES (?, ?, ?, ?, ?)");
        ins.addBindValue("Test User 2");
        ins.addBindValue("test");
        ins.addBindValue(hashPassword("test"));
        ins.addBindValue("test");
        ins.addBindValue("user");
        if (ins.exec()) {
            int uid = ins.lastInsertId().toInt();
            QSqlQuery statIns;
            statIns.prepare("INSERT INTO stats (user_id) VALUES (?)");
            statIns.addBindValue(uid);
            statIns.exec();
            qDebug() << "✅ Test user created: test / test";
        }
    }

    // Выводим подсказку для тестирования
    qDebug() << "========================================";
    qDebug() << "📋 Тестовые пользователи:";
    qDebug() << "   admin / admin123 (admin)";
    qDebug() << "   user / 123 (user)";
    qDebug() << "   test / test (user)";
    qDebug() << "========================================";

    return true;
}

void DatabaseManager::close() {
    if (db.isOpen()) db.close();
}

bool DatabaseManager::isOpen() const {
    return db.isOpen();
}

/*
 * hashPassword()
 * Преобразует пароль в хэш SHA-256.
 * В базе данных хранится именно хэш, а не сам пароль.
 */
QString DatabaseManager::hashPassword(const QString& password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

/*
 * registerUser()
 * Регистрирует нового пользователя.
 * Проверяет, не занят ли логин, затем вставляет запись в таблицу users
 * и создаёт запись в stats.
 */
bool DatabaseManager::registerUser(const QString& name, const QString& login, const QString& password) {
    if (!db.isOpen()) return false;

    // Проверка уникальности логина
    QSqlQuery check;
    check.prepare("SELECT id FROM users WHERE login = ?");
    check.addBindValue(login);
    if (check.exec() && check.next()) return false;  // Логин уже занят

    // Вставка нового пользователя
    QSqlQuery q;
    q.prepare("INSERT INTO users (name, login, password, password_plain, role) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(name);
    q.addBindValue(login);
    q.addBindValue(hashPassword(password));
    q.addBindValue(password);  // Сохраняем открытый пароль (упрощение для сброса)
    q.addBindValue("user");
    if (!q.exec()) return false;

    // Создаём запись в статистике
    int userId = q.lastInsertId().toInt();
    QSqlQuery q2;
    q2.prepare("INSERT INTO stats (user_id) VALUES (?)");
    q2.addBindValue(userId);
    q2.exec();
    return true;
}

/*
 * loginUser()
 * Проверяет логин и пароль.
 * Сравнивает хэш введённого пароля с хэшем в базе данных.
 * Возвращает user_id при успехе, -1 при ошибке.
 */
int DatabaseManager::loginUser(const QString& login, const QString& password) {
    if (!db.isOpen()) return -1;
    QSqlQuery q;
    q.prepare("SELECT id FROM users WHERE login = ? AND password = ?");
    q.addBindValue(login);
    q.addBindValue(hashPassword(password));
    if (!q.exec()) return -1;
    if (q.next()) return q.value(0).toInt();
    return -1;
}

/*
 * getUserRole()
 * Возвращает роль пользователя: "admin" или "user".
 * По умолчанию возвращает "user".
 */
QString DatabaseManager::getUserRole(int userId) {
    QSqlQuery q;
    q.prepare("SELECT role FROM users WHERE id = ?");
    q.addBindValue(userId);
    if (q.exec() && q.next()) return q.value(0).toString();
    return "user";
}

/*
 * adminResetPassword()
 * Административный сброс пароля.
 * Возвращает текущий пароль пользователя (из password_plain).
 * Используется только администратором.
 */
QString DatabaseManager::adminResetPassword(const QString& login) {
    if (!db.isOpen()) return QString();
    QSqlQuery check;
    check.prepare("SELECT password_plain FROM users WHERE login = ?");
    check.addBindValue(login);
    if (!check.exec() || !check.next()) return QString();
    return check.value(0).toString();
}

/*
 * save()
 * Сохраняет результат вычисления в таблицу calculations.
 * Привязывает запись к user_id.
 */
bool DatabaseManager::save(int userId, double x, double a, double b, double c, double d, double e, double result) {
    if (!db.isOpen()) return false;
    QSqlQuery q;
    q.prepare("INSERT INTO calculations (x, a, b, c, d, e, result, user_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(x);
    q.addBindValue(a);
    q.addBindValue(b);
    q.addBindValue(c);
    q.addBindValue(d);
    q.addBindValue(e);
    q.addBindValue(result);
    q.addBindValue(userId);
    return q.exec();
}

/*
 * getStats()
 * Возвращает статистику пользователя в виде QVector<int> из трёх элементов:
 * [0] = solved (решённые задачи)
 * [1] = total (всего попыток)
 * [2] = total_time (суммарное время)
 */
QVector<int> DatabaseManager::getStats(int userId) {
    QVector<int> res(3, 0);
    QSqlQuery q;
    q.prepare("SELECT solved, total, total_time FROM stats WHERE user_id = ?");
    q.addBindValue(userId);
    if (q.exec() && q.next()) {
        res[0] = q.value(0).toInt();
        res[1] = q.value(1).toInt();
        res[2] = q.value(2).toInt();
    }
    return res;
}

/*
 * updateStats()
 * Обновляет статистику после каждого вычисления.
 * total увеличивается на 1, total_time увеличивается на timeSpent.
 * Если solved == true, увеличивается solved на 1.
 */
void DatabaseManager::updateStats(int userId, bool solved, int timeSpent) {
    QSqlQuery q;
    q.prepare("UPDATE stats SET total = total + 1, total_time = total_time + ? WHERE user_id = ?");
    q.addBindValue(timeSpent);
    q.addBindValue(userId);
    q.exec();

    if (solved) {
        QSqlQuery q2;
        q2.prepare("UPDATE stats SET solved = solved + 1 WHERE user_id = ?");
        q2.addBindValue(userId);
        q2.exec();
    }
}



int DatabaseManager::getUserCount() {
    QSqlQuery q("SELECT COUNT(*) FROM users");
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}

int DatabaseManager::getTodayRegisteredCount() {
    QSqlQuery q("SELECT COUNT(*) FROM users WHERE date(created_at) = date('now')");
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}

int DatabaseManager::getTotalCalculationsCount() {
    QSqlQuery q("SELECT COUNT(*) FROM calculations");
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}

int DatabaseManager::getUserCalculationsCount(int userId) {
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM calculations WHERE user_id = ?");
    q.addBindValue(userId);
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}

int DatabaseManager::getActiveUserCount() {
    QSqlQuery q("SELECT COUNT(DISTINCT user_id) FROM calculations");
    if (q.exec() && q.next()) return q.value(0).toInt();
    return -1;
}

double DatabaseManager::getAvgCalculationsPerUser() {
    QSqlQuery q("SELECT AVG(cnt) FROM (SELECT COUNT(*) as cnt FROM calculations GROUP BY user_id)");
    if (q.exec() && q.next()) return q.value(0).toDouble();
    return -1;
}
