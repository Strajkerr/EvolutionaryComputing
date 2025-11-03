import csv
from collections import Counter
import math
import os

# ----- Paste your paths here -----
path_m_tspb = [139, 11, 29, 0, 109, 35, 34, 18, 62, 124, 106, 143, 81, 153, 187, 163, 89, 127, 137, 114, 103, 113, 180, 176, 88, 194, 166, 86, 95, 185, 179, 66, 94, 47, 148, 20, 28, 140, 183, 152, 184, 155, 3, 70, 161, 15, 145, 168, 195, 13, 132, 169, 188, 6, 147, 10, 133, 107, 40, 63, 102, 135, 122, 90, 51, 121, 131, 38, 27, 16, 1, 156, 198, 117, 193, 31, 54, 73, 136, 190, 80, 45, 175, 78, 5, 177, 36, 61, 141, 77, 82, 21, 104, 8, 111, 144, 160, 33, 138, 182, 139]

path_m_tspa = [101, 75, 2, 120, 44, 25, 82, 129, 92, 57, 179, 145, 78, 16, 171, 175, 113, 56, 31, 196, 81, 90, 27, 164, 7, 95, 165, 40, 185, 55, 52, 106, 178, 3, 49, 14, 144, 62, 148, 12, 94, 152, 1, 97, 100, 63, 79, 133, 80, 176, 137, 23, 89, 183, 143, 0, 117, 108, 69, 18, 22, 146, 159, 193, 41, 139, 46, 115, 96, 42, 181, 34, 160, 48, 54, 177, 10, 190, 184, 84, 4, 112, 127, 123, 149, 131, 43, 116, 65, 59, 118, 51, 151, 162, 135, 70, 154, 180, 53, 86, 101]

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