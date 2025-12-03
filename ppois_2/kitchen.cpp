/**
 * @file kitchen.cpp
 * @brief Реализация классов кухни: ингредиенты, инструменты, блюда, повар и меню.
 */

#include "kitchen.hpp"



Unit::Unit(const char* n, double g, bool l, int i)
    : name(n), gramsPerUnit(g), liquid(l), id(i) {}

double Unit::toGrams(double amount) const {
    return amount * gramsPerUnit;
}

bool Unit::isLiquid() const {
    return liquid;
}

int Unit::getId() const {
    return id;
}



Quantity::Quantity(double v, Unit* u, bool appr, int i)
    : value(v), unit(u), approximate(appr), id(i) {}

double Quantity::toGrams() const {
    if (!unit) {
        throw StorageException("Unit is not set for quantity");
    }
    return unit->toGrams(value);
}

void Quantity::scale(double factor) {
    value *= factor;
}

bool Quantity::isZero() const {
    return value <= 0.0;
}

/* ===== Ingredient ===== */

Ingredient::Ingredient(const char* n, const Quantity& q, double cal, bool per)
    : name(n), quantity(q), calories(cal), perishable(per) {}

void Ingredient::addAmount(double v) {
    double oldGrams = quantity.toGrams();
    double newGrams = oldGrams + v;
    if (oldGrams > 0.0) {
        double factor = newGrams / oldGrams;
        quantity.scale(factor);
    }
}

void Ingredient::useAmount(double vGrams) {
    double grams = quantity.toGrams();
    if (vGrams > grams) {
        throw NotEnoughIngredientException("Not enough ingredient");
    }
    if (grams > 0.0) {
        double factor = (grams - vGrams) / grams;
        quantity.scale(factor);
    }
}

bool Ingredient::isPerishable() const {
    return perishable;
}

/* ===== KitchenTool ===== */

KitchenTool::KitchenTool(const char* n,
                         bool c,
                         bool a,
                         int  d)
    : name(n), clean(c), available(a), busy(false), durability(d) {}

void KitchenTool::useTool() {
    if (!available || !clean || durability <= 0) {
        throw ToolNotAvailableException("Tool not usable (unavailable, dirty, or broken)");
    }
    busy = true;
    durability--;
    if (durability <= 0) {
        durability = 0;
        available = false;
    }
    busy = false;
}

void KitchenTool::cleanTool() {
    clean = true;
}

void KitchenTool::breakTool() {
    available = false;
    durability = 0;
}

bool KitchenTool::isAvailable() const {
    return available && clean && durability > 0;
}

/* ===== Knife ===== */

Knife::Knife(const char* n, bool s, int sz, int i)
    : KitchenTool(n, true, true, 100),
      sharp(s),
      size(sz),
      id(i) {}

void Knife::sharpen() {
    sharp = true;
}

void Knife::dull() {
    sharp = false;
}

bool Knife::canCut() const {
    return isAvailable() && sharp;
}

/* ===== CuttingBoard ===== */

CuttingBoard::CuttingBoard(const char* n, bool w, bool we, int i)
    : KitchenTool(n, true, true, 100),
      wooden(w),
      wet(we),
      id(i) {}

void CuttingBoard::dry() {
    wet = false;
}

void CuttingBoard::wetBoard() {
    wet = true;
}

bool CuttingBoard::isSafeForBread() const {
    return wooden && !wet && isAvailable();
}

/* ===== Pan ===== */

Pan::Pan(const char* n, double d, bool ns, bool on)
    : KitchenTool(n, true, true, 100),
      diameter(d),
      nonStick(ns),
      onStove(on) {}

void Pan::heatUp() {
    if (!isAvailable()) {
        throw ToolNotAvailableException("Pan not available");
    }
    useTool();
    onStove = true;
}

void Pan::coolDown() {
    onStove = false;
}

bool Pan::isHot() const {
    return onStove;
}

/* ===== Pot ===== */

Pot::Pot(const char* n, double v, bool lid, bool on)
    : KitchenTool(n, true, true, 100),
      volume(v),
      hasLid(lid),
      onStove(on) {}

void Pot::startBoil() {
    if (!isAvailable()) {
        throw ToolNotAvailableException("Pot not available");
    }
    useTool();
    onStove = true;
}

void Pot::stopBoil() {
    onStove = false;
}

bool Pot::canBoil(double liters) const {
    return liters <= volume;
}

/* ===== Timer ===== */

Timer::Timer(int s, bool r, int e, int i)
    : seconds(s), running(r), elapsed(e), id(i) {}

void Timer::start(int s) {
    if (s <= 0) {
        throw TimerNotSetException("Timer seconds must be > 0");
    }
    seconds = s;
    elapsed = 0;
    running = true;
}

void Timer::tick(int delta) {
    if (delta <= 0) return;
    if (!running) return;
    elapsed += delta;
    if (elapsed >= seconds) {
        elapsed = seconds;
        running = false;
    }
}

bool Timer::isFinished() const {
    return !running && elapsed >= seconds && seconds > 0;
}

/* ===== Mixer ===== */

Mixer::Mixer(const char* n, bool plugged)
    : KitchenTool(n, true, true, 100),
      pluggedIn(plugged) {}

bool Mixer::plugIn() {
    pluggedIn = true;
    cout << "Миксер включён в сеть.\n";
    return pluggedIn;
}

bool Mixer::unplug() {
    pluggedIn = false;
    cout << "Миксер выключен из сети.\n";
    return pluggedIn;
}

void Mixer::mix() {
    if (!pluggedIn) {
        cout << "Миксер не включён в сеть. Не могу начать работу.\n";
        return;
    }
    if (!isAvailable()) {
        throw ToolNotAvailableException("Mixer is not available");
    }
    useTool();
    cout << "Включаем миксер...\n";
    cout << "Bzzzzzz-bzzz-bzzz...\n";
    cout << "Миксер поработал и остановился.\n";
}

/* ===== PotatoMasher ===== */

PotatoMasher::PotatoMasher(const char* n, int i)
    : KitchenTool(n, true, true, 100),
      id(i) {}

void PotatoMasher::mash() {
    if (!isAvailable()) {
        throw ToolNotAvailableException("Potato masher is not available");
    }
    useTool();
    cout << "Using potato masher... mash-mash-mash...\n";
}



TemperatureProfile::TemperatureProfile(double s, double t, int d, bool g)
    : startTemp(s), targetTemp(t), duration(d), gradual(g) {}

