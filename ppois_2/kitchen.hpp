/**
 * @file kitchen.hpp
 * @brief Модель кухни: ингредиенты, инструменты, плита, духовка, повар и набор блюд.
 *
 * Заголовочный файл содержит объявления всех классов предметной области:
 * - единицы измерения и количества продуктов;
 * - ингредиенты и кухонные инструменты;
 * - плита, духовка и профиль температуры;
 * - абстрактное блюдо и конкретные блюда;
 * - повар, который реализует алгоритмы приготовления;
 * - меню, позволяющее пользователю выбирать и готовить блюда.
 */

#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <stdexcept>

using namespace std;

/**
 * @class IngredientNotFoundException
 * @brief Исключение: требуемый ингредиент отсутствует.
 */
class IngredientNotFoundException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    IngredientNotFoundException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class NotEnoughIngredientException
 * @brief Исключение: ингредиента недостаточно для операции.
 */
class NotEnoughIngredientException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    NotEnoughIngredientException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class ToolNotAvailableException
 * @brief Исключение: кухонный инструмент недоступен или неисправен.
 */
class ToolNotAvailableException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    ToolNotAvailableException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class InvalidTemperatureException
 * @brief Исключение: некорректная температура духовки или условия нагрева.
 */
class InvalidTemperatureException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    InvalidTemperatureException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class TimerNotSetException
 * @brief Исключение: не задан или задан неверно таймер.
 */
class TimerNotSetException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    TimerNotSetException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class OvercookedDishException
 * @brief Исключение: блюдо пережарено/переварено.
 */
class OvercookedDishException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    OvercookedDishException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class UndercookedDishException
 * @brief Исключение: блюдо недоготовлено.
 */
class UndercookedDishException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    UndercookedDishException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class StorageException
 * @brief Исключение: ошибка, связанная с хранением или единицей измерения.
 */
class StorageException : public runtime_error {
public:
    /**
     * @brief Создаёт исключение с текстом ошибки.
     * @param msg Сообщение об ошибке.
     */
    StorageException(const char* msg) : runtime_error(msg) {}
};

/**
 * @class Unit
 * @brief Единица измерения (граммы, литры, штуки и т.п.).
 *
 * Оборачивает коэффициент перевода в граммы и позволяет проверять,
 * является ли единица жидкой (объёмной) или нет.
 */
class Unit {
private:
    const char* name;        ///< Название единицы измерения (например, "шт", "мл").
    double gramsPerUnit;     ///< Сколько граммов соответствует одной единице.
    bool liquid;             ///< Признак жидкой формы (true — жидкость).
    int id;                  ///< Идентификатор единицы (для возможного расширения).

public:
    /**
     * @brief Конструктор единицы измерения.
     * @param n Название единицы.
     * @param g Количество граммов в одной единице.
     * @param l Признак, что единица относится к жидкости.
     * @param i Идентификатор.
     */
    Unit(const char* n = "unit", double g = 1.0, bool l = false, int i = 0);

    /**
     * @brief Перевод количества в граммы.
     * @param amount Количество в текущих единицах.
     * @return Масса в граммах.
     */
    double toGrams(double amount) const;

    /**
     * @brief Проверяет, относится ли единица к жидкостям.
     * @return true, если единица жидкая; иначе false.
     */
    bool isLiquid() const;

    /**
     * @brief Возвращает идентификатор единицы.
     * @return Целочисленный идентификатор.
     */
    int getId() const;
};

/**
 * @class Quantity
 * @brief Количество продукта в определённой единице измерения.
 *
 * Хранит значение, ссылку на Unit и флаг примерности измерения.
 */
class Quantity {
private:
    double value;    ///< Числовое значение количества.
    Unit* unit;      ///< Единица измерения.
    bool approximate;///< Признак примерного значения.
    int id;          ///< Идентификатор количества.

public:
    /**
     * @brief Конструктор количества.
     * @param v Числовое значение.
     * @param u Указатель на единицу измерения.
     * @param appr Флаг примерности значения.
     * @param i Идентификатор.
     */
    Quantity(double v = 0.0, Unit* u = nullptr, bool appr = false, int i = 0);

    /**
     * @brief Переводит количество в граммы.
     * @throw StorageException если unit не задан.
     * @return Масса в граммах.
     */
    double toGrams() const;

    /**
     * @brief Масштабирует значение на заданный множитель.
     * @param factor Множитель (>0) для изменения количества.
     */
    void scale(double factor);

    /**
     * @brief Проверяет, является ли количество нулевым или отрицательным.
     * @return true, если значение <= 0; иначе false.
     */
    bool isZero() const;
};

