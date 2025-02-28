#!/bin/python3
import numpy as np

def main():
    # Define initial values
    x = np.array([0.0, 0.0])         # Initial state vector (e.g., [position, velocity])
    p = np.eye(2) * 100             # Initial covariance matrix
    
    A = np.array([[1, 1], [0, 1]])   # State transition matrix
    Q = np.eye(2) * 0.1            # Process noise covariance
    
    H = np.array([1, 0])            # Measurement matrix (only measuring position)
    R = 1e-4                       # Measurement noise covariance
    
    z = np.array([5.0])             # Measurement vector
    I = np.eye(2)                   # Identity matrix

    # Time Update (Prediction)
    x_pred = A @ x                  # Predicted state estimate
    P_pred = A @ p @ A.T + Q       # Predicted covariance
    
    # Measurement Update
    y = z - H.dot(x_pred)           # Innovation (residual)
    S = H.dot(P_pred).dot(H.T) + R  # Innovation covariance
    K = (P_pred.dot(H.T)) / S      # Kalman gain

    x = x_pred + K.dot(y)          # Updated state estimate
    p = I - K.dot(H).dot(P_pred)   # Updated covariance

    print("Updated State Estimate:", x)
    print("Updated Covariance Matrix:\n", p)

if __name__ == "__main__":
    main()