double TemperatureProfile::currentTemp(int elapsed) const {
    if (!gradual || elapsed >= duration || duration <= 0) {
        return targetTemp;
    }
    double ratio = static_cast<double>(elapsed) / duration;
    return startTemp + (targetTemp - startTemp) * ratio;
}

bool TemperatureProfile::isReached(double current) const {
    return current >= targetTemp;
}

void TemperatureProfile::reset(double s, double t, int d) {
    startTemp  = s;
    targetTemp = t;
    duration   = d;
}



Oven::Oven(double t, bool o, bool d)
    : temperature(t),
      on(o),
      doorClosed(d),
      bakingTimer(),
      profile(),
      elapsedSeconds(0) {}

void Oven::preheat(double t, int warmupMinutes) {
    if (t <= 0.0 || t > 300.0) {
        throw InvalidTemperatureException("Invalid oven temperature");
    }
    if (!doorClosed) {
        throw InvalidTemperatureException("Oven door is open");
    }
    if (warmupMinutes <= 0) {
        throw InvalidTemperatureException("Warmup minutes must be > 0");
    }

    profile.reset(temperature, t, warmupMinutes * 60);
    elapsedSeconds = 0;
    on = true;
}

void Oven::turnOff() {
    on = false;
    temperature = 0.0;
}

void Oven::closeDoor() {
    doorClosed = true;
}

void Oven::openDoor() {
    doorClosed = false;
}

void Oven::setTimerMinutes(int minutes) {
    if (minutes <= 0) {
        throw TimerNotSetException("Timer minutes must be > 0");
    }
    bakingTimer.start(minutes * 60);
}

void Oven::tick(int secondsDelta) {
    if (secondsDelta <= 0) return;

    if (on) {
        elapsedSeconds += secondsDelta;
        temperature = profile.currentTemp(elapsedSeconds);
    }

    bakingTimer.tick(secondsDelta);
    if (bakingTimer.isFinished() && on) {
        turnOff();
    }
}

bool Oven::isOn() const {
    return on;
}

double Oven::getTemperature() const {
    return temperature;
}

bool Oven::isDoorClosed() const {
    return doorClosed;
}



Stove::Stove(int b, int act, bool g, bool o)
    : burners(b), activeBurners(act), gas(g), on(o) {}

void Stove::turnOnBurner() {
    if (activeBurners < burners) {
        activeBurners++;
        on = true;
    }
}

void Stove::turnOffBurner() {
    if (activeBurners > 0) {
        activeBurners--;
        if (activeBurners == 0) {
            on = false;
        }
    }
}

int Stove::freeBurners() const {
    return burners - activeBurners;
}

//Виртуальный метод cook() в наследнике реализован через делегирование. Конкретное блюдо (например, ChickenSoupDish) хранит указатель на объект повара — это реализация агрегации: блюдо пользуется поваром, но не владеет им и не создаёт его само.

//Когда вызывается метод cook() у блюда, оно просто передаёт this в соответствующий метод повара (chef->cookChickenSoup(this)), а дальше уже повар пошагово выполняет весь сценарий приготовления. При этом класс Cook объявлен дружественным по отношению к классам блюд, поэтому он имеет доступ к их закрытым полям (ингредиентам, посуде, таймерам и т.д.) и может напрямую управлять ими, не нарушая инкапсуляцию снаружи.


Dish::Dish(const char* n) : name(n) {}
ChickenSoupDish::ChickenSoupDish(const char* n,
                                 Ingredient* c,
                                 Ingredient* v,
                                 Pot* p,
                                 Stove* s,
                                 Timer* t,
                                 Cook* ck)
    : Dish(n),
      chicken(c),
      veggies(v),
      use_pot(p),
      use_stove(s),
      use_boilTimer(t),
      chef(ck) {}

void ChickenSoupDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для куриного супа");
    }
    chef->cookChickenSoup(this);
}

SaladDish::SaladDish(const char* n,
                     Ingredient* v,
                     Ingredient* o,
                     Knife* k,
                     CuttingBoard* b,
                     Cook* ck)
    : Dish(n), veggies(v), oil(o),
      knife(k), board(b), chef(ck) {}

void SaladDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для салата");
    }
    chef->cookSalad(this);
}

BakedMeatDish::BakedMeatDish(const char* n,
                             Ingredient* m,
                             Oven* o,
                             Cook* ck)
    : Dish(n), meat(m), oven(o), chef(ck) {}

void BakedMeatDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для запечённого мяса");
    }
    chef->cookBakedMeat(this);
}

PancakeDish::PancakeDish(const char* n,
                         Ingredient* f,
                         Ingredient* e,
                         Ingredient* s,
                         Ingredient* m,
                         Pan* p,
                         Stove* st,
                         Timer* t,
                         Mixer* mx,
                         Cook* ck)
    : Dish(n), flour(f), eggs(e), sugar(s), milk(m),
      pan(p), stove(st), fryTimer(t), mixer(mx), chef(ck) {}

void PancakeDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для блинов");
    }
    chef->cookPancakes(this);
}

PastaDish::PastaDish(const char* n,
                     Ingredient* p,
                     Ingredient* s,
                     Pot* pot_,
                     Stove* st,
                     Timer* t,
                     Cook* ck)
    : Dish(n), pasta(p), sauce(s), pot(pot_), stove(st), boilTimer(t), chef(ck) {}

void PastaDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для пасты");
    }
    chef->cookPasta(this);
}

ScrambledEggsDish::ScrambledEggsDish(const char* n,
                                     Ingredient* e,
                                     Ingredient* m,
                                     Pan* p,
                                     Stove* s,
                                     Timer* t,
                                     Mixer* mx,
                                     Cook* ck)
    : Dish(n), eggs(e), milk(m), pan(p), stove(s),
      fryTimer(t), mixer(mx), chef(ck) {}

void ScrambledEggsDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для яичницы");
    }
    chef->cookScrambledEggs(this);
}

VegGrillDish::VegGrillDish(const char* n,
                           Ingredient* v,
                           Pan* p,
                           Stove* s,
                           Timer* t,
                           Knife* k,
                           CuttingBoard* b,
                           Cook* ck)
    : Dish(n), veggies(v), pan(p), stove(s),
      fryTimer(t), knife(k), board(b), chef(ck) {}

void VegGrillDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для овощей-гриль");
    }
    chef->cookVegGrill(this);
}

