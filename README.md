# BP Game Project (SFML)

A simple Sudoku game implemented in **C++17** with **SFML 2.6** for our BP project.  
I used SFML so I could have more freedom in graphics and interaction.  
What I liked most was the chance to spice it up with a little story:  
you’re a spy taking a test to prove your skills.

---

## Features

- Sudoku grid generation & validation  
- Login & password system  
- Score saving & leaderboard  
- difficulty levels:  
  - **Spy** (easy)  
  - **Super Spy** (medium)  
  - **Semnani** (hard)  

---

## Build (macOS with Homebrew)

```bash
brew install sfml@2
cmake -S . -B build -DSFML_DIR="/opt/homebrew/Cellar/sfml@2/2.6.2/lib/cmake/SFML"
cmake --build build
./build/SFML_Project
```

---

## Screenshots

<img width="818" height="623" alt="intro" src="https://github.com/user-attachments/assets/65d997b0-9421-49f4-bb6f-807a0cf29443" />
<img width="799" height="629" alt="menu" src="https://github.com/user-attachments/assets/40928146-065e-40a7-b597-e8d76a3c0e99" />
<img width="784" height="632" alt="game" src="https://github.com/user-attachments/assets/bab473ce-8402-4c27-a567-c2277cb7d009" />