/**
 * @class Ingredient
 * @brief Ингредиент с количеством, калорийностью и признаком скоропортимости.
 */
class Ingredient {
private:
    const char* name;     ///< Название ингредиента.
    Quantity    quantity; ///< Количество ингредиента.
    double      calories; ///< Калорийность (на условную порцию).
    bool        perishable; ///< Признак скоропортящегося продукта.

public:
    /**
     * @brief Конструктор ингредиента.
     * @param n Название.
     * @param q Количество.
     * @param cal Калорийность.
     * @param per Признак скоропортимости.
     */
    Ingredient(const char* n, const Quantity& q, double cal, bool per);

    /**
     * @brief Добавляет массу к текущему количеству.
     * @param v Масса в граммах, которую нужно добавить.
     */
    void addAmount(double v);

    /**
     * @brief Использует часть ингредиента.
     * @param vGrams Масса в граммах, которую нужно забрать.
     * @throw NotEnoughIngredientException если vGrams больше доступного количества.
     */
    void useAmount(double vGrams);

    /**
     * @brief Проверяет, является ли ингредиент скоропортящимся.
     * @return true, если продукт perishable; иначе false.
     */
    bool isPerishable() const;
};

/**
 * @class KitchenTool
 * @brief Базовый класс для кухонных инструментов (ножи, кастрюли, сковороды и т.д.).
 *
 * Хранит информацию о чистоте, доступности, занятости и «прочности» инструмента.
 */
class KitchenTool {
protected:
    const char* name; ///< Название инструмента.
    bool clean;       ///< Инструмент чистый.
    bool available;   ///< Инструмент доступен для использования.
    bool busy;        ///< Инструмент занят.
    int  durability;  ///< Остаточный ресурс/прочность.

public:
    /**
     * @brief Конструктор кухонного инструмента.
     * @param n Название.
     * @param c Признак чистоты.
     * @param a Признак доступности.
     * @param d Прочность (кол-во возможных использований).
     */
    KitchenTool(const char* n = "tool",
                bool c = true,
                bool a = true,
                int  d = 100);

    /**
     * @brief Использует инструмент один раз.
     *
     * Уменьшает прочность и может сделать инструмент недоступным.
     * @throw ToolNotAvailableException если инструмент грязный, недоступен или сломан.
     */
    void useTool();

    /**
     * @brief Помечает инструмент как чистый.
     */
    void cleanTool();

    /**
     * @brief Полностью ломает инструмент.
     *
     * Прочность становится нулевой, инструмент недоступен.
     */
    void breakTool();

    /**
     * @brief Проверяет, можно ли использовать инструмент.
     * @return true, если инструмент доступен, чист и имеет ресурс; иначе false.
     */
    bool isAvailable() const;
};

/**
 * @class Knife
 * @brief Нож — специализированный кухонный инструмент.
 */
class Knife : public KitchenTool {
private:
    bool sharp; ///< Признак заточенности ножа.
    int  size; ///< Размер/длина лезвия.
    int  id;   ///< Идентификатор ножа.

public:
    /**
     * @brief Конструктор ножа.
     * @param n Название.
     * @param s Признак заточенности.
     * @param sz Размер.
     * @param i Идентификатор.
     */
    Knife(const char* n = "Нож",
          bool s  = true,
          int  sz = 20,
          int  i  = 0);

    /**
     * @brief Затачивает нож.
     */
    void sharpen();

    /**
     * @brief Тупит нож.
     */
    void dull();

    /**
     * @brief Проверяет, можно ли резать этим ножом.
     * @return true, если нож доступен и заточен.
     */
    bool canCut() const;
};

/**
 * @class CuttingBoard
 * @brief Разделочная доска.
 */
class CuttingBoard : public KitchenTool {
private:
    bool wooden; ///< Деревянная ли доска.
    bool wet;    ///< Мокрая ли доска.
    int  id;     ///< Идентификатор.

public:
    /**
     * @brief Конструктор разделочной доски.
     * @param n Название.
     * @param w Признак деревянной доски.
     * @param we Признак влажности.
     * @param i Идентификатор.
     */
    CuttingBoard(const char* n = "Доска",
                 bool w  = true,
                 bool we = false,
                 int  i  = 0);

    /**
     * @brief Высушивает доску.
     */
    void dry();

    /**
     * @brief Делает доску мокрой.
     */
    void wetBoard();

    /**
     * @brief Проверяет, безопасна ли доска для хлеба.
     * @return true для деревянной и сухой доски.
     */
    bool isSafeForBread() const;
};

