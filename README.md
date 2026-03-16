# Weapon Throw & Recall System (Unreal Engine C++)

Contains the gameplay code for a throwable weapon system developed in Unreal Engine.
Demo Video - https://youtu.be/C4jqIEb3IbA?si=XvAKY3tTIT2iWpLL

## Features

- Third-person throwable weapon mechanic
- Weapon state system (InHand, Flying, Returning)
- Distance-based auto recall
- Curved spline-based return path to the player's hand
- Continuous weapon spin while flying and returning
- Catch animation trigger on successful return
- Blueprint hooks for VFX, SFX, and animations

## Technical Highlights

- Implemented using **Unreal Engine C++**
- Uses `UProjectileMovementComponent` for throw mechanics
- Uses `USplineComponent` to generate a dynamic recall trajectory


