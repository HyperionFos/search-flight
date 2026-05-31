#!/usr/bin/env python3
"""Построение графиков для ЛР2 «Алгоритмы поиска данных» (вариант 21).

Читает data/timings.csv и data/collisions_study.csv,
строит 3 файла с графиками в папке data/.
"""

import os
import pandas as pd
import matplotlib.pyplot as plt

# путь к папке data (на уровень выше scripts/)
HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(HERE, "..", "data")

# методы поиска: столбец в csv -> (подпись, маркер)
METHODS = [
    ("linear",   "Линейный поиск",       "o"),
    ("bst",      "BST",                   "s"),
    ("rbt",      "Красно-чёрное дерево",  "^"),
    ("hash",     "Хэш-таблица",           "D"),
    ("multimap", "std::multimap",         "v"),
]


def plot_search_time():
    df = pd.read_csv(os.path.join(DATA, "timings.csv"))
    # время в csv в наносекундах -> переведём в микросекунды для читаемости
    for col, _, _ in METHODS:
        df[col] = df[col] / 1000.0

    # --- линейная шкала ---
    plt.figure(figsize=(10, 6))
    for col, label, mk in METHODS:
        plt.plot(df["size"], df[col], marker=mk, label=label)
    plt.xlabel("Размер массива n")
    plt.ylabel("Среднее время одного поиска, мкс")
    plt.title("Время поиска от размера массива (линейная шкала)")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(DATA, "plot_search_linear.png"), dpi=130)
    plt.close()

    # --- log-log шкала ---
    plt.figure(figsize=(10, 6))
    for col, label, mk in METHODS:
        plt.plot(df["size"], df[col], marker=mk, label=label)
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Размер массива n (log)")
    plt.ylabel("Среднее время одного поиска, мкс (log)")
    plt.title("Время поиска от размера массива (log-log шкала)")
    plt.legend()
    plt.grid(True, which="both", alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(DATA, "plot_search_log.png"), dpi=130)
    plt.close()


def plot_collisions():
    timings = pd.read_csv(os.path.join(DATA, "timings.csv"))
    study = pd.read_csv(os.path.join(DATA, "collisions_study.csv"))
    by_table = study[study["experiment"] == "table_size"]
    by_keys = study[study["experiment"] == "unique_keys"]

    fig, axes = plt.subplots(1, 3, figsize=(16, 5))

    # (1) коллизии vs размер массива n — по букве ТЗ (плоская линия)
    axes[0].plot(timings["size"], timings["collisions"], marker="o", color="tab:green")
    axes[0].set_xscale("log")
    axes[0].set_xlabel("Размер массива n (log)")
    axes[0].set_ylabel("Число коллизий")
    axes[0].set_title("Коллизии vs размер массива\n(~50 ключей, таблица 101)")
    axes[0].grid(True, alpha=0.3)

    # (2) коллизии vs размер таблицы
    axes[1].plot(by_table["param"], by_table["collisions"], marker="o", color="tab:red")
    axes[1].set_xlabel("Размер таблицы (число корзин)")
    axes[1].set_ylabel("Число коллизий")
    axes[1].set_title("Коллизии vs размер таблицы\n(50 ключей)")
    axes[1].grid(True, alpha=0.3)

    # (3) коллизии vs число уникальных ключей
    axes[2].plot(by_keys["param"], by_keys["collisions"], marker="s", color="tab:blue")
    axes[2].set_xlabel("Число уникальных ключей")
    axes[2].set_ylabel("Число коллизий")
    axes[2].set_title("Коллизии vs число ключей\n(таблица 211)")
    axes[2].grid(True, alpha=0.3)

    plt.tight_layout()
    plt.savefig(os.path.join(DATA, "plot_collisions.png"), dpi=130)
    plt.close()


if __name__ == "__main__":
    plot_search_time()
    plot_collisions()
    print("Графики сохранены в", os.path.normpath(DATA))