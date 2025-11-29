# Recipe Manager

A simple C++ web application for managing recipes with nutritional filtering, dietary preferences, and CRUD operations.

## Features

### Functional Requirements ✓

- Load recipes from SQLite database
- Filter recipes by protein and carb content
- Display recipe title, image, and description
- Full CRUD operations (Create, Read, Update, Delete)
- Dietary filters: Vegan, Vegetarian, Gluten-Free
- Sort recipes by cook time or difficulty
- Responsive UI that works on all devices

### Technical Stack

- **Backend**: C++ with cpp-httplib (HTTP server) + SQLite3 (database)
- **Frontend**: HTML5, CSS3, Vanilla JavaScript
- **Database**: SQLite (no server setup required)

## Quick Start

### Prerequisites

- C++17 compatible compiler (g++/clang)
- SQLite3

### Installation

1. **Clone the repository**

   ```bash
   git clone https://github.com/syedajuhimoosavi/cs301-Recipebook-Fall2025.git
   ```

2. **Build the backend**

   ```bash
   cd backend
   chmod +x ./run.sh
   make
   chmod +x ./recipe_server
   ```

3. **Initialize database with sample data**

   ```bash
   sqlite3 recipes.db < schema.sql
   ```

4. **Run the server**

   ```bash
   ./run.sh
   ```

5. **Open in browser**
   ```
   http://localhost:8080
   ```