MeatStewDish::MeatStewDish(const char* n,
                           Ingredient* m,
                           Ingredient* v,
                           Pot* p,
                           Stove* s,
                           Timer* t,
                           Knife* k,
                           CuttingBoard* b,
                           Cook* ck)
    : Dish(n), meat(m), veggies(v), pot(p), stove(s),
      boilTimer(t), knife(k), board(b), chef(ck) {}

void MeatStewDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для рагу");
    }
    chef->cookMeatStew(this);
}

SandwichDish::SandwichDish(const char* n,
                           Ingredient* b,
                           Ingredient* ch,
                           Ingredient* m,
                           Knife* k,
                           CuttingBoard* bBoard,
                           Cook* ck)
    : Dish(n), bread(b), cheese(ch), meat(m),
      knife(k), board(bBoard), chef(ck) {}

void SandwichDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для сэндвича");
    }
    chef->cookSandwich(this);
}

CookieDish::CookieDish(const char* n,
                       Ingredient* f,
                       Ingredient* e,
                       Ingredient* m,
                       Ingredient* s,
                       Oven* o,
                       Mixer* mx,
                       Cook* ck)
    : Dish(n), flour(f), eggs(e), milk(m), sugar(s),
      oven(o), mixer(mx), chef(ck) {}

void CookieDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для печенья");
    }
    chef->cookCookies(this);
}

RiceDish::RiceDish(const char* n,
                   Ingredient* r,
                   Pot* p,
                   Stove* s,
                   Timer* t,
                   Cook* ck)
    : Dish(n), rice(r), pot(p), stove(s), boilTimer(t), chef(ck) {}

void RiceDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для риса");
    }
    chef->cookRice(this);
}

BoiledEggDish::BoiledEggDish(const char* n,
                             Ingredient* e,
                             Pot* p,
                             Stove* s,
                             Timer* t,
                             Cook* ck)
    : Dish(n), eggs(e), pot(p), stove(s), boilTimer(t), chef(ck) {}

void BoiledEggDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для варёных яиц");
    }
    chef->cookBoiledEggs(this);
}

MashedPotatoDish::MashedPotatoDish(const char* n,
                                   Ingredient* p,
                                   Ingredient* m,
                                   Pot* pot_,
                                   Stove* s,
                                   Timer* t,
                                   PotatoMasher* mash,
                                   Cook* ck)
    : Dish(n), potatoes(p), milk(m), pot(pot_), stove(s),
      boilTimer(t), masher(mash), chef(ck) {}

void MashedPotatoDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для пюре");
    }
    chef->cookMashedPotato(this);
}

GrilledCheeseDish::GrilledCheeseDish(const char* n,
                                     Ingredient* b,
                                     Ingredient* ch,
                                     Pan* p,
                                     Stove* s,
                                     Timer* t,
                                     Knife* k,
                                     CuttingBoard* brd,
                                     Cook* ck)
    : Dish(n), bread(b), cheese(ch), pan(p), stove(s),
      fryTimer(t), knife(k), board(brd), chef(ck) {}

void GrilledCheeseDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для грилл-сэндвича");
    }
    chef->cookGrilledCheese(this);
}

FriedFishDish::FriedFishDish(const char* n,
                             Ingredient* f,
                             Pan* p,
                             Stove* s,
                             Timer* t,
                             Cook* ck)
    : Dish(n), fish(f), pan(p), stove(s), fryTimer(t), chef(ck) {}

void FriedFishDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для жареной рыбы");
    }
    chef->cookFriedFish(this);
}

FruitSaladDish::FruitSaladDish(const char* n,
                               Ingredient* f,
                               Knife* k,
                               CuttingBoard* b,
                               Cook* ck)
    : Dish(n), fruits(f), knife(k), board(b), chef(ck) {}

void FruitSaladDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для фруктового салата");
    }
    chef->cookFruitSalad(this);
}

PorridgeDish::PorridgeDish(const char* n,
                           Ingredient* o,
                           Ingredient* m,
                           Pot* p,
                           Stove* s,
                           Timer* t,
                           Cook* ck)
    : Dish(n), oats(o), milk(m), pot(p), stove(s), boilTimer(t), chef(ck) {}

void PorridgeDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для каши");
    }
    chef->cookPorridge(this);
}

SteakDish::SteakDish(const char* n,
                     Ingredient* m,
                     Pan* p,
                     Stove* s,
                     Timer* t,
                     Cook* ck)
    : Dish(n), meat(m), pan(p), stove(s), fryTimer(t), chef(ck) {}

void SteakDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для стейка");
    }
    chef->cookSteak(this);
}

HotDogDish::HotDogDish(const char* n,
                       Ingredient* b,
                       Ingredient* s,
                       Pan* p,
                       Stove* st,
                       Timer* t,
                       Cook* ck)
    : Dish(n), bun(b), sausage(s), pan(p),
      stove(st), fryTimer(t), chef(ck) {}

void HotDogDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для хот-дога");
    }
    chef->cookHotDog(this);
}

SauteedMushroomsDish::SauteedMushroomsDish(const char* n,
                                           Ingredient* m,
                                           Pan* p,
                                           Stove* s,
                                           Timer* t,
                                           Cook* ck)
    : Dish(n), mushrooms(m), pan(p), stove(s), fryTimer(t), chef(ck) {}

void SauteedMushroomsDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для грибов");
    }
    chef->cookSauteedMushrooms(this);
}

FriedPotatoDish::FriedPotatoDish(const char* n,
                                 Ingredient* p,
                                 Pan* pa,
                                 Stove* s,
                                 Timer* t,
                                 Knife* k,
                                 CuttingBoard* b,
                                 Cook* ck)
    : Dish(n), potatoes(p), pan(pa), stove(s),
      fryTimer(t), knife(k), board(b), chef(ck) {}

void FriedPotatoDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для жареного картофеля");
    }
    chef->cookFriedPotato(this);
}

TomatoSoupDish::TomatoSoupDish(const char* n,
                               Ingredient* t,
                               Ingredient* v,
                               Pot* p,
                               Stove* s,
                               Timer* b,
                               Knife* k,
                               CuttingBoard* bd,
                               Cook* ck)
    : Dish(n), tomatoes(t), veggies(v),
      pot(p), stove(s), boilTimer(b),
      knife(k), board(bd), chef(ck) {}

void TomatoSoupDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для томатного супа");
    }
    chef->cookTomatoSoup(this);
}

