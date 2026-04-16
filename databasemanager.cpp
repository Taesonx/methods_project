#include "databasemanager.h"
#include <QCoreApplication>
#include <QRandomGenerator>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath() + "/database.db");
}

DatabaseManager::~DatabaseManager() {
    close();
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect() {
    if (!db.open()) {
        qDebug() << "Database connection error:" << db.lastError().text();
        return false;
    }
    qDebug() << "Connected to SQLite!";

    QSqlQuery query;

    // Таблица users с полем role
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT, "
                    "login TEXT UNIQUE, "
                    "password TEXT, "
                    "mail TEXT, "
                    "role TEXT DEFAULT 'user', "
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        qDebug() << "Error creating users table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Users table ready";

    if (!query.exec("CREATE TABLE IF NOT EXISTS calculations ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "x REAL, a REAL, b REAL, c REAL, d REAL, e REAL, result REAL, "
                    "user_id INTEGER, "
                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        qDebug() << "Error creating calculations table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Calculations table ready";

    if (!query.exec("CREATE TABLE IF NOT EXISTS stats ("
                    "user_id INTEGER PRIMARY KEY, "
                    "solved INTEGER DEFAULT 0, "
                    "total INTEGER DEFAULT 0, "
                    "total_time INTEGER DEFAULT 0)")) {
        qDebug() << "Error creating stats table:" << query.lastError().text();
        return false;
    }
    qDebug() << "Stats table ready";

    // Создаём администратора, если нет
    QSqlQuery checkAdmin;
    checkAdmin.exec("SELECT COUNT(*) FROM users WHERE login = 'admin'");
    if (checkAdmin.next() && checkAdmin.value(0).toInt() == 0) {
        QString adminPass = hashPassword("admin123");
        QSqlQuery insertAdmin;
        insertAdmin.prepare("INSERT INTO users (name, login, password, mail, role) VALUES (?, ?, ?, ?, ?)");
        insertAdmin.addBindValue("Administrator");
        insertAdmin.addBindValue("admin");
        insertAdmin.addBindValue(adminPass);
        insertAdmin.addBindValue("admin@mail.com");
        insertAdmin.addBindValue("admin");
        if (insertAdmin.exec()) {
            qDebug() << "Admin user created: admin/admin123";
        } else {
            qDebug() << "Admin creation error:" << insertAdmin.lastError().text();
        }
    }

    return true;
}

void DatabaseManager::close() {
    if (db.isOpen()) db.close();
}

bool DatabaseManager::isOpen() const {
    return db.isOpen();
}

QString DatabaseManager::hashPassword(const QString& password) {
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

static QString generateRandomPassword(int length = 8) {
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    QString pw;
    for (int i = 0; i < length; ++i)
        pw.append(chars.at(QRandomGenerator::global()->bounded(chars.length())));
    return pw;
}

bool DatabaseManager::registerUser(const QString& name, const QString& login, const QString& password, const QString& mail) {
    if (!db.isOpen()) return false;

    // Проверка существования
    QSqlQuery check;
    check.prepare("SELECT id FROM users WHERE login = ?");
    check.addBindValue(login);
    if (check.exec() && check.next()) {
        qDebug() << "User already exists:" << login;
        return false;
    }

    QSqlQuery q;
    q.prepare("INSERT INTO users (name, login, password, mail, role) VALUES (?, ?, ?, ?, ?)");
    q.addBindValue(name);
    q.addBindValue(login);
    q.addBindValue(hashPassword(password));
    q.addBindValue(mail);
    q.addBindValue("user");
    if (!q.exec()) {
        qDebug() << "Insert error:" << q.lastError().text();
        return false;
    }

    int userId = q.lastInsertId().toInt();
    QSqlQuery q2;
    q2.prepare("INSERT INTO stats (user_id) VALUES (?)");
    q2.addBindValue(userId);
    q2.exec();
    return true;
}

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

QString DatabaseManager::getUserRole(int userId) {
    QSqlQuery q;
    q.prepare("SELECT role FROM users WHERE id = ?");
    q.addBindValue(userId);
    if (q.exec() && q.next()) return q.value(0).toString();
    return "user";
}

QString DatabaseManager::resetPassword(const QString& login, const QString& email) {
    if (!db.isOpen()) return QString();
    QSqlQuery check;
    check.prepare("SELECT id FROM users WHERE login = ? AND mail = ?");
    check.addBindValue(login);
    check.addBindValue(email);
    if (!check.exec() || !check.next()) return QString();
    int userId = check.value(0).toInt();
    QString newPass = generateRandomPassword(8);
    QSqlQuery upd;
    upd.prepare("UPDATE users SET password = ? WHERE id = ?");
    upd.addBindValue(hashPassword(newPass));
    upd.addBindValue(userId);
    if (!upd.exec()) return QString();
    return newPass;
}

QString DatabaseManager::adminResetPassword(const QString& login) {
    if (!db.isOpen()) return QString();
    QSqlQuery check;
    check.prepare("SELECT id FROM users WHERE login = ?");
    check.addBindValue(login);
    if (!check.exec() || !check.next()) return QString();
    int userId = check.value(0).toInt();
    QString newPass = generateRandomPassword(8);
    QSqlQuery upd;
    upd.prepare("UPDATE users SET password = ? WHERE id = ?");
    upd.addBindValue(hashPassword(newPass));
    upd.addBindValue(userId);
    if (!upd.exec()) return QString();
    return newPass;
}

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
