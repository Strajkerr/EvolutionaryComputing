import csv
from collections import Counter
import math
import os

# ----- Paste your paths here -----
path_m_tspa = [13, 56, 31, 81, 90, 165, 119, 40, 185, 55, 52, 106, 178, 3, 138, 14, 144, 49, 102, 62, 9, 15, 114, 186, 148, 124, 94, 63, 122, 79, 80, 176, 137, 23, 89, 183, 143, 0, 117, 93, 140, 68, 46, 115, 139, 41, 193, 159, 18, 22, 146, 34, 160, 48, 54, 177, 184, 84]
path_m_tspb = [131, 121, 1, 24, 156, 198, 117, 54, 31, 193, 190, 80, 45, 175, 78, 5, 177, 36, 61, 91, 141, 21, 104, 8, 82, 77, 81, 153, 187, 163, 103, 89, 127, 137, 114, 113, 180, 176, 194, 166, 86, 95, 130, 99, 185, 179, 66, 94, 47, 148, 60, 20, 28, 149, 4, 199, 140, 183, 152, 170, 34, 55, 18, 62, 128, 124, 106, 159, 143, 111, 35, 109, 0, 29, 160, 33, 138, 182, 11, 139, 168, 195, 145, 15, 3, 70, 169, 188, 6, 147, 51, 191, 90, 10, 133, 107, 40, 63, 135, 122]
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