VegOmeletteDish::VegOmeletteDish(const char* n,
                                 Ingredient* e,
                                 Ingredient* v,
                                 Ingredient* m,
                                 Pan* p,
                                 Stove* s,
                                 Timer* t,
                                 Knife* k,
                                 CuttingBoard* b,
                                 Mixer* mx,
                                 Cook* ck)
    : Dish(n), eggs(e), veggies(v), milk(m),
      pan(p), stove(s), fryTimer(t),
      knife(k), board(b), mixer(mx), chef(ck) {}

void VegOmeletteDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для омлета");
    }
    chef->cookVegOmelette(this);
}

GarlicBreadDish::GarlicBreadDish(const char* n,
                                 Ingredient* b,
                                 Ingredient* g,
                                 Oven* ov,
                                 Knife* k,
                                 CuttingBoard* bd,
                                 Cook* ck)
    : Dish(n), bread(b), garlic(g),
      oven(ov), knife(k), board(bd), chef(ck) {}

void GarlicBreadDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для чесночного хлеба");
    }
    chef->cookGarlicBread(this);
}

SimpleSauceDish::SimpleSauceDish(const char* n,
                                 Ingredient* b,
                                 Ingredient* c,
                                 Pan* p,
                                 Stove* s,
                                 Timer* t,
                                 Mixer* m,
                                 Cook* ck)
    : Dish(n), base(b), cream(c),
      pan(p), stove(s), heatTimer(t),
      mixer(m), chef(ck) {}

void SimpleSauceDish::cook() {
    if (!chef) {
        throw ToolNotAvailableException("Нет повара для соуса");
    }
    chef->cookSimpleSauce(this);
}

/* ===== Cook ===== */

Cook::Cook(const char* n)
    : name(n) {}

void Cook::cookChickenSoup(ChickenSoupDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->use_pot)        throw ToolNotAvailableException("Нет кастрюли для супа");
    if (!dish->use_stove)      throw ToolNotAvailableException("Нет плиты для супа");
    if (!dish->use_boilTimer)  throw TimerNotSetException("Нет таймера для супа");

    if (!dish->use_pot->canBoil(1.5)) {
        throw NotEnoughIngredientException("Кастрюля слишком маленькая для супа");
    }
    cout << "Нарезаем курицу и овощи для супа...\n";
    dish->chicken->useAmount(150.0);
    dish->veggies->useAmount(100.0);
    cout << "Ставим кастрюлю на плиту и включаем конфорку...\n";
    dish->use_stove->turnOnBurner();
    dish->use_pot->startBoil();

    const int totalSeconds = 30 * 60;
    dish->use_boilTimer->start(totalSeconds);

    int step = 5 * 60;
    int minutesPassed = 0;

    while (!dish->use_boilTimer->isFinished()) {
        dish->use_boilTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин варки супа...\n";
    }

    cout << "Варка супа по таймеру завершена.\n";

    dish->use_stove->turnOffBurner();
    dish->use_pot->stopBoil();

    cout << "Куриный суп готов!\n";
}

void Cook::cookSalad(SaladDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->knife || !dish->knife->canCut()) {
        throw ToolNotAvailableException("Нож недоступен для салата");
    }
    if (!dish->board || !dish->board->isSafeForBread()) {
        cout << "Доска не сухая или не деревянная — используем её с осторожностью.\n";
    }
    cout << "Нарезаем овощи для салата...\n";
    dish->veggies->useAmount(120.0);
    cout << "Заправляем салат маслом...\n";
    dish->oil->useAmount(10.0);
    cout << "Салат готов!\n";
}

void Cook::cookBakedMeat(BakedMeatDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->oven) {
        throw ToolNotAvailableException("Нет духовки для мяса");
    }
    cout << "Подготавливаем и нарезаем мясо...\n";
    dish->meat->useAmount(200.0);

    dish->oven->closeDoor();
    dish->oven->preheat(180.0);
    dish->oven->setTimerMinutes(30);

    cout << "Запекаем мясо 30 минут при 180C...\n";
    int cookedSeconds = 0;
    while (true) {
        dish->oven->tick(300);
        cookedSeconds += 300;
        cout << "Прошло " << cookedSeconds / 60 << " минут...\n";
        if (!dish->oven->isOn()) break;
    }

    int cookedMinutes   = cookedSeconds / 60;
    int expectedMinutes = 30;
    if (cookedMinutes > expectedMinutes + 5) {
        throw OvercookedDishException("Мясо передержали в духовке");
    }
    if (cookedMinutes < expectedMinutes - 5) {
        throw UndercookedDishException("Мясо недоготовлено");
    }

    cout << "Запечённое мясо готово!\n";
}

void Cook::cookPancakes(PancakeDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";

    if (!dish->flour || !dish->eggs || !dish->sugar || !dish->milk) {
        throw IngredientNotFoundException("Нет муки, яиц, сахара или молока для блинов");
    }
    if (!dish->pan)       throw ToolNotAvailableException("Нет сковороды для блинов");
    if (!dish->stove)     throw ToolNotAvailableException("Нет плиты для блинов");
    if (!dish->fryTimer)  throw TimerNotSetException("Нет таймера для блинов");
    if (!dish->mixer)     throw ToolNotAvailableException("Нет миксера для теста блинов");
    if (!dish->mixer->plugIn()) {
        throw ToolNotAvailableException("Миксер не включён в розетку");
    }

    cout << "Подготавливаем ингредиенты для теста: мука, яйца, сахар, молоко...\n";
    dish->flour->useAmount(150.0);
    dish->eggs->useAmount(2.0);
    dish->sugar->useAmount(20.0);
    dish->milk->useAmount(200.0);

    cout << "Смешиваем всё миксером...\n";
    dish->mixer->mix();

    dish->stove->turnOnBurner();
    dish->pan->heatUp();
    cout << "Разогреваем сковороду...\n";

    const int secondsPerPancake = 2 * 60;
    const int flipTime          = 1 * 60;
    const int step              = 30;

    cout << "Наливаем тесто и жарим 3 блина...\n";
    for (int i = 1; i <= 3; ++i) {
        cout << "\nБлин " << i << ": наливаем порцию теста на сковороду...\n";

        dish->fryTimer->start(secondsPerPancake);
        int  sec_passed = 0;
        bool flipped    = false;

        while (!dish->fryTimer->isFinished()) {
            dish->fryTimer->tick(step);
            sec_passed += step;
            if (!flipped && sec_passed >= flipTime) {
                cout << "Блин " << i << ": переворачиваем на другую сторону...\n";
                flipped = true;
            }
            cout << "Блин " << i << ": прошло " << sec_passed / 60.0
                 << " мин жарки...\n";
        }
        cout << "Блин " << i << " готов.\n";
    }

    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "\nВсе блины готовы!\n";
}

