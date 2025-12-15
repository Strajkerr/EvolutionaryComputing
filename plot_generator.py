import csv
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import LinearSegmentedColormap

def read_nodes_from_csv(filename):
    """
    Reads node data from a CSV file.
    Each row: x; y; cost
    Returns:
        nodes (list of (x, y)),
        costs (list of float)
    """
    nodes = []
    costs = []
    with open(filename, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=';')
        for row in reader:
            if not row or len(row) < 3:
                continue
            x, y, cost = map(float, row[:3])
            nodes.append((x, y))
            costs.append(cost)
    return nodes, costs


def visualize_solution(nodes, path=None, costs=None,
                       title="Graph Visualization",
                       edges=None,
                       show=True,
                       save_path=None):
    """
    Visualize a 2D graph or TSP-like path.
    - nodes: list of (x, y)
    - path: optional ordered list of node indices
    - costs: list of node costs (optional, used for color)
    - edges: optional list of (i, j) pairs to draw connections
    """
    nodes = np.array(nodes)
    x, y = nodes[:, 0], nodes[:, 1]

    if costs is not None:
        costs = np.array(costs)
        normalized_costs = (costs - np.min(costs)) / (np.max(costs) - np.min(costs) + 1e-9)
    else:
        normalized_costs = np.zeros(len(nodes))

    plt.figure(figsize=(8, 6))

    if edges is None and path is not None:
        edges = [(path[i], path[i + 1]) for i in range(len(path) - 1)]
    elif edges is None:
        edges = []

    for (i, j) in edges:
        plt.plot([nodes[i, 0], nodes[j, 0]], [nodes[i, 1], nodes[j, 1]],
                 color='black', lw=1, alpha=0.8, zorder=4)


    pink_map = LinearSegmentedColormap.from_list("white_pink", ["#ffffff", "#ff69b4"])

    scatter = plt.scatter(
        x, y,
        c=normalized_costs,
        cmap=pink_map,
        s=80,
        edgecolor='black',
        alpha=0.95,
        zorder=3
    )

    plt.title(title)
    if costs is not None:
        plt.colorbar(scatter, label="Node Cost")

    plt.axis("equal")
    plt.xlabel("X coordinate")
    plt.ylabel("Y coordinate")
    plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300)
        print(f"Saved figure {save_path}")
    if show:
        plt.show()
    plt.close()


