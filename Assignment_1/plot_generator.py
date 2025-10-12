import csv
import matplotlib.pyplot as plt
import numpy as np

def read_nodes_from_csv(filename):
    """
    Reads node data from a CSV file.
    Each row: x, y, cost
    Returns:
        nodes (list of (x, y)),
        costs (list of float)
    """
    nodes = []
    costs = []
    with open(filename, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=';')
        for row in reader:
            print(row)
            if not row or len(row) < 3:
                continue  # skip empty or invalid lines
            x, y, cost = map(int, row[:3])
            nodes.append((x, y))
            costs.append(cost)
    return nodes, costs


def visualize_solution(nodes, path, costs, title="Best Solution", show=True, save_path=None):
    """
    Visualize a 2D solution path with node costs shown as color or size.
    """
    nodes = np.array(nodes)
    x, y = nodes[:, 0], nodes[:, 1]
    normalized_costs = (np.array(costs) - np.min(costs)) / (np.max(costs) - np.min(costs) + 1e-9)

    # Plot edges for the path
    for i in range(len(path) - 1):
        x0, y0 = nodes[path[i]]
        x1, y1 = nodes[path[i + 1]]
        plt.plot([x0, x1], [y0, y1], 'k-', lw=1, alpha=0.7)

    # Plot nodes
    scatter = plt.scatter(
        x, y,
        c=normalized_costs,
        cmap='viridis',
        s=50 + 200 * normalized_costs,
        edgecolor='black',
        alpha=0.9
    )

    # Label nodes
    for i, node in enumerate(path):
        plt.text(nodes[node, 0], nodes[node, 1], str(node),
                 fontsize=8, ha='center', va='center', color='white', fontweight='bold')

    plt.title(title)
    plt.colorbar(scatter, label="Node Cost")
    plt.axis("equal")
    plt.xlabel("X coordinate")
    plt.ylabel("Y coordinate")
    plt.tight_layout()

    if save_path:
        plt.savefig(save_path, dpi=300)
    if show:
        plt.show()
    plt.close()


# ==== Example usage ====
if __name__ == "__main__":
    # Load data
    nodes, costs = read_nodes_from_csv(r"C:\Users\kubal\OneDrive\Pulpit\uni\7term\EvolutionaryComputing\TSPA.csv")
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
    86, 6, 59, 8, 34, 156, 74, 91, 182, 103, 127, 163, 153, 168, 148, 44, 164, 185,
    18, 194, 70, 97, 176, 116, 120, 54, 10, 146, 111, 72, 154, 31, 132, 50, 184, 100,
    95, 93, 7, 82, 1, 90, 183, 191, 19, 126, 63, 197, 166, 198, 94, 79, 181, 22, 23,
    53, 139, 11, 137, 138, 57, 5, 15, 162, 41, 161, 151, 117, 51, 46, 48, 28, 55, 109,
    133, 21, 65, 42, 29, 47, 145, 125, 196, 130, 121, 40, 12, 0, 69, 78, 108, 123, 81,
    101, 129, 62, 66, 140, 99, 113
    ]
    path_greedy = [
    154, 180, 53, 63, 176, 80, 151, 59, 65, 116, 42, 193, 41, 139, 115, 46, 0, 183,
    143, 117, 93, 18, 22, 34, 160, 184, 123, 135, 70, 127, 162, 133, 79, 94, 97, 101,
    1, 152, 120, 78, 145, 185, 40, 165, 90, 81, 113, 175, 171, 16, 31, 44, 92, 57,
    106, 49, 144, 62, 14, 178, 52, 55, 129, 2, 75, 86, 26, 100, 121, 148, 137, 23,
    186, 89, 114, 15, 9, 102, 138, 21, 164, 7, 95, 39, 27, 196, 179, 25, 82, 167,
    124, 51, 118, 43, 149, 131, 112, 4, 177, 54
    ]
    print(len(nodes))
    visualize_solution(nodes, path_random, costs, title="Random Solution")
    visualize_solution(nodes, path_neigbour_end, costs, title="Neighbour End Solution") 
    visualize_solution(nodes, path_neighbour, costs, title="Nearest Neighbour Solution")
    visualize_solution(nodes, path_greedy, costs, title="Greedy Solution") 