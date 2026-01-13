# ⚔️ Cursed Crypt

A third-person action-adventure game built with **Unreal Engine 5.7** featuring dungeon exploration, melee combat, and AI enemies.

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.7-blue?logo=unrealengine)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)
![Blueprints](https://img.shields.io/badge/Blueprints-Hybrid-orange)

---

## 🎮 About

**Cursed Crypt** is a dungeon crawler where players navigate through a cursed crypt, battling skeleton enemies and collecting keys to unlock doors. The game features a hybrid C++/Blueprint architecture following industry-standard practices.

---

## ✨ Features

### 🗡️ Combat System
- **Light & Heavy Attacks** with different stamina costs and damage values
- **Combo System** with animation montages
- **Hit Reactions** for both player and enemies
- **Weapon Hitbox Collision** that activates during attack animations

### 🤖 AI Enemies
- **Skeleton Enemies** with Behavior Tree AI
- **Dynamic Health Bars** above enemies
- **Attack Animations** with damage dealing
- **Death Animations** and proper cleanup

### 📊 Attribute System
- **Health & Stamina** management via `UAttributeComponent`
- **Event-driven Updates** using delegates (`OnHealthChanged`, `OnStaminaChanged`)
- **Stamina Regeneration** system
- **Damage & Healing** functionality

### 🔑 Interaction System
- **Interface-based Interactions** (`IInteractInterface`)
- **Key Collection** system
- **Door Unlocking** mechanics
- **Health Potions** and other interactable objects

### 🎨 UI System
- **HUD** with health and stamina bars
- **Pause Menu** with settings
- **Tutorial Widget** for new players
- **Notification System** for in-game messages
- **Death Screen** on player death

---

## 🏗️ Architecture

```
Source/CursedCrypt/
├── CryptCharacter.cpp/h      # Main player character
├── AttributeComponent.cpp/h  # Health & Stamina system
├── SkeletonEnemy.cpp/h       # AI enemy character
├── SkeletonAIController.cpp/h # Enemy AI controller
├── AN_SkeletonAttack.cpp/h   # Animation notify for attacks
└── InteractInterface.cpp/h   # Interaction interface
```

### Key Components

| Component | Description |
|-----------|-------------|
| `ACryptCharacter` | Player character with combat, movement, and interaction |
| `UAttributeComponent` | Reusable component for health/stamina management |
| `ASkeletonEnemy` | AI-controlled enemy with behavior tree |
| `ASkeletonAIController` | AI controller for skeleton enemies |
| `IInteractInterface` | Blueprint-friendly interface for interactions |

---

## 🎯 Technical Highlights

- **C++/Blueprint Hybrid** - Core systems in C++, visual scripting for rapid iteration
- **Component-Based Design** - Modular `UAttributeComponent` for any actor
- **Interface-Based Interactions** - Decoupled interaction system using `IInteractInterface`
- **Behavior Trees** - AI controlled via Unreal's Behavior Tree system
- **Enhanced Input System** - Modern input handling with `UInputAction` and `UInputMappingContext`
- **Animation Notifies** - Custom `UAnimNotify` for attack damage windows
- **Event Delegates** - `DECLARE_DYNAMIC_MULTICAST_DELEGATE` for reactive systems

---

## 🛠️ Requirements

- **Unreal Engine 5.7**
- **Visual Studio 2022** (Windows)
- **C++17** compatible compiler

---

## 🚀 Getting Started

1. **Clone the repository**
   ```bash
   git clone https://github.com/your-username/CursedCrypt.git
   ```

2. **Open the project**
   - Double-click `CursedCrypt.uproject`
   - Or right-click → "Generate Visual Studio project files"

3. **Build and run**
   - Open in Visual Studio and build (F5)
   - Or use Unreal Editor's Play button

---

## 🎮 Controls

| Action | Key |
|--------|-----|
| Move | WASD |
| Look | Mouse |
| Light Attack | Left Mouse Button |
| Heavy Attack | Right Mouse Button |
| Sprint | Left Shift |
| Interact | E |
| Pause | Escape |

---

## 📁 Project Structure

```
CursedCrypt/
├── Source/CursedCrypt/   # C++ source files
├── Content/              # Blueprints, assets, levels
├── Config/               # Project configuration
└── Binaries/             # Compiled binaries
```

---

## 📄 License

This project is for educational purposes.

---

## 👤 Author

**Mehmet Kerim Artan**  

---

*Made with ❤️ and Unreal Engine 5.7*
