"""Приклади Python-коду до практичного завдання № 1.

Тема: «Правила оформлення програмного коду (Code Convention).
Мова програмування Python».
Авторка: Андренко Анна Сергіївна, ПЗПІ-24-5.

Джерело фрагментів: фінальна презентація до ПЗ1, слайди 5–10.
Невдалі приклади збережено в рядкових константах: вони призначені
для порівняння та не впливають на виконання програми.
Покращені приклади виконуються як Python-код.

У презентації на слайді 7 функція get_total містить лише docstring.
Тут її доповнено обчисленням зі слайда 6, щоб приклад був робочим.
Додаткові перевірки test_bulk_discount_threshold та
test_calculate_difference підготовлені для демонстрації тестування.
"""

# Слайд 5. Форматування коду за PEP 8.
BAD_FORMATTING = """def calc(a,b):
  if a>b:return a-b
  return b-a
"""


def calculate_difference(first, second):
    """Повертає невід'ємну різницю двох чисел."""
    if first > second:
        return first - second
    return second - first


# Слайди 6–7. Іменування, константи та документування.
BAD_NAMING = """def f(p, q):
    return p * q * 0.85
"""
BAD_COMMENT = "i = i + 1  # додаємо 1"

DISCOUNT_RATE = 0.15


def get_total(price, count):
    """Повертає вартість зі знижкою.

    :param price: ціна одиниці
    :param count: кількість
    :return: підсумкова вартість
    """
    # Доповнення відносно слайда 7: реалізація зі слайда 6.
    total = price * count
    return total * (1 - DISCOUNT_RATE)


# Слайд 8. Конвенції Python: створення списку квадратів.
BAD_SQUARES = """squares = []
for n in range(5):
    squares.append(n * n)
"""
squares = [n * n for n in range(5)]

# Команди з презентації, які слід виконувати в терміналі:
# black . && flake8 .                        # слайд 8
# black --check . && flake8 && pytest        # слайд 9


# Слайд 9. Тест очікуваної поведінки.
def test_discount_applied():
    assert get_total(100, 2) == 170


# Слайд 10. Комплексний приклад «до і після».
BAD_ORDER = """def f(p, q):
    t = p * q
    if q > 10:
        t = t * 0.85
    return t
"""

BULK_COUNT = 10


def get_order_total(price, count):
    """Повертає вартість замовлення."""
    total = price * count
    if count > BULK_COUNT:
        total *= 1 - DISCOUNT_RATE
    return total


# Додаткові перевірки прикладів зі слайдів 5 та 10.
def test_calculate_difference():
    assert calculate_difference(8, 3) == 5
    assert calculate_difference(3, 8) == 5


def test_bulk_discount_threshold():
    assert get_order_total(100, 10) == 1000
    assert get_order_total(100, 12) == 1020


if __name__ == "__main__":
    # Самоперевірка без установлення сторонніх пакетів.
    test_discount_applied()
    test_calculate_difference()
    test_bulk_discount_threshold()
    assert squares == [0, 1, 4, 9, 16]
    print("Усі приклади перевірено успішно.")
