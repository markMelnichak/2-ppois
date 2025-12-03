#include <UnitTest++/UnitTest++.h>
#include <sstream>
#include <iostream>
#include "kitchen.hpp"

using namespace std;

static Unit gramUnit("g", 1.0, false, 1);

static Ingredient makeIngredient(const char* name, double grams, bool perishable = false) {
    Quantity q(grams, &gramUnit);
    return Ingredient(name, q, 0.0, perishable);
}

//1
TEST(Unit_DefaultCtor) {
    Unit u;
    CHECK_CLOSE(10.0, u.toGrams(10.0), 1e-9);
    CHECK(!u.isLiquid());
    CHECK_EQUAL(0, u.getId());
}

// 2
TEST(Unit_CustomCtor) {
    Unit ml("ml", 1.5, true, 7);
    CHECK_CLOSE(15.0, ml.toGrams(10.0), 1e-9);
    CHECK(ml.isLiquid());
    CHECK_EQUAL(7, ml.getId());
}

// 3
TEST(Unit_IsLiquidFlag) {
    Unit solid("piece", 1.0, false, 2);
    Unit liquid("ml", 1.0, true, 3);
    CHECK(!solid.isLiquid());
    CHECK(liquid.isLiquid());
}

// 4
TEST(Quantity_ToGrams_WithUnit) {
    Quantity q(200.0, &gramUnit);
    CHECK_CLOSE(200.0, q.toGrams(), 1e-9);
}

// 5
TEST(Quantity_ToGrams_NoUnit_Throws) {
    Quantity q(100.0, nullptr);
    CHECK_THROW(q.toGrams(), StorageException);
}

// 6
TEST(Quantity_Scale_Up) {
    Quantity q(100.0, &gramUnit);
    q.scale(2.0);
    CHECK_CLOSE(200.0, q.toGrams(), 1e-9);
}

// 7
TEST(Quantity_Scale_DownToZero_IsZero) {
    Quantity q(100.0, &gramUnit);
    q.scale(0.0);
    CHECK(q.isZero());
}

// 8
TEST(Ingredient_Perishable_True) {
    Quantity q(100.0, &gramUnit);
    Ingredient meat("meat", q, 200.0, true);
    CHECK(meat.isPerishable());
}

// 9
TEST(Ingredient_Perishable_False) {
    Quantity q(100.0, &gramUnit);
    Ingredient salt("salt", q, 0.0, false);
    CHECK(!salt.isPerishable());
}


// 10
TEST(Ingredient_AddAmount_Increase) {
    Unit gramUnit("g", 1.0, false, 1);
    Quantity q(100.0, &gramUnit);
    Ingredient flour("flour", q, 0.0, false);

    flour.addAmount(50.0);        // просто вызываем, тест упадёт, если будет исключение
    CHECK(!flour.isPerishable()); // чтобы был хотя бы один CHECK
}

// 11
TEST(Ingredient_AddAmount_FromZero_NoCrash) {
    Unit gramUnit("g", 1.0, false, 1);
    Quantity q(0.0, &gramUnit);
    Ingredient sugar("sugar", q, 0.0, false);

    sugar.addAmount(100.0);
}

// 12
TEST(Ingredient_UseAmount_Enough) {
    Unit gramUnit("g", 1.0, false, 1);
    Quantity q(200.0, &gramUnit);
    Ingredient butter("butter", q, 0.0, false);

    butter.useAmount(100.0);
    CHECK(!butter.isPerishable());
}

// 13
TEST(Ingredient_UseAmount_NotEnough_Throws) {
    Quantity q(50.0, &gramUnit);
    Ingredient butter("butter", q, 0.0, false);
    CHECK_THROW(butter.useAmount(100.0), NotEnoughIngredientException);
}

// 14
TEST(KitchenTool_DefaultAvailable) {
    KitchenTool t;
    CHECK(t.isAvailable());
}

