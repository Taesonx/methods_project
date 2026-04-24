#include "DatabaseManager.h"

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("database.db");
}

DatabaseManager& DatabaseManager::getInstance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect() {
    if (!db.open()) {
        qDebug() << "Ошибка подключения:" << db.lastError().text();
        return false;
    }
    qDebug() << "Подключено к SQLite!";
    return true;
}

void DatabaseManager::close() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Соединение закрыто";
    }
}

QString DatabaseManager::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex();
}

bool DatabaseManager::registerUser(const QString& name, const QString& login, const QString& password, const QString& mail) {
    if (!db.isOpen()) return false;

    QString hashedPassword = hashPassword(password);

    QSqlQuery query;
    query.prepare("INSERT INTO users (name, login, password, mail) "
                  "VALUES (:name, :login, :password, :mail)");

    query.bindValue(":name", name);
    query.bindValue(":login", login);
    query.bindValue(":password", hashedPassword);
    query.bindValue(":mail", mail);

    if (!query.exec()) {
        qDebug() << "Ошибка регистрации:" << query.lastError().text();
        return false;
    }

    qDebug() << "Пользователь" << login << "зарегистрирован!";
    return true;
}

int DatabaseManager::loginUser(const QString& login, const QString& password) {
    if (!db.isOpen()) return -1;

    QString hashedPassword = hashPassword(password);

    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE login = :login AND password = :password");
    query.bindValue(":login", login);
    query.bindValue(":password", hashedPassword);

    if (!query.exec()) {
        qDebug() << "Ошибка входа:" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        int userId = query.value(0).toInt();
        qDebug() << "Пользователь" << login << "вошёл! ID:" << userId;
        return userId;
    }

    qDebug() << "Неверный логин или пароль";
    return -1;
}

bool DatabaseManager::save(int userId, double x, double a, double b, double c, double d, double e, double result) {
    if (!db.isOpen()) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO calculations (x, a, b, c, d, e, result, user_id) "
                  "VALUES (:x, :a, :b, :c, :d, :e, :result, :user_id)");

    query.bindValue(":x", x);
    query.bindValue(":a", a);
    query.bindValue(":b", b);
    query.bindValue(":c", c);
    query.bindValue(":d", d);
    query.bindValue(":e", e);
    query.bindValue(":result", result);
    query.bindValue(":user_id", userId);

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения:" << query.lastError().text();
        return false;
    }

    qDebug() << "Сохранено: f(" << x << ") = " << result << " для пользователя ID:" << userId;
    return true;
}

bool DatabaseManager::resetPassword(const QString& login, const QString& newPassword) {
    if (!db.isOpen()) return false;

    QString hashedPassword = hashPassword(newPassword);

    QSqlQuery query;
    query.prepare("UPDATE users SET password = :password WHERE login = :login");
    query.bindValue(":password", hashedPassword);
    query.bindValue(":login", login);

    if (!query.exec()) {
        qDebug() << "Ошибка сброса пароля:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qDebug() << "Пользователь с логином" << login << "не найден";
        return false;
    }

    qDebug() << "Пароль для" << login << "успешно изменён!";
    return true;
}

int DatabaseManager::getUserCount() {
    if (!db.isOpen()) return -1;
    QSqlQuery query("SELECT COUNT(*) FROM users;");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

int DatabaseManager::getTodayRegisteredCount() {
    if (!db.isOpen()) return -1;
    QSqlQuery query("SELECT COUNT(*) FROM users WHERE date(created_at) = date('now');");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

int DatabaseManager::getTotalCalculationsCount() {
    if (!db.isOpen()) return -1;
    QSqlQuery query("SELECT COUNT(*) FROM calculations;");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

int DatabaseManager::getUserCalculationsCount(int userId) {
    if (!db.isOpen()) return -1;
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM calculations WHERE user_id = ?;");
    query.bindValue(0, userId);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

int DatabaseManager::getActiveUserCount() {
    if (!db.isOpen()) return -1;
    QSqlQuery query("SELECT COUNT(DISTINCT user_id) FROM calculations;");
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

double DatabaseManager::getAvgCalculationsPerUser() {
    if (!db.isOpen()) return -1;
    QSqlQuery query("SELECT AVG(cnt) FROM (SELECT COUNT(*) as cnt FROM calculations GROUP BY user_id);");
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return -1;
}

DatabaseManager::~DatabaseManager() {
    close();
}

