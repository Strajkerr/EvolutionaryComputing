import csv
from collections import Counter
import math
import os

# ----- Paste your paths here -----
path_m_tspb = [12, 29, 160, 33, 144, 111, 8, 104, 138, 182, 11, 139, 43, 168, 195, 145, 15, 3, 70, 13, 132, 169, 188, 6, 134, 147, 71, 120, 191, 90, 125, 51, 98, 118, 116, 121, 131, 122, 135, 63, 38, 1, 198, 117, 54, 31, 193, 190, 80, 175, 5, 177, 21, 82, 61, 36, 79, 91, 141, 97, 77, 81, 153, 159, 143, 106, 124, 128, 86, 176, 163, 165, 127, 89, 103, 113, 194, 166, 185, 95, 99, 179, 94, 47, 148, 20, 28, 149, 4, 140, 183, 152, 55, 83, 62, 18, 34, 35, 109, 0]

path_m_tspa = [178, 3, 14, 144, 49, 102, 62, 9, 15, 148, 167, 124, 94, 63, 79, 133, 80, 176, 137, 23, 186, 114, 89, 183, 143, 0, 117, 68, 46, 115, 139, 69, 108, 18, 22, 159, 193, 41, 5, 42, 181, 34, 160, 54, 177, 10, 4, 112, 84, 184, 43, 116, 65, 59, 118, 51, 151, 162, 123, 127, 70, 135, 154, 180, 53, 100, 26, 86, 75, 101, 1, 97, 152, 2, 129, 120, 44, 25, 16, 171, 175, 113, 56, 31, 78, 145, 92, 57, 55, 52, 179, 196, 81, 90, 27, 165, 119, 40, 185, 106]

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

    analyze(path_m_tspb, "path_m_tspb", nodes_n)
    analyze(path_m_tspa, "path_m_tspa", nodes_n)