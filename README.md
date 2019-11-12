# Duckytown-Robot
Code for our Duckytown group project, CS503. This is an autonomous robot which can navigate small-scale roads.

Python Requirements:
pySerial

## Setting up the Python virtual environment:
Create the environment:
```
python -m venv duckytown-venv
```
Activate the environment:
```
.\duckytown-venv\Scripts\activate
```
Install required packages:
```
pip install pySerial
pip install numpy
pip install pycamera
pip install pillow
pip install pygame==2.0.0.dev6  # THIS IS FOR PYTHON3.8. THE CURRENT PYGAME IS NOT SUPPORTED 
sudo apt-get install python3-pil python3-pil.imagetk
```
Deactivate the environment:
```
deactivate
```

# Setting up the Arduino
Required libraries:
1. DualMC33926MotorShield