import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np

def generate_charts_for_instance(instance_name):
    csv_file = f"{instance_name}_correlations.csv"
    
    if not os.path.exists(csv_file):
        print(f"File {csv_file} not found. Skipping...")
        return

    # Read data
    df = pd.read_csv(csv_file)
    
    # Define the mapping for the 6 charts per instance
    # Structure: (Column Name, Title, Row Index, Col Index)
    # Row 0 = Nodes, Row 1 = Edges
    charts_config = [
        # Node Similarity (Row 0)
        ("AvgSim_Nodes", "Avg Similarity (Nodes)", 0, 0),
        ("BestLO_Nodes", "Similarity to Best-of-1000 (Nodes)", 0, 1),
        ("Global_Nodes", "Similarity to Global Best (Nodes)", 0, 2),
        
        # Edge Similarity (Row 1)
        ("AvgSim_Edges", "Avg Similarity (Edges)", 1, 0),
        ("BestLO_Edges", "Similarity to Best-of-1000 (Edges)", 1, 1),
        ("Global_Edges", "Similarity to Global Best (Edges)", 1, 2),
    ]

    # Create a figure with 2 rows and 3 columns
    fig, axes = plt.subplots(2, 3, figsize=(18, 10))
    fig.suptitle(f'Global Convexity Analysis - {instance_name}', fontsize=16)

    # Generate each subplot
    for col_name, title, r, c in charts_config:
        ax = axes[r, c]
        
        # Extract data
        x = df["Objective"]
        y = df[col_name]
        
        # Filter out self-similarity (similarity = 1.0) for "BestLO" plots
        # to avoid skewing the correlation/visuals
        mask = y < 0.999
        x_filtered = x[mask]
        y_filtered = y[mask]
        
        # Calculate correlation for title
        if len(x_filtered) > 1:
            correlation = np.corrcoef(x_filtered, y_filtered)[0, 1]
        else:
            correlation = 0.0

        # Scatter plot
        ax.scatter(x_filtered, y_filtered, alpha=0.5, s=10, c='blue')
        
        # Formatting
        ax.set_title(f"{title}\nCorr: {correlation:.4f}")
        ax.set_xlabel("Objective Function")
        ax.set_ylabel("Similarity")
        ax.grid(True, linestyle='--', alpha=0.6)

    plt.tight_layout(rect=[0, 0.03, 1, 0.95]) # Adjust for suptitle
    
    # Save the figure
    output_filename = f"{instance_name}_charts.png"
    plt.savefig(output_filename, dpi=300)
    print(f"Generated charts for {instance_name}: {output_filename}")
    plt.close()

if __name__ == "__main__":
    instances = ["TSPA", "TSPB"]
    print("Generating charts...")
    for instance in instances:
        generate_charts_for_instance(instance)
    print("Done!")