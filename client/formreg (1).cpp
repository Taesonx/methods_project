#include "formreg.h"
#include "ui_formreg.h"
#include <QMessageBox>

//КОНСТРУКТОР ФОРМЫ РЕГИСТРАЦИИ
FormReg::FormReg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReg),
    isPasswordVisible(false)      // Изначально пароль скрыт (показываем точечками)
{
    ui->setupUi(this);             // Загружаем визуальные элементы из .ui файла

    // Подключаем сигнал нажатия кнопки "Зарегистрироваться" к слоту
    connect(ui->registerButton, &QPushButton::clicked,
            this, &FormReg::onRegisterButtonClicked);

    // Подключаем сигнал нажатия кнопки "Назад" к слоту
    connect(ui->backButton, &QPushButton::clicked,
            this, &FormReg::onBackButtonClicked);

    // Подключаем сигнал нажатия кнопки-глаза к слоту показа/скрытия пароля
    connect(ui->togglePasswordButton, &QPushButton::clicked,
            this, &FormReg::onTogglePassword);
}


//ДЕСТРУКТОР
FormReg::~FormReg()
{
    delete ui;
}


//ОБРАБОТЧИК КНОПКИ-ГЛАЗА (ПОКАЗАТЬ/СКРЫТЬ ПАРОЛЬ)
void FormReg::onTogglePassword()
{
    if (isPasswordVisible) {
        // СЛУЧАЙ 1: пароль сейчас виден → нужно скрыть
        ui->passwordEdit->setEchoMode(QLineEdit::Password);  // Режим "точечки"
        ui->togglePasswordButton->setText("👁️");            // Закрытый глаз
        isPasswordVisible = false;                           // Обновляем флаг
    } else {
        // СЛУЧАЙ 2: пароль сейчас скрыт → нужно показать
        ui->passwordEdit->setEchoMode(QLineEdit::Normal);    // Режим "обычный текст"
        ui->togglePasswordButton->setText("🙈");             // Открытый глаз
        isPasswordVisible = true;                            // Обновляем флаг
    }
}


//ВСПОМОГАТЕЛЬНЫЙ МЕТОД ДЛЯ ПОКАЗА ОШИБОК
void FormReg::showError(const QString& title, const QString& message)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("❌ " + title);                    // Заголовок с крестиком
    msgBox.setText("<b style='font-size:14px; color:#c62828;'>" + message + "</b>"); // Красный текст
    msgBox.setIcon(QMessageBox::Critical);                   // Иконка "критическая ошибка"
    msgBox.setStandardButtons(QMessageBox::Ok);              // Только кнопка OK
    msgBox.setMinimumSize(350, 150);                         // Минимальный размер окна
    msgBox.exec();                                           // Показываем и ждём нажатия OK
}


//ПРОВЕРКА КОРРЕКТНОСТИ EMAIL
bool FormReg::isValidEmail(const QString& email)
{
    if (email.isEmpty()) return false;               // Пустая строка → невалидно

    int atPos = email.indexOf('@');                  // Ищем позицию @
    if (atPos == -1) return false;                   // Нет @ → невалидно
    if (atPos == 0 || atPos == email.length() - 1) return false;  // @ в начале или в конце

    int dotPos = email.indexOf('.', atPos);          // Ищем точку после @
    if (dotPos == -1) return false;                  // Нет точки → невалидно
    if (dotPos == atPos + 1) return false;           // Точка сразу после @ → невалидно
    if (dotPos == email.length() - 1) return false;  // Точка в конце → невалидно

    return true;                                      // Все проверки пройдены
}


//ОБРАБОТЧИК КНОПКИ "ЗАРЕГИСТРИРОВАТЬСЯ"
void FormReg::onRegisterButtonClicked()
{
    // Получаем данные из полей ввода
    QString login = ui->loginEdit->text();
    QString password = ui->passwordEdit->text();
    QString email = ui->emailEdit->text();

    // ПРОВЕРКА 1: логин не пустой
    if (login.isEmpty()) {
        showError("Ошибка", "Введите логин!");
        ui->loginEdit->setFocus();          // Устанавливаем курсор в поле логина
        return;
    }

    // ПРОВЕРКА 2: пароль не пустой
    if (password.isEmpty()) {
        showError("Ошибка", "Введите пароль!");
        ui->passwordEdit->setFocus();       // Устанавливаем курсор в поле пароля
        return;
    }

    // ПРОВЕРКА 3: email не пустой
    if (email.isEmpty()) {
        showError("Ошибка", "Введите email!");
        ui->emailEdit->setFocus();          // Устанавливаем курсор в поле email
        return;
    }

    // ПРОВЕРКА 4: email имеет корректный формат
    if (!isValidEmail(email)) {
        showError("Ошибка", "Введите корректный email!\nПример: user@example.com");
        ui->emailEdit->setFocus();          // Фокус на поле email
        ui->emailEdit->selectAll();         // Выделяем весь текст для удобной замены
        return;
    }

    // Все проверки пройдены → отправляем сигнал в контроллер
    emit registerAttempt(login, password, email);
}


//ОБРАБОТЧИК КНОПКИ "НАЗАД"
void FormReg::onBackButtonClicked()
{
    emit backToAuthRequested();   // Сообщаем контроллеру: вернуться к авторизации
}


//СЛОТ — УСПЕШНАЯ РЕГИСТРАЦИЯ
void FormReg::onRegisterSuccess()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("✅ УСПЕХ!");     // Заголовок с галочкой
    msgBox.setText("<b style='font-size:16px; color:#2e7d32;'>✨ Регистрация прошла успешно! ✨</b>");
    msgBox.setInformativeText("<b>Теперь вы можете войти в систему</b>\n\nИспользуйте свои логин и пароль для входа.");
    msgBox.setIcon(QMessageBox::Information);      // Иконка "информация"
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setMinimumSize(400, 250);
    msgBox.exec();                                 // Показываем сообщение

    this->hide();                                  // Скрываем форму регистрации
}


//СЛОТ — ОШИБКА РЕГИСТРАЦИИ
void FormReg::onRegisterFailed(const QString& error)
{
    showError("Ошибка регистрации", error);   // Показываем ошибку
}