void Cook::cookPasta(PastaDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";

    if (!dish->pot || !dish->stove) {
        throw ToolNotAvailableException("Нет кастрюли или плиты для пасты");
    }
    if (!dish->boilTimer) {
        throw TimerNotSetException("Нет таймера для пасты");
    }
    if (!dish->pasta) {
        throw IngredientNotFoundException("Нет пасты");
    }
    if (!dish->sauce) {
        throw IngredientNotFoundException("Нет соуса");
    }

    if (!dish->pot->canBoil(2.0)) {
        throw NotEnoughIngredientException("Кастрюля слишком маленькая для пасты");
    }

    cout << "Проверяем и подготавливаем ингредиенты...\n";
    dish->pasta->useAmount(100.0);
    dish->sauce->useAmount(50.0);

    cout << "Ставим кастрюлю на плиту и включаем конфорку...\n";
    dish->stove->turnOnBurner();
    dish->pot->startBoil();

    const int totalSeconds = 10 * 60;
    dish->boilTimer->start(totalSeconds);

    int step = 5 * 60;
    int minutesPassed = 0;

    while (!dish->boilTimer->isFinished()) {
        dish->boilTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин варки пасты...\n";
    }

    cout << "Варка пасты по таймеру завершена.\n";
    dish->pot->stopBoil();
    dish->stove->turnOffBurner();
    cout << "Паста с соусом готова!\n";
}

void Cook::cookScrambledEggs(ScrambledEggsDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";

    if (!dish->pan || !dish->stove) {
        throw ToolNotAvailableException("Нет сковороды или плиты для яичницы");
    }
    if (!dish->fryTimer) {
        throw TimerNotSetException("Нет таймера для яичницы");
    }
    if (!dish->eggs || !dish->milk) {
        throw IngredientNotFoundException("Нет яиц или молока для яичницы");
    }
    if (!dish->mixer) {
        throw ToolNotAvailableException("Нет миксера для яичницы");
    }
    if (!dish->mixer->plugIn()) {
        throw ToolNotAvailableException("Миксер не включён в розетку для яичницы");
    }

    cout << "Подготавливаем яйца и молоко для яичницы...\n";
    dish->eggs->useAmount(3.0);
    dish->milk->useAmount(50.0);

    cout << "Смешиваем яйца с молоком миксером...\n";
    dish->mixer->mix();

    dish->stove->turnOnBurner();
    dish->pan->heatUp();
    cout << "Разогреваем сковороду для яичницы...\n";

    const int totalSeconds = 5 * 60;
    const int step  = 60;
    int secondsPassed = 0;

    dish->fryTimer->start(totalSeconds);
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60
             << " мин жарки яичницы...\n";
    }

    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Яичница готова!\n";
}

void Cook::cookVegGrill(VegGrillDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->pan || !dish->stove) {
        throw ToolNotAvailableException("Нет сковороды или плиты для овощей-гриль");
    }
    if (!dish->fryTimer) {
        throw TimerNotSetException("Нет таймера для овощей-гриль");
    }
    if (!dish->veggies) {
        throw IngredientNotFoundException("Нет овощей для гриля");
    }
    if (!dish->knife || !dish->knife->canCut()) {
        throw ToolNotAvailableException("Нож недоступен для нарезки овощей-гриль");
    }
    if (!dish->board) {
        throw ToolNotAvailableException("Нет разделочной доски для овощей-гриль");
    }
    cout << "Нарезаем овощи для гриля...\n";
    dish->veggies->useAmount(150.0);
    dish->stove->turnOnBurner();
    dish->pan->heatUp();
    cout << "Разогреваем сковороду для овощей-гриль...\n";
    const int totalSeconds = 10 * 60;
    const int step = 2 * 60;
    dish->fryTimer->start(totalSeconds);
    int secondsPassed = 0;
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60
             << " мин жарки овощей...\n";
    }
    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Овощи-гриль готовы!\n";
}

void Cook::cookMeatStew(MeatStewDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->pot || !dish->stove) {
        throw ToolNotAvailableException("Нет кастрюли или плиты для рагу");
    }
    if (!dish->boilTimer) {
        throw TimerNotSetException("Нет таймера для рагу");
    }
    if (!dish->meat) {
        throw IngredientNotFoundException("Нет мяса для рагу");
    }
    if (!dish->veggies) {
        throw IngredientNotFoundException("Нет овощей для рагу");
    }
    if (!dish->knife || !dish->knife->canCut()) {
        throw ToolNotAvailableException("Нож недоступен для нарезки рагу");
    }
    if (!dish->board) {
        throw ToolNotAvailableException("Нет разделочной доски для рагу");
    }
    cout << "Нарезаем мясо и овощи для рагу...\n";
    dish->meat->useAmount(150.0);
    dish->veggies->useAmount(100.0);
    cout << "Ставим кастрюлю на плиту и включаем конфорку...\n";
    dish->stove->turnOnBurner();
    dish->pot->startBoil();

    const int totalSeconds = 45 * 60;
    dish->boilTimer->start(totalSeconds);

    int step = 5 * 60;
    int minutesPassed = 0;
    while (!dish->boilTimer->isFinished()) {
        dish->boilTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин тушения рагу...\n";
    }
    cout << "Тушение рагу по таймеру завершено.\n";
    dish->stove->turnOffBurner();
    dish->pot->stopBoil();
    cout << "Говяжье рагу готово!\n";
}

void Cook::cookSandwich(SandwichDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->knife || !dish->knife->canCut()) {
        throw ToolNotAvailableException("Нож недоступен для сэндвича");
    }
    if (!dish->board) {
        throw ToolNotAvailableException("Нет разделочной доски для сэндвича");
    }
    if (!dish->board->isSafeForBread()) {
        throw ToolNotAvailableException("Доска непригодна для хлеба (не деревянная или мокрая)");
    }
    dish->bread->useAmount(2.0);
    dish->cheese->useAmount(30.0);
    dish->meat->useAmount(20.0);
    cout << "Нарезаем хлеб, сыр и мясо на доске...\n";
    cout << "Собираем сэндвич из хлеба, сыра и мяса...\n";
    cout << "Сэндвич готов!\n";
}

