import csv
from collections import Counter
import math
import os

# ----- Paste your paths here -----
path_random = [
    86, 6, 59, 8, 34, 156, 74, 91, 182, 103, 127, 163, 153, 168, 148, 44, 164, 185,
    18, 194, 70, 97, 176, 116, 120, 54, 10, 146, 111, 72, 154, 31, 132, 50, 184, 100,
    95, 93, 7, 82, 1, 90, 183, 191, 19, 126, 63, 197, 166, 198, 94, 79, 181, 22, 23,
    53, 139, 11, 137, 138, 57, 5, 15, 162, 41, 161, 151, 117, 51, 46, 48, 28, 55, 109,
    133, 21, 65, 42, 29, 47, 145, 125, 196, 130, 121, 40, 12, 0, 69, 78, 108, 123, 81,
    101, 129, 62, 66, 140, 99, 113
]
path_neigbour_end = [
    154, 180, 53, 63, 176, 80, 151, 59, 65, 116, 42, 193, 41, 139, 115, 46, 0, 183,
    143, 117, 93, 18, 22, 34, 160, 184, 123, 135, 70, 127, 162, 133, 79, 94, 97, 101,
    1, 152, 120, 78, 145, 185, 40, 165, 90, 81, 113, 175, 171, 16, 31, 44, 92, 57,
    106, 49, 144, 62, 14, 178, 52, 55, 129, 2, 75, 86, 26, 100, 121, 148, 137, 23,
    186, 89, 114, 15, 9, 102, 138, 21, 164, 7, 95, 39, 27, 196, 179, 25, 82, 167,
    124, 51, 118, 43, 149, 131, 112, 4, 177, 54
]
path_neighbour = [
    196, 81, 90, 165, 119, 40, 185, 106, 178, 14, 144, 62, 9, 148, 102, 49, 52, 55,
    57, 129, 92, 179, 145, 78, 31, 56, 113, 175, 171, 16, 25, 44, 120, 2, 75, 101,
    1, 152, 97, 26, 100, 86, 53, 154, 70, 135, 180, 94, 63, 79, 133, 127, 123, 162,
    151, 51, 80, 176, 137, 23, 186, 89, 183, 143, 0, 117, 93, 140, 68, 46, 139, 115,
    118, 59, 65, 116, 43, 184, 35, 84, 112, 4, 190, 10, 177, 54, 48, 160, 34, 181,
    42, 5, 41, 193, 159, 146, 22, 18, 69, 108
]
path_greedy = [
    40, 119, 185, 52, 55, 57, 129, 92, 179, 145, 78, 31, 56, 113, 175, 171, 16, 25,
    44, 120, 2, 75, 86, 100, 26, 101, 1, 97, 152, 124, 94, 63, 53, 180, 154, 135,
    70, 127, 123, 112, 4, 84, 35, 184, 190, 10, 177, 54, 48, 160, 34, 181, 146, 22,
    18, 108, 69, 159, 193, 41, 139, 68, 46, 115, 5, 42, 43, 116, 65, 59, 118, 51,
    162, 151, 133, 79, 80, 176, 137, 0, 117, 143, 183, 89, 23, 186, 15, 148, 9, 62,
    102, 144, 14, 49, 178, 106, 165, 90, 81, 196
]

# ----- helper functions -----
def analyze(path, name, nodes_count=None):
    cnt = Counter(path)
    dupes = [x for x, c in cnt.items() if c > 1]
    max_idx = max(path) if path else -1
    inferred_n = max_idx + 1
    n = nodes_count if nodes_count is not None else inferred_n
    out_of_range = [i for i in path if i < 0 or i >= n]
    missing = sorted(set(range(n)) - set(path))
    print(f"--- {name} ---")
    print(f"Length: {len(path)}, Max index: {max_idx}, Inferred nodes: {inferred_n}")
    print("Duplicates:" , dupes or "None")
    print("Out of range indices:" , out_of_range or "None")
    print("Missing indices count:" , len(missing))
    if len(missing) <= 20:
        print("Missing indices:", missing)
    else:
        print("Missing indices (first 20):", missing[:20], "...")
    print()

def try_read_csv_nodes(csv_path):
    if not os.path.exists(csv_path):
        return None
    nodes = []
    with open(csv_path, 'r', newline='') as f:
        sample = f.read(2048); f.seek(0)
        try:
            dialect = csv.Sniffer().sniff(sample, delimiters=';,')
            reader = csv.reader(f, dialect)
        except csv.Error:
            reader = csv.reader(f, delimiter=';')
        for row in reader:
            if not row or len(row) < 2: continue
            try:
                x = float(row[0]); y = float(row[1])
            except ValueError:
                continue
            nodes.append((x, y))
    return nodes

if __name__ == "__main__":
    CSV = r"C:\Users\maria\OneDrive\Dokumenty\GitHub\EvolutionaryComputing\TSPA.csv"
    nodes = try_read_csv_nodes(CSV)
    nodes_n = len(nodes) if nodes is not None else None
    if nodes_n is not None:
        print("Loaded nodes from CSV:", CSV, "count =", nodes_n)
    else:
        print("CSV not found at", CSV, "- using inferred node counts from paths.")

    analyze(path_random, "path_random", nodes_n)
    analyze(path_neigbour_end, "path_neigbour_end", nodes_n)
    analyze(path_neighbour, "path_neighbour", nodes_n)
    analyze(path_greedy, "path_greedy", nodes_n)