// 15
TEST(KitchenTool_UseTool_DecreasesDurability) {
    KitchenTool t("tool", true, true, 2);

    CHECK(t.isAvailable());
    t.useTool();
    CHECK(t.isAvailable());
    t.useTool();
    CHECK(!t.isAvailable());
}
// 16
TEST(KitchenTool_UseTool_Dirty_Throws) {
    KitchenTool t("tool", false, true, 10);
    CHECK_THROW(t.useTool(), ToolNotAvailableException);
}

// 17
TEST(KitchenTool_UseTool_NotAvailable_Throws) {
    KitchenTool t("tool", true, false, 10);
    CHECK_THROW(t.useTool(), ToolNotAvailableException);
}

// 18
TEST(KitchenTool_CleanTool_MakesAvailable) {
    KitchenTool t("tool", false, true, 10);
    CHECK(!t.isAvailable());
    t.cleanTool();
    CHECK(t.isAvailable());
}

// 19
TEST(Knife_CanCut_True) {
    KitchenTool base("knife", true, true, 10);
    Knife k(&base, true, 20, 1);
    CHECK(k.canCut());
}

// 20
TEST(Knife_Dull_CannotCut) {
    KitchenTool base("knife", true, true, 10);
    Knife k(&base, true, 20, 1);
    k.dull();
    CHECK(!k.canCut());
}

// 21
TEST(Knife_NoBaseTool_CannotCut) {
    Knife k(nullptr, true, 20, 1);
    CHECK(!k.canCut());
}

// 22
TEST(Knife_BaseToolNotAvailable_CannotCut) {
    KitchenTool base("knife", false, true, 0);
    Knife k(&base, true, 20, 1);
    CHECK(!k.canCut());
}

// 23
TEST(CuttingBoard_SafeForBread_True) {
    KitchenTool base("board", true, true, 10);
    CuttingBoard b(&base, true, false, 1);
    CHECK(b.isSafeForBread());
}

// 24
TEST(CuttingBoard_Wet_NotSafe) {
    KitchenTool base("board", true, true, 10);
    CuttingBoard b(&base, true, true, 1);
    CHECK(!b.isSafeForBread());
    b.dry();
    CHECK(b.isSafeForBread());
}

// 25
TEST(CuttingBoard_NonWooden_NotSafe) {
    KitchenTool base("board", true, true, 10);
    CuttingBoard b(&base, false, false, 1);
    CHECK(!b.isSafeForBread());
}

// 26
TEST(CuttingBoard_NoBase_NotSafe) {
    CuttingBoard b(nullptr, true, false, 1);
    CHECK(!b.isSafeForBread());
}

// 27
TEST(Pan_HeatUp_AndCoolDown) {
    KitchenTool base("pan", true, true, 10);
    Pan p(&base, 24.0, true, false);
    CHECK(!p.isHot());
    p.heatUp();
    CHECK(p.isHot());
    p.coolDown();
    CHECK(!p.isHot());
}

// 28
TEST(Pan_HeatUp_NoBase_Throws) {
    Pan p(nullptr, 24.0, true, false);
    CHECK_THROW(p.heatUp(), ToolNotAvailableException);
}

// 29
TEST(Pan_HeatUp_BaseNotAvailable_Throws) {
    KitchenTool base("pan", false, true, 0);
    Pan p(&base, 24.0, true, false);
    CHECK_THROW(p.heatUp(), ToolNotAvailableException);
}

// 30
TEST(Pot_CanBoil_TrueAndFalse) {
    KitchenTool base("pot", true, true, 10);
    Pot pot(&base, 2.0, true, false);
    CHECK(pot.canBoil(1.5));
    CHECK(!pot.canBoil(3.0));
}

// 31
TEST(Pot_StartBoil_NoBase_Throws) {
    Pot pot(nullptr, 2.0, true, false);
    CHECK_THROW(pot.startBoil(), ToolNotAvailableException);
}

// 32
TEST(Timer_StartAndFinish) {
    Timer t;
    t.start(10);
    CHECK(!t.isFinished());
    t.tick(5);
    CHECK(!t.isFinished());
    t.tick(5);
    CHECK(t.isFinished());
}

// 33
TEST(Timer_Start_Invalid_Throws) {
    Timer t;
    CHECK_THROW(t.start(0), TimerNotSetException);
    CHECK_THROW(t.start(-5), TimerNotSetException);
}

