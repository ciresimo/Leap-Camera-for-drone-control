import os
import pandas as pd
import numpy as np
import torch
import torch.nn as nn

# Define the pretrained model class
class GestureNet(nn.Module):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)

        self.sequence = nn.Sequential(
            nn.BatchNorm1d(17 * 100),
            nn.Linear(17 * 100, 300),
            nn.ELU(),
            nn.BatchNorm1d(300),
            nn.Linear(300, 200),
            nn.ELU(),
            nn.BatchNorm1d(200),
            nn.Linear(200, 120),
            nn.ELU(),
            nn.BatchNorm1d(120),
            nn.Linear(120, 60),
            nn.ELU(),
            nn.BatchNorm1d(60),
            nn.Linear(60, 30),
            nn.ELU(),
            nn.BatchNorm1d(30),
            nn.Linear(30, 10),
            nn.Softmax(dim=1),
        )

    def forward(self, x):
        x = torch.flatten(x, 1)
        return self.sequence(x)

# Function to load the model
def load_model(model_path):
    model = GestureNet()
    model.load_state_dict(torch.load(model_path))
    model.eval()
    return model

# Function to check if file exists, read data, and make prediction
def process_and_predict(file_path, model_path):
    if not os.path.exists(file_path):
        print("File does not exist.")
        return

    # Read the CSV file
    data = pd.read_csv(file_path, header=None)
    if data.shape != (100, 17):
        print("Invalid data shape.")
        return
    
    # Transform the data to the required format
    input_tensor = torch.tensor(data.values.T, dtype=torch.float32).unsqueeze(0)  # Shape: [1, 17, 100]

    # Load the model
    model = load_model(model_path)

    # Make prediction
    with torch.no_grad():
        output = model(input_tensor)
        _, predicted_idx = torch.max(output, 1)
    
    # Map the prediction to the corresponding gesture
    gestures = ['ECL', 'ECR', 'P1L','P1R', 'P2L', 'P2R', 'R1L', 'R2L', 'Y1R', 'Y2R']
    predicted_gesture = gestures[predicted_idx.item()]

    # Print the prediction
    print(f"Predicted gesture: {predicted_gesture}")

# Example usage
file_path = "Output/Output.csv"
model_path = "GestureRecognitionNetwork.pth"  # Path to your saved model
process_and_predict(file_path, model_path)