void Cook::cookCookies(CookieDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";

    if (!dish->flour || !dish->eggs || !dish->milk || !dish->sugar) {
        throw IngredientNotFoundException("Нет муки, яиц, молока или сахара для печенья");
    }
    if (!dish->oven) {
        throw ToolNotAvailableException("Нет духовки для печенья");
    }
    if (!dish->mixer) {
        throw ToolNotAvailableException("Нет миксера для печенья");
    }
    if (!dish->mixer->plugIn()) {
        throw ToolNotAvailableException("Миксер не включён в розетку для печенья");
    }

    cout << "Подготавливаем ингредиенты для теста печенья...\n";
    dish->flour->useAmount(200.0);
    dish->sugar->useAmount(50.0);
    dish->eggs->useAmount(2.0);
    dish->milk->useAmount(100.0);

    cout << "Смешиваем муку, сахар, яйца и молоко миксером до теста...\n";
    dish->mixer->mix();

    dish->oven->closeDoor();
    dish->oven->preheat(190.0);
    dish->oven->setTimerMinutes(15);

    int bakedSeconds = 0;
    cout << "Выпекаем печенье 15 минут при 190C...\n";
    while (true) {
        dish->oven->tick(180);
        bakedSeconds += 180;
        cout << "Прошло " << bakedSeconds / 60 << " минут...\n";
        if (!dish->oven->isOn()) break;
    }

    int bakedMin    = bakedSeconds / 60;
    int expectedMin = 15;
    if (bakedMin > expectedMin + 5) {
        throw OvercookedDishException("Печенье сгорело");
    }
    if (bakedMin < expectedMin - 5) {
        throw UndercookedDishException("Печенье сырое");
    }

    cout << "Печенье готово!\n";
}

void Cook::cookRice(RiceDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->rice)  throw IngredientNotFoundException("Нет риса");
    if (!dish->pot || !dish->stove) throw ToolNotAvailableException("Нет кастрюли или плиты для риса");
    if (!dish->boilTimer)     throw TimerNotSetException("Нет таймера для риса");

    dish->rice->useAmount(80.0);
    cout << "Промываем и засыпаем рис в кастрюлю...\n";
    dish->stove->turnOnBurner();
    dish->pot->startBoil();

    const int totalSeconds = 15 * 60;
    const int step = 5 * 60;
    int minutesPassed = 0;

    dish->boilTimer->start(totalSeconds);
    while (!dish->boilTimer->isFinished()) {
        dish->boilTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин варки риса...\n";
    }
    dish->pot->stopBoil();
    dish->stove->turnOffBurner();
    cout << "Рис готов!\n";
}

void Cook::cookBoiledEggs(BoiledEggDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->eggs) throw IngredientNotFoundException("Нет яиц");
    if (!dish->pot || !dish->stove) throw ToolNotAvailableException("Нет кастрюли или плиты для яиц");
    if (!dish->boilTimer) throw TimerNotSetException("Нет таймера для яиц");

    dish->eggs->useAmount(3.0);
    cout << "Кладём яйца в кастрюлю с водой...\n";
    dish->stove->turnOnBurner();
    dish->pot->startBoil();

    const int totalSeconds = 8 * 60;
    const int step = 2 * 60;
    int minutesPassed = 0;

    dish->boilTimer->start(totalSeconds);
    while (!dish->boilTimer->isFinished()) {
        dish->boilTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин варки яиц...\n";
    }

    dish->pot->stopBoil();
    dish->stove->turnOffBurner();
    cout << "Яйца сварены вкрутую!\n";
}

void Cook::cookMashedPotato(MashedPotatoDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->potatoes || !dish->milk) throw IngredientNotFoundException("Нет картофеля или молока для пюре");
    if (!dish->pot || !dish->stove)  throw ToolNotAvailableException("Нет кастрюли или плиты для пюре");
    if (!dish->boilTimer) throw TimerNotSetException("Нет таймера для пюре");
    if (!dish->masher) throw ToolNotAvailableException("Нет potato masher для пюре");

    dish->potatoes->useAmount(200.0);
    dish->milk->useAmount(50.0);
    cout << "Чистим и нарезаем картофель...\n";

    dish->stove->turnOnBurner();
    dish->pot->startBoil();

    const int totalSeconds = 20 * 60;
    const int step = 5 * 60;
    int minutesPassed = 0;

    dish->boilTimer->start(totalSeconds);
    while (!dish->boilTimer->isFinished()) {
        dish->boilTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин варки картофеля...\n";
    }
    dish->pot->stopBoil();
    dish->stove->turnOffBurner();
    cout << "Разминаем картофель с молоком при помощи potato masher...\n";
    dish->masher->mash();
    cout << "Пюре готово!\n";
}

void Cook::cookGrilledCheese(GrilledCheeseDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->bread || !dish->cheese) throw IngredientNotFoundException("Нет хлеба или сыра для грилл-сэндвича");
    if (!dish->pan || !dish->stove) throw ToolNotAvailableException("Нет сковороды или плиты для грилл-сэндвича");
    if (!dish->fryTimer)  throw TimerNotSetException("Нет таймера для грилл-сэндвича");
    if (!dish->knife || !dish->knife->canCut()) throw ToolNotAvailableException("Нож недоступен для грилл-сэндвича");
    if (!dish->board)  throw ToolNotAvailableException("Нет доски для грилл-сэндвича");
    if (!dish->board->isSafeForBread()) {
        throw ToolNotAvailableException("Доска непригодна для хлеба при приготовлении грилл-сэндвича");
    }

    dish->bread->useAmount(2.0);
    dish->cheese->useAmount(40.0);
    cout << "Нарезаем хлеб и сыр на доске...\n";
    dish->stove->turnOnBurner();
    dish->pan->heatUp();
    cout << "Обжариваем сэндвич на сковороде...\n";

    const int totalSeconds = 5 * 60;
    const int step = 60;
    int secondsPassed = 0;

    dish->fryTimer->start(totalSeconds);
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60 << " мин жарки сэндвича...\n";
    }
    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Горячий грилл-сэндвич с сыром готов!\n";
}