if __name__ == "__main__":
    nodesa, costsa = read_nodes_from_csv(r".\TSPA.csv")
    nodesb, costsb = read_nodes_from_csv(r".\TSPB.csv")
    op1A = [133, 151, 51, 118, 59, 115, 46, 68, 139, 41, 193, 159, 181, 42, 43, 116, 65, 149, 131, 184, 84, 112, 4, 190, 10, 177, 54, 48, 160, 34, 146, 22, 18, 108, 140, 93, 117, 0, 143, 183, 89, 186, 23, 137, 176, 80, 79, 63, 94, 124, 148, 9, 62, 102, 144, 14, 49, 178, 106, 52, 55, 57, 129, 92, 179, 185, 40, 119, 165, 90, 81, 196, 145, 78, 31, 56, 113, 175, 171, 16, 25, 44, 120, 2, 152, 97, 1, 101, 75, 86, 26, 100, 53, 180, 154, 135, 70, 127, 123, 162]
    op2A = [106, 52, 55, 57, 92, 129, 2, 152, 124, 94, 63, 79, 133, 151, 80, 176, 137, 23, 186, 89, 183, 143, 0, 117, 93, 140, 108, 69, 18, 199, 22, 146, 159, 193, 41, 139, 68, 46, 115, 118, 51, 59, 65, 116, 43, 42, 181, 34, 160, 48, 54, 177, 10, 4, 112, 84, 184, 131, 149, 162, 123, 127, 70, 135, 154, 180, 53, 100, 26, 97, 1, 101, 86, 75, 120, 44, 25, 16, 171, 175, 113, 31, 78, 145, 179, 196, 81, 90, 40, 185, 165, 138, 14, 144, 62, 9, 148, 102, 49, 178]
    op2_nolsA = [79, 80, 176, 122, 63, 94, 124, 167, 148, 137, 183, 89, 23, 186, 15, 9, 62, 144, 14, 49, 178, 106, 52, 55, 185, 40, 165, 90, 81, 196, 157, 31, 113, 175, 171, 16, 25, 44, 120, 78, 145, 179, 57, 92, 129, 2, 152, 1, 97, 26, 100, 101, 75, 86, 53, 158, 180, 154, 135, 70, 127, 123, 112, 4, 190, 10, 177, 54, 48, 34, 160, 184, 131, 149, 65, 116, 43, 42, 5, 41, 193, 159, 181, 146, 22, 18, 108, 117, 143, 0, 46, 68, 139, 115, 59, 118, 51, 151, 162, 133]
    op1B = [185, 95, 130, 99, 179, 66, 94, 47, 148, 60, 20, 28, 149, 4, 140, 183, 152, 170, 34, 55, 18, 62, 124, 106, 143, 35, 109, 0, 29, 111, 82, 21, 8, 104, 144, 160, 33, 138, 11, 139, 168, 195, 13, 145, 15, 3, 70, 132, 169, 188, 6, 147, 90, 51, 121, 131, 135, 122, 107, 40, 63, 38, 27, 16, 1, 156, 198, 117, 193, 31, 54, 73, 136, 190, 80, 45, 142, 175, 78, 5, 177, 36, 61, 91, 141, 77, 81, 153, 187, 163, 89, 127, 137, 114, 103, 113, 176, 194, 166, 86]
    op2B = [145, 15, 3, 70, 132, 169, 188, 6, 134, 147, 178, 10, 133, 107, 40, 63, 135, 122, 90, 191, 51, 121, 131, 38, 1, 156, 198, 117, 193, 31, 54, 73, 136, 190, 80, 162, 45, 175, 78, 5, 177, 25, 182, 138, 104, 144, 111, 8, 82, 21, 61, 36, 91, 141, 77, 81, 153, 187, 163, 89, 127, 103, 113, 176, 194, 166, 86, 95, 185, 179, 172, 57, 66, 94, 47, 148, 20, 28, 140, 183, 152, 170, 34, 55, 18, 62, 124, 106, 143, 35, 109, 0, 29, 160, 33, 11, 139, 168, 195, 13]
    op2_nolsB = [113, 180, 176, 194, 166, 86, 185, 95, 130, 99, 179, 94, 47, 148, 60, 20, 28, 149, 4, 140, 183, 152, 170, 34, 55, 18, 83, 62, 128, 124, 106, 35, 109, 0, 29, 111, 8, 82, 21, 104, 144, 160, 33, 49, 138, 182, 11, 139, 168, 195, 126, 132, 13, 145, 15, 3, 70, 169, 188, 6, 134, 147, 51, 191, 90, 122, 63, 135, 131, 121, 1, 38, 27, 156, 198, 117, 193, 31, 54, 73, 136, 190, 80, 175, 78, 142, 5, 177, 36, 61, 91, 141, 97, 77, 81, 153, 163, 89, 127, 103]
    visualize_solution(nodesa, op1A, costsa, title="HEA_Op1 (Common) TSPA", save_path="HEA_Op1_A.png")
    visualize_solution(nodesb, op1B, costsb, title="HEA_Op1 (Common) TSPB", save_path="HEA_Op1_B.png")
    visualize_solution(nodesa, op2A, costsa, title="HEA_Op2 (Filter+LS) TSPA", save_path="HEA_Op2_A.png")
    visualize_solution(nodesb, op2B, costsb, title="HEA_Op2 (Filter+LS) TSPB", save_path="HEA_Op2_B.png")
    visualize_solution(nodesa, op2_nolsA, costsa, title="HEA_Op2_NoLS (Filter) TSPA", save_path="HEA_Op2_NoLS_A.png")
    visualize_solution(nodesb, op2_nolsB, costsb, title="HEA_Op2_NoLS (Filter) TSPB", save_path="HEA_Op2_NoLS_B.png")