/**
 * @class Pan
 * @brief Сковорода.
 */
class Pan : public KitchenTool {
private:
    double diameter; ///< Диаметр сковороды.
    bool   nonStick; ///< Признак антипригарного покрытия.
    bool   onStove;  ///< Стоит ли на плите.

public:
    /**
     * @brief Конструктор сковороды.
     * @param n Название.
     * @param d Диаметр.
     * @param ns Антипригарное покрытие.
     * @param on Изначально на плите.
     */
    Pan(const char* n = "Сковорода",
        double d  = 24.0,
        bool   ns = true,
        bool   on = false);

    /**
     * @brief Разогревает сковороду (использует инструмент и ставит на плиту).
     * @throw ToolNotAvailableException если сковорода недоступна.
     */
    void heatUp();

    /**
     * @brief Остужает сковороду и снимает с плиты.
     */
    void coolDown();

    /**
     * @brief Проверяет, стоит ли сковорода на плите (горячая).
     * @return true, если onStove == true.
     */
    bool isHot() const;
};

/**
 * @class Pot
 * @brief Кастрюля.
 */
class Pot : public KitchenTool {
private:
    double volume; ///< Объём кастрюли в литрах.
    bool   hasLid; ///< Наличие крышки.
    bool   onStove;///< Стоит ли кастрюля на плите.

public:
    /**
     * @brief Конструктор кастрюли.
     * @param n Название.
     * @param v Объём.
     * @param lid Есть ли крышка.
     * @param on Изначальное положение на плите.
     */
    Pot(const char* n = "Кастрюля",
        double v   = 2.0,
        bool   lid = true,
        bool   on  = false);

    /**
     * @brief Начинает кипячение (использует инструмент, ставит на плиту).
     * @throw ToolNotAvailableException если кастрюля недоступна.
     */
    void startBoil();

    /**
     * @brief Останавливает кипячение.
     */
    void stopBoil();

    /**
     * @brief Проверяет, можно ли кипятить указанный объём.
     * @param liters Объём жидкости.
     * @return true, если liters <= volume.
     */
    bool canBoil(double liters) const;
};

/**
 * @class Timer
 * @brief Простой таймер с секундной точностью.
 */
class Timer {
private:
    int seconds; ///< Заданное время в секундах.
    bool running;///< Таймер запущен.
    int elapsed; ///< Сколько секунд прошло.
    int id;      ///< Идентификатор таймера.

public:
    /**
     * @brief Конструктор таймера.
     * @param s Начальное количество секунд.
     * @param r Признак запущенного таймера.
     * @param e Начальное прошедшее время.
     * @param i Идентификатор.
     */
    Timer(int s = 0, bool r = false, int e = 0, int i = 0);

    /**
     * @brief Запускает таймер на заданное количество секунд.
     * @param s Время в секундах (>0).
     * @throw TimerNotSetException если s <= 0.
     */
    void start(int s);

    /**
     * @brief Продвигает таймер на delta секунд.
     * @param delta Приращение времени (игнорируется, если <=0 или таймер остановлен).
     */
    void tick(int delta);

    /**
     * @brief Проверяет, завершился ли таймер.
     * @return true, если таймер не работает и elapsed >= seconds.
     */
    bool isFinished() const;
};

/**
 * @class Mixer
 * @brief Миксер — инструмент для смешивания ингредиентов.
 */
class Mixer : public KitchenTool {
private:
    bool pluggedIn; ///< Включен ли миксер в сеть.

public:
    /**
     * @brief Конструктор миксера.
     * @param n Название.
     * @param plugged Изначальный статус подключения к сети.
     */
    Mixer(const char* n = "Миксер",
          bool plugged = false);

    /**
     * @brief Включить миксер в сеть.
     * @return Текущее состояние (true — включён).
     */
    bool plugIn();

    /**
     * @brief Выключить миксер из сети.
     * @return Текущее состояние (false — выключен).
     */
    bool unplug();

    /**
     * @brief Запускает процесс смешивания.
     *
     * Использует инструмент и выводит текстовое описание работы.
     * @throw ToolNotAvailableException если миксер недоступен.
     */
    void mix();
};

/**
 * @class PotatoMasher
 * @brief Толкушка для приготовления пюре.
 */
class PotatoMasher : public KitchenTool {
private:
    int id; ///< Идентификатор.

public:
    /**
     * @brief Конструктор толкушки.
     * @param n Название.
     * @param i Идентификатор.
     */
    PotatoMasher(const char* n = "Толкушка",
                 int  i = 0);

    /**
     * @brief Имитация процесса разминания.
     * @throw ToolNotAvailableException если инструмент недоступен.
     */
    void mash();
};

