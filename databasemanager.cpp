#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QSqlError>

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("function29.db");
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

    QSqlQuery query;
    QString createUsers = "CREATE TABLE IF NOT EXISTS users ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "name TEXT NOT NULL, "
                          "login TEXT NOT NULL UNIQUE, "
                          "password TEXT NOT NULL, "
                          "mail TEXT NOT NULL)";

    QString createCalculations = "CREATE TABLE IF NOT EXISTS calculations ("
                                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                 "x REAL NOT NULL, "
                                 "a REAL NOT NULL, "
                                 "b REAL NOT NULL, "
                                 "c REAL NOT NULL, "
                                 "d REAL NOT NULL, "
                                 "e REAL NOT NULL, "
                                 "result REAL NOT NULL, "
                                 "user_id INTEGER)";

    if (!query.exec(createUsers)) {
        qDebug() << "Ошибка создания users:" << query.lastError().text();
    }
    if (!query.exec(createCalculations)) {
        qDebug() << "Ошибка создания calculations:" << query.lastError().text();
    }

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

DatabaseManager::~DatabaseManager() {
    close();
}