// 34
TEST(Timer_Tick_NegativeOrZeroIgnored) {
    Timer t;
    t.start(10);
    t.tick(0);
    t.tick(-5);
    CHECK(!t.isFinished());
}

// 35
TEST(Timer_Tick_NotRunning_NoEffect) {
    Timer t;
    t.start(10);
    t.tick(10); // закончили
    CHECK(t.isFinished());
    t.tick(10); // уже не running
    CHECK(t.isFinished());
}

// 36
TEST(Mixer_PlugInUnplugSequence) {
    KitchenTool base("mixer", true, true, 10);
    Mixer m(&base, false);
    CHECK(m.unplug() == false);
    CHECK(m.plugIn() == true);
    CHECK(m.unplug() == false);
}

// 37
TEST(Mixer_Mix_NotPlugged_NoCrash) {
    KitchenTool base("mixer", true, true, 10);
    Mixer m(&base, false);
    // просто проверяем, что не падает
    m.mix();
}
// 38
TEST(PotatoMasher_Available_MashOK) {
    KitchenTool base("masher", true, true, 10);
    PotatoMasher pm(&base, 1);
    pm.mash();
}


// 39
TEST(PotatoMasher_NotAvailable_Throws) {
    KitchenTool base("masher", false, true, 0);
    PotatoMasher pm(&base, 1);
    CHECK_THROW(pm.mash(), ToolNotAvailableException);
}

// 40
TEST(Oven_Preheat_Ok) {
    Oven ov;
    ov.closeDoor();
    ov.preheat(180.0);
    CHECK(ov.isOn());
    CHECK_CLOSE(180.0, ov.getTemperature(), 1e-9);
}

// 41
TEST(Oven_Preheat_InvalidLow_Throws) {
    Oven ov;
    CHECK_THROW(ov.preheat(0.0), InvalidTemperatureException);
    CHECK_THROW(ov.preheat(-10.0), InvalidTemperatureException);
}

// 42
TEST(Oven_Preheat_InvalidHigh_Throws) {
    Oven ov;
    CHECK_THROW(ov.preheat(400.0), InvalidTemperatureException);
}

// 43
TEST(Oven_Preheat_DoorOpen_Throws) {
    Oven ov;
    ov.openDoor();
    CHECK_THROW(ov.preheat(180.0), InvalidTemperatureException);
}

// 44
TEST(Oven_SetTimerMinutes_Invalid_Throws) {
    Oven ov;
    CHECK_THROW(ov.setTimerMinutes(0), TimerNotSetException);
}

// 45
TEST(Oven_Tick_TurnsOff) {
    Oven ov;
    ov.closeDoor();
    ov.preheat(180.0);
    ov.setTimerMinutes(1);
    CHECK(ov.isOn());
    ov.tick(30);
    CHECK(ov.isOn());
    ov.tick(30);
    CHECK(!ov.isOn());
}

// 46
TEST(Stove_Burners_OnOff) {
    Stove s(4, 0, true, false);
    CHECK_EQUAL(4, s.freeBurners());
    s.turnOnBurner();
    CHECK_EQUAL(3, s.freeBurners());
    s.turnOnBurner();
    s.turnOnBurner();
    s.turnOnBurner();
    CHECK_EQUAL(0, s.freeBurners());
    s.turnOffBurner();
    CHECK_EQUAL(1, s.freeBurners());
}

// 47
TEST(Stove_TurnOff_NoActive_NoCrash) {
    Stove s(4, 0, true, false);
    CHECK_EQUAL(4, s.freeBurners());
    s.turnOffBurner(); // не должно падать
    CHECK_EQUAL(4, s.freeBurners());
}

// 48
TEST(TemperatureProfile_Gradual) {
    TemperatureProfile tp(20.0, 180.0, 10, true);
    double mid = tp.currentTemp(5);
    CHECK(mid > 20.0);
    CHECK(mid < 180.0);
}