/**
 * @class TemperatureProfile
 * @brief Профиль нагрева: начальная и целевая температура, длительность, режим.
 */
class TemperatureProfile {
private:
    double startTemp;   ///< Начальная температура.
    double targetTemp;  ///< Целевая температура.
    int    duration;    ///< Длительность нагрева в секундах.
    bool   gradual;     ///< Плавный ли нагрев.

public:
    /**
     * @brief Конструктор профиля температуры.
     * @param s Начальная температура.
     * @param t Целевая температура.
     * @param d Длительность в секундах.
     * @param g Признак плавного нагрева.
     */
    TemperatureProfile(double s = 20.0, double t = 180.0,
                       int d = 600, bool g = true);

    /**
     * @brief Текущая температура по истекшему времени.
     * @param elapsed Прошедшее время в секундах.
     * @return Температура в текущий момент.
     */
    double currentTemp(int elapsed) const;

    /**
     * @brief Проверяет, достигнута ли целевая температура.
     * @param current Текущая температура.
     * @return true, если current >= targetTemp.
     */
    bool isReached(double current) const;

    /**
     * @brief Переопределяет профиль температуры.
     * @param s Новая начальная температура.
     * @param t Новая целевая температура.
     * @param d Новая длительность.
     */
    void reset(double s, double t, int d);
};

/**
 * @class Oven
 * @brief Духовка с профилем температуры и встроенным таймером.
 */
class Oven {
private:
    double temperature;        ///< Текущая температура духовки.
    bool   on;                 ///< Признак включённой духовки.
    bool   doorClosed;         ///< Дверца закрыта.
    Timer  bakingTimer;        ///< Таймер выпечки.
    TemperatureProfile profile;///< Профиль нагрева.
    int elapsedSeconds;        ///< Сколько секунд прошло с начала нагрева.

public:
    /**
     * @brief Конструктор духовки.
     * @param t Начальная температура.
     * @param o Признак включения.
     * @param d Состояние дверцы.
     */
    Oven(double t = 0.0, bool o = false, bool d = true);

    /**
     * @brief Начинает разогрев духовки по профилю.
     * @param t Целевая температура.
     * @param warmupMinutes Длительность разогрева в минутах (>0).
     * @throw InvalidTemperatureException при некорректных параметрах.
     */
    void preheat(double t, int warmupMinutes = 10);

    /**
     * @brief Выключает духовку и сбрасывает температуру.
     */
    void turnOff();

    /**
     * @brief Закрывает дверцу духовки.
     */
    void closeDoor();

    /**
     * @brief Открывает дверцу духовки.
     */
    void openDoor();

    /**
     * @brief Устанавливает таймер выпечки.
     * @param minutes Время в минутах (>0).
     * @throw TimerNotSetException если minutes <= 0.
     */
    void setTimerMinutes(int minutes);

    /**
     * @brief Продвигает время духовки.
     * @param secondsDelta Приращение времени в секундах.
     *
     * Обновляет температуру по профилю и состояние таймера.
     */
    void tick(int secondsDelta);

    /**
     * @brief Проверяет, включена ли духовка.
     * @return true, если духовка включена.
     */
    bool isOn() const;

    /**
     * @brief Возвращает текущую температуру духовки.
     * @return Температура в градусах Цельсия.
     */
    double getTemperature() const;

    /**
     * @brief Проверяет, закрыта ли дверца духовки.
     * @return true, если дверца закрыта.
     */
    bool isDoorClosed() const;
};

/**
 * @class Stove
 * @brief Плита с несколькими конфорками.
 */
class Stove {
private:
    int burners;        ///< Общее количество конфорок.
    int activeBurners;  ///< Активные конфорки.
    bool gas;           ///< Тип плиты (газовая/нет).
    bool on;            ///< Включена ли плита.

public:
    /**
     * @brief Конструктор плиты.
     * @param b Количество конфорок.
     * @param act Активные конфорки.
     * @param g Признак газовой плиты.
     * @param o Признак включения.
     */
    Stove(int b = 4, int act = 0, bool g = true, bool o = false);

    /**
     * @brief Включает ещё одну конфорку (если есть свободная).
     */
    void turnOnBurner();

    /**
     * @brief Выключает одну активную конфорку.
     *
     * Если активных конфорок не остаётся, плита считается выключенной.
     */
    void turnOffBurner();

    /**
     * @brief Возвращает количество свободных конфорок.
     * @return burners - activeBurners.
     */
    int freeBurners() const;
};

class Cook; ///< Объявление повара для friend-связей





