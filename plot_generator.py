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
    nodesa, costsa = read_nodes_from_csv("TSPA.csv")
    nodesb, costsb = read_nodes_from_csv("TSPB.csv")
    
    msls_tspa = [43, 42, 5, 96, 115, 118, 59, 72, 151, 109, 51, 66, 137, 176, 80, 133, 79, 122, 63, 94, 152, 97, 1, 101, 26, 100, 121, 180, 154, 158, 53, 86, 75, 2, 120, 44, 25, 129, 92, 57, 179, 145, 78, 16, 171, 175, 113, 31, 196, 81, 90, 165, 119, 40, 185, 55, 52, 106, 178, 49, 14, 144, 62, 9, 148, 15, 186, 23, 89, 183, 143, 117, 0, 46, 139, 68, 93, 140, 108, 69, 18, 22, 193, 41, 181, 34, 160, 54, 177, 184, 112, 127, 70, 135, 162, 123, 149, 131, 65, 116]
    
    msls_tspb = [145, 195, 168, 49, 33, 138, 182, 11, 139, 74, 118, 51, 121, 131, 90, 122, 107, 40, 63, 135, 38, 1, 156, 198, 117, 54, 73, 31, 193, 190, 80, 175, 78, 5, 177, 25, 157, 104, 56, 8, 111, 144, 160, 29, 12, 0, 109, 35, 34, 55, 18, 62, 124, 106, 143, 159, 81, 82, 87, 21, 61, 36, 91, 141, 97, 77, 153, 187, 163, 165, 127, 89, 103, 114, 113, 180, 176, 194, 166, 86, 95, 185, 179, 94, 47, 148, 20, 140, 183, 152, 155, 3, 70, 188, 6, 147, 134, 169, 132, 13]
    
    # ILS Best Solutions
    ils_tspa = [23, 186, 114, 89, 183, 143, 117, 0, 46, 115, 139, 41, 193, 159, 22, 146, 181, 42, 5, 43, 116, 65, 47, 149, 131, 35, 184, 160, 34, 54, 177, 10, 190, 4, 112, 123, 127, 70, 135, 154, 180, 158, 53, 121, 100, 26, 97, 1, 101, 86, 75, 120, 44, 25, 16, 171, 175, 113, 31, 78, 145, 179, 196, 81, 90, 165, 40, 185, 14, 144, 62, 9, 148, 102, 49, 178, 106, 52, 55, 57, 92, 129, 2, 152, 19, 189, 124, 94, 63, 122, 79, 133, 151, 162, 59, 118, 51, 80, 176, 137]
    
    ils_tspb = [141, 77, 81, 153, 187, 163, 89, 127, 103, 113, 176, 194, 166, 86, 106, 159, 143, 124, 62, 18, 34, 55, 95, 185, 179, 66, 94, 47, 148, 60, 20, 28, 140, 183, 152, 155, 3, 70, 15, 145, 168, 195, 13, 132, 169, 188, 6, 192, 147, 134, 85, 74, 118, 98, 51, 121, 90, 122, 133, 107, 40, 63, 135, 38, 27, 1, 198, 117, 193, 31, 54, 164, 73, 136, 190, 80, 175, 78, 5, 177, 25, 182, 138, 139, 11, 33, 160, 29, 0, 109, 35, 111, 144, 104, 8, 82, 21, 61, 36, 91]
    
    # Generate MSLS plots
    visualize_solution(nodesa, msls_tspa, costsa, 
                      title="MSLS Best Solution - TSPA (Cost: 72344)", 
                      save_path="msls_tspa.png")
    
    visualize_solution(nodesb, msls_tspb, costsb, 
                      title="MSLS Best Solution - TSPB (Cost: 47766)", 
                      save_path="msls_tspb.png")
    
    # Generate ILS plots
    visualize_solution(nodesa, ils_tspa, costsa, 
                      title="ILS Best Solution - TSPA (Cost: 71906)", 
                      save_path="ils_tspa.png")
    
    visualize_solution(nodesb, ils_tspb, costsb, 
                      title="ILS Best Solution - TSPB (Cost: 45809)", 
                      save_path="ils_tspb.png")