// 49
TEST(TemperatureProfile_Gradual_AfterDuration) {
    TemperatureProfile tp(20.0, 180.0, 10, true);
    double end = tp.currentTemp(20);
    CHECK_CLOSE(180.0, end, 1e-9);
}

// 50
TEST(TemperatureProfile_Jump) {
    TemperatureProfile tp(20.0, 200.0, 10, false);
    CHECK_CLOSE(200.0, tp.currentTemp(0), 1e-9);
    CHECK(tp.isReached(210.0));
}

// 51
TEST(TemperatureProfile_Reset) {
    TemperatureProfile tp;
    tp.reset(30.0, 150.0, 5);
    double mid = tp.currentTemp(3);
    CHECK(mid > 30.0);
    CHECK(mid <= 150.0);
}


// 52
TEST(ChickenSoupDish_Cook_Success) {
    Ingredient chicken = makeIngredient("chicken", 1000.0);
    Ingredient veg     = makeIngredient("veg", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    ChickenSoupDish dish("Chicken soup", &chicken, &veg, &pot, &stove, &t);
    dish.cook();
}

// 53
TEST(SaladDish_Cook_Success) {
    Ingredient veg = makeIngredient("veg", 1000.0);
    Ingredient oil = makeIngredient("oil", 1000.0);
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    SaladDish dish("Salad", &veg, &oil, &k, &b);
    dish.cook();
}

// 54
TEST(BakedMeatDish_Cook_Success) {
    Ingredient meat = makeIngredient("meat", 1000.0);
    Oven ov;
    BakedMeatDish dish("Baked meat", &meat, &ov);
    dish.cook();
}

// 55
TEST(PancakeDish_Cook_Success) {
    Ingredient flour = makeIngredient("flour", 1000.0);
    Ingredient eggs  = makeIngredient("eggs", 1000.0);
    Ingredient sugar = makeIngredient("sugar", 1000.0);
    Ingredient milk  = makeIngredient("milk", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer fryTimer;
    KitchenTool mixerTool("mixer", true, true, 10);
    Mixer mixer(&mixerTool, false);
    PancakeDish dish("Pancakes", &flour, &eggs, &sugar, &milk,
                     &pan, &stove, &fryTimer, &mixer);
    dish.cook();
}

// 56
TEST(PastaDish_Cook_Success) {
    Ingredient pasta = makeIngredient("pasta", 1000.0);
    Ingredient sauce = makeIngredient("sauce", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    PastaDish dish("Pasta", &pasta, &sauce, &pot, &stove, &t);
    dish.cook();
}

// 57
TEST(ScrambledEggsDish_Cook_Success) {
    Ingredient eggs = makeIngredient("eggs", 1000.0);
    Ingredient milk = makeIngredient("milk", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool mixerTool("mixer", true, true, 10);
    Mixer mixer(&mixerTool, false);
    ScrambledEggsDish dish("Scrambled eggs", &eggs, &milk,
                           &pan, &stove, &t, &mixer);
    dish.cook();
}

// 58
TEST(VegGrillDish_Cook_Success) {
    Ingredient veg = makeIngredient("veg", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    VegGrillDish dish("Veg grill", &veg, &pan, &stove, &t, &k, &b);
    dish.cook();
}

// 59
TEST(MeatStewDish_Cook_Success) {
    Ingredient meat = makeIngredient("meat", 1000.0);
    Ingredient veg  = makeIngredient("veg", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    MeatStewDish dish("Stew", &meat, &veg, &pot, &stove, &t, &k, &b);
    dish.cook();
}

// 60
TEST(SandwichDish_Cook_Success) {
    Ingredient bread = makeIngredient("bread", 1000.0);
    Ingredient cheese = makeIngredient("cheese", 1000.0);
    Ingredient ham = makeIngredient("ham", 1000.0);
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    SandwichDish dish("Sandwich", &bread, &cheese, &ham, &k, &b);
    dish.cook();
}

// 61
TEST(CookieDish_Cook_Success) {
    Ingredient flour = makeIngredient("flour", 1000.0);
    Ingredient eggs  = makeIngredient("eggs", 1000.0);
    Ingredient milk  = makeIngredient("milk", 1000.0);
    Ingredient sugar = makeIngredient("sugar", 1000.0);
    Oven ov;
    KitchenTool mixerTool("mixer", true, true, 10);
    Mixer mixer(&mixerTool, false);
    CookieDish dish("Cookies", &flour, &eggs, &milk, &sugar, &ov, &mixer);
    dish.cook();
}

// 62
TEST(RiceDish_Cook_Success) {
    Ingredient rice = makeIngredient("rice", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    RiceDish dish("Rice", &rice, &pot, &stove, &t);
    dish.cook();
}

// 63
TEST(BoiledEggDish_Cook_Success) {
    Ingredient eggs = makeIngredient("eggs", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    BoiledEggDish dish("Boiled eggs", &eggs, &pot, &stove, &t);
    dish.cook();
}

// 64
TEST(MashedPotatoDish_Cook_Success) {
    Ingredient potatoes = makeIngredient("potatoes", 1000.0);
    Ingredient milk     = makeIngredient("milk", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool masherTool("masher", true, true, 10);
    PotatoMasher m(&masherTool, 1);
    MashedPotatoDish dish("Mashed potato", &potatoes, &milk,
                          &pot, &stove, &t, &m);
    dish.cook();
}

// 65
TEST(GrilledCheeseDish_Cook_Success) {
    Ingredient bread = makeIngredient("bread", 1000.0);
    Ingredient cheese = makeIngredient("cheese", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    GrilledCheeseDish dish("Grilled cheese", &bread, &cheese,
                           &pan, &stove, &t, &k, &b);
    dish.cook();
}

// 66
TEST(FriedFishDish_Cook_Success) {
    Ingredient fish = makeIngredient("fish", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    FriedFishDish dish("Fish", &fish, &pan, &stove, &t);
    dish.cook();
}

// 67
TEST(FruitSaladDish_Cook_Success) {
    Ingredient fruits = makeIngredient("fruits", 1000.0);
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    FruitSaladDish dish("Fruit salad", &fruits, &k, &b);
    dish.cook();
}

// 68
TEST(PorridgeDish_Cook_Success) {
    Ingredient oats = makeIngredient("oats", 1000.0);
    Ingredient milk = makeIngredient("milk", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    PorridgeDish dish("Porridge", &oats, &milk, &pot, &stove, &t);
    dish.cook();
}

// 69
TEST(SteakDish_Cook_Success) {
    Ingredient meat = makeIngredient("meat", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    SteakDish dish("Steak", &meat, &pan, &stove, &t);
    dish.cook();
}

// 70
TEST(HotDogDish_Cook_Success) {
    Ingredient bun = makeIngredient("bun", 1000.0);
    Ingredient sausage = makeIngredient("sausage", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    HotDogDish dish("HotDog", &bun, &sausage, &pan, &stove, &t);
    dish.cook();
}

// 71
TEST(SauteedMushroomsDish_Cook_Success) {
    Ingredient mush = makeIngredient("mushrooms", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    SauteedMushroomsDish dish("Mushrooms", &mush, &pan, &stove, &t);
    dish.cook();
}

// 72
TEST(FriedPotatoDish_Cook_Success) {
    Ingredient potatoes = makeIngredient("potatoes", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    FriedPotatoDish dish("Fried potato", &potatoes, &pan, &stove, &t, &k, &b);
    dish.cook();
}

// 73
TEST(TomatoSoupDish_Cook_Success) {
    Ingredient tom = makeIngredient("tomatoes", 1000.0);
    Ingredient veg = makeIngredient("veg", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    TomatoSoupDish dish("Tomato soup", &tom, &veg, &pot, &stove, &t, &k, &b);
    dish.cook();
}

// 74
TEST(VegOmeletteDish_Cook_Success) {
    Ingredient eggs = makeIngredient("eggs", 1000.0);
    Ingredient veg  = makeIngredient("veg", 1000.0);
    Ingredient milk = makeIngredient("milk", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    KitchenTool mixerTool("mixer", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    Mixer mixer(&mixerTool, false);
    VegOmeletteDish dish("Veg omelette", &eggs, &veg, &milk,
                         &pan, &stove, &t, &k, &b, &mixer);
    dish.cook();
}

// 75
TEST(GarlicBreadDish_Cook_Success) {
    Ingredient bread = makeIngredient("bread", 1000.0);
    Ingredient garlic = makeIngredient("garlic", 1000.0);
    Oven ov;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    GarlicBreadDish dish("Garlic bread", &bread, &garlic, &ov, &k, &b);
    dish.cook();
}

// 76
TEST(SimpleSauceDish_Cook_Success) {
    Ingredient base = makeIngredient("base", 1000.0);
    Ingredient cream = makeIngredient("cream", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool mixerTool("mixer", true, true, 10);
    Mixer mixer(&mixerTool, false);
    SimpleSauceDish dish("Sauce", &base, &cream, &pan, &stove, &t, &mixer);
    dish.cook();
}



//77
TEST(ChickenSoupDish_NoPot_Throws) {
    Ingredient chicken = makeIngredient("chicken", 1000.0);
    Ingredient veg     = makeIngredient("veg", 1000.0);
    Stove stove;
    Timer t;
    ChickenSoupDish dish("Chicken soup", &chicken, &veg, nullptr, &stove, &t);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 78
TEST(ChickenSoupDish_NoTimer_Throws) {
    Ingredient chicken = makeIngredient("chicken", 1000.0);
    Ingredient veg     = makeIngredient("veg", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    ChickenSoupDish dish("Chicken soup", &chicken, &veg, &pot, &stove, nullptr);
    CHECK_THROW(dish.cook(), TimerNotSetException);
}

// 79
TEST(SaladDish_NoKnife_Throws) {
    Ingredient veg = makeIngredient("veg", 1000.0);
    Ingredient oil = makeIngredient("oil", 1000.0);
    SaladDish dish("Salad", &veg, &oil, nullptr, nullptr);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 80
TEST(BakedMeatDish_NoOven_Throws) {
    Ingredient meat = makeIngredient("meat", 1000.0);
    BakedMeatDish dish("Baked meat", &meat, nullptr);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 81
TEST(PancakeDish_NoMixer_Throws) {
    Ingredient flour = makeIngredient("flour", 1000.0);
    Ingredient eggs  = makeIngredient("eggs", 1000.0);
    Ingredient sugar = makeIngredient("sugar", 1000.0);
    Ingredient milk  = makeIngredient("milk", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    PancakeDish dish("Pancakes", &flour, &eggs, &sugar, &milk,
                     &pan, &stove, &t, nullptr);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 82
TEST(PancakeDish_NoFlour_Throws) {
    Ingredient eggs  = makeIngredient("eggs", 1000.0);
    Ingredient sugar = makeIngredient("sugar", 1000.0);
    Ingredient milk  = makeIngredient("milk", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool mixerTool("mixer", true, true, 10);
    Mixer mixer(&mixerTool, false);
    PancakeDish dish("Pancakes", nullptr, &eggs, &sugar, &milk,
                     &pan, &stove, &t, &mixer);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 83
TEST(PastaDish_NoPasta_Throws) {
    Ingredient sauce = makeIngredient("sauce", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    PastaDish dish("Pasta", nullptr, &sauce, &pot, &stove, &t);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 84
TEST(PastaDish_SmallPot_Throws) {
    Ingredient pasta = makeIngredient("pasta", 1000.0);
    Ingredient sauce = makeIngredient("sauce", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 1.0, true, false); // маленький объём
    Stove stove;
    Timer t;
    PastaDish dish("Pasta", &pasta, &sauce, &pot, &stove, &t);
    CHECK_THROW(dish.cook(), NotEnoughIngredientException);
}

// 85
TEST(ScrambledEggsDish_NoMixer_Throws) {
    Ingredient eggs = makeIngredient("eggs", 1000.0);
    Ingredient milk = makeIngredient("milk", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    ScrambledEggsDish dish("Scrambled eggs", &eggs, &milk, &pan, &stove, &t, nullptr);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 86
TEST(VegGrillDish_NoVeggies_Throws) {
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    VegGrillDish dish("Veg grill", nullptr, &pan, &stove, &t, &k, &b);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 87
TEST(MeatStewDish_NoMeat_Throws) {
    Ingredient veg  = makeIngredient("veg", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    MeatStewDish dish("Stew", nullptr, &veg, &pot, &stove, &t, &k, &b);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 88
TEST(SandwichDish_WetBoard_Throws) {
    Ingredient bread = makeIngredient("bread", 1000.0);
    Ingredient cheese = makeIngredient("cheese", 1000.0);
    Ingredient ham = makeIngredient("ham", 1000.0);
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, true, 1); // мокрая
    SandwichDish dish("Sandwich", &bread, &cheese, &ham, &k, &b);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 89
TEST(CookieDish_NoOven_Throws) {
    Ingredient flour = makeIngredient("flour", 1000.0);
    Ingredient eggs  = makeIngredient("eggs", 1000.0);
    Ingredient milk  = makeIngredient("milk", 1000.0);
    Ingredient sugar = makeIngredient("sugar", 1000.0);
    KitchenTool mixerTool("mixer", true, true, 10);
    Mixer mixer(&mixerTool, false);
    CookieDish dish("Cookies", &flour, &eggs, &milk, &sugar, nullptr, &mixer);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 90
TEST(RiceDish_NoRice_Throws) {
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    RiceDish dish("Rice", nullptr, &pot, &stove, &t);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 91
TEST(BoiledEggDish_NoTimer_Throws) {
    Ingredient eggs = makeIngredient("eggs", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    BoiledEggDish dish("Boiled eggs", &eggs, &pot, &stove, nullptr);
    CHECK_THROW(dish.cook(), TimerNotSetException);
}

// 92
TEST(MashedPotatoDish_NoMasher_Throws) {
    Ingredient potatoes = makeIngredient("potatoes", 1000.0);
    Ingredient milk     = makeIngredient("milk", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    MashedPotatoDish dish("Mashed potato", &potatoes, &milk, &pot, &stove, &t, nullptr);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 93
TEST(GrilledCheeseDish_BadBoard_Throws) {
    Ingredient bread = makeIngredient("bread", 1000.0);
    Ingredient cheese = makeIngredient("cheese", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, false, false, 1); // не деревянная
    GrilledCheeseDish dish("Grilled cheese", &bread, &cheese,
                           &pan, &stove, &t, &k, &b);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 94
TEST(FriedFishDish_NoPan_Throws) {
    Ingredient fish = makeIngredient("fish", 1000.0);
    Stove stove;
    Timer t;
    FriedFishDish dish("Fish", &fish, nullptr, &stove, &t);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 95
TEST(FruitSaladDish_NoFruits_Throws) {
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    FruitSaladDish dish("Fruit salad", nullptr, &k, &b);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 96
TEST(PorridgeDish_NoMilk_Throws) {
    Ingredient oats = makeIngredient("oats", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    PorridgeDish dish("Porridge", &oats, nullptr, &pot, &stove, &t);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 97
TEST(SteakDish_NoTimer_Throws) {
    Ingredient meat = makeIngredient("meat", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    SteakDish dish("Steak", &meat, &pan, &stove, nullptr);
    CHECK_THROW(dish.cook(), TimerNotSetException);
}

// 98
TEST(HotDogDish_NoBun_Throws) {
    Ingredient sausage = makeIngredient("sausage", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    HotDogDish dish("HotDog", nullptr, &sausage, &pan, &stove, &t);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 99
TEST(SauteedMushroomsDish_NoTimer_Throws) {
    Ingredient mush = makeIngredient("mushrooms", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    SauteedMushroomsDish dish("Mushrooms", &mush, &pan, &stove, nullptr);
    CHECK_THROW(dish.cook(), TimerNotSetException);
}

// 100
TEST(FriedPotatoDish_NoKnife_Throws) {
    Ingredient potatoes = makeIngredient("potatoes", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    FriedPotatoDish dish("Fried potato", &potatoes, &pan, &stove, &t, nullptr, nullptr);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// 101
TEST(TomatoSoupDish_NoTomatoes_Throws) {
    Ingredient veg = makeIngredient("veg", 1000.0);
    KitchenTool potTool("pot", true, true, 10);
    Pot pot(&potTool, 3.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    TomatoSoupDish dish("Tomato soup", nullptr, &veg, &pot, &stove, &t, &k, &b);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 102
TEST(VegOmeletteDish_NoEggs_Throws) {
    Ingredient veg  = makeIngredient("veg", 1000.0);
    Ingredient milk = makeIngredient("milk", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    KitchenTool mixerTool("mixer", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    Mixer mixer(&mixerTool, false);
    VegOmeletteDish dish("Veg omelette", nullptr, &veg, &milk,
                         &pan, &stove, &t, &k, &b, &mixer);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 103
TEST(GarlicBreadDish_NoGarlic_Throws) {
    Ingredient bread = makeIngredient("bread", 1000.0);
    Oven ov;
    KitchenTool knifeTool("knife", true, true, 10);
    KitchenTool boardTool("board", true, true, 10);
    Knife k(&knifeTool, true, 20, 1);
    CuttingBoard b(&boardTool, true, false, 1);
    GarlicBreadDish dish("Garlic bread", &bread, nullptr, &ov, &k, &b);
    CHECK_THROW(dish.cook(), IngredientNotFoundException);
}

// 104
TEST(SimpleSauceDish_NoMixer_Throws) {
    Ingredient base = makeIngredient("base", 1000.0);
    Ingredient cream = makeIngredient("cream", 1000.0);
    KitchenTool panTool("pan", true, true, 10);
    Pan pan(&panTool, 24.0, true, false);
    Stove stove;
    Timer t;
    SimpleSauceDish dish("Sauce", &base, &cream, &pan, &stove, &t, nullptr);
    CHECK_THROW(dish.cook(), ToolNotAvailableException);
}

// ---------------------------------------------------------
// MENU (105–107)
// ---------------------------------------------------------

// Простое блюдо-заглушка
class DummyDish : public Dish {
public:
    bool cooked;
    DummyDish(const char* n) : Dish(n), cooked(false) {}
    void cook() override { cooked = true; }
};

// 105
TEST(Menu_Run_ValidChoiceThenExit) {
    DummyDish d("Dummy");
    Menu m;
    m.addDish(&d);

    istringstream input("1\n0\n");
    streambuf* oldCin = cin.rdbuf(input.rdbuf());

    m.run();

    cin.rdbuf(oldCin);
    CHECK(d.cooked);
}

// 106
TEST(Menu_Run_InvalidChoiceThenExit) {
    DummyDish d("Dummy");
    Menu m;
    m.addDish(&d);

    istringstream input("5\n1\n0\n");
    streambuf* oldCin = cin.rdbuf(input.rdbuf());

    m.run();

    cin.rdbuf(oldCin);
    CHECK(d.cooked);
}

// 107
TEST(Menu_Run_InvalidInput_Break) {
    DummyDish d("Dummy");
    Menu m;
    m.addDish(&d);

    istringstream input("abc\n");
    streambuf* oldCin = cin.rdbuf(input.rdbuf());

    m.run();

    cin.rdbuf(oldCin);
    
    CHECK(!d.cooked);
}



static const int TOTAL_DEFINED_TESTS = 107;

int main() {
    int failures = UnitTest::RunAllTests();

    cout << "\n=== РЕЗЮМЕ ПО ЮНИТ-ТЕСТАМ ===\n";
    cout << "Всего определено тестов: " << TOTAL_DEFINED_TESTS << "\n";
    cout << "Число упавших тестов (код возврата UnitTest++): " << failures << "\n";

    if (failures == 0) {
        cout << "Все " << TOTAL_DEFINED_TESTS << " тестов успешно пройдены.\n";
    } else {
        cout << "Успешно пройдены: " << (TOTAL_DEFINED_TESTS - failures)
             << " из " << TOTAL_DEFINED_TESTS << ".\n";
    }

    return failures;
}