/**
 * @class Dish
 * @brief Абстрактное блюдо.
 *
 * Любое конкретное блюдо наследуется от Dish и реализует метод cook(),
 * который в данной версии делегирует работу объекту Cook.
 */
class Dish {
protected:
    const char* name; ///< Название блюда.

public:
    /**
     * @brief Конструктор блюда.
     * @param n Название блюда.
     */
    Dish(const char* n);

    /// Виртуальный деструктор.
    virtual ~Dish() {}

    /**
     * @brief Приготовить блюдо.
     *
     * Чисто виртуальный метод, реализуется в производных классах через повара.
     */
    virtual void cook() = 0;

    /**
     * @brief Возвращает название блюда.
     * @return Строка с названием.
     */
    const char* getName() const {
        return name;
    }
};

/**
 * @class ChickenSoupDish
 * @brief Блюдо: куриный суп.
 *
 * Все детали приготовления описаны в Cook::cookChickenSoup().
 */
class ChickenSoupDish : public Dish {
    friend class Cook;
private:
    Ingredient* chicken;      ///< Курица.
    Ingredient* veggies;      ///< Овощи.
    Pot*        use_pot;      ///< Кастрюля.
    Stove*      use_stove;    ///< Плита.
    Timer*      use_boilTimer;///< Таймер варки.
    Cook*       chef;         ///< Повар, реализующий алгоритм.

public:
    /**
     * @brief Конструктор куриного супа.
     */
    ChickenSoupDish(const char* n,
                    Ingredient* c,
                    Ingredient* v,
                    Pot* p,
                    Stove* s,
                    Timer* t,
                    Cook* ck);

    /**
     * @brief Делегирует приготовление повару.
     * @throw ToolNotAvailableException если chef == nullptr.
     */
    void cook() override;
};

/**
 * @class SaladDish
 * @brief Блюдо: овощной салат.
 */
class SaladDish : public Dish {
    friend class Cook;
private:
    Ingredient* veggies; ///< Овощи.
    Ingredient* oil;     ///< Масло.
    Knife* knife;        ///< Нож.
    CuttingBoard* board; ///< Доска.
    Cook* chef;          ///< Повар.

public:
    /**
     * @brief Конструктор салата.
     */
    SaladDish(const char* n,
              Ingredient* v,
              Ingredient* o,
              Knife* k,
              CuttingBoard* b,
              Cook* ck);

    /**
     * @brief Просит повара приготовить салат.
     */
    void cook() override;
};

/**
 * @class BakedMeatDish
 * @brief Блюдо: запечённое мясо.
 */
class BakedMeatDish : public Dish {
    friend class Cook;
private:
    Ingredient* meat; ///< Мясо.
    Oven*       oven; ///< Духовка.
    Cook*       chef; ///< Повар.

public:
    /**
     * @brief Конструктор запечённого мяса.
     */
    BakedMeatDish(const char* n,
                  Ingredient* m,
                  Oven* o,
                  Cook* ck);

    void cook() override;
};

/**
 * @class PancakeDish
 * @brief Блюдо: блины.
 */
class PancakeDish : public Dish {
    friend class Cook;
private:
    Ingredient* flour;    ///< Мука.
    Ingredient* eggs;     ///< Яйца.
    Ingredient* sugar;    ///< Сахар.
    Ingredient* milk;     ///< Молоко.
    Pan*       pan;       ///< Сковорода.
    Stove*     stove;     ///< Плита.
    Timer*     fryTimer;  ///< Таймер жарки.
    Mixer*     mixer;     ///< Миксер.
    Cook*      chef;      ///< Повар.

public:
    /**
     * @brief Конструктор блинов.
     */
    PancakeDish(const char* n,
                Ingredient* f,
                Ingredient* e,
                Ingredient* s,
                Ingredient* m,
                Pan* p,
                Stove* st,
                Timer* t,
                Mixer* mx,
                Cook* ck);

    void cook() override;
};

/**
 * @class PastaDish
 * @brief Блюдо: паста с соусом.
 */
class PastaDish : public Dish {
    friend class Cook;
private:
    Ingredient* pasta; ///< Паста.
    Ingredient* sauce; ///< Соус.
    Pot*        pot;   ///< Кастрюля.
    Stove*      stove; ///< Плита.
    Timer*      boilTimer; ///< Таймер.
    Cook*       chef;  ///< Повар.

public:
    /**
     * @brief Конструктор пасты.
     */
    PastaDish(const char* n,
              Ingredient* p,
              Ingredient* s,
              Pot* pot_,
              Stove* st,
              Timer* t,
              Cook* ck);

