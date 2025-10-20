import csv
from collections import Counter
import math
import os

# ----- Paste your paths here -----
path_2regret_a = [49, 144, 62, 148, 15, 114, 186, 23, 89, 183, 153, 170, 117, 93, 140, 36, 67, 108, 69, 18, 20, 22, 146, 103, 34, 160, 192, 42, 5, 96, 115, 198, 46, 60, 141, 66, 176, 80, 79, 133, 151, 72, 118, 197, 116, 77, 166, 28, 184, 147, 177, 10, 190, 4, 112, 156, 123, 162, 194, 70, 6, 154, 158, 53, 182, 99, 26, 1, 152, 87, 2, 172, 55, 57, 92, 129, 82, 120, 44, 25, 78, 88, 175, 50, 31, 38, 157, 196, 81, 90, 27, 71, 58, 7, 95, 39, 165, 8, 178, 106, 49]
path_2regret_b = [117, 30, 42, 196, 108, 80, 162, 45, 5, 7, 177, 123, 25, 182, 139, 11, 49, 160, 144, 104, 8, 82, 21, 36, 141, 97, 81, 146, 187, 186, 129, 163, 165, 137, 75, 93, 76, 48, 166, 194, 88, 64, 86, 95, 130, 22, 52, 57, 66, 47, 148, 60, 20, 59, 28, 149, 199, 140, 183, 174, 83, 34, 170, 53, 184, 155, 84, 3, 15, 145, 13, 132, 169, 188, 6, 134, 2, 74, 118, 98, 51, 125, 191, 71, 147, 115, 10, 133, 44, 17, 40, 100, 63, 92, 38, 16, 1, 24, 31, 73, 117]
path_weights_a = [89, 183, 143, 117, 0, 46, 68, 139, 193, 41, 115, 5, 42, 181, 159, 69, 108, 18, 22, 146, 34, 160, 48, 54, 177, 10, 190, 4, 112, 84, 184, 43, 116, 65, 59, 118, 51, 151, 133, 162, 123, 127, 70, 135, 154, 180, 53, 121, 100, 26, 86, 75, 44, 25, 16, 171, 175, 113, 56, 31, 78, 145, 179, 196, 81, 90, 40, 165, 185, 106, 178, 138, 14, 144, 62, 9, 148, 102, 49, 52, 55, 92, 57, 129, 82, 120, 2, 101, 1, 97, 152, 124, 94, 63, 79, 80, 176, 137, 23, 186, 89]
path_weights_b = [148, 47, 94, 60, 20, 59, 28, 149, 4, 140, 183, 152, 170, 34, 55, 18, 62, 128, 124, 106, 143, 35, 109, 0, 29, 160, 33, 11, 134, 74, 118, 121, 51, 90, 131, 54, 31, 193, 117, 1, 38, 135, 63, 122, 133, 10, 115, 147, 6, 188, 169, 132, 13, 70, 3, 15, 145, 195, 168, 139, 182, 138, 104, 8, 111, 82, 21, 177, 5, 45, 142, 78, 175, 36, 61, 91, 141, 77, 81, 153, 187, 163, 89, 127, 137, 114, 103, 26, 176, 113, 194, 166, 95, 86, 185, 179, 22, 99, 130, 9, 148]

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

    analyze(path_2regret_a, "path_2regret_a", nodes_n)
    analyze(path_2regret_b, "path_2regret_b", nodes_n)
    analyze(path_weights_a, "path_weights_a", nodes_n)
    analyze(path_weights_b, "path_weights_b", nodes_n)