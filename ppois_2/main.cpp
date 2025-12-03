/**
 * @file main.cpp
 * @brief Точка входа программы: инициализирует кухню, создаёт блюда и запускает меню.
 *
 * В функции main:
 * - создаются единицы измерения, количества и ингредиенты;
 * - инициализируются кухонные инструменты (плита, духовка, ножи, посуда, таймеры и т.п.);
 * - создаётся один повар, который агрегируется во все блюда;
 * - формируется меню из нескольких блюд;
 * - запускается интерактивный цикл выбора и приготовления блюд.
 */

#include "kitchen.hpp"

/**
 * @brief Точка входа в программу.
 *
 * Настраивает кухню, создаёт повара, блюда и запускает меню.
 * При возникновении исключений выводит сообщение об ошибке и корректно завершает работу.
 *
 * @return 0 при успешном завершении, 1 при критической ошибке.
 */
int main() {
    
    //В блоке try выполняется «опасный» участок кода: если где-то внутри него происходит throw, то выполнение сразу прерывается, оставшаяся часть try больше не выполняется, а управление передаётся в первый подходящий catch (по типу исключения), который найдётся при подъёме по стеку вызовов. В этом catch мы уже обрабатываем ошибку: можем вывести сообщение, освободить ресурсы, что-то починить в состоянии программы; после завершения catch выполнение продолжается уже после всей конструкции try { … } catch (…) { … }, как после обычного блока. Если же ни на этом уровне, ни выше в стеке не нашлось ни одного catch, подходящего под выброшенный тип исключения, то обработчик так и не находится, и в итоге вызывается std::terminate, после чего программа аварийно завершается.
    try {
        // ==== ЕДИНИЦЫ ИЗМЕРЕНИЯ ====
        Unit gramUnit("г", 1.0, false, 1);
        Unit mlUnit("мл", 1.0, true, 2);
        Unit pieceUnit("шт", 50.0, false, 3); ///< Условно 1 шт ~ 50 г.

        // ==== КОЛИЧЕСТВА И ИНГРЕДИЕНТЫ ====
        // Запасы сделаны с запасом, чтобы можно было несколько раз готовить разные блюда.

        Ingredient chicken(
            "Курица",
            Quantity(1000.0, &gramUnit, false, 1),
            215.0,
            true
        );

        Ingredient beef(
            "Говядина",
            Quantity(1200.0, &gramUnit, false, 2),
            250.0,
            true
        );

        Ingredient mixedVeggies(
            "Овощи микс",
            Quantity(1500.0, &gramUnit, false, 3),
            45.0,
            true
        );

        Ingredient tomatoes(
            "Томаты",
            Quantity(1000.0, &gramUnit, false, 4),
            20.0,
            true
        );

        Ingredient potatoes(
            "Картофель",
            Quantity(2000.0, &gramUnit, false, 5),
            80.0,
            true
        );

        Ingredient rice(
            "Рис",
            Quantity(1000.0, &gramUnit, false, 6),
            330.0,
            false
        );

        Ingredient pasta(
            "Паста",
            Quantity(1000.0, &gramUnit, false, 7),
            340.0,
            false
        );

        Ingredient saladOil(
            "Оливковое масло",
            Quantity(500.0, &mlUnit, true, 8),
            880.0,
            true
        );

        Ingredient milk(
            "Молоко",
            Quantity(1500.0, &mlUnit, true, 9),
            60.0,
            true
        );

        Ingredient cream(
            "Сливки",
            Quantity(500.0, &mlUnit, true, 10),
            200.0,
            true
        );

        Ingredient flour(
            "Мука",
            Quantity(1500.0, &gramUnit, false, 11),
            340.0,
            false
        );

        Ingredient sugar(
            "Сахар",
            Quantity(500.0, &gramUnit, false, 12),
            400.0,
            false
        );

        Ingredient eggs(
            "Яйца",
            Quantity(12.0, &pieceUnit, false, 13),
            155.0,
            true
        );

        Ingredient bread(
            "Хлеб",
            Quantity(1000.0, &gramUnit, false, 14),
            250.0,
            true
        );

        Ingredient cheese(
            "Сыр",
            Quantity(800.0, &gramUnit, false, 15),
            330.0,
            true
        );

        Ingredient sauce(
            "Готовый соус для пасты",
            Quantity(700.0, &gramUnit, false, 16),
            80.0,
            true
        );

        Ingredient fruits(
            "Фрукты микс",
            Quantity(1500.0, &gramUnit, false, 17),
            60.0,
            true
        );

        Ingredient garlic(
            "Чеснок",
            Quantity(100.0, &gramUnit, false, 18),
            120.0,
            true
        );

        Ingredient baseForSauce(
            "Основа для соуса",
            Quantity(300.0, &gramUnit, false, 19),
            150.0,
            false
        );


        Knife chefKnife("Шеф-нож", true, 20, 1);
        CuttingBoard woodenBoard("Деревянная доска", true, false, 1);

        Pan universalPan("Универсальная сковорода", 26.0, true, false);
        Pot soupPot("Кастрюля для супа", 3.0, true, false);
        Pot pastaPot("Кастрюля для пасты", 4.0, true, false);

        PotatoMasher potatoMasherTool("Толкушка для пюре", 1);

        Mixer kitchenMixer("Кухонный миксер", false);

        Oven mainOven(0.0, false, true);
        Stove mainStove(4, 0, true, false);

        
        Timer soupTimer(0, false, 0, 1);
        Timer pastaTimer(0, false, 0, 2);
        Timer pancakesTimer(0, false, 0, 3);
        Timer steakTimer(0, false, 0, 4);
        Timer cookiesTimer(0, false, 0, 5);
        Timer mashedPotatoTimer(0, false, 0, 6);
        Timer tomatoSoupTimer(0, false, 0, 7);
        Timer riceTimer(0, false, 0, 8);

       
        Cook mainCook("Главный повар"); ///< Один повар, агрегируется во все блюда.

        
        ChickenSoupDish chickenSoup(
            "Куриный суп",
            &chicken,
            &mixedVeggies,
            &soupPot,
            &mainStove,
            &soupTimer,
            &mainCook
        );

       
        SaladDish veggieSalad(
            "Овощной салат",
            &mixedVeggies,
            &saladOil,
            &chefKnife,
            &woodenBoard,
            &mainCook
        );

       
        PastaDish pastaDish(
            "Паста с соусом",
            &pasta,
            &sauce,
            &pastaPot,
            &mainStove,
            &pastaTimer,
            &mainCook
        );

        
        PancakeDish pancakes(
            "Блины",
            &flour,
            &eggs,
            &sugar,
            &milk,
            &universalPan,
            &mainStove,
            &pancakesTimer,
            &kitchenMixer,
            &mainCook
        );

       
        SteakDish steak(
            "Стейк на сковороде",
            &beef,
            &universalPan,
            &mainStove,
            &steakTimer,
            &mainCook
        );

       
        MashedPotatoDish mashedPotato(
            "Картофельное пюре",
            &potatoes,
            &milk,
            &soupPot,
            &mainStove,
            &mashedPotatoTimer,
            &potatoMasherTool,
            &mainCook
        );

       
        CookieDish cookies(
            "Домашнее печенье",
            &flour,
            &eggs,
            &milk,
            &sugar,
            &mainOven,
            &kitchenMixer,
            &mainCook
        );

        
        TomatoSoupDish tomatoSoup(
            "Томатный суп",
            &tomatoes,
            &mixedVeggies,
            &soupPot,
            &mainStove,
            &tomatoSoupTimer,
            &chefKnife,
            &woodenBoard,
            &mainCook
        );

        FruitSaladDish fruitSalad(
            "Фруктовый салат",
            &fruits,
            &chefKnife,
            &woodenBoard,
            &mainCook
        );

       
        SimpleSauceDish simpleSauce(
            "Простой сливочный соус",
            &baseForSauce,
            &cream,
            &universalPan,
            &mainStove,
            &cookiesTimer,  ///< Переиспользуем таймер, это допустимо для демонстрации.
            &kitchenMixer,
            &mainCook
        );

        
        Menu menu;
        menu.addDish(&chickenSoup);
        menu.addDish(&veggieSalad);
        menu.addDish(&pastaDish);
        menu.addDish(&pancakes);
        menu.addDish(&steak);
        menu.addDish(&mashedPotato);
        menu.addDish(&cookies);
        menu.addDish(&tomatoSoup);
        menu.addDish(&fruitSalad);
        menu.addDish(&simpleSauce);

        
        menu.run();
    }
    
    catch (const std::exception& ex) {
        std::cerr << "\nКритическая ошибка во время работы программы: "
                  << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