    void cook() override;
};

/**
 * @class ScrambledEggsDish
 * @brief Блюдо: яичница.
 */
class ScrambledEggsDish : public Dish {
    friend class Cook;
private:
    Ingredient* eggs;  ///< Яйца.
    Ingredient* milk;  ///< Молоко.
    Pan* pan;          ///< Сковорода.
    Stove* stove;      ///< Плита.
    Timer* fryTimer;   ///< Таймер.
    Mixer* mixer;      ///< Миксер.
    Cook* chef;        ///< Повар.

public:
    ScrambledEggsDish(const char* n,
                      Ingredient* e,
                      Ingredient* m,
                      Pan* p,
                      Stove* s,
                      Timer* t,
                      Mixer* mx,
                      Cook* ck);

    void cook() override;
};

/**
 * @class VegGrillDish
 * @brief Блюдо: овощи-гриль.
 */
class VegGrillDish : public Dish {
    friend class Cook;
private:
    Ingredient* veggies;
    Pan* pan;
    Stove* stove;
    Timer* fryTimer;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    VegGrillDish(const char* n,
                 Ingredient* v,
                 Pan* p,
                 Stove* s,
                 Timer* t,
                 Knife* k,
                 CuttingBoard* b,
                 Cook* ck);

    void cook() override;
};

/**
 * @class MeatStewDish
 * @brief Блюдо: мясное рагу.
 */
class MeatStewDish : public Dish {
    friend class Cook;
private:
    Ingredient* meat;
    Ingredient* veggies;
    Pot* pot;
    Stove* stove;
    Timer* boilTimer;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    MeatStewDish(const char* n,
                 Ingredient* m,
                 Ingredient* v,
                 Pot* p,
                 Stove* s,
                 Timer* t,
                 Knife* k,
                 CuttingBoard* b,
                 Cook* ck);

    void cook() override;
};

/**
 * @class SandwichDish
 * @brief Блюдо: сэндвич.
 */
class SandwichDish : public Dish {
    friend class Cook;
private:
    Ingredient* bread;
    Ingredient* cheese;
    Ingredient* meat;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    SandwichDish(const char* n,
                 Ingredient* b,
                 Ingredient* ch,
                 Ingredient* m,
                 Knife* k,
                 CuttingBoard* bBoard,
                 Cook* ck);

    void cook() override;
};

/**
 * @class CookieDish
 * @brief Блюдо: печенье.
 */
class CookieDish : public Dish {
    friend class Cook;
private:
    Ingredient* flour;
    Ingredient* eggs;
    Ingredient* milk;
    Ingredient* sugar;
    Oven*       oven;
    Mixer*      mixer;
    Cook*       chef;

public:
    CookieDish(const char* n,
               Ingredient* f,
               Ingredient* e,
               Ingredient* m,
               Ingredient* s,
               Oven* o,
               Mixer* mx,
               Cook* ck);

    void cook() override;
};

/**
 * @class RiceDish
 * @brief Блюдо: варёный рис.
 */
class RiceDish : public Dish {
    friend class Cook;
private:
    Ingredient* rice;
    Pot* pot;
    Stove* stove;
    Timer* boilTimer;
    Cook* chef;

public:
    RiceDish(const char* n,
             Ingredient* r,
             Pot* p,
             Stove* s,
             Timer* t,
             Cook* ck);

    void cook() override;
};

/**
 * @class BoiledEggDish
 * @brief Блюдо: варёные яйца.
 */
class BoiledEggDish : public Dish {
    friend class Cook;
private:
    Ingredient* eggs;
    Pot* pot;
    Stove* stove;
    Timer* boilTimer;
    Cook* chef;

public:
    BoiledEggDish(const char* n,
                  Ingredient* e,
                  Pot* p,
                  Stove* s,
                  Timer* t,
                  Cook* ck);

    void cook() override;
};

/**
 * @class MashedPotatoDish
 * @brief Блюдо: картофельное пюре.
 */
class MashedPotatoDish : public Dish {
    friend class Cook;
private:
    Ingredient* potatoes;
    Ingredient* milk;
    Pot* pot;
    Stove* stove;
    Timer* boilTimer;
    PotatoMasher* masher;
    Cook* chef;

public:
    MashedPotatoDish(const char* n,
                     Ingredient* p,
                     Ingredient* m,
                     Pot* pot_,
                     Stove* s,
                     Timer* t,
                     PotatoMasher* mash,
                     Cook* ck);

    void cook() override;
};

/**
 * @class GrilledCheeseDish
 * @brief Блюдо: горячий сырный сэндвич (grilled cheese).
 */