void Cook::cookFriedFish(FriedFishDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->fish)  throw IngredientNotFoundException("Нет рыбы");
    if (!dish->pan || !dish->stove) throw ToolNotAvailableException("Нет сковороды или плиты для рыбы");
    if (!dish->fryTimer)      throw TimerNotSetException("Нет таймера для рыбы");

    dish->fish->useAmount(150.0);
    cout << "Подготавливаем рыбу к жарке...\n";
    dish->stove->turnOnBurner();
    dish->pan->heatUp();

    const int totalSeconds = 7 * 60;
    const int step = 60;
    int secondsPassed = 0;

    dish->fryTimer->start(totalSeconds);
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60 << " мин жарки рыбы...\n";
    }

    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Жареная рыба готова!\n";
}

void Cook::cookFruitSalad(FruitSaladDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->fruits) throw IngredientNotFoundException("Нет фруктов для салата");
    if (!dish->knife || !dish->knife->canCut()) {
        throw ToolNotAvailableException("Нож недоступен для фруктового салата");
    }
    if (!dish->board) {
        throw ToolNotAvailableException("Нет доски для фруктового салата");
    }
    dish->fruits->useAmount(200.0);
    cout << "Нарезаем фрукты на доске и смешиваем — фруктовый салат готов!\n";
}

void Cook::cookPorridge(PorridgeDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->oats || !dish->milk) throw IngredientNotFoundException("Нет овсянки или молока");
    if (!dish->pot || !dish->stove) throw ToolNotAvailableException("Нет кастрюли или плиты для каши");
    if (!dish->boilTimer)     throw TimerNotSetException("Нет таймера для каши");

    dish->oats->useAmount(50.0);
    dish->milk->useAmount(150.0);
    cout << "Смешиваем овсянку с молоком в кастрюле...\n";
    dish->stove->turnOnBurner();
    dish->pot->startBoil();

    const int totalSeconds = 7 * 60;
    const int step = 60;
    int secondsPassed = 0;

    dish->boilTimer->start(totalSeconds);
    while (!dish->boilTimer->isFinished()) {
        dish->boilTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60 << " мин варки каши...\n";
    }
    dish->pot->stopBoil();
    dish->stove->turnOffBurner();
    cout << "Овсяная каша готова!\n";
}

void Cook::cookSteak(SteakDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->meat) throw IngredientNotFoundException("Нет мяса для стейка");
    if (!dish->pan || !dish->stove) throw ToolNotAvailableException("Нет сковороды или плиты для стейка");
    if (!dish->fryTimer)      throw TimerNotSetException("Нет таймера для стейка");

    dish->meat->useAmount(180.0);
    cout << "Разогреваем сковороду и выкладываем стейк...\n";

    dish->stove->turnOnBurner();
    dish->pan->heatUp();

    const int totalSeconds = 8 * 60;
    const int step = 60;
    int secondsPassed = 0;

    dish->fryTimer->start(totalSeconds);
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60 << " мин жарки стейка...\n";
    }

    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Стейк готов!\n";
}

void Cook::cookHotDog(HotDogDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";

    if (!dish->bun || !dish->sausage) {
        throw IngredientNotFoundException("Нет булки или сосиски для хот-дога");
    }
    if (!dish->pan || !dish->stove) {
        throw ToolNotAvailableException("Нет сковороды или плиты для сосиски");
    }
    if (!dish->fryTimer) {
        throw TimerNotSetException("Нет таймера для хот-дога");
    }

    dish->bun->useAmount(1.0);
    dish->sausage->useAmount(1.0);

    dish->stove->turnOnBurner();
    dish->pan->heatUp();
    cout << "Разогреваем сковороду для сосиски...\n";

    const int totalSeconds = 2 * 60;
    const int step  = 60;
    int       secondsPassed = 0;
    dish->fryTimer->start(totalSeconds);
    cout << "Обжариваем сосиску на сковороде...\n";
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60
             << " мин обжарки сосиски...\n";
    }
    dish->pan->coolDown();
    dish->stove->turnOffBurner();

    cout << "Собираем хот-дог — булка + горячая сосиска. Хот-дог готов!\n";
}

void Cook::cookSauteedMushrooms(SauteedMushroomsDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->mushrooms) throw IngredientNotFoundException("Нет грибов");
    if (!dish->pan || !dish->stove) throw ToolNotAvailableException("Нет сковороды или плиты для грибов");
    if (!dish->fryTimer)      throw TimerNotSetException("Нет таймера для грибов");

    dish->mushrooms->useAmount(120.0);
    cout << "Кладём грибы на разогретую сковороду...\n";
    dish->stove->turnOnBurner();
    dish->pan->heatUp();
    const int totalSeconds = 6 * 60;
    const int step = 60;
    int secondsPassed = 0;
    dish->fryTimer->start(totalSeconds);
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60 << " мин жарки грибов...\n";
    }

    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Жареные грибы готовы!\n";
}

void Cook::cookFriedPotato(FriedPotatoDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->potatoes) throw IngredientNotFoundException("Нет картофеля");
    if (!dish->pan || !dish->stove) throw ToolNotAvailableException("Нет сковороды или плиты для картофеля");
    if (!dish->fryTimer) throw TimerNotSetException("Нет таймера для картофеля");
    if (!dish->knife || !dish->knife->canCut()) throw ToolNotAvailableException("Нож недоступен для нарезки картофеля");
    if (!dish->board) throw ToolNotAvailableException("Нет разделочной доски для картофеля");

    cout << "Нарезаем картофель на доске...\n";
    dish->potatoes->useAmount(200.0);
    cout << "Выкладываем картофель на сковороду...\n";

    dish->stove->turnOnBurner();
    dish->pan->heatUp();

    const int totalSeconds = 12 * 60;
    const int step = 3 * 60;
    int minutesPassed = 0;

    dish->fryTimer->start(totalSeconds);
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин жарки картофеля...\n";
    }
    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Жареный картофель готов!\n";
}

void Cook::cookTomatoSoup(TomatoSoupDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->tomatoes || !dish->veggies) throw IngredientNotFoundException("Нет томатов или овощей для супа");
    if (!dish->pot || !dish->stove) throw ToolNotAvailableException("Нет кастрюли или плиты для томатного супа");
    if (!dish->boilTimer) throw TimerNotSetException("Нет таймера для томатного супа");
    if (!dish->knife || !dish->knife->canCut()) throw ToolNotAvailableException("Нож недоступен для нарезки томатного супа");
    if (!dish->board) throw ToolNotAvailableException("Нет разделочной доски для томатного супа");

    cout << "Нарезаем томаты и овощи на доске...\n";
    dish->tomatoes->useAmount(150.0);
    dish->veggies->useAmount(80.0);

    cout << "Кладём томаты и овощи в кастрюлю и ставим на плиту...\n";
    dish->stove->turnOnBurner();
    dish->pot->startBoil();
    const int totalSeconds = 25 * 60;
    const int step = 5 * 60;
    int minutesPassed = 0;
    dish->boilTimer->start(totalSeconds);

    while (!dish->boilTimer->isFinished()) {
        dish->boilTimer->tick(step);
        minutesPassed += step / 60;
        cout << "Прошло " << minutesPassed << " мин варки томатного супа...\n";
    }
    dish->pot->stopBoil();
    dish->stove->turnOffBurner();
    cout << "Томатный суп готов!\n";
}

