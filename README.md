# JAMC
Justice Asura Model Converter. Script for processing and converting Asura Engine model binary files of the game Judge Dredd: Dredd Vs. Death.
## Usage
Drag model file into exe or make cmd file in the following format: ```jamc.exe yourmodel```
## List of supported types models
Name            | Compiled
----------------| ----------------------
Props           | Yes
Ragdolls        | Yes
Levels MP       | Yes/But VERY buggy/Half of the maps can be compiled successfully.
Levels Campaign | Yes/But VERY buggy and VERY unstable.
## Building Code
To compile the code use: ```Visual Studio .Net 2003``` or ```Dev-C++```
## Examples of converted models
### Prop example:
![00](https://github.com/user-attachments/assets/b70f9e9a-d7f6-4747-8174-ff054ec63c43)
### Ragdoll example:
![02](https://github.com/user-attachments/assets/82c5907a-7c3d-43d2-8a4b-52d0c6ec2cb0)
At this moment, only as mesh, without bones.
### Level example:
![blender_KR3G2tjNVd](https://github.com/user-attachments/assets/107462e8-6e59-46c9-a3bd-fecadedecae9)
Converting levels has a side effect in the form of extra faces. This is an experimental feature at the moment.