class GrilledCheeseDish : public Dish {
    friend class Cook;
private:
    Ingredient* bread;
    Ingredient* cheese;
    Pan* pan;
    Stove* stove;
    Timer* fryTimer;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    GrilledCheeseDish(const char* n,
                      Ingredient* b,
                      Ingredient* ch,
                      Pan* p,
                      Stove* s,
                      Timer* t,
                      Knife* k,
                      CuttingBoard* brd,
                      Cook* ck);

    void cook() override;
};

/**
 * @class FriedFishDish
 * @brief Блюдо: жареная рыба.
 */
class FriedFishDish : public Dish {
    friend class Cook;
private:
    Ingredient* fish;
    Pan* pan;
    Stove* stove;
    Timer* fryTimer;
    Cook* chef;

public:
    FriedFishDish(const char* n,
                  Ingredient* f,
                  Pan* p,
                  Stove* s,
                  Timer* t,
                  Cook* ck);

    void cook() override;
};

/**
 * @class FruitSaladDish
 * @brief Блюдо: фруктовый салат.
 */
class FruitSaladDish : public Dish {
    friend class Cook;
private:
    Ingredient* fruits;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    FruitSaladDish(const char* n,
                   Ingredient* f,
                   Knife* k,
                   CuttingBoard* b,
                   Cook* ck);

    void cook() override;
};

/**
 * @class PorridgeDish
 * @brief Блюдо: овсяная каша.
 */
class PorridgeDish : public Dish {
    friend class Cook;
private:
    Ingredient* oats;
    Ingredient* milk;
    Pot* pot;
    Stove* stove;
    Timer* boilTimer;
    Cook* chef;

public:
    PorridgeDish(const char* n,
                 Ingredient* o,
                 Ingredient* m,
                 Pot* p,
                 Stove* s,
                 Timer* t,
                 Cook* ck);

    void cook() override;
};

/**
 * @class SteakDish
 * @brief Блюдо: стейк.
 */
class SteakDish : public Dish {
    friend class Cook;
private:
    Ingredient* meat;
    Pan* pan;
    Stove* stove;
    Timer* fryTimer;
    Cook* chef;

public:
    SteakDish(const char* n,
              Ingredient* m,
              Pan* p,
              Stove* s,
              Timer* t,
              Cook* ck);

    void cook() override;
};

/**
 * @class HotDogDish
 * @brief Блюдо: хот-дог.
 */
class HotDogDish : public Dish {
    friend class Cook;
private:
    Ingredient* bun;
    Ingredient* sausage;
    Pan* pan;
    Stove* stove;
    Timer*  fryTimer;
    Cook*   chef;

public:
    HotDogDish(const char* n,
               Ingredient* b,
               Ingredient* s,
               Pan* p,
               Stove* st,
               Timer* t,
               Cook* ck);

    void cook() override;
};

/**
 * @class SauteedMushroomsDish
 * @brief Блюдо: обжаренные грибы.
 */
class SauteedMushroomsDish : public Dish {
    friend class Cook;
private:
    Ingredient* mushrooms;
    Pan* pan;
    Stove* stove;
    Timer* fryTimer;
    Cook* chef;

public:
    SauteedMushroomsDish(const char* n,
                         Ingredient* m,
                         Pan* p,
                         Stove* s,
                         Timer* t,
                         Cook* ck);

    void cook() override;
};

/**
 * @class FriedPotatoDish
 * @brief Блюдо: жареный картофель.
 */
class FriedPotatoDish : public Dish {
    friend class Cook;
private:
    Ingredient* potatoes;
    Pan* pan;
    Stove* stove;
    Timer* fryTimer;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    FriedPotatoDish(const char* n,
                    Ingredient* p,
                    Pan* pa,
                    Stove* s,
                    Timer* t,
                    Knife* k,
                    CuttingBoard* b,
                    Cook* ck);

    void cook() override;
};

/**
 * @class TomatoSoupDish
 * @brief Блюдо: томатный суп.
 */
class TomatoSoupDish : public Dish {
    friend class Cook;
private:
    Ingredient* tomatoes;
    Ingredient* veggies;
    Pot* pot;
    Stove* stove;
    Timer* boilTimer;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    TomatoSoupDish(const char* n,
                   Ingredient* t,
                   Ingredient* v,
                   Pot* p,
                   Stove* s,
                   Timer* b,
                   Knife* k,
                   CuttingBoard* bd,
                   Cook* ck);

    void cook() override;
};

/**
 * @class VegOmeletteDish
 * @brief Блюдо: овощной омлет.
 */
