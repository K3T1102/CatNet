# CatNet

![GitHub stars](https://img.shields.io/github/stars/K3T1102/CatNet?style=for-the-badge&logo=github) ![GitHub forks](https://img.shields.io/github/forks/K3T1102/CatNet?style=for-the-badge&logo=github) ![GitHub issues](https://img.shields.io/github/issues/K3T1102/CatNet?style=for-the-badge&logo=github) ![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)

## 📑 Table of Contents

- [Description](#description)
- [Tech Stack](#tech-stack)
- [Quick Start](#quick-start)
- [Key Dependencies](#key-dependencies)
- [Screenshots](#screenshots)
- [Project Structure](#project-structure)
- [Development Setup](#development-setup)
- [Contributing](#contributing)

## 📝 Description

CatNet is a versatile Python-based networking platform specifically designed for feline enthusiasts and pet owners. This project provides a robust framework for managing feline-related data, facilitating community interaction, and organizing cat-centric content. Leveraging the power and simplicity of Python, CatNet offers an intuitive interface for users to connect over shared interests in cat health, behavior, and welfare, creating a digital hub for all things feline.

## 🛠️ Tech Stack

- 🐍 Python

## ⚡ Quick Start

```bash

# Clone the repository
git clone https://github.com/K3T1102/CatNet.git

# Create virtual environment
python -m venv venv && source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

# Create db
cd app
flask db init
flask db migrate -m "migration"
flask db upgrade
```

## 📦 Key Dependencies

```
alembic: 1.18.4
blinker: 1.9.0
certifi: 2026.4.22
charset-normalizer: 3.4.7
click: 8.3.3
colorama: 0.4.6
Flask: 3.1.3
Flask-Migrate: 4.1.0
Flask-SQLAlchemy: 3.1.1
greenlet: 3.5.0
idna: 3.13
itsdangerous: 2.2.0
Jinja2: 3.1.6
Mako: 1.3.12
MarkupSafe: 3.0.3
```

## 📸 Screenshots

<img width="1823" height="778" alt="frontend-before" src="https://github.com/user-attachments/assets/16b53f59-f0d8-436b-a477-93368a365cc9" />
<br>
<img width="1371" height="470" alt="frontend-after" src="https://github.com/user-attachments/assets/6fb5f3cb-0d8a-49dc-9308-daf2f5e36430" />

## 📁 Project Structure

```
.
├── agents
│   ├── activeService.h
│   ├── agent.cpp
│   ├── agent.exe
│   ├── download.h
│   ├── service.cpp
│   └── service.exe
├── app
│   ├── app.py
│   └── templates
│       └── index.html
└── requirements.txt
```

## 🛠️ Development Setup

### Python Setup
1. Install Python (v3.8+ recommended)
2. Create a virtual environment: `python -m venv venv`
3. Activate the environment:
   - Windows: `venv\Scripts\activate`
   - Unix/MacOS: `source venv/bin/activate`
4. Install dependencies: `pip install -r requirements.txt`

## 👥 Contributing

Contributions are welcome! Here's how you can help:

1. **Fork** the repository
2. **Clone** your fork: `git clone https://github.com/K3T1102/CatNet.git`
3. **Create** a new branch: `git checkout -b feature/your-feature`
4. **Commit** your changes: `git commit -am 'Add some feature'`
5. **Push** to your branch: `git push origin feature/your-feature`
6. **Open** a pull request

Please ensure your code follows the project's style guidelines and includes tests where applicable.