void Cook::cookVegOmelette(VegOmeletteDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";

    if (!dish->eggs || !dish->veggies || !dish->milk) {
        throw IngredientNotFoundException("Нет яиц, овощей или молока для омлета");
    }
    if (!dish->pan || !dish->stove) {
        throw ToolNotAvailableException("Нет сковороды или плиты для омлета");
    }
    if (!dish->fryTimer) {
        throw TimerNotSetException("Нет таймера для омлета");
    }
    if (!dish->knife || !dish->knife->canCut()) {
        throw ToolNotAvailableException("Нож недоступен для нарезки овощей для омлета");
    }
    if (!dish->board) {
        throw ToolNotAvailableException("Нет разделочной доски для овощей для омлета");
    }
    if (!dish->mixer) {
        throw ToolNotAvailableException("Нет миксера для омлета");
    }
    if (!dish->mixer->plugIn()) {
        throw ToolNotAvailableException("Миксер не включён в розетку для омлета");
    }

    cout << "Нарезаем овощи для омлета на доске...\n";
    dish->veggies->useAmount(50.0);

    cout << "Разбиваем яйца и добавляем молоко...\n";
    dish->eggs->useAmount(3.0);
    dish->milk->useAmount(30.0);

    cout << "Смешиваем яйца, молоко и нарезанные овощи миксером...\n";
    dish->mixer->mix();

    dish->stove->turnOnBurner();
    dish->pan->heatUp();

    const int totalSeconds = 6 * 60;
    const int step = 60;
    int secondsPassed = 0;

    dish->fryTimer->start(totalSeconds);
    while (!dish->fryTimer->isFinished()) {
        dish->fryTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60 << " мин жарки омлета...\n";
    }

    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Овощной омлет готов!\n";
}

void Cook::cookGarlicBread(GarlicBreadDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";

    if (!dish->bread || !dish->garlic) {
        throw IngredientNotFoundException("Нет хлеба или чеснока для чесночного хлеба");
    }
    if (!dish->oven) {
        throw ToolNotAvailableException("Нет духовки для чесночного хлеба");
    }
    if (!dish->knife || !dish->knife->canCut()) {
        throw ToolNotAvailableException("Нож недоступен для нарезки чеснока");
    }
    if (!dish->board) {
        throw ToolNotAvailableException("Нет разделочной доски для чесночного хлеба");
    }
    if (!dish->board->isSafeForBread()) {
        throw ToolNotAvailableException("Доска непригодна для хлеба (не деревянная или мокрая)");
    }
    cout << "Нарезаем чеснок на доске...\n";
    dish->garlic->useAmount(5.0);
    cout << "Берём ломтики хлеба...\n";
    dish->bread->useAmount(2.0);
    cout << "Намазываем хлеб нарезанным чесноком...\n";
    dish->oven->closeDoor();
    dish->oven->preheat(180.0);
    dish->oven->setTimerMinutes(8);
    int bakedSeconds = 0;
    cout << "Запекаем чесночный хлеб 8 минут при 180C...\n";
    while (true) {
        dish->oven->tick(120);
        bakedSeconds += 120;
        cout << "Прошло " << bakedSeconds / 60 << " минут...\n";
        if (!dish->oven->isOn()) break;
    }

    cout << "Чесночный хлеб готов!\n";
}

void Cook::cookSimpleSauce(SimpleSauceDish* dish) {
    cout << "\n=== Готовим блюдо: " << dish->getName() << " ===\n";
    if (!dish->base || !dish->cream) throw IngredientNotFoundException("Нет основы или сливок для соуса");
    if (!dish->pan || !dish->stove)  throw ToolNotAvailableException("Нет сковороды или плиты для соуса");
    if (!dish->heatTimer)  throw TimerNotSetException("Нет таймера для соуса");
    if (!dish->mixer)   throw ToolNotAvailableException("Нет миксера для соуса");
    if (!dish->mixer->plugIn()) {
        throw ToolNotAvailableException("Миксер не включён в розетку для соуса");
    }

    dish->base->useAmount(50.0);
    dish->cream->useAmount(50.0);
    cout << "Смешиваем основу и сливки миксером...\n";
    dish->mixer->mix();

    cout << "Переливаем соус на сковороду и прогреваем...\n";
    dish->stove->turnOnBurner();
    dish->pan->heatUp();

    const int totalSeconds = 4 * 60;
    const int step = 60;
    int secondsPassed = 0;
    dish->heatTimer->start(totalSeconds);
    while (!dish->heatTimer->isFinished()) {
        dish->heatTimer->tick(step);
        secondsPassed += step;
        cout << "Прошло " << secondsPassed / 60 << " мин прогрева соуса...\n";
    }
    dish->pan->coolDown();
    dish->stove->turnOffBurner();
    cout << "Простой сливочный соус готов!\n";
}

/* ===== Menu ===== */

void Menu::show() const {
    cout << "\n==== МЕНЮ ====\n";
    for (size_t i = 0; i < dishes.size(); ++i) {
        cout << (i + 1) << ") " << dishes[i]->getName() << "\n";
    }
    cout << "0) Выход\n";
}

void Menu::addDish(Dish* d) {
    if (d) dishes.push_back(d);
}

void Menu::run() {
    while (true) {
        show();
        cout << "Выберите номер блюда: ";
        int choice = 0;
        if (!(cin >> choice)) {
            cout << "Некорректный ввод, выходим.\n";
            return;
        }
        if (choice == 0) {
            cout << "Выход из меню.\n";
            return;
        }
        if (choice < 1 || choice > static_cast<int>(dishes.size())) {
            cout << "Нет такого пункта.\n";
            continue;
        }
        dishes[choice - 1]->cook();

        cout << "Блюдо \"" << dishes[choice - 1]->getName()
             << "\" успешно приготовлено.\n";
    }
}