class VegOmeletteDish : public Dish {
    friend class Cook;
private:
    Ingredient* eggs;
    Ingredient* veggies;
    Ingredient* milk;
    Pan* pan;
    Stove* stove;
    Timer* fryTimer;
    Knife* knife;
    CuttingBoard* board;
    Mixer* mixer;
    Cook* chef;

public:
    VegOmeletteDish(const char* n,
                    Ingredient* e,
                    Ingredient* v,
                    Ingredient* m,
                    Pan* p,
                    Stove* s,
                    Timer* t,
                    Knife* k,
                    CuttingBoard* b,
                    Mixer* mx,
                    Cook* ck);

    void cook() override;
};

/**
 * @class GarlicBreadDish
 * @brief Блюдо: чесночный хлеб.
 */
class GarlicBreadDish : public Dish {
    friend class Cook;
private:
    Ingredient* bread;
    Ingredient* garlic;
    Oven* oven;
    Knife* knife;
    CuttingBoard* board;
    Cook* chef;

public:
    GarlicBreadDish(const char* n,
                    Ingredient* b,
                    Ingredient* g,
                    Oven* ov,
                    Knife* k,
                    CuttingBoard* bd,
                    Cook* ck);

    void cook() override;
};

/**
 * @class SimpleSauceDish
 * @brief Блюдо: простой сливочный соус.
 */
class SimpleSauceDish : public Dish {
    friend class Cook;
private:
    Ingredient* base;
    Ingredient* cream;
    Pan* pan;
    Stove* stove;
    Timer* heatTimer;
    Mixer* mixer;
    Cook* chef;

public:
    SimpleSauceDish(const char* n,
                    Ingredient* b,
                    Ingredient* c,
                    Pan* p,
                    Stove* s,
                    Timer* t,
                    Mixer* m,
                    Cook* ck);

    void cook() override;
};

/**
 * @class Cook
 * @brief Повар, реализующий алгоритмы приготовления всех блюд.
 *
 * Каждый метод Cook::cookXxx() получает указатель на конкретное блюдо
 * и пошагово выполняет действия над его ингредиентами и инструментами.
 */
class Cook {
private:
    const char* name; ///< Имя повара (для вывода на экран).

public:
    /**
     * @brief Конструктор повара.
     * @param n Имя повара.
     */
    Cook(const char* n = "Повар");

    void cookChickenSoup(ChickenSoupDish* dish);
    void cookSalad(SaladDish* dish);
    void cookBakedMeat(BakedMeatDish* dish);
    void cookPancakes(PancakeDish* dish);
    void cookPasta(PastaDish* dish);
    void cookScrambledEggs(ScrambledEggsDish* dish);
    void cookVegGrill(VegGrillDish* dish);
    void cookMeatStew(MeatStewDish* dish);
    void cookSandwich(SandwichDish* dish);
    void cookCookies(CookieDish* dish);
    void cookRice(RiceDish* dish);
    void cookBoiledEggs(BoiledEggDish* dish);
    void cookMashedPotato(MashedPotatoDish* dish);
    void cookGrilledCheese(GrilledCheeseDish* dish);
    void cookFriedFish(FriedFishDish* dish);
    void cookFruitSalad(FruitSaladDish* dish);
    void cookPorridge(PorridgeDish* dish);
    void cookSteak(SteakDish* dish);
    void cookHotDog(HotDogDish* dish);
    void cookSauteedMushrooms(SauteedMushroomsDish* dish);
    void cookFriedPotato(FriedPotatoDish* dish);
    void cookTomatoSoup(TomatoSoupDish* dish);
    void cookVegOmelette(VegOmeletteDish* dish);
    void cookGarlicBread(GarlicBreadDish* dish);
    void cookSimpleSauce(SimpleSauceDish* dish);
};

/**
 * @class Menu
 * @brief Меню, содержащее список доступных блюд.
 *
 * Позволяет пользователю выбрать блюдо по номеру и запустить его приготовление.
 */
class Menu {
private:
    vector<Dish*> dishes; ///< Список блюд, доступных в меню.

public:
    /**
     * @brief Показывает список блюд в консоли.
     */
    void show() const;

    /**
     * @brief Добавляет блюдо в меню.
     * @param d Указатель на блюдо (не должен быть nullptr).
     */
    void addDish(Dish* d);

    /**
     * @brief Основной цикл работы меню.
     *
     * Выводит список блюд, читает выбор пользователя и вызывает Dish::cook()
     * для выбранного блюда. Выход — выбор пункта 0 или некорректный ввод.
     */
    void run();
};
