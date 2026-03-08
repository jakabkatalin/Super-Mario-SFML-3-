# Super Mario SFML Engine (C++ Demonstration)

Acest proiect reprezintă dezvoltarea unui motor de joc 2D de la zero, utilizând limbajul **C++** și biblioteca **SFML 3**. Proiectul servește ca demonstrație tehnică pentru aplicarea principiilor OOP, gestionarea resurselor hardware și implementarea sistemelor de fizică în timp real.

> ### Legal & Assets Disclaimer
> * **Proprietate Intelectuală:** Proiect creat strict în scop educativ, non-comercial. Toate drepturile asupra numelui "Super Mario" și personajelor aparțin **Nintendo**.
> * **Resurse Grafice:** Sprite-urile au fost preluate din baze de date publice (Sprite Resources) și aparțin Nintendo. Nu revendic drepturi asupra activelor vizuale/audio.
> * **Cod Sursă:** Întreaga logică de programare (gameplay, fizică, map management) este scrisă integral de mine.

---

## Detalii Tehnice & Arhitectură

Proiectul utilizează un cod modular axat pe performanță:

### 1. Sistem de Animație
* **Implementare:** Gestionarea cadrelor prin `sf::IntRect` pentru decuparea din sprite-sheets.
* **Logic:** State Machine pentru tranzițiile între animații (Idle, Walking, Jumping) bazate pe vectorii de viteză.

### 2. Map & Tile Manager
* **Structură:** Sistem de grid (Tilemap) pentru randarea eficientă a nivelului.
* **Performanță:** Implementare **Culling** pentru a minimiza apelurile către GPU (Draw Calls).

### 3. Fizică și Coliziuni
* **AABB Collision:** Algoritm *Axis-Aligned Bounding Box* pentru interacțiunea jucător-mediu-inamici.
* **Cinematică:** Sistem de gravitate, accelerație și inerție pentru fluiditatea mișcării.

### 4. Design Patterns & OOP
* **Moștenire și Polimorfism:** Entitățile moștenesc dintr-o clasă de bază comună pentru gestionare uniformă în game loop.
* **Encapsulare:** Separarea logicii de business de logica de randare.

---

## Demonstrație Gameplay

În acest demo puteți vedea funcționalitățile de bază implementate:


https://github.com/user-attachments/assets/b9c47895-b0ed-4147-9fd3-bd02647d4